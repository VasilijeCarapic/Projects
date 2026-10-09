/**
 ******************************************************************************
 * @file    device_config.h
 *
 * @brief   Global configuration state types and allocation-pool
 *          declarations for the device, ADC streams, status links and
 *          endpoint links, implemented in device_config.c. Backs the
 *          "device"/"stream"/"slink"/"eplink" command family handled by
 *          configurationControl.c.
 *
 * @authors Vasilije Čarapić, Veljko Sokolov
 * @date    August, 2026
 ******************************************************************************
 */
#ifndef STREAM_CONFIG_H
#define STREAM_CONFIG_H

#include <stdint.h>
#include <string.h>

#define SLINK_CONNECTIONS_MAX_NUMBER    2
#define ELINK_CONNECTIONS_MAX_NUMBER    3
#define SSTREAM_CONNECTIONS_MAX_NUMBER  3
#define DEVICE_NAME_MAX_LEN             32

/**
 * @brief   Device-wide (not stream-scoped) configuration state: name,
 *          DAC, load/battery/power-path switches and charger settings.
 */
typedef struct{
    char device_name[DEVICE_NAME_MAX_LEN];
    int dac_enabled;
    int dac_value;
    int load_enabled;
    int bat_enabled;
    int ppath_enabled;
    int charger_enabled;
    int charger_current;
    int charger_termcurrent;
    float charger_termvoltage;
} Device_Config_t;

/**
 * @brief   Per-stream ADC acquisition configuration, allocated from the
 *          ::streams pool via STREAMCFG_AllocateStream() and addressed by index
 *          ("sid").
 */
typedef struct{
    uint8_t in_use;
    int adc_resolution;
    int adc_clk_div;
    int adc_stime;
    uint32_t adc_period;
    uint32_t adc_prescaler;
    int adc_chavrratio;
    int adc_voffset;
    int adc_coffset;
    int adc_samplesno;
    int is_streaming;
} Stream_Config_t;


/**
 * @brief   Status-link slot state, allocated from the ::slinks pool via
 *          STREAMCFG_AllocateSlink().
 */
typedef struct {
    uint8_t in_use;
} StatusLink_Config_t;

/**
 * @brief   Endpoint-link slot state, allocated from the ::eplinks pool via
 *          STREAMCFG_AllocateEplink().
 */
typedef struct {
    uint8_t in_use;
} EpLink_Config_t;

/* declarations */
extern Device_Config_t device_config;
extern Stream_Config_t streams[SSTREAM_CONNECTIONS_MAX_NUMBER];
extern StatusLink_Config_t slinks[SLINK_CONNECTIONS_MAX_NUMBER];
extern EpLink_Config_t eplinks[ELINK_CONNECTIONS_MAX_NUMBER];


/**
 * @brief   Full configuration reset: device config plus all stream/slink/
 *          eplink pools.
 * @retval  None
 */
void STREAMCFG_ConfigurationInit(void);

/**
 * @brief   Resets ::device_config to its default values.
 * @retval  None
 */
void STREAMCFG_InitDevice(void);

/**
 * @brief   Resets every entry in the ::streams pool to its default,
 *          unused state.
 * @retval  None
 */
void STREAMCFG_InitStreams(void);

/**
 * @brief   Marks every entry in the ::slinks pool as unused.
 * @retval  None
 */
void STREAMCFG_InitSLink(void);

/**
 * @brief   Marks every entry in the ::eplinks pool as unused.
 * @retval  None
 */
void STREAMCFG_InitEpLink(void);

/**
 * @brief   Reserves the first free slot in the ::streams pool.
 * @retval  Allocated stream index, or -1 if the pool is full.
 */
int STREAMCFG_AllocateStream(void);

/**
 * @brief   Reserves the first free slot in the ::slinks pool.
 * @retval  Allocated status-link index, or -1 if the pool is full.
 */
int STREAMCFG_AllocateSlink(void);

/**
 * @brief   Reserves the first free slot in the ::eplinks pool.
 * @retval  Allocated endpoint-link index, or -1 if the pool is full.
 */
int STREAMCFG_AllocateEplink(void);


#endif
