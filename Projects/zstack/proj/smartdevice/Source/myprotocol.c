 /**
 ******************************************************************************
  * @file       myprotocol.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      myprotocol 源文件
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
 * @defgroup myprotocol模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "myprotocol.h"
#include "hal_uart.h"
#include <string.h>
#include "gizwits_protocol.h"
#include "devicelist.h"
#include "NLMEDE.h"
#include "smart_device.h"

/* Exported macro ------------------------------------------------------------*/
/**
* @name 日志打印宏定义
* @{
*/
#define USE_MYPROTOCOL_DEBUG 1

#if USE_MYPROTOCOL_DEBUG
    #define MYPROTOCOL_LOG(n) HalUARTWrite(1,n,sizeof(n))  ///<运行日志打印
#else 
    #define MYPROTOCOL_LOG(n)
#endif

/**@} */
#define MYPROTOCOL_UART_PORT 0
#define MYPROTOCOL_UART_WRITE(data,len)  HalUARTWrite(GIZWITS_UART_PORT,data,len)

/* Exported types ------------------------------------------------------------*/
typedef bool (*packet_func)(void *ctx, MYPROTOCOL_FORMAT *packet);

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MYPROTOCOL_PACKET_SIZE     (sizeof(MYPROTOCOL_FORMAT))
#if defined(USE_GIZWITS_MOD)
#define MYPROTOCOL_PACKET_REPORT() gizwitsReport((uint8 *)&tx_packet)
#else
#define MYPROTOCOL_PACKET_REPORT()
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static MYPROTOCOL_FORMAT tx_packet;
//static myprotocol_format rx_packet;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_init( void )
{
    memset(&tx_packet,0,sizeof(MYPROTOCOL_FORMAT));
}

/**
 *******************************************************************************
 * @brief       计算数据包的校验和
 * @param       [in/out]  packet    数据包地址
 * @return      [in/out]  checksum  校验和
 * @note        None
 *******************************************************************************
 */
uint8 myprotocol_compute_checksum( uint8 *packet )
{
    uint8 i;
    uint8 checksum;
    
//    if( packet == NULL )
//    {
//        return 0;
//    }
    
    for( i=0, checksum=0; i<MYPROTOCOL_PACKET_SIZE-1; i++ )
    {
        checksum += packet[i];
    }
    
    return checksum;
}

/**
 *******************************************************************************
 * @brief       检测数据包是否合法
 * @param       [in/out]  data    事件序号
 * @return      [in/out]  bool    数据包合法状态
 * @note        None
 *******************************************************************************
 */
bool myprotocol_packet_check( uint8 *data )
{
    MYPROTOCOL_FORMAT *packet = (MYPROTOCOL_FORMAT *)data;
    
//    if( data == NULL )
//    {
//        return 0;
//    }
    
    if( packet->check_sum != myprotocol_compute_checksum(data) )
    {
        return false;
    }
    
    if( packet->commtype == MYPROTOCOL_W2D_READ_ACK \
        && packet->commtype == MTPROTOCOL_W2D_WRITE_ACK \
        && packet->commtype == MYPROTOCOL_D2W_READ_WAIT \
        && packet->commtype == MYPROTOCOL_D2W_REPORT_WAIT )
    {
        return false;
    }
    
    return true;
}

/**
 *******************************************************************************
 * @brief        SmartDevice发送信息函数
 * @param       [in/out]   ctx              上下文
 *              [in/out]   create_packet    创建数据包功能
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void create_sdtick_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    MYPROTOCOL_DEVICE_INFO device_info;
    uint8 *mac_addr = NULL;
    
    packet->commtype = MYPROTOCOL_COMM_TICK;
    packet->sn = 0;
    
    mac_addr = NLME_GetExtAddr();
    memcpy(&device_info.mac,mac_addr,sizeof(device_info.mac));

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
    
    memcpy(&packet->user_data.data,&device_info,sizeof(MYPROTOCOL_DEVICE_INFO));
    
    packet->user_data.cmd = MYPROTOCOL_TICK_CMD;
    packet->user_data.len = sizeof(MYPROTOCOL_DEVICE_INFO);
}

/**
 *******************************************************************************
 * @brief        SmartDevice发送信息函数
 * @param       [in/out]   ctx              上下文
 *              [in/out]   create_packet    创建数据包功能
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void create_sdtick_ack_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    MYPROTOCOL_DEVICE_INFO *device_info = (MYPROTOCOL_DEVICE_INFO *)ctx;
    
    packet->commtype = MYPROTOCOL_COMM_TICK;
    packet->sn = 0;
    packet->device.device = device_info->device;
    memcpy(&packet->device.mac,device_info->mac,sizeof(packet->device.mac));
    packet->user_data.cmd = MYPROTOCOL_TICK_CMD;
    packet->user_data.len = sizeof(packet->user_data.cmd);
}

/**
 *******************************************************************************
 * @brief       SmartDevice信息处理
 * @param       [in]   pkt    信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void SmartDevice_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
    if( pkt->clusterId != SmartDevice_Comm_ClustersID )
    {
        return;
    }
    
    MYPROTOCOL_FORMAT *packet = (MYPROTOCOL_FORMAT *)pkt->cmd.Data;

    if( packet->check_sum != myprotocol_compute_checksum(pkt->cmd.Data) )
    {
        return;
    }
    
    switch( packet->commtype )
    {
        case MYPROTOCOL_COMM_TICK:
#if defined ( USE_GIZWITS_MOD )
            // 如果为父设备
            DEVICE_INFO *device_info = (DEVICE_INFO *)packet->user_data.data;

            // 添加设备
            if( Add_Device_Forlist(device_info) == false )
            {
                // 增加设备心跳计数
                Add_DeviceTick_ForList(device_info);
            }
            
            // 心跳应答
            SmartDevice_Send_Message(pkt->srcAddr,create_sdtick_ack_packet,&device_info->device);
            
            DEVICE_LOG("Coord get one end device tick packet!\n");
#endif
            break;
        case MYPROTOCOL_W2D_READ_WAIT:
            break;
        case MYPROTOCOL_W2D_WRITE_WAIT:
            break;
        case MYPROTOCOL_D2W_READ_ACK:
            break;
        case MYPROTOCOL_D2W_REPORT_ACK:
            break;
        default:
            break;
    }
}

/**
 *******************************************************************************
 * @brief       myprotocol 创建并发送一个数据包
 * @param       [in/out]  packet        接收到的数据包
 *              [in/out]  packet_type   创建数据包的类型
 *              [in/out]  data          需要用到的数据   
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool myprotol_send_packet( MYPROTOCOL_FORMAT *packet, packet_func packet_create, void *ctx )
{
    bool status = false;
    
    if( packet == NULL || packet_create == NULL )
    {
        return false;
    }
    
    memset(&tx_packet,0,MYPROTOCOL_PACKET_SIZE);
    
    status = packet_create(ctx, &tx_packet);
    
    tx_packet.sn = packet->sn;
    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);
    
    MYPROTOCOL_PACKET_REPORT();
    
    return status;
}

///**
// *******************************************************************************
// * @brief       创建错误代码数据包
// * @param       [in/out]  sn    事件序号
// * @return      [in/out]  void
// * @note        None
// *******************************************************************************
// */
//void myprotocol_creat_errcode_packet( uint8 sn )
//{
//    memset(&tx_packet,0,MYPROTOCOL_PACKET_SIZE);
//    
//    tx_packet.commtype = MYPROTOCOL_COMM_ERROR;
//    tx_packet.sn = sn;
//    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);
//}
/**
 *******************************************************************************
 * @brief       创建错误代码数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_errcode_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    packet->commtype = MYPROTOCOL_COMM_ERROR;
    return true;
}


///**
// *******************************************************************************
// * @brief       创建结束通讯数据包
// * @param       [in/out]  sn    事件序号
// * @return      [in/out]  void
// * @note        None
// *******************************************************************************
// */
//void myprotocol_creat_commend_packet( uint8 sn )
//{
//    memset(&tx_packet,0,MYPROTOCOL_PACKET_SIZE);
//    
//    tx_packet.commtype = MYPROTOCOL_COMM_END;
//    tx_packet.sn = sn;
//    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);
//}
/**
 *******************************************************************************
 * @brief       创建结束通讯数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_commend_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    packet->commtype = MYPROTOCOL_COMM_END;
    return true;
}

///**
// *******************************************************************************
// * @brief       创建写入设备信息应答数据包
// * @param       [in/out]  sn    事件序号
// * @return      [in/out]  void
// * @note        None
// *******************************************************************************
// */
//void myprotocol_create_writeack_packet(MYPROTOCOL_FORMAT *packet)
//{
//    memset(&tx_packet,0,MYPROTOCOL_PACKET_SIZE);
//    
//    tx_packet.commtype = MTPROTOCOL_W2D_WRITE_ACK;
//    tx_packet.sn = packet->sn;
//    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);    
//}
/**
 *******************************************************************************
 * @brief       创建写入设备信息应答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_writeack_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    packet->commtype = MTPROTOCOL_W2D_WRITE_ACK;
    return true;
}

///**
// *******************************************************************************
// * @brief       创建读取设备信息应答数据包
// * @param       [in/out]  sn    事件序号
// * @return      [in/out]  void
// * @note        None
// *******************************************************************************
// */
//void myprotocol_create_readack_packet(MYPROTOCOL_FORMAT *packet)
//{
//    memset(&tx_packet,0,MYPROTOCOL_PACKET_SIZE);
//    
//    tx_packet.commtype = MYPROTOCOL_W2D_READ_ACK;
//    tx_packet.sn = packet->sn;
//    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);    
//}
/**
 *******************************************************************************
 * @brief       创建读取设备信息应答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_readack_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{
    packet->commtype = MYPROTOCOL_W2D_READ_ACK;
    return true;
}

/**
 *******************************************************************************
 * @brief       创建读取设备数量应答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_devicenum_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{ 
    uint8 *num = (uint8 *)ctx;
    
    if( ctx == NULL )
    {
        return false;
    }
    
    packet->commtype = MYPROTOCOL_W2D_READ_ACK;
    packet->user_data.cmd = W2D_GET_DEVICE_NUM_CMD;
    packet->user_data.data[0] = *num;
    packet->user_data.len = sizeof(uint8);
    
    return true;
}

/**
 *******************************************************************************
 * @brief       创建读取设备信息应答数据包
 * @param       [in/out]  ctx     上下文
 *              [in/out]  packet  数据包  
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool create_deviceinfo_packet( void *ctx, MYPROTOCOL_FORMAT *packet )
{ 
    MYPROTOCOL_DEVCICE_ACK *device_info = (MYPROTOCOL_DEVCICE_ACK *)ctx;
    
    if( ctx == NULL )
    {
        return false;
    }

    packet->commtype = MYPROTOCOL_W2D_READ_ACK;
    packet->user_data.cmd = W2D_GET_DEVICE_NUM_CMD;
    packet->user_data.data[0] = device_info->id;
    packet->user_data.data[1] = device_info->info->device;
    memcpy(&packet->user_data.data[2],&device_info->info->mac,sizeof(device_info->info->mac));
    
    packet->user_data.len = sizeof(uint8);
    
    return true;
}

#if defined ( USE_GIZWITS_MOD )       
 
/**
 *******************************************************************************
 * @brief       数据包处理函数
 * @param       [in/out]  data    数据包
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_packet_handler( uint8 *report_data, uint8 *packet_data )
{
    MYPROTOCOL_FORMAT *packet = (MYPROTOCOL_FORMAT *)packet_data;
    
//    if( report_data == NULL || packet_data == NULL )
//    {
//        return 0;
//    }
    
    if( myprotocol_packet_check(packet_data) == false )
    {
        myprotol_send_packet(packet,create_errcode_packet,NULL);
        return;
    }
    else 
    {
        switch( packet->commtype )
        {
            // WIFI模组读取设备数据等待应答
            case MYPROTOCOL_W2D_READ_WAIT:
                if( packet->device.device == MYPROTOCOL_DEVICE_COORD )
                {
                    switch( packet->user_data.cmd )
                    {
                        case W2D_GET_DEVICE_NUM_CMD:
                        {
                            uint8 num = Get_DeviceNum_ForList();
                            myprotol_send_packet(packet,create_devicenum_packet,(void *)&num);
                        }
                            break;
                        case W2D_GET_DEVICE_INFO_CMD:
                        {
                            MYPROTOCOL_DEVCICE_ACK device;
                            device.id = packet->user_data.data[0];
                            device.info = (MYPROTOCOL_DEVICE_INFO *)Get_DeviceInfo_InList(device.id);
                            myprotol_send_packet(packet,create_deviceinfo_packet,(void *)&device);
                        }
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    myprotol_send_packet(packet,create_readack_packet,NULL);
                }
                    break;
            // WIFI模组写入设备数据等待应答
            case MYPROTOCOL_W2D_WRITE_WAIT:
                myprotol_send_packet(packet,create_writeack_packet,NULL);
                    break;
            // 设备读取WIFI模组信息应答
            case MYPROTOCOL_D2W_READ_ACK:
                myprotol_send_packet(packet,create_commend_packet,NULL);
                    break;
            // 设备上报数据应答
            case MYPROTOCOL_D2W_REPORT_ACK:
                myprotol_send_packet(packet,create_commend_packet,NULL);
                    break;
            // 通讯错误
            case MYPROTOCOL_COMM_ERROR:
                MYPROTOCOL_PACKET_REPORT();
                    break;
            case MYPROTOCOL_COMM_END:
                break;
            default:
                break;
        }
    }
}

#endif

/** @}*/     /* myprotocol模块 */

/**********************************END OF FILE*********************************/
