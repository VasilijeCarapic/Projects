/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

/* Hardware includes. */
#include "msp430.h"

/* User's includes */
#include "ETF5529_HAL/hal_ETF_5529.h"

/** constants */
#define ADC_CONV_PERIOD    ( pdMS_TO_TICKS( 1500 ) )
#define QUEUE_SIZE         ( 10  )
#define QUEUE_MAILBOX_SIZE ( 1   )
#define NUM_OF_CHARS       ( 100 )
#define CONV_SAMPLE_SIZE   ( 4   )
#define COEFF_SCALE        ( 1600 )
#define DEBOUNCE_PERIOD    ( 10  )

/** define Masks **/
#define SW1_PRESSED        ( 1 << 0 )
#define SW2_PRESSED        ( 1 << 1 )
#define LED3_ON            ( 1 << 2 )
#define LED4_ON            ( 1 << 3 )
#define LEDS_OFF           ( 1 << 4 )

/** task priorities */
#define DISP_TASK_PRIO     ( 1 )
#define TASK_1_PRIO        ( 2 )
#define TASK_2_PRIO        ( 3 )
#define TASK_3_PRIO        ( 5 )
#define DIODE_TASK_PRIO    ( 4 )

#define ULONG_MAX          0xFFFFFFFF

typedef enum { ACTION_UART_VAL_SENT, ACTION_SW_PRESSED, ACTION_UNDEFINED } action_t;
typedef enum { LED_ON, LED_OFF, LED_UNDEFINED } diode_state_t;

/* -------------------------------------------------------
 * FreeRTOS Objects & Global Variables
 * ------------------------------------------------------- */
SemaphoreHandle_t xGuardMutex;
SemaphoreHandle_t xAdcBinarySem;

QueueHandle_t xAdcQueue, xCh0Mailbox, xCh1Mailbox, xQueueT2, xDisplayMailbox;
TimerHandle_t xTimerAdc;
TaskHandle_t  xTask3Handle, xDiodeHandle;

volatile uint8_t channel = 0;

/* Global state variables - Guarded by Mutex */
volatile uint16_t limit = 100;
volatile diode_state_t led3Status = LED_OFF;
volatile diode_state_t led4Status = LED_OFF;

/* -------------------------------------------------------
 * Forward Declarations
 * ------------------------------------------------------- */
static void xTask1       ( void *pvParameters );
static void xTask2       ( void *pvParameters );
static void xTask3       ( void *pvParameters );
static void xDiodeTask   ( void *pvParameters );
static void xDisplayTask ( void *pvParameters );
static void prvSetupHardware( void );
static void prvAdcConvTimer( TimerHandle_t xTimer );

void sendViaUart(char string[]);
uint16_t update_channel_average(uint16_t *meanVals, uint16_t adcValue, uint16_t* sampleCount);
void processAndSend(QueueHandle_t mailbox, uint16_t* meanVals, uint16_t adcValue, uint16_t* sampleCount);

void update_limit(char* meanStringVal, uint16_t volatile * currentLimit);
action_t decideAction(char c, char* meanStringVal, uint16_t* ctr, uint8_t* carriagePassed);
void reportDiodeStatus(diode_state_t led3Status, diode_state_t led4Status);
void notifyLed(uint16_t limitVal, volatile diode_state_t* led3Stat, 
    volatile diode_state_t* led4Stat, uint8_t activeChannel );

/* -------------------------------------------------------
 * Mutex Protected functions for thread-safe behaviour 
 * ------------------------------------------------------- */

/*write in global parameters*/
void setDiodeAndLimit(diode_state_t led3, diode_state_t led4, uint16_t newLimit) {
    if (xSemaphoreTake(xGuardMutex, portMAX_DELAY) == pdTRUE) {
        led3Status = led3;
        led4Status = led4;
        limit = newLimit;
        xSemaphoreGive(xGuardMutex);
    }
}

/*read global parameters*/
void getDiodeAndLimit(diode_state_t *led3, diode_state_t *led4, uint16_t *currentLimit) {
    if (xSemaphoreTake(xGuardMutex, portMAX_DELAY) == pdTRUE) {
        *led3 = led3Status;
        *led4 = led4Status;
        *currentLimit = limit;
        xSemaphoreGive(xGuardMutex);
    }
}

/* -------------------------------------------------------
 * UART Helper
 * ------------------------------------------------------- */
void sendViaUart(char string[]){
    char *tmp = string;
    while(*tmp != 0){
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *tmp;
        tmp      += 1;
    }
}

/* -------------------------------------------------------
 * ADC Average Calculations
 * ------------------------------------------------------- */
uint16_t update_channel_average(uint16_t *meanVals, uint16_t adcValue, uint16_t* sampleCount) {
    uint16_t i;
    for (i = CONV_SAMPLE_SIZE - 1; i > 0; i--) {
        meanVals[i] = meanVals[i - 1];
    }
    meanVals[0] = adcValue; 
    if(*sampleCount < CONV_SAMPLE_SIZE){(*sampleCount)++;}

    return (meanVals[0] + meanVals[1] + meanVals[2] + meanVals[3]) / (*sampleCount);
}

void processAndSend(QueueHandle_t mailbox, uint16_t* meanVals, uint16_t adcValue, uint16_t* sampleCount) {
    uint16_t meanVal = update_channel_average(meanVals, adcValue, sampleCount);
    xQueueOverwrite(mailbox, &meanVal);
}

/* -------------------------------------------------------
 * xTask2 Helper Functions
 * ------------------------------------------------------- */
void update_limit(char* meanStringVal, uint16_t volatile * currentLimit) {
    if (meanStringVal[1] == '1') {
        if (*currentLimit < 0xFFF) (*currentLimit)++;
    }
    else if (meanStringVal[1] == '2') {
        if (*currentLimit > 0) (*currentLimit)--;
    }
}

action_t decideAction(char c, char* meanStringVal, uint16_t* ctr, uint8_t* carriagePassed) {
    if (c == '\r') {
        *carriagePassed = 1;
        return ACTION_UNDEFINED;
    }

    if (*carriagePassed == 1 && c == '\n') {
        meanStringVal[*ctr] = '\0'; 
        *ctr = 0;
        *carriagePassed = 0;

        if (meanStringVal[0] >= '0' && meanStringVal[0] <= '9') {
            return ACTION_UART_VAL_SENT;
        }
        else if (meanStringVal[0] == 'S' || meanStringVal[0] == 's') {
            return ACTION_SW_PRESSED;
        }
        return ACTION_UNDEFINED;
    }

    if (*carriagePassed == 1) {
        *carriagePassed = 0;
    }

    if (*ctr < NUM_OF_CHARS - 1) {
        meanStringVal[(*ctr)++] = c;
    }
    return ACTION_UNDEFINED;
}

void reportDiodeStatus(diode_state_t led3Status, diode_state_t led4Status) {
    static char uartBuf[NUM_OF_CHARS]; 

    if(led3Status == LED_ON && led4Status == LED_OFF){
        snprintf(uartBuf, sizeof(uartBuf), "L3:ON L4:OFF\r\n");
    }
    else if(led3Status == LED_ON && led4Status == LED_ON){
        snprintf(uartBuf, sizeof(uartBuf), "L3:ON L4:ON\r\n");
    }
    else if(led3Status == LED_OFF && led4Status == LED_OFF){
        snprintf(uartBuf, sizeof(uartBuf), "L3:OFF L4:OFF\r\n");
    }
    else if(led3Status == LED_OFF && led4Status == LED_ON){
        snprintf(uartBuf, sizeof(uartBuf), "L3:OFF L4:ON\r\n");
    }
    sendViaUart(uartBuf);
}

void notifyLed(uint16_t limitVal, volatile diode_state_t* led3Stat, 
    volatile diode_state_t* led4Stat, uint8_t activeChannel){
    uint16_t meanVal0 = 0, meanVal1 = 0;

    if (activeChannel == 0) {
        if (xQueuePeek(xCh0Mailbox, &meanVal0, 0) == pdPASS) {
            if (meanVal0 < limitVal) {
                xTaskNotify(xDiodeHandle, LED3_ON, eSetBits);
                *led3Stat = LED_ON;
            } else {
                xTaskNotify(xDiodeHandle, LEDS_OFF, eSetBits);
                *led3Stat = LED_OFF;
            }
        }
    }
    else {
        if (xQueuePeek(xCh1Mailbox, &meanVal1, 0) == pdPASS) {
            if (meanVal1 < limitVal) {
                xTaskNotify(xDiodeHandle, LED4_ON, eSetBits);
                *led4Stat = LED_ON;
            } else {
                xTaskNotify(xDiodeHandle, LEDS_OFF, eSetBits);
                *led4Stat = LED_OFF;
            }
        }
    }
}

/* -------------------------------------------------------
 * FreeRTOS Tasks Implementation
 * ------------------------------------------------------- */
static void xDiodeTask(void *pvParameters){
    uint32_t notifyValue;
    for( ;; ){
        xTaskNotifyWait(0, ULONG_MAX, &notifyValue, portMAX_DELAY);

        halCLR_LED(LED3);
        halCLR_LED(LED4);

        if(notifyValue & LEDS_OFF){ continue; }
        if(notifyValue & LED3_ON){ halSET_LED(LED3); }
        if(notifyValue & LED4_ON){ halSET_LED(LED4); }
    }
}

static void xTask1(void *pvParameters){
    uint16_t adcValue, sampleCount0 = 0, sampleCount1 = 0, convValue;
    uint8_t  activeChannel, scaledVal;
    diode_state_t led3Stat, led4Stat;
    uint16_t localLimit;

    uint16_t meanValsCh0[CONV_SAMPLE_SIZE] = {0};
    uint16_t meanValsCh1[CONV_SAMPLE_SIZE] = {0};

    for(;;){
        xSemaphoreTake(xAdcBinarySem, portMAX_DELAY);
        
        if(xQueueReceive(xAdcQueue, &adcValue, 0) == pdTRUE) {
            activeChannel = (adcValue >> 12) & 0x01;
            convValue     = adcValue & 0x0FFF;
            
            /* Fixed point scaling from 0% to 99% */
            scaledVal = (uint8_t)(((uint32_t)convValue * COEFF_SCALE) >> 16);
            
            //scaledVal = (uint8_t)(((uint32_t)convValue * 100) / 4096);
            xQueueOverwrite(xDisplayMailbox, &scaledVal);

            if(activeChannel == 0){
                processAndSend(xCh0Mailbox, meanValsCh0, convValue , &sampleCount0);
            }
            else{
                processAndSend(xCh1Mailbox, meanValsCh1, convValue , &sampleCount1);
            }

            /* Read state safely through the shared mutex */
            getDiodeAndLimit(&led3Stat, &led4Stat, &localLimit);
            
            notifyLed(localLimit, &led3Stat, &led4Stat, activeChannel);
            
            /* Save mutated state back safely */
            setDiodeAndLimit(led3Stat, led4Stat, localLimit);
        }
    }
}

static void xTask2(void *pvParameters){
    static char meanStringVal[NUM_OF_CHARS]; 
    char     c;

    uint16_t ctr            = 0;
    uint16_t parsedVal = 0, i = 0;
    uint8_t  carriagePassed = 0, localChannel;
    diode_state_t led3Stat, led4Stat;
    uint16_t localLimit;
    action_t action;

    for( ;; ){
        action = ACTION_UNDEFINED;
        xQueueReceive(xQueueT2, &c, portMAX_DELAY);
        action = decideAction(c, meanStringVal, &ctr, &carriagePassed);

        switch(action){
            case ACTION_UNDEFINED:
                continue;

            case ACTION_SW_PRESSED:
                /* Mutex-protected read */
                getDiodeAndLimit(&led3Stat, &led4Stat, &localLimit);
                
                update_limit(meanStringVal, &localLimit);
                
                /* Mutex-protected write */
                setDiodeAndLimit(led3Stat, led4Stat, localLimit);
                
                reportDiodeStatus(led3Stat, led4Stat);
                memset(meanStringVal, 0, NUM_OF_CHARS);
                break;
                
            case ACTION_UART_VAL_SENT:
                i = 0;
                parsedVal = 0;
                
                /*extract uart limit value*/
                while (meanStringVal[i] != '\0') {
                    if (meanStringVal[i] >= '0' && meanStringVal[i] <= '9') {
                        parsedVal = (parsedVal * 10) + (meanStringVal[i] - '0');
                    }
                    i++;
                }

                /* Synchronize data before applying new UART limit */
                getDiodeAndLimit(&led3Stat, &led4Stat, &localLimit);
                
                localLimit = parsedVal; 
                localChannel = channel;
                
                /* Instantly refresh diode states using the new limit */
                notifyLed(localLimit, &led3Stat, &led4Stat, localChannel); 
                
                /*  write back to memory */
                setDiodeAndLimit(led3Stat, led4Stat, localLimit);
                
                memset(meanStringVal, 0, NUM_OF_CHARS);
                break;

            default:
                break;
        }
    }
}

static void xTask3(void *pvParameters){
    uint32_t  i;
    uint8_t   currentButtonState  = 1;
    static char taster[NUM_OF_CHARS]; 

    for( ;; ){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        /* Software debounce delay */
        for(i = 0; i < 1000; i++);
        //vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_PERIOD));

        currentButtonState = ((P2IN & 0x02) >> 1);
        if(currentButtonState == 0){
            strcpy(taster, "S1\r\n");
            for(i = 0; i < strlen(taster); i++){ xQueueSendToBack(xQueueT2, &taster[i], portMAX_DELAY); }
        }

        currentButtonState = ((P1IN & 0x02) >> 1);
        if(currentButtonState == 0){
            strcpy(taster, "S2\r\n");
            for(i = 0; i < strlen(taster); i++){ xQueueSendToBack(xQueueT2, &taster[i], portMAX_DELAY); }
        }
    }
}

static void xDisplayTask( void *pvParameters )
{
    uint8_t         NewValueToShow      = 0;
    uint8_t         digitLow = 0, digitHigh = 0;
    for ( ;; )
    {
        if(xQueueReceive(xDisplayMailbox, &NewValueToShow, 0) == pdTRUE){
            digitHigh = NewValueToShow / 10;
            digitLow = NewValueToShow - digitHigh*10;
        }
        HAL_7SEG_DISPLAY_1_ON;
        HAL_7SEG_DISPLAY_2_OFF;
        vHAL7SEGWriteDigit(digitLow);
        vTaskDelay(pdMS_TO_TICKS(5));

        HAL_7SEG_DISPLAY_2_ON;
        HAL_7SEG_DISPLAY_1_OFF;
        vHAL7SEGWriteDigit(digitHigh);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

/* -------------------------------------------------------
 * Hardware Timers & Main
 * ------------------------------------------------------- */
static void prvAdcConvTimer( TimerHandle_t xTimer ){
    channel = (channel == 0) ? 1 : 0;
    
    ADC12CTL0 &= ~ADC12ENC;
    switch(channel){
        case 0:
            ADC12MCTL0 &= ~ADC12INCH_1;
            ADC12MCTL0 |=  ADC12INCH_0;
            break;
        case 1:
            ADC12MCTL0 &= ~ADC12INCH_0;
            ADC12MCTL0 |=  ADC12INCH_1;
            break;
    }
    ADC12CTL0 |= ADC12ENC | ADC12SC;
}

void main( void )
{
    prvSetupHardware();

    /* create timer */
    xTimerAdc = xTimerCreate("Adc Timer", ADC_CONV_PERIOD, pdTRUE, NULL, prvAdcConvTimer);

    /* crete Objects */
    xAdcQueue       = xQueueCreate(QUEUE_SIZE,          sizeof(uint16_t));
    xQueueT2        = xQueueCreate(50,                  sizeof(char));
    xCh0Mailbox     = xQueueCreate(QUEUE_MAILBOX_SIZE,  sizeof(uint16_t));
    xCh1Mailbox     = xQueueCreate(QUEUE_MAILBOX_SIZE,  sizeof(uint16_t));
    xDisplayMailbox = xQueueCreate(QUEUE_MAILBOX_SIZE,  sizeof(uint8_t));

    xAdcBinarySem = xSemaphoreCreateBinary();
    xGuardMutex   = xSemaphoreCreateMutex();

    /* create tasks */
    xTaskCreate(xTask1,       "Task1",        configMINIMAL_STACK_SIZE + 40,  NULL, TASK_1_PRIO,       NULL         );
    xTaskCreate(xTask2,       "Task2",        configMINIMAL_STACK_SIZE + 140, NULL, TASK_2_PRIO,       NULL         );
    xTaskCreate(xTask3,       "Task3",        configMINIMAL_STACK_SIZE + 60,  NULL, TASK_3_PRIO,       &xTask3Handle );
    xTaskCreate(xDiodeTask,   "DiodeTask",    configMINIMAL_STACK_SIZE,       NULL, DIODE_TASK_PRIO,   &xDiodeHandle);
    xTaskCreate(xDisplayTask, "DisplayTask",  configMINIMAL_STACK_SIZE + 20,  NULL, DISP_TASK_PRIO,    NULL);

    /*start timer*/
    xTimerStart(xTimerAdc, portMAX_DELAY);
    
    vTaskStartScheduler();

    for( ;; );
}

/*initialize hardware*/
static void prvSetupHardware( void )
{
    taskDISABLE_INTERRUPTS();

    WDTCTL = WDTPW + WDTHOLD;

    hal430SetSystemClock( configCPU_CLOCK_HZ, configLFXT_CLOCK_HZ );

    /*initialize Sw1*/
    P1DIR &= ~0x02;
    P1REN |=  0x02;
    P1OUT |=  0x02;
    P1IE  |=  0x02;
    P1IFG &= ~0x02;
    P1IES |=  0x02;


    /*initialize Sw2*/
    P2DIR &= ~0x02;
    P2REN |=  0x02;
    P2OUT |=  0x02;
    P2IE  |=  0x02;
    P2IFG &= ~0x02;
    P2IES |=  0x02;

    /*initialize uart*/
    P4SEL    |= BIT4 + BIT5;
    UCA1CTL1 |= UCSWRST;
    UCA1CTL1 |= UCSSEL_2;
    UCA1BRW   = 1041;
    UCA1MCTL |= UCBRS_6 + UCBRF_0;
    UCA1CTL1 &= ~UCSWRST;
    UCA1IE   |= UCRXIE;

    /*initialize adc conversion*/
    ADC12CTL0  = ADC12SHT02 + ADC12ON;
    ADC12CTL1  = ADC12SHP;
    ADC12IE    = 0x01;
    ADC12MCTL0 |= ADC12INCH_0;
    ADC12CTL0  |= ADC12ENC;
    P6SEL      |= 0x03;

    vHALInitLED();
    vHAL7SEGInit();
}

/* -------------------------------------------------------
 * ISR Vectors
 * ------------------------------------------------------- */
void __attribute__ ( ( interrupt( PORT1_VECTOR ) ) ) vPORT1ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if((P1IFG & 0x02) == 0x02){
        vTaskNotifyGiveFromISR(xTask3Handle, &xHigherPriorityTaskWoken);
    }
    P1IFG &= ~0x02;
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void __attribute__ ( ( interrupt( PORT2_VECTOR ) ) ) vPORT2ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if((P2IFG & 0x02) == 0x02){
        vTaskNotifyGiveFromISR(xTask3Handle, &xHigherPriorityTaskWoken);
    }
    P2IFG &= ~0x02;
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void __attribute__ ( ( interrupt( USCI_A1_VECTOR ) ) ) vUARTISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    //char rxByte; 

    switch(UCA1IV)
    {
        case 0: break;
        case 2:
            //rxByte = UCA1RXBUF; 
            xQueueSendToBackFromISR(xQueueT2, &UCA1RXBUF, &xHigherPriorityTaskWoken);
            break;
        case 4: break;
        default: break;
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void __attribute__ ( ( interrupt( ADC12_VECTOR ) ) ) vADC12ISR( void )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint16_t   temp;
    
    switch(__even_in_range(ADC12IV, 34))
    {
        case  6:
            channel = (ADC12MCTL0 & ADC12INCH_1) ? 1 : 0;     
            temp = ADC12MEM0 | (channel << 12);
            xQueueSendToBackFromISR(xAdcQueue, &temp, &xHigherPriorityTaskWoken);
            xSemaphoreGiveFromISR(xAdcBinarySem, &xHigherPriorityTaskWoken);
            break;
        default: break;
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}