/**
 ******************************************************************************
 * @file     Constants.h 
 *
 * @brief    Defines default network parameters(default IP, SSID, PASSWORD), as well 
 *           as the temperature and pressure parameters for measuring weather statistics 
 *
 * @authors  Veljko Sokolov, Vasilije Carapic 
 * @date     August 2026 
 ******************************************************************************
 */

/**
 * @defgroup NetParams Default Network Parameters 
 * @{
 */

#define DEFAULT_HTTP_PORT    80 
#define DEFAULT_TCP_PORT     8080
#define DEFAULT_SSID  "HUAWEI_B535_11C6"
#define DEFAULT_PASS  "6aLhEDnaG2F"   
/** @} */ 

/**
 * @defgroup Connection Parameters 
 * @{
 */

#define BAUD_RATE            921600
#define CONNECT_TIMEOUT_MS   500 
#define MAX_PAIRS            10

/** @} */ 

/**
 * @defgroup Links Max allowed connections Params 
 * 
 */
#define SSTREAM_CONNECTIONS_MAX_NUMBER  3
#define ELINK_CONNECTIONS_MAX_NUMBER    3
#define SLINK_CONNECTIONS_MAX_NUMBER    2
/** @} */ 

/**
 * @defgroup BinaryData Binary data maximum accepted length  
 * @{
 */

 #define MAX_BINARY_DATA_LENGTH  1024 /*!>  8 header + max samplesNo*2*2 (250 samples -> 1000)*/


/** @} */ 

/**
 * @defgroup WeatherParams Weather Limit parameters 
 * @{
 */

#define TEMP_LOW    10.0
#define TEMP_MEDIUM 30.0
#define TEMP_HIGH   50.0


#define PRESSURE_LOW    300.0
#define PRESSURE_MEDIUM 700.0
#define PRESSURE_HIGH   1100.0

/** @} */