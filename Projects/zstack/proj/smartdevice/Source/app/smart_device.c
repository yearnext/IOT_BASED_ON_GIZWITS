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
#include "nv_save.h"
#include "OSAL_Nv.h"
#include "hal_key.h"
#include "devicelist.h"
#include "MT.h"
#include "MT_UART.h"
#include "hal_uart.h"
#include "myprotocol.h"
#include "timer_config.h"

#if SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_COORD

#elif SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_LIGHT
#include "bsp_light.h"
#elif SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_SOCKET

#elif SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_CURTAIN

#elif SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_HT_SENSOR

#else

#endif
     
#if defined(USE_GIZWITS_MOD)
#include "gizwits_protocol.h"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
// 机智云数据点
#if defined ( USE_GIZWITS_MOD )
dataPoint_t currentDataPoint;
#endif

/* Private define ------------------------------------------------------------*/
// 输出调试信息配置
#define USE_SMARTDEVICE_DEBUG 1

#if USE_SMARTDEVICE_DEBUG
    #if defined ( USE_GIZWITS_MOD )
        #define DEVICE_LOG(n) HalUARTWrite(1,n,sizeof(n))  ///<运行日志打印
    #else
        #define DEVICE_LOG(n) HalUARTWrite(0,n,sizeof(n))  ///<运行日志打印
    #endif
#else
    #define DEVICE_LOG(n)
#endif
     
/** 设备定时时间 */
#define SMART_DEVICE_TIME       (3000)
/** 设备定时事件 */
#define SMART_DEVICE_TIMER_EVEN (0x0002)

/** 机智云事件处理时间 */
#define GIZWITS_HANDLER_TIME (50)
/** 清除僵尸设备时间 */
#define CLEAR_ZOMBIE_DEVICE_TIME (30000)

/** Smart Device 通讯状态指示灯 */
#define SMARTDEVICE_LED_DISCONNED_STATE (0x00)
#define SMARTDEVICE_LED_CONNED_STATE    (0x01)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** 任务ID */
static byte SmartDevice_TaskID;
/** 设备状态 */
static devStates_t SmartDevice_NwkState;

/* Private functions ---------------------------------------------------------*/
void SmartDevice_Key_Headler( uint8 keys, uint8 state );
void ZDO_STATE_CHANGE_CB( devStates_t status );
void SmartDevice_CommLED_Control( uint8 state );

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
    
    Timer3_Init();
#if SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_COORD
    
#elif SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_LIGHT
    bsp_light_init();
#elif SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_SOCKET

#elif SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_CURTAIN

#elif SMART_DEVICE_TYPE == MYPROTOCOL_DEVICE_HT_SENSOR

#else

#endif
    
    osal_nv_item_init(DEVICE_COORD_SAVE_ID,DEVICE_COORD_DATA_SIZE,NULL);

    myprotocol_init( SmartDevice_EndPoint, &SmartDevice_TaskID );

    /**  注册按键事件 */
    RegisterForKeys( SmartDevice_TaskID );
    
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
                case KEY_CHANGE:    
                    SmartDevice_Key_Headler(((keyChange_t *)MSGpkt)->keys,((keyChange_t *)MSGpkt)->state);
                    break;
                /** 接收到数据 */
                case AF_INCOMING_MSG_CMD:
                    SmartDevice_Message_Headler(MSGpkt);
                    break;
                /** 状态改变 */
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
 
    if( events & SMART_DEVICE_TIMER_EVEN )
    {      
        MYPROTOCOL_SEND_TICK_PACKET();

        osal_start_timerEx( SmartDevice_TaskID, 
                            SMART_DEVICE_TIMER_EVEN, 
                            SMART_DEVICE_TIME );
        
        return (events ^ SMART_DEVICE_TIMER_EVEN);
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
            osal_start_timerEx( SmartDevice_TaskID, 
                                SMART_DEVICE_TIMER_EVEN, 
                                SMART_DEVICE_TIME );
            SmartDevice_CommLED_Control(SMARTDEVICE_LED_CONNED_STATE);
                break;
        case DEV_END_DEVICE:
            DEVICE_LOG("I am End Device!\n");
            osal_start_timerEx( SmartDevice_TaskID, 
                                SMART_DEVICE_TIMER_EVEN, 
                                SMART_DEVICE_TIME );
            SmartDevice_CommLED_Control(SMARTDEVICE_LED_CONNED_STATE);
                break;
        case DEV_ZB_COORD:
            DEVICE_LOG("I am Coord Device!\n");
            SmartDevice_CommLED_Control(SMARTDEVICE_LED_CONNED_STATE);
                break;
        case DEV_NWK_DISC:
            DEVICE_LOG("Discovering PAN's to join!\n");
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
 * @brief       SmartDevice按键处理
 * @param       [in]   pkt    信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void SmartDevice_Key_Headler( uint8 keys, uint8 state )
{
    
}

/**
 *******************************************************************************
 * @brief       定时器3中断处理函数
 * @param       [in/out]  task_id    任务ID
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
#define TIM3_CLOSE() ( T3CTL &= ~0x10, T3CTL |= 0x04, TIMIF &= ~0x01 )
#define TIM3_OPEN()  ( T3CTL |= 0x10 )
HAL_ISR_FUNCTION( halTimer3Isr, T3_VECTOR )
{
#if defined ( USE_GIZWITS_MOD )
    static uint8 Gizwits_Timer = 0;
#endif
    static uint16 Device_Clear_Timer = 0;
    
    TIM3_CLOSE();
    
#if defined ( USE_GIZWITS_MOD )
    gizTimerMs();
    
    if( ++Gizwits_Timer >= GIZWITS_HANDLER_TIME )
    {
        gizwitsHandle(&currentDataPoint);
        Gizwits_Timer = 0;
    } 
#endif    
    
    if( ++Device_Clear_Timer >= CLEAR_ZOMBIE_DEVICE_TIME )
    {
        Del_ZombieDevice_ForList();
        Del_DeviceTickCount();
        Device_Clear_Timer = 0;
    }

    TIM3_OPEN();
}

/** @}*/     /* smartlight模块 */

/**********************************END OF FILE*********************************/
