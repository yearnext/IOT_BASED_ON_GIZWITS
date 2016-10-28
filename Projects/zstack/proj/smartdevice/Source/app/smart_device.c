 /**
 ******************************************************************************
  * @file       smart_device.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      smart_device 源文件
  * @par        工作平台                                  
  *                  CC2530
  * @par        工作频率                                  
  *                  32MHz
  * @par        编译平台									                         
  * 				 IAR
 ******************************************************************************
  * @note
  * 1.XXXXX                  						                     
 ******************************************************************************
 */

/**
 * @defgroup smartlight模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZGlobals.h"
#include <string.h>
#include "NLMEDE.h"
#include "smart_device.h"
#include "onboard.h"
#include "hal_led.h"
#include "hal_timer.h"
#include "app_save.h"
#include "OSAL_Nv.h"
#include "hal_key.h"
#include "devicelist.h"
#include "MT.h"
#include "MT_UART.h"
#include "hal_uart.h"
#include "myprotocol.h"
#include "timer_config.h"
#include "bsp_key.h"
#include "app_time.h"

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_LIGHT)
#include "bsp_light.h"
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_SOCKET)
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_CURTAIN)
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_HT_SENSOR)
#include "hal_dht11.h"
#else
#endif
     
#if defined(USE_GIZWITS_MOD)
#include "gizwits_protocol.h"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
// 机智云数据点
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
dataPoint_t currentDataPoint;
#endif

/* Private define ------------------------------------------------------------*/
// 输出调试信息配置
#define USE_SMARTDEVICE_DEBUG 1

#if USE_SMARTDEVICE_DEBUG
    #if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
        #define DEVICE_LOG(n) HalUARTWrite(1,n,sizeof(n))  ///<运行日志打印
    #else
        #define DEVICE_LOG(n) HalUARTWrite(0,n,sizeof(n))  ///<运行日志打印
    #endif
#else
    #define DEVICE_LOG(n)
#endif
     
/** 设备定时时间 */
#if defined (USE_GIZWITS_MOD)
    #define DEVICE_LIST_TIME            (30000)
#else
    #define DEVICE_LIST_TIME            (3000)
#endif
/** 设备定时事件 */
#define DEVICE_LIST_TIMER_EVEN          (0x0002)

/** 设备后台运行时间 */
#define DEVICE_BACKSTAGE_TIME           (10)
/** 设备定时器事件 */
#define DEVICE_TIMER_EVEN               (0x0008)

/** 定时器计数值 */
#define TIMER_20MS_COUNT  (2)
#define TIMER_50MS_COUNT  (5)
#define TIMER_100MS_COUNT (10)
#define TIMER_300MS_COUNT (30)

/** Smart Device 通讯状态指示灯 */
#define SMARTDEVICE_LED_DISCONNED_STATE (0x00)
#define SMARTDEVICE_LED_CONNED_STATE    (0x01)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** 任务ID */
static byte SmartDevice_TaskID;

/** 设备状态 */
static devStates_t SmartDevice_NwkState;

/* functions statement -------------------------------------------------------*/
void SmartDevice_Key_Headler( uint8 keys, uint8 state );
void ZDO_STATE_CHANGE_CB( devStates_t status );
void SmartDevice_CommLED_Control( uint8 state );
void device_timer_cb( void );

/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       OSAL任务初始化函数
 * @param       [in/out]  task_id    任务ID
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void SmartDevice_Init( byte task_id )
{
    SmartDevice_TaskID = task_id;
    SmartDevice_NwkState = DEV_INIT;
    
    MT_UartInit();
    MT_UartRegisterTaskID(SmartDevice_TaskID);
    
    myprotocol_init( &SmartDevice_TaskID );
    
    hal_key_init();
    
    app_time_init();

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
    gizwitsInit();
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_LIGHT)
    bsp_light_init();
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_SOCKET)
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_CURTAIN)
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_HT_SENSOR)
#endif
    
    osal_start_timerEx( SmartDevice_TaskID, 
                        DEVICE_TIMER_EVEN, 
                        DEVICE_BACKSTAGE_TIME );

    SmartDevice_CommLED_Control(SMARTDEVICE_LED_DISCONNED_STATE);
    
    DEVICE_LOG("Smart device init finish!\n");
}

/**
 *******************************************************************************
 * @brief       SmartDevice事件处理
 * @param       [in]   task_id    任务ID
                       events     事件
 * @return      [out]  uint16     0
 * @note        None
 *******************************************************************************
 */
uint16 SamrtDevice_ProcessEven( uint8 task_id, uint16 events )
{
    afIncomingMSGPacket_t *MSGpkt;
    
    (void)task_id;
        
    if( events & SYS_EVENT_MSG )
    {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(SmartDevice_TaskID);
        while( MSGpkt )
        {
            switch( MSGpkt->hdr.event )
            {
                case AF_INCOMING_MSG_CMD:
                    SmartDevice_Message_Headler(MSGpkt);
                    break;
                case ZDO_STATE_CHANGE:
                    SmartDevice_NwkState = (devStates_t)(MSGpkt->hdr.status);
                    ZDO_STATE_CHANGE_CB(SmartDevice_NwkState);
                    break;
                
                default:
                    break;
            }
            
            // Release the memory
            osal_msg_deallocate( (uint8 *)MSGpkt );
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(SmartDevice_TaskID);
        }
        
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    
    if( events & DEVICE_LIST_TIMER_EVEN )
    {   
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
        if( Del_ZombieDevice_ForList() == true )
        {
            MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,NULL,create_devicelist_update_packet,NULL);
        }
        
        Del_DeviceTickCount();
        DEVICE_LOG("COORD FRESH DEVICE LIST!\n");
#else
        MYPROTOCO_S2H_MSG_SEND(create_tick_packet,NULL);
        DEVICE_LOG("DEVICE SEND TICK PACKET!\n");
#endif

        osal_start_timerEx( SmartDevice_TaskID, 
                            DEVICE_LIST_TIMER_EVEN, 
                            DEVICE_LIST_TIME );
        
        return (events ^ DEVICE_LIST_TIMER_EVEN);
    }
    
    if( events & DEVICE_TIMER_EVEN )
    {
        device_timer_cb();
        
        osal_start_timerEx( SmartDevice_TaskID, 
                            DEVICE_TIMER_EVEN, 
                            DEVICE_BACKSTAGE_TIME );
        
        return (events ^ DEVICE_TIMER_EVEN);
    }
    
    // Discard unknown events
    return 0;
}
 
/**
 *******************************************************************************
 * @brief       ZDO状态改变回调哈数
 * @param       [in/out]  status    设备状态
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void ZDO_STATE_CHANGE_CB( devStates_t status )
{
    switch( status )
    {
        case DEV_ROUTER:
            DEVICE_LOG("I am Router Device!\n");
            SmartDevice_CommLED_Control(SMARTDEVICE_LED_CONNED_STATE);
            osal_start_timerEx( SmartDevice_TaskID, 
                                DEVICE_LIST_TIMER_EVEN, 
                                DEVICE_LIST_TIME );
                break;
        case DEV_END_DEVICE:
            DEVICE_LOG("I am End Device!\n");
            SmartDevice_CommLED_Control(SMARTDEVICE_LED_CONNED_STATE);
            osal_start_timerEx( SmartDevice_TaskID, 
                                DEVICE_LIST_TIMER_EVEN, 
                                DEVICE_LIST_TIME );
                break;
        case DEV_ZB_COORD:
            DEVICE_LOG("I am Coord Device!\n");
            SmartDevice_CommLED_Control(SMARTDEVICE_LED_CONNED_STATE);
            osal_start_timerEx( SmartDevice_TaskID, 
                                DEVICE_LIST_TIMER_EVEN, 
                                DEVICE_LIST_TIME );
                break;
        case DEV_NWK_DISC:
            DEVICE_LOG("Discovering PAN's to join!\n");
            osal_stop_timerEx( SmartDevice_TaskID, DEVICE_LIST_TIMER_EVEN);
            break;
        default:
            break;
    }
}

/**
 *******************************************************************************
 * @brief       Smart Device 通讯状态指示灯控制
 * @param       [in/out]  state    通讯状态
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void SmartDevice_CommLED_Control( uint8 state )
{
    if( state == SMARTDEVICE_LED_DISCONNED_STATE )
    {
        HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
        HalLedSet(HAL_LED_2,HAL_LED_MODE_OFF);
    }
    else if( state == SMARTDEVICE_LED_CONNED_STATE )
    {
        HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
        HalLedSet(HAL_LED_2,HAL_LED_MODE_ON);
    }
    else
    {
        HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
        HalLedSet(HAL_LED_2,HAL_LED_MODE_OFF);
    }
}

/**
 *******************************************************************************
 * @brief       设备定时器回调函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        执行周期为10MS
 *******************************************************************************
 */
void device_timer_cb( void )
{
    static uint8 timer_20ms  = 0;
    static uint8 timer_50ms  = 0;
    static uint8 timer_100ms = 0;
    static uint8 timer_300ms = 0;
    
    if( ++timer_20ms >= TIMER_20MS_COUNT )
    {
        key_scan();
        timer_20ms = 0;
    }
    
    if( ++timer_50ms >= TIMER_50MS_COUNT )
    {
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
        gizTimer50Ms();
#endif
        key_handler();
        timer_50ms = 0;
    }
    
    if( ++timer_100ms >= TIMER_100MS_COUNT )
    {
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
        gizwitsHandle(&currentDataPoint);
#endif
        timer_100ms = 0;
    }
    
    if( ++timer_300ms >= TIMER_300MS_COUNT )
    {
        app_time_update();
        
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_LIGHT)
        light_working_headler();
#elif (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_HT_SENSOR)
#else
#endif  
        timer_20ms = 0;
    }
}

/**
 *******************************************************************************
 * @brief       SmartDevice按键处理
 * @param       [in]   message    按键信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void key_switch_handler( key_message_t message )
{
    switch (message)
    {
		case KEY_MESSAGE_PRESS_EDGE:
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_LIGHT)
        light_switch_headler();
#endif
			break;
		default:
			break;
    }
}

/** @}*/     /* smartlight模块 */

/**********************************END OF FILE*********************************/
