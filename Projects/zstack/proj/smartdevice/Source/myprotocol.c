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

#if defined ( USE_GIZWITS_MOD )

/* Includes ------------------------------------------------------------------*/
#include "myprotocol.h"
#include "hal_uart.h"
#include <string.h>
#include "gizwits_protocol.h"

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
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MYPROTOCOL_PACKET_SIZE     (sizeof(myprotocol_format))
#define MYPROTOCOL_PACKET_REPORT() gizwitsReport((uint8 *)&tx_packet)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static myprotocol_format tx_packet;
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
    memset(&tx_packet,0,sizeof(myprotocol_format));
//    memset(&rx_packet,0,sizeof(myprotocol_format));
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
 * @brief       创建错误代码数据包
 * @param       [in/out]  sn    事件序号
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_creat_errcode_packet( uint8 sn )
{
    memset(&tx_packet,0,sizeof(myprotocol_format));
    
    tx_packet.commtype = MYPROTOCOL_COMM_ERROR;
    tx_packet.sn = sn;
    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);
}

/**
 *******************************************************************************
 * @brief       创建结束通讯数据包
 * @param       [in/out]  sn    事件序号
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_creat_commend_packet( uint8 sn )
{
    memset(&tx_packet,0,sizeof(myprotocol_format));
    
    tx_packet.commtype = MYPROTOCOL_COMM_END;
    tx_packet.sn = sn;
    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);
}

/**
 *******************************************************************************
 * @brief       创建写入设备信息应答数据包
 * @param       [in/out]  sn    事件序号
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_create_writeack_packet(myprotocol_format *packet)
{
    memset(&tx_packet,0,sizeof(myprotocol_format));
    
    tx_packet.commtype = MTPROTOCOL_W2D_WRITE_ACK;
    tx_packet.sn = packet->sn;
    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);    
}

/**
 *******************************************************************************
 * @brief       创建读取设备信息应答数据包
 * @param       [in/out]  sn    事件序号
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_create_readack_packet(myprotocol_format *packet)
{
    memset(&tx_packet,0,sizeof(myprotocol_format));
    
    tx_packet.commtype = MYPROTOCOL_W2D_READ_ACK;
    tx_packet.sn = packet->sn;
    tx_packet.check_sum = myprotocol_compute_checksum((uint8 *)&tx_packet);    
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
    myprotocol_format *packet = (myprotocol_format *)data;
    
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
 * @brief       数据包处理函数
 * @param       [in/out]  data    数据包
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void myprotocol_packet_handler( uint8 *report_data, uint8 *packet_data )
{
    myprotocol_format *packet = (myprotocol_format *)packet_data;
    
//    if( report_data == NULL || packet_data == NULL )
//    {
//        return 0;
//    }
    
    if( myprotocol_packet_check(packet_data) == false )
    {
        myprotocol_creat_errcode_packet(packet->sn);
        MYPROTOCOL_PACKET_REPORT();
        return;
    }
    else 
    {
        switch( packet->commtype )
        {
//            case MYPROTOCOL_COMM_END:
//                break;
            case MYPROTOCOL_W2D_READ_WAIT:
                myprotocol_create_readack_packet(packet);
                MYPROTOCOL_PACKET_REPORT();
                    break;
            case MYPROTOCOL_W2D_WRITE_WAIT:
                myprotocol_create_writeack_packet(packet);
                MYPROTOCOL_PACKET_REPORT();
                    break;
            case MYPROTOCOL_D2W_READ_ACK:
                myprotocol_creat_commend_packet(packet->sn);
                MYPROTOCOL_PACKET_REPORT();
                    break;
            case MYPROTOCOL_D2W_REPORT_ACK:
                myprotocol_creat_commend_packet(packet->sn);
                MYPROTOCOL_PACKET_REPORT();
                    break;
            case MYPROTOCOL_COMM_ERROR:
                myprotocol_creat_commend_packet(packet->sn);
                MYPROTOCOL_PACKET_REPORT();
                    break;
            default:
                break;
        }
    }
}

#endif
/** @}*/     /* myprotocol模块 */

/**********************************END OF FILE*********************************/
