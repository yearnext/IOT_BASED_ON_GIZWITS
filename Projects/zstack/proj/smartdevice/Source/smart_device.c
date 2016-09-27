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

#include "smart_device.h"

#include "onboard.h"

/* HAL */
//#include "hal_lcd.h"
//#include "hal_led.h"
//#include "hal_key.h"

#include "MT.h"
#include "MT_UART.h"
#include "hal_uart.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** 任务ID */
static byte zclSmartDevice_TaskID;

/** 端点描述符 */
static endPointDesc_t SmartDevice_epDesc;

/** 簇表 */
const cId_t zclSmartDevice_InClusterList[] =
{
    SmartDevice_Report_ClustersID,
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
/* Private functions ---------------------------------------------------------*/
void zclSamrtDevice_SendPeriodic_Message( void );
void zclSmartDevice_MessageMSGCB( afIncomingMSGPacket_t *pkt );

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
    zclSmartDevice_TaskID = task_id;
    SmartDevice_NwkState = DEV_INIT;
    SmartDevice_TransID = 0;
    
    MT_UartInit();
    MT_UartRegisterTaskID(zclSmartDevice_TaskID);
    
    /** 注册AF层应用对象 */
    SmartDevice_epDesc.endPoint = SmartDevice_EndPoint;
    SmartDevice_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&zclSmartDevice_SimpleDesc;
    SmartDevice_epDesc.task_id = &zclSmartDevice_TaskID;
    SmartDevice_epDesc.latencyReq = noLatencyReqs;
    afRegister(&SmartDevice_epDesc);
    
    /** 周期性的发送数据给协调器 */
    SmartDevice_Periodic_DstAddr.addr.shortAddr= 0x0000;
    SmartDevice_Periodic_DstAddr.addrMode = afAddr16Bit;
    SmartDevice_Periodic_DstAddr.endPoint = SmartDevice_EndPoint;
    
    /**  注册按键事件 */
    RegisterForKeys( zclSmartDevice_TaskID );
    // Register the Application to receive the unprocessed Foundation command/response messages
//    zcl_registerForMsg( zclSmartDevice_TaskID );

    /** 创建一个组表 */
    SmartDevice_Group.ID = 0x0001;
    osal_memcpy( SmartDevice_Group.name, "Group 1", sizeof("Group 1") );
    aps_AddGroup(SmartDevice_EndPoint, &SmartDevice_Group);
    
    HalUARTWrite(0,"Smart device init finish!\n",sizeof("Smart device init finish!\n"));
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
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(zclSmartDevice_TaskID);
        while( MSGpkt )
        {
            switch( MSGpkt->hdr.event )
            {
                case KEY_CHANGE:
                    break;
                /** 接收到数据 */
                case AF_INCOMING_MSG_CMD:
                    zclSmartDevice_MessageMSGCB(MSGpkt);
                    break;
                /** 状态改变 */
                case ZDO_STATE_CHANGE:
                    SmartDevice_NwkState = (devStates_t)(MSGpkt->hdr.status);

                    switch( SmartDevice_NwkState )
                    {
                        case DEV_ROUTER:
                            HalUARTWrite(0,"I am Router Device!\n",sizeof("I am Router Device!\n"));
                            osal_start_timerEx( zclSmartDevice_TaskID, SmartDevice_Report_Enent,
                                                SmartDevice_Report_Time );
                                break;
                        case DEV_END_DEVICE:
                            HalUARTWrite(0,"I am End Device!\n",sizeof("I am End Device!\n"));
                            osal_start_timerEx( zclSmartDevice_TaskID, SmartDevice_Report_Enent,
                                                SmartDevice_Report_Time );
                                break;
                        case DEV_ZB_COORD:
                            HalUARTWrite(0,"I am Coord Device!\n",sizeof("I am Coord Device!\n"));
                                break;
                        case DEV_NWK_DISC:
                            HalUARTWrite(0,"Discovering PAN's to join!\n",sizeof("Discovering PAN's to join!\n"));
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
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(zclSmartDevice_TaskID);
        }
        
        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }
    
    /** 周期发送数据任务 */
    if ( events & SmartDevice_Report_Enent )
    {
        // 数据上报
        zclSamrtDevice_SendPeriodic_Message();

        // Setup to send message again in normal period (+ a little jitter)
        osal_start_timerEx( zclSmartDevice_TaskID, SmartDevice_Report_Enent,
        (SmartDevice_Report_Time + (osal_rand() & 0x00FF)) );

        // return unprocessed events
            return (events ^ SmartDevice_Report_Enent);
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
static void zclSamrtDevice_SendPeriodic_Message( void )
{
    uint8 data[] = {1,2,3,4,5,6,7,8,9,10};
    if( AF_DataRequest(&SmartDevice_Periodic_DstAddr,
                       &SmartDevice_epDesc,
                       SmartDevice_Report_ClustersID,
                       sizeof(data),
                       (uint8 *)&data,
                       &SmartDevice_TransID,
                       AF_DISCV_ROUTE,
                       AF_DEFAULT_RADIUS) == afStatus_SUCCESS )
    {
        HalUARTWrite(0,"Send data finish!\n",sizeof("Send data finish!\n"));
    }
    else
    {
        HalUARTWrite(0,"Send data failed!\n",sizeof("Send data finish!\n"));
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
static void zclSmartDevice_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
    switch( pkt->clusterId )
    {
        case SmartDevice_Report_ClustersID:
            HalUARTWrite(0,"Get data is \n",sizeof("Get data is \n"));
            HalUARTWrite(0,pkt->cmd.Data,pkt->cmd.DataLength);
            HalUARTWrite(0,"\n",sizeof(uint8));
            break;
        default:
            break;
    }
}
 
/** @}*/     /* smartlight模块 */

/**********************************END OF FILE*********************************/
