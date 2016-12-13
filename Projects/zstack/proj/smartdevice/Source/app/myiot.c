 /**
 ******************************************************************************
  * @file       myIot.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      myIot 源文件
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
 * @defgroup myiot模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "myiot.h"
//#include "ZComDef.h"
//#include "OSAL.h"
//#include "AF.h"
#include "ZDApp.h"
//#include "ZGlobals.h"
//#include <string.h>
//#include "NLMEDE.h"
//#include "onboard.h"
#include "hal_led.h"
//#include "hal_timer.h"
//#include "app_save.h"
//#include "OSAL_Nv.h"
//#include "hal_key.h"
//#include "devicelist.h"
#include "MT_UART.h"
#include "myprotocol.h"

//#include "mcu_timer.h"
#include "bsp_key.h"
#include "app_time.h"

#if MYPROTOCOL_DEVICE_IS_COORD
#include "bsp_coord.h"
#include "devicelist.h"
#include "gizwits_protocol.h"

#elif MYPROTOCOL_DEVICE_IS_LIGHT
#include "bsp_light.h"

#elif MYPROTOCOL_DEVICE_IS_SOCKET
#include "bsp_socket.h"

#elif MYPROTOCOL_DEVICE_IS_CURTAIN
#elif MYPROTOCOL_DEVICE_IS_HT_SENSOR
#include "bsp_htsensor.h"

#else
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
//// 机智云数据点
//#if MYPROTOCOL_DEVICE_IS_COORD
//dataPoint_t currentDataPoint;
//#endif

/* Private define ------------------------------------------------------------*/
// 输出调试信息配置
#define USE_SMARTDEVICE_DEBUG 1

#if USE_SMARTDEVICE_DEBUG
    #define DEVICE_LOG(n) MYPROTOCOL_LOG(n)  ///<运行日志打印
#else
    #define DEVICE_LOG(n)
#endif
     
/** 设备定时时间 */
#if MYPROTOCOL_DEVICE_IS_COORD
    #define DEVICE_LIST_TIME            (15000)
#else
    #define DEVICE_LIST_TIME            (15000)
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
#define TIMER_350MS_COUNT (30)
//#define TIMER_1MIN_COUNT  (600)
#define TIMER_1MIN_COUNT  (6000)

/** Smart Device 通讯状态指示灯 */
#define MYIOT_DISCONNED_STATE (0x00)
#define MYIOT_CONNED_STATE    (0x01)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** 任务ID */
static byte myIotTaskID;

/** 设备状态 */
//static devStates_t myIotNwkState;

/* functions statement -------------------------------------------------------*/
void ZDO_STATE_CHANGE_CB( devStates_t status );
void myiotCommLedControl( uint8 state );
void deviceTimerCallBack( void );

/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       OSAL任务初始化函数
 * @param       [in/out]  task_id    任务ID
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myIotInit( byte task_id )
{
    myIotTaskID = task_id;
//    myIotNwkState = DEV_INIT;
    
    MT_UartInit();
    MT_UartRegisterTaskID(myIotTaskID);
    
    MyprotocolInit( &myIotTaskID );
    
    bsp_key_init();
    
    app_time_init();

#if MYPROTOCOL_DEVICE_IS_COORD
    bspCoordInit();
#elif MYPROTOCOL_DEVICE_IS_LIGHT
    bsp_light_init();
#elif MYPROTOCOL_DEVICE_IS_SOCKET
    bsp_socket_init();
#elif MYPROTOCOL_DEVICE_IS_CURTAIN
#elif MYPROTOCOL_DEVICE_IS_HT_SENSOR
	ht_sensor_init();
#else
#endif
    
    osal_start_timerEx( myIotTaskID, 
                        DEVICE_TIMER_EVEN, 
                        DEVICE_BACKSTAGE_TIME );

    myiotCommLedControl(MYIOT_DISCONNED_STATE);
    
    DEVICE_LOG("Smart device init finish!\n");
}

/**
 *******************************************************************************
 * @brief       myiot事件处理
 * @param       [in]   task_id    任务ID
                       events     事件
 * @return      [out]  uint16     0
 * @note        None
 *******************************************************************************
 */
uint16 myIotProcessEven( uint8 task_id, uint16 events )
{
    afIncomingMSGPacket_t *MSGpkt;
    
    (void)task_id;
        
    if( events & SYS_EVENT_MSG )
    {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(myIotTaskID);
        while( MSGpkt )
        {
            switch( MSGpkt->hdr.event )
            {
                case AF_INCOMING_MSG_CMD:
                    MyprotocolD2DReceiveData((void *)MSGpkt->cmd.Data,coordMessageHandler);
//                    SmartDevice_Message_Handler(MSGpkt);
                    break;
                case ZDO_STATE_CHANGE:
//                    myIotNwkState = (devStates_t)(MSGpkt->hdr.status);
//                    ZDO_STATE_CHANGE_CB(myIotNwkState);
                    ZDO_STATE_CHANGE_CB((devStates_t)(MSGpkt->hdr.status));
                    break;
                default:
                    break;
            }          
            // Release the memory
            osal_msg_deallocate( (uint8 *)MSGpkt );
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(myIotTaskID);
        }
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    
    if( events & DEVICE_LIST_TIMER_EVEN )
    {   
#if MYPROTOCOL_DEVICE_IS_COORD
        if( allZombieDeviceClr() == true )
        {
            MyprotocolSendData(NULL,NULL, DeviceListChangePacket, MyprotocolD2WSendData);
        }
        
        allDeviceTickClr();
        DEVICE_LOG("COORD FRESH DEVICE LIST!\n");
#else
        MyprotocolSendData(NULL,&recPacket->device.mac, DeviceTickPacket, MyprotocolD2DSendData);
        DEVICE_LOG("DEVICE SEND TICK PACKET!\n");
#endif

        osal_start_timerEx( myIotTaskID, 
                            DEVICE_LIST_TIMER_EVEN, 
                            DEVICE_LIST_TIME );
        
        return (events ^ DEVICE_LIST_TIMER_EVEN);
    }
    
    if( events & DEVICE_TIMER_EVEN )
    {
        deviceTimerCallBack();
        
        osal_start_timerEx( myIotTaskID, 
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
            myiotCommLedControl(MYIOT_CONNED_STATE);
            osal_start_timerEx( myIotTaskID, 
                                DEVICE_LIST_TIMER_EVEN, 
                                DEVICE_LIST_TIME );
                break;
        case DEV_END_DEVICE:
            DEVICE_LOG("I am End Device!\n");
            myiotCommLedControl(MYIOT_CONNED_STATE);
            osal_start_timerEx( myIotTaskID, 
                                DEVICE_LIST_TIMER_EVEN, 
                                DEVICE_LIST_TIME );
                break;
        case DEV_ZB_COORD:
            DEVICE_LOG("I am Coord Device!\n");
            myiotCommLedControl(MYIOT_CONNED_STATE);
            osal_start_timerEx( myIotTaskID, 
                                DEVICE_LIST_TIMER_EVEN, 
                                DEVICE_LIST_TIME );
                break;
        case DEV_NWK_DISC:
            DEVICE_LOG("Discovering PAN's to join!\n");
            myiotCommLedControl(MYIOT_DISCONNED_STATE);
            osal_stop_timerEx( myIotTaskID, DEVICE_LIST_TIMER_EVEN);
            break;
        default:
            break;
    }
}

/**
 *******************************************************************************
 * @brief       myiot通讯状态指示灯控制
 * @param       [in/out]  state    通讯状态
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void myiotCommLedControl( uint8 state )
{
    if( state == MYIOT_DISCONNED_STATE )
    {
        HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
        HalLedSet(HAL_LED_2,HAL_LED_MODE_OFF);
    }
    else if( state == MYIOT_CONNED_STATE )
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
void deviceTimerCallBack( void )
{
    static uint8 timer_20ms  = 0;
    static uint8 timer_50ms  = 0;
    static uint8 timer_100ms = 0;
    static uint8 timer_350ms = 0;
    static uint16 timer_1min = 0;
    
    if( ++timer_20ms >= TIMER_20MS_COUNT )
    {
        key_scan();
        timer_20ms = 0;
    }
    
    if( ++timer_50ms >= TIMER_50MS_COUNT )
    {
#if MYPROTOCOL_DEVICE_IS_COORD
        gizTimer50Ms();
#endif
        key_handler();
        timer_50ms = 0;
    }
    
    if( ++timer_100ms >= TIMER_100MS_COUNT )
    {
#if MYPROTOCOL_DEVICE_IS_COORD
        gizwitsHandle();
#endif
        timer_100ms = 0;
    }
    
    if( ++timer_350ms >= TIMER_350MS_COUNT )
    {
#if MYPROTOCOL_DEVICE_IS_LIGHT
        app_time_update();
        light_working_handler();
#elif MYPROTOCOL_DEVICE_IS_SOCKET
        app_time_update();
        socket_working_handler();
#else
#endif  
        timer_350ms = 0;
    }
    
    if( ++timer_1min >= TIMER_1MIN_COUNT )
    {
#if MYPROTOCOL_DEVICE_IS_HT_SENSOR
		report_ht_sensor_data();
#endif
        timer_1min = 0;
    }
}

/** @}*/     /* smartlight模块 */

/**********************************END OF FILE*********************************/
