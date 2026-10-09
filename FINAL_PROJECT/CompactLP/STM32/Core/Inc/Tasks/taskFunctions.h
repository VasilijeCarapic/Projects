#ifndef TASK_FUNCTIONS_H
#define TASK_FUNCTIONS_H

#include <Communication/commandParser.h>

/* flagovi za constante i notifikaciju */
#define MSG_RECEIVED_FLAG       (1U << 0)
#define FLAG_HP_QUEUE           (1U << 1)
#define FLAG_LP_QUEUE           (1U << 2)
#define FLAG_SEN_SAMPLE         (1U << 3)
#define FLAG_SEN_STREAM_START   (1U << 4)
#define FLAG_SEN_STREAM_STOP    (1U << 5)

#define ULONG_MAX               0xFFFFFFFF


#define PARSER_TASK_PRIO        ( 10 )
#define SENSOR_TASK_PRIO        ( 20 )
#define UART_TX_TASK_PRIO       ( 30 )


extern TaskHandle_t ParserTaskHandle;
extern TaskHandle_t SensorTaskHandle;
extern TaskHandle_t UartTxTaskHandle;

/* Task functions */
void prvParserTaskFunction(void *pvParameter);
void prvSensorTaskFunction(void *pvParameter);
void prvUartTxTaskFunction(void *pvParameter);

#endif /* TASK_FUNCTIONS_H */
