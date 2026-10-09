/**
 ******************************************************************************
 * @file    device_config.c
 *
* @brief Global configuration, allocation pools, and slot-allocators
 *        for device, ADC streams, status links, and endpoint links.
 *        Used by configurationControl.c for configuration commands.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#include <Communication/streamConfig.h>

Device_Config_t device_config;
Stream_Config_t streams[SSTREAM_CONNECTIONS_MAX_NUMBER];


StatusLink_Config_t slinks[SLINK_CONNECTIONS_MAX_NUMBER];
EpLink_Config_t eplinks[ELINK_CONNECTIONS_MAX_NUMBER];

/**
 * @brief   Resets every entry in the ::streams pool to its default,
 *          unused ADC-stream configuration.
 * @retval  None
 */
void STREAMCFG_InitStreams(void){
    for(int i = 0; i < SSTREAM_CONNECTIONS_MAX_NUMBER; i++){
        streams[i].in_use = 0;
        streams[i].adc_resolution = 16;
        streams[i].adc_clk_div    = 2;
        streams[i].adc_stime      = 0;
        streams[i].adc_period     = 1000;
        streams[i].adc_prescaler  = 1;
        streams[i].adc_chavrratio = 1;
        streams[i].adc_voffset    = 0;
        streams[i].adc_coffset    = 0;
        streams[i].adc_samplesno  = 250;
        streams[i].is_streaming   = 0;
    }
}

// INICIJALIZACIJA NOVIH LINKOVA
/**
 * @brief   Marks every entry in the ::slinks pool as unused.
 * @retval  None
 */
void STREAMCFG_InitSLink(void){
    for(int i = 0; i < SLINK_CONNECTIONS_MAX_NUMBER; i++){
        slinks[i].in_use = 0;
    }
}
/**
 * @brief   Marks every entry in the ::eplinks pool as unused.
 * @retval  None
 */
void STREAMCFG_InitEpLink(void){
    for(int i = 0; i < ELINK_CONNECTIONS_MAX_NUMBER; i++){
        eplinks[i].in_use = 0;
    }
}

/**
 * @brief   Resets ::device_config to its default values: device name
 *          "default", DAC/load/battery/power-path disabled, and default
 *          charger current/termination-current/termination-voltage.
 * @retval  None
 */
void STREAMCFG_InitDevice(void){
    strncpy(device_config.device_name, "default", sizeof(device_config.device_name) - 1);
    device_config.device_name[sizeof(device_config.device_name) - 1] = '\0';

    device_config.dac_enabled = 0;
    device_config.dac_value   = 0;

    device_config.load_enabled  = 0;
    device_config.bat_enabled   = 0;
    device_config.ppath_enabled = 0;

    device_config.charger_enabled     = 0;
    device_config.charger_current     = 500;
    device_config.charger_termcurrent = 50;
    device_config.charger_termvoltage = 4.20f; // PROMENJENO SA 4200 NA FLOAT FORMAT
}

/**
 * @brief   Full configuration reset: re-initializes the device config and
 *          all stream/slink/eplink pools. Called at startup and on
 *          "device hello".
 * @retval  None
 */
void STREAMCFG_ConfigurationInit(void){
    STREAMCFG_InitDevice();
    STREAMCFG_InitStreams();
    STREAMCFG_InitSLink(); // UBACEN POZIV
    STREAMCFG_InitEpLink();// UBACEN POZIV
}

/**
 * @brief   Finds and reserves the first free slot in the ::streams pool.
 * @retval  Index of the newly allocated stream, or -1 if the pool is full.
 */
int STREAMCFG_AllocateStream(void){
    for(int i = 0; i < SSTREAM_CONNECTIONS_MAX_NUMBER; i++){
        if(streams[i].in_use == 0){
            streams[i].in_use = 1;
            return i;
        }
    }
    return -1;
}

// NOVE FUNKCIJE ZA DODELJIVANJE ID-ja
/**
 * @brief   Finds and reserves the first free slot in the ::slinks pool.
 * @retval  Index of the newly allocated status link, or -1 if the pool is
 *          full.
 */
int STREAMCFG_AllocateSlink(void){
    for(int i = 0; i < SLINK_CONNECTIONS_MAX_NUMBER; i++){
        if(slinks[i].in_use == 0){
            slinks[i].in_use = 1;
            return i;
        }
    }
    return -1;
}

/**
 * @brief   Finds and reserves the first free slot in the ::eplinks pool.
 * @retval  Index of the newly allocated endpoint link, or -1 if the pool
 *          is full.
 */
int STREAMCFG_AllocateEplink(void){
    for(int i = 0; i < ELINK_CONNECTIONS_MAX_NUMBER; i++){
        if(eplinks[i].in_use == 0){
            eplinks[i].in_use = 1;
            return i;
        }
    }
    return -1;
}
