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
#include "aps_groups.h"
#include "ZDApp.h"
#include "ZGlobals.h"
#include <string.h>
#include "NLMEDE.h"
#include "smart_device.h"

#include "onboard.h"

/* HAL */
//#include "hal_lcd.h"
//#include "hal_led.h"
//#include "hal_key.h"

#include "MT.h"
#include "MT_UART.h"
#include "hal_uart.h"
#include "myprotocol.h"

#if defined ( USE_GIZWITS_MOD )
#include "gizwits.h"
#include "devicelist.h"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
dataPoint_t currentDataPoint;
 
/* Private define ------------------------------------------------------------*/
/**
* @name 日志打印宏定义
* @{
*/
#define USE_SMARTDEVICE_DEBUG 1

#if USE_SMARTDEVICE_DEBUG
    #define DEVICE_LOG(n) HalUARTWrite(1,n,sizeof(n))  ///<运行日志打印
#else 
    #define DEVICE_LOG(n)
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** 任务ID */
static byte SmartDevice_TaskID;

/** 端点描述符 */
static endPointDesc_t SmartDevice_epDesc;

/** 簇表 */
const cId_t zclSmartDevice_InClusterList[] =
{
    SmartDevice_Comm_ClustersID,
};

/** 简单描述符 */
static const SimpleDescriptionFormat_t zclSmartDevice_SimpleDesc =
{
    SmartDevice_EndPoint, 
    SmartDevice_ProfileID,
    SmartDevice_DeviceID,
    SmartDevice_Version,
    SmartDevice_Flags,                  
    SamrtDevice_ClustersNum,
    (cId_t *)&zclSmartDevice_InClusterList,
    SamrtDevice_ClustersNum,
    (cId_t *)&zclSmartDevice_InClusterList,
};

/** 周期性发送数据 */
static afAddrType_t SmartDevice_Periodic_DstAddr;

/** 设备组别 */
static aps_Group_t SmartDevice_Group;

/** 设备状态 */
static devStates_t SmartDevice_NwkState;

/** 发送ID */
static uint8 SmartDevice_TransID;

#if defined ( USE_GIZWITS_MOD ) 
    static uint16 Device_Msg_Timer = 0;
    static uint8 Gizwits_Msg_Timer = 0;
    static uint8 Gizwits_Timer = 0;
    static uint16 Device_Clear_Timer = 0;
#endif

/* Private functions ---------------------------------------------------------*/
void SamrtDevice_Tick_Message( void );
void SmartDevice_MessageMSGCB( afIncomingMSGPacket_t *pkt );
void SmartDevice_Message_Handler( void *packet );

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
    SmartDevice_TransID = 0;
    
    MT_UartInit();
    MT_UartRegisterTaskID(SmartDevice_TaskID);
    
#if defined ( USE_GIZWITS_MOD )   
    app_gizwits_init();
    
    osal_start_timerEx( SmartDevice_TaskID, SMART_DEVICE_TIMER_EVEN, SMART_DEVICE_TIME );
#endif
    
    /** 注册AF层应用对象 */
    SmartDevice_epDesc.endPoint = SmartDevice_EndPoint;
    SmartDevice_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&zclSmartDevice_SimpleDesc;
    SmartDevice_epDesc.task_id = &SmartDevice_TaskID;
    SmartDevice_epDesc.latencyReq = noLatencyReqs;
    afRegister(&SmartDevice_epDesc);
    
    /** 周期性的发送数据给协调器 */
    SmartDevice_Periodic_DstAddr.addr.shortAddr= 0x0000;
    SmartDevice_Periodic_DstAddr.addrMode = afAddr16Bit;
    SmartDevice_Periodic_DstAddr.endPoint = SmartDevice_EndPoint;
    
    /**  注册按键事件 */
    RegisterForKeys( SmartDevice_TaskID );

    /** 创建一个组表 */
    SmartDevice_Group.ID = 0x0001;
    osal_memcpy( SmartDevice_Group.name, "Group 1", sizeof("Group 1") );
    aps_AddGroup(SmartDevice_EndPoint, &SmartDevice_Group);
    
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
                    break;
                /** 接收到数据 */
                case AF_INCOMING_MSG_CMD:
                    SmartDevice_MessageMSGCB(MSGpkt);
                    break;
                /** 状态改变 */
                case ZDO_STATE_CHANGE:
                    SmartDevice_NwkState = (devStates_t)(MSGpkt->hdr.status);

                    switch( SmartDevice_NwkState )
                    {
                        case DEV_ROUTER:
                            DEVICE_LOG("I am Router Device!\n");
#if !defined ( USE_GIZWITS_MOD )
                            osal_start_timerEx( SmartDevice_TaskID, 
                                                SMART_DEVICE_TIMER_EVEN, 
                                                SMART_DEVICE_TIME );
#endif
                                break;
                        case DEV_END_DEVICE:
#if !defined ( USE_GIZWITS_MOD )
                            DEVICE_LOG("I am End Device!\n");
                            osal_start_timerEx( SmartDevice_TaskID, 
                                                SMART_DEVICE_TIMER_EVEN, 
                                                SMART_DEVICE_TIME );
#endif
                                break;
                        case DEV_ZB_COORD:
                            DEVICE_LOG("I am Coord Device!\n");
                                break;
                        case DEV_NWK_DISC:
                            DEVICE_LOG("Discovering PAN's to join!\n");
                            break;
                        default:
                            break;
                    }
                    
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
#if defined ( USE_GIZWITS_MOD )
        if( SmartDevice_NwkState == DEV_ROUTER \
            || SmartDevice_NwkState == DEV_END_DEVICE )
        {
            if( ++Device_Msg_Timer >= DEVICE_TICK_TIME )
            {
                SamrtDevice_Tick_Message();
                Device_Msg_Timer = 0;
            }
        }
 
        if( ++Gizwits_Timer >= GIZWITS_TIMER_TIME )
        {
            gizTimer10Ms();
            Gizwits_Timer = 0;
        }
        
        if( ++Gizwits_Msg_Timer >= GIZWITS_HANDLER_TIME )
        {
            gizwitsHandle(&currentDataPoint);
            Gizwits_Msg_Timer = 0;
        }
        
        if( ++Device_Clear_Timer >= CLEAR_ZOMBIE_DEVICE_TIME )
        {
            Del_ZombieDevice_ForList();
            Del_DeviceTickCount();
            Device_Clear_Timer = 0;
        }
        
#else        
        SamrtDevice_Tick_Message();
#endif
        
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
 * @brief        SmartDevice上报数据
 * @param       [in]   void
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
static void SamrtDevice_Tick_Message( void )
{
    MYPROTOCOL_FORMAT packet;
    MYPROTOCOL_DEVICE_INFO device_info;
    memset(&packet,0,sizeof(MYPROTOCOL_FORMAT));
    
    packet.commtype = MYPROTOCOL_COMM_TICK;
    packet.sn = 0;
    device_info.addr = NLME_GetShortAddr();
#if (SmartDevice_ProfileID) == (SmartLight_ProfileID)
    device_info.device = MYPROTOCOL_DEVICE_LIGHT;
#elif (SmartDevice_ProfileID) == (SmartSwitch_ProfileID)
    device_info.device = MYPROTOCOL_DEVICE_SOCKET;
#elif (SmartDevice_ProfileID) == (SmartCurtain_ProfileID)
    device_info.device = MYPROTOCOL_DEVICE_CURTAIN;
#elif (SmartDevice_ProfileID) == (SmartCurtain_ProfileID)
    device_info.device = MYPROTOCOL_DEVICE_HT_SENSOR;
#else 
    device_info.device = MYPROTOCOL_DEVICE_COORD;
#endif
    memcpy(&packet.user_data.data,&device_info,sizeof(MYPROTOCOL_DEVICE_INFO));
    packet.user_data.cmd = MYPROTOCOL_TICK_CMD;
    packet.user_data.len = sizeof(MYPROTOCOL_DEVICE_INFO);
    packet.check_sum = myprotocol_compute_checksum((uint8 *)&packet);
    
    if( AF_DataRequest(&SmartDevice_Periodic_DstAddr,
                       &SmartDevice_epDesc,
                       SmartDevice_Comm_ClustersID,
                       sizeof(MYPROTOCOL_FORMAT),
                       (uint8 *)&packet,
                       &SmartDevice_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS) == afStatus_SUCCESS )
    {
//        HalUARTWrite(0,"Send data finish!\n",sizeof("Send data finish!\n"));
    }
    else
    {
//        HalUARTWrite(0,"Send data failed!\n",sizeof("Send data finish!\n"));
    }
}
 
/**
 *******************************************************************************
 * @brief        SmartDevice信息处理
 * @param       [in]   pkt    信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
static void SmartDevice_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
    switch( pkt->clusterId )
    {
        case SmartDevice_Comm_ClustersID:
            SmartDevice_Message_Handler(&pkt->cmd.Data);
            break;
        default:
            break;
    }
}

static void SmartDevice_Message_Handler( void *packet )
{
    
}
 
/** @}*/     /* smartlight模块 */

/**********************************END OF FILE*********************************/
