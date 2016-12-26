/**
 ******************************************************************************
  * @file       gizwits_protocol.c
  * @author     Gizwits
  * @par        Modify
                    yearnext
  * @version    V03010101
  * @date       2016年12月1日
  * @brief      gizwits_protocol 源文件
  * @par        工作平台                                  
  *                  CC2530
  * @par        工作频率                                  
  *                  32MHz
  * @par        编译平台									                          
  * 				 IAR
  ******************************************************************************
  * @note
  * 机智云.只为智能硬件而生
  * Gizwits Smart Cloud  for Smart Products
  * 链接|增值ֵ|开放|中立|安全|自有|自由|生态
  * www.gizwits.com               						                      
  ******************************************************************************
 */

/**
 * @defgroup gizwits_protocol模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "Onboard.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** 协议全局变量 **/
static gizwitsProtocol_t gizwitsProtocol;

/** 环形缓冲区结构体变量 **/
static rb_t pRb;                                    

/* Private functions ---------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       环形缓冲区创建函数
 * @param       [in/out]  *rb         指向环形缓冲区的指针
 * @return      [in/out]  bool        返回状态
 * @note        None
 *******************************************************************************
 */
static bool rbCreate( rb_t* rb )
{
#if USE_MYPROTOCOL_DEBUG
    if(NULL == rb)
    {
        GIZWITS_LOG("rbCreate ERROR: input rb is NULL\n");
        return false;
    }
#endif
    rb->rbHead = rb->rbBuff;
    rb->rbTail = rb->rbBuff;
    
    return true;
}

///**
// *******************************************************************************
// * @brief       环形缓冲区删除函数
// * @param       [in/out]  *rb         指向环形缓冲区的指针
// * @return      [in/out]  bool        返回状态
// * @note        None
// *******************************************************************************
// */
//static bool rbDelete( rb_t* rb )
//{
//#if USE_MYPROTOCOL_DEBUG
//    if(NULL == rb)
//    {
//        GIZWITS_LOG("rbDelete ERROR: input rb is NULL\n");
//        return false;
//    }
//#endif
//    rb->rbBuff = NULL;
//    rb->rbHead = NULL;
//    rb->rbTail = NULL;
//    rb->rbCapacity = 0;
//    
//    return true;
//}

/**
 *******************************************************************************
 * @brief       获取环形缓冲区大小
 * @param       [in/out]  *rb         指向环形缓冲区的指针
 * @return      [in/out]  uint8       环形缓冲区的大小
 * @note        None
 *******************************************************************************
 */
static int16 rbCapacity( rb_t *rb )
{
#if USE_MYPROTOCOL_DEBUG
    if(NULL == rb)
    {
        GIZWITS_LOG("rbCapacity ERROR: input rb is NULL\n");
        return -1;
    }
#endif

    return rb->rbCapacity;
}

/**
 *******************************************************************************
 * @brief       获取环形缓冲区能够读取数据的数量
 * @param       [in/out]  *rb         指向环形缓冲区的指针
 * @return      [in/out]  uint8       够读取数据的数量
 * @note        None
 *******************************************************************************
 */
static int16 rbCanRead( rb_t *rb )
{
#if USE_MYPROTOCOL_DEBUG
    if(NULL == rb)
    {
        GIZWITS_LOG("rbCanRead ERROR: input rb is NULL\n");
        return -1;
    }
#endif

    if (rb->rbHead == rb->rbTail)
    {
        return 0;
    }

    if (rb->rbHead < rb->rbTail)
    {
        return rb->rbTail - rb->rbHead;
    }

    return rb->rbCapacity - (rb->rbHead - rb->rbTail);
}

/**
 *******************************************************************************
 * @brief       获取环形缓冲区能够写入数据的数量
 * @param       [in/out]  *rb         指向环形缓冲区的指针
 * @return      [in/out]  uint8       够读写入据的数量
 * @note        None
 *******************************************************************************
 */
static int16 rbCanWrite( rb_t *rb )
{
#if USE_MYPROTOCOL_DEBUG
    if(NULL == rb)
    {
        GIZWITS_LOG("rbCanWrite ERROR: input rb is NULL\n");
        return -1;
    }
#endif

    return rb->rbCapacity - rbCanRead(rb);
}

/**
 *******************************************************************************
 * @brief       向环形缓冲区读取数据
 * @param       [in/out]  *rb           指向环形缓冲区的指针
 * @param       [in/out]  *data         指向数据缓冲区的指针
 * @param       [in/out]  count         读取数据的数量
 * @return      [in/out]  uint8         读到数据的数量
 * @note        None
 *******************************************************************************
 */
static int16 rbRead( rb_t *rb, void *data, uint16 count )
{
    int copySz = 0;
    
#if USE_MYPROTOCOL_DEBUG
    if(NULL == rb)
    {
        GIZWITS_LOG("rbRead ERROR: input rb is NULL\n");
        return -1;
    }

    if(NULL == data)
    {
        GIZWITS_LOG("rbRead ERROR: input data is NULL\n");
        return -1;
    }
#endif

    if (rb->rbHead < rb->rbTail)
    {
        copySz = min(count, rbCanRead(rb));
        memcpy(data, rb->rbHead, copySz);
        rb->rbHead += copySz;
        return copySz;
    }
    else
    {
        if (count < rbCapacity(rb)-(rb->rbHead - rb->rbBuff))
        {
            copySz = count;
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead += copySz;
            return copySz;
        }
        else
        {
            copySz = rbCapacity(rb) - (rb->rbHead - rb->rbBuff);
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead = rb->rbBuff;
            copySz += rbRead(rb, (uint8 *)data+copySz, count-copySz);
            return copySz;
        }
    }
    
//    return copySz;
}

/**
 *******************************************************************************
 * @brief       向环形缓冲区写入数据
 * @param       [in/out]  *rb           指向环形缓冲区的指针
 * @param       [in/out]  *data         指向数据缓冲区的指针
 * @param       [in/out]  count         写入数据的数量
 * @return      [in/out]  uint8         写入数据的数量
 * @note        None
 *******************************************************************************
 */
static int16 rbWrite( rb_t *rb, const void *data, uint16 count )
{
    int tailAvailSz = 0;

#if USE_MYPROTOCOL_DEBUG
    if( NULL == rb )
    {
        GIZWITS_LOG("rbWrite ERROR: rb is empty \n");
        return -1;
    }

    if( NULL == data )
    {
        GIZWITS_LOG("rbWrite ERROR: data is empty \n");
        return -1;
    }

#endif

    if ( count >= rbCanWrite(rb) )
    {
        GIZWITS_LOG("rbWrite ERROR: no memory \n");
        return -1;
    }

    if ( rb->rbHead <= rb->rbTail )
    {
        tailAvailSz = rbCapacity(rb) - (rb->rbTail - rb->rbBuff);
        if (count <= tailAvailSz)
        {
            memcpy(rb->rbTail, data, count);
            rb->rbTail += count;
            if (rb->rbTail == rb->rbBuff+rbCapacity(rb))
            {
                rb->rbTail = rb->rbBuff;
            }
            
            return count;
        }
        else
        {
            memcpy(rb->rbTail, data, tailAvailSz);
            rb->rbTail = rb->rbBuff;

            return tailAvailSz + rbWrite(rb, (uint8 *)data+tailAvailSz, count-tailAvailSz);
        }
    }
    else
    {
        memcpy(rb->rbTail, data, count);
        rb->rbTail += count;
        
        return count;
    }
}

/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       向环形缓冲区写入数据
 * @param       [in/out]  *buf          字节长度
 * @param       [in/out]  len           字节长度
 * @return      [in/out]  bool          返回状态
 * @note        None
 *******************************************************************************
 */
bool gizPutData( uint8 *buf, uint16 len )
{
#if USE_MYPROTOCOL_DEBUG    
	if( NULL == buf )
    {
        GIZWITS_LOG("gizPutData ERROR: gizPutData buf is empty \n");
        return false;
    }
#endif
    if( rbWrite(&pRb, buf, len) != len )
    {
        GIZWITS_LOG("gizPutData ERROR: Failed to rbWrite \n");
        return false;
    }

    return true;
}

/**
* @brief 系统毫秒计时维护函数,毫秒自增,溢出归零

* @param none
* @return none
*/
/**
 *******************************************************************************
 * @brief       系统1毫秒计时维护函数,毫秒自增,溢出归零
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void gizTimerMs(void)
{
    gizwitsProtocol.timerMsCount++;
}

/**
 *******************************************************************************
 * @brief       系统50毫秒计时维护函数,毫秒自增,溢出归零
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void gizTimer100Ms(void)
{
    gizwitsProtocol.timerMsCount++;
}

/**
 *******************************************************************************
 * @brief       读取系统时间毫秒计时数
 * @param       [in/out]  void
 * @return      [in/out]  uint32    系统时间毫秒数
 * @note        None
 *******************************************************************************
 */
uint32 gizGetTimerCount(void)
{
    return gizwitsProtocol.timerMsCount;
}

/**
 *******************************************************************************
 * @brief       报文数据校验和计算
 * @param       [in/out]  *buf     报文地址
 * @param       [in/out]  len      字节长度
 * @return      [in/out]  uint8    校验和
 * @note        None
 *******************************************************************************
 */
static uint8 gizProtocolSum( uint8 *buf, uint8 len )
{
    uint8 sum = 0;
    uint8 i = 0;
	
#if USE_MYPROTOCOL_DEBUG
    if(buf == NULL || len <= 0)
    {
        return 0;
    }
#endif

    for( i=2; i<len-1; i++ )
    {
        sum += buf[i];
    }

    return sum;
}

/**
 *******************************************************************************
 * @brief       16位数据字节序转换
 * @param       [in/out]  value    需要转换的数据
 * @return      [in/out]  uint16   转换后的数据
 * @note        None
 *******************************************************************************
 */
static uint16 gizProtocolExchangeBytes(uint16 value)
{
    uint16    tmp_value;
    uint8     *index_1, *index_2;

    index_1 = (uint8 *)&tmp_value;
    index_2 = (uint8 *)&value;

    *index_1 = *(index_2+1);
    *(index_1+1) = *index_2;

    return tmp_value;
}

/**
 *******************************************************************************
 * @brief       32位数据字节序转换
 * @param       [in/out]  value    需要转换的数据
 * @return      [in/out]  uint32   转换后的数据
 * @note        None
 *******************************************************************************
 */
static uint32 gizExchangeWord(uint32  value)
{
   return ((value & 0x000000FF) << 24) \
           |((value & 0x0000FF00) << 8) \
		   |((value & 0x00FF0000) >> 8) \
		   |((value & 0xFF000000) >> 24);
}

/**
 *******************************************************************************
 * @brief       数组缓冲区网络字节序转换
 * @param       [in/out]  *buf      buf地址
 * @param       [in/out]  dataLen   字节长度
 * @return      [in/out]  bool      返回状态
 * @note        None
 *******************************************************************************
 */
static bool gizByteOrderExchange( uint8 *buf, uint8 dataLen )
{
    uint32 i = 0;
    uint8 preData = 0;
    uint8 aftData = 0;
	
#if USE_MYPROTOCOL_DEBUG
    if(NULL == buf)
    {
        GIZWITS_LOG("gizByteOrderExchange Error , Illegal Param\n");
        return false;
    }
#endif

    for(i = 0; i<dataLen/2; i++)
    {
        preData = buf[i];
        aftData = buf[dataLen - i - 1];
        buf[i] = aftData;
        buf[dataLen - i - 1] = preData;
    }
	
    return true;
}

/**
 *******************************************************************************
 * @brief       从环形缓冲区中抓取一包数据
 * @param       [in/out]  rb      输入数据地址
 * @param       [in/out]  data    输出数据地址
 * @param       [in/out]  len     输出数据长度
 * @return      [in/out]  int8    返回状态
 * @note        None
 *******************************************************************************
 */
#if 0
static int8 gizProtocolGetOnePacket( rb_t *rb, uint8 *data, uint16 *len )
{
	static uint8 lastData = 0;
	static uint8 dataLen = 0;
	
	uint8 nowData = 0;
	uint16 packetLen = 0;
	
#if USE_MYPROTOCOL_DEBUG
    if((NULL == rb) || (NULL == data) ||(NULL == len))
    {
        GIZWITS_LOG("gizProtocolGetOnePacket Error , Illegal Param\n");
        return false;
    }
#endif
	
	while( rbRead( rb, &nowData, 1 ) == 1 )
	{
		if( lastData == 0xFF && nowData == 0xFF )
		{
            dataLen = 0;
            
			data[dataLen++] = 0xFF;
			data[dataLen++] = 0xFF;
		}
		else if( lastData == 0xFF && nowData == 0x55 )
		{
		}
		else
		{
			if( dataLen >= 2 )
			{
				data[dataLen++] = nowData;
                
				if( dataLen > 4 )
				{
					packetLen = gizProtocolExchangeBytes(((protocolHead_t *)data)->len)+4;

					if( dataLen == packetLen )
					{
						*len = packetLen;
                        dataLen = 0;
						lastData = 0;
						
						if( gizProtocolSum(data, packetLen) == data[packetLen-1] )
						{
							return 0;
						}
						else
						{
							return -2;
						}
					}
				}
			}
			else
			{
				lastData = 0;
                dataLen = 0;
			}
		}
		lastData = nowData;
	}
	
	return -1;
}

#else
  /**
* @brief 从环形缓冲区中抓取一包数据
*
* @param [in]   rb           : 输入数据地址
* @param [out]  data         : 输出数据地址
* @param [out]  len          : 输出数据长度
*
* @return : 0,正确返回;-1，错误返回;-2，数据校验失败
*/
static int8 gizProtocolGetOnePacket(rb_t *rb, uint8 *data, uint16 *len)
{
    uint8 ret = 0;
    uint8 sum = 0;
    uint8 i = 0;
    uint8 tmpData;
    uint8 tmpLen = 0;
    uint16 tmpCount = 0;
    static uint8 protocolFlag = 0;
    static uint16 protocolCount = 0;
    static uint8 lastData = 0;
    static uint8 debugCount = 0;
    uint8 *protocolBuff = data;
    protocolHead_t *head = NULL;

    if((NULL == rb) || (NULL == data) ||(NULL == len))
    {
        GIZWITS_LOG("gizProtocolGetOnePacket Error , Illegal Param\n");
        return -1;
    }

    tmpLen = rbCanRead(rb);
    if(0 == tmpLen)
    {
        return -1;
    }

    for(i=0; i<tmpLen; i++)
    {
        ret = rbRead(rb, &tmpData, 1);
        if(0 != ret)
        {
            if((0xFF == lastData) && (0xFF == tmpData))
            {
                if(0 == protocolFlag)
                {
                    protocolBuff[0] = 0xFF;
                    protocolBuff[1] = 0xFF;
                    protocolCount = 2;
                    protocolFlag = 1;
                }
                else
                {
                    if((protocolCount > 4) && (protocolCount != tmpCount))
                    {
                        protocolBuff[0] = 0xFF;
                        protocolBuff[1] = 0xFF;
                        protocolCount = 2;
                    }
                }
            }
            else if((0xFF == lastData) && (0x55 == tmpData))
            {
            }
            else
            {
                if(1 == protocolFlag)
                {
                    protocolBuff[protocolCount] = tmpData;
                    protocolCount++;

                    if(protocolCount > 4)
                    {
                        head = (protocolHead_t *)protocolBuff;
                        tmpCount = gizProtocolExchangeBytes(head->len)+4;
                        if(protocolCount == tmpCount)
                        {
                            break;
                        }
                    }
                }
            }

            lastData = tmpData;
            debugCount++;
        }
    }

    if((protocolCount > 4) && (protocolCount == tmpCount))
    {
        sum = gizProtocolSum(protocolBuff, protocolCount);

        if(protocolBuff[protocolCount-1] == sum)
        {
            memcpy(data, protocolBuff, tmpCount);
            *len = tmpCount;
            protocolFlag = 0;

            protocolCount = 0;
            debugCount = 0;
            lastData = 0;

            return 0;
        }
        else
        {
            return -2;
        }
    }

    return 1;
}

#endif 

/**
 *******************************************************************************
 * @brief       串口发送数据
 * @param       [in/out]  rb      输入数据地址
 * @param       [in/out]  data    输出数据地址
 * @param       [in/out]  len     输出数据长度
 * @return      [in/out]  bool    返回状态
 * @note        None
 *******************************************************************************
 */
bool GIZWITS_UART_WRITE( uint8 *data, uint8 len )
{
    uint8 i,j;
    
    gizwitsProtocol.protocolTxBuff[0] = data[0];
    gizwitsProtocol.protocolTxBuff[1] = data[1];
    
    for( i=2,j=2; i<len; i++ )
    {
        gizwitsProtocol.protocolTxBuff[j] = data[i];
        
        if( gizwitsProtocol.protocolTxBuff[j++] == 0xFF )
        {
            gizwitsProtocol.protocolTxBuff[j++] = 0x55;
        }
    }
    
    HalUARTWrite(HAL_UART_PORT_0,(uint8 *)&gizwitsProtocol.protocolTxBuff,j);
    
    return true;
}

/**
 *******************************************************************************
 * @brief       根据协议生成“控制型事件”
 * @param       [in/out]  issuedData     控制型数据
 * @param       [in/out]  info           事件队列
 * @param       [in/out]  dataPoints     数据点数据
 * @return      [in/out]  bool           返回状态
 * @note        None
 *******************************************************************************
 */
static bool gizDataPoint2Event(gizwitsIssued_t *issuedData, eventInfo_t *info, dataPoint_t *dataPoints)
{
#if USE_MYPROTOCOL_DEBUG
    if((NULL == issuedData) || (NULL == info) ||(NULL == dataPoints))
    {
        GIZWITS_LOG("gizDataPoint2Event Error , Illegal Param\n");
        return false;
    }
#endif

    /** 大于1字节做位序转换 **/
    if(sizeof(issuedData->attrFlags) > 1)
    {
        if(false == gizByteOrderExchange((uint8 *)&issuedData->attrFlags,sizeof(attrFlags_t)))
        {
            GIZWITS_LOG("gizByteOrderExchange Error\n");
            return false;
        }
    }
    if(0x01 == issuedData->attrFlags.flagPacket)
    {
        info->event[info->num++] = EVENT_PACKET;
        memcpy((uint8 *)dataPoints->valuePacket,issuedData->attrVals.valuePacket,sizeof(issuedData->attrVals.valuePacket));
    }

    return true;
}

///**
// *******************************************************************************
// * @brief       对比当前数据和上次数据
// * @param       [in/out]  cur     当前数据点数据
// * @param       [in/out]  last    上次数据点数据
// * @return      [in/out]  bool    返回状态
// * @note        None
// *******************************************************************************
// */
//static bool gizCheckReport(dataPoint_t *cur, dataPoint_t *last)
//{
//#if USE_MYPROTOCOL_DEBUG
//    if((NULL == cur) || (NULL == last))
//    {
//        GIZWITS_LOG("gizCheckReport Error , Illegal Param\n");
//        return false;
//    }
//#endif
//
//    if(!memcmp(&cur->valuePacket, &last->valuePacket, sizeof(dataPoint_t)))
//    {
//        GIZWITS_LOG("valueLED_OnOff Changed\n");
//        return true;
//    }
//
//    return false;
//}

/**
 *******************************************************************************
 * @brief       用户数据点数据转换为机智云上报数据点数据
 * @param       [in/out]  dataPoints      用户数据点数据地址
 * @param       [in/out]  devStatusPtr    机智云上报数据点数据地址
 * @return      [in/out]  bool    返回状态
 * @note        None
 *******************************************************************************
 */
static bool gizDataPoints2ReportData(dataPoint_t *dataPoints , devStatus_t *devStatusPtr)
{
#if USE_MYPROTOCOL_DEBUG
    if((NULL == dataPoints) || (NULL == devStatusPtr))
    {
        GIZWITS_LOG("gizDataPoints2ReportData Error , Illegal Param\n");
        return false;
    }
#endif

    memcpy((uint8 *)devStatusPtr->valuePacket,(uint8 *)&dataPoints->valuePacket,sizeof(dataPoints->valuePacket));
    
    return true;
}

/**
 *******************************************************************************
 * @brief       协议头初始化
 * @param       [in/out]  head      协议头指针
 * @return      [in/out]  bool    返回状态
 * @note        None
 *******************************************************************************
 */
static bool gizProtocolHeadInit(protocolHead_t *head)
{
#if USE_MYPROTOCOL_DEBUG
    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: gizProtocolHeadInit head is empty \n");
        return false;
    }
#endif

    memset((uint8 *)head, 0, sizeof(protocolHead_t));
    head->head[0] = 0xFF;
    head->head[1] = 0xFF;

    return true;
}

/**
 *******************************************************************************
 * @brief       协议ACK校验处理函数
 * @param       [in/out]  data    数据地址
 * @param       [in/out]  len     数据长度
 * @return      [in/out]  bool    返回状态
 * @note        None
 *******************************************************************************
 */
static bool gizProtocolWaitAck(uint8 *data, uint32 len)
{
#if USE_MYPROTOCOL_DEBUG
    if(NULL == data)
    {
        GIZWITS_LOG("gizProtocolWaitAck ERROR: data is empty \n");
        return false;
    }
#endif

    memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    memcpy((uint8 *)gizwitsProtocol.waitAck.buf, data, len);
    gizwitsProtocol.waitAck.dataLen = (int16)len;
    
    gizwitsProtocol.waitAck.flag = 1;
    gizwitsProtocol.waitAck.sendTime = gizGetTimerCount();

    return true;
}

/**
 *******************************************************************************
 * @brief       协议数据重发
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        校验超时且满足重发次数限制的协议数据进行重发
 *******************************************************************************
 */
static void gizProtocolResendData( void )
{
    if(0 == gizwitsProtocol.waitAck.flag)
    {
        return;
    }

    GIZWITS_UART_WRITE(gizwitsProtocol.waitAck.buf, gizwitsProtocol.waitAck.dataLen);

    gizwitsProtocol.waitAck.sendTime = gizGetTimerCount();
}

/**
 *******************************************************************************
 * @brief       清除ACK协议报文
 * @param       [in/out]  head    协议头地址
 * @return      [in/out]  void
 * @note        校验超时且满足重发次数限制的协议数据进行重发
 *******************************************************************************
 */
static bool gizProtocolWaitAckCheck(protocolHead_t *head)
{
    protocolHead_t *waitAckHead = (protocolHead_t *)gizwitsProtocol.waitAck.buf;

#if USE_MYPROTOCOL_DEBUG
    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: data is empty \n");
        return false;
    }
#endif

    if(waitAckHead->cmd+1 == head->cmd)
    {
        memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    }

    return true;
}

/**
 *******************************************************************************
 * @brief       发送通用协议报文数据
 * @param       [in/out]  head    协议头地址
 * @return      [in/out]  bool    执行结果
 * @note        NONE
 *******************************************************************************
 */
static bool gizProtocolCommonAck(protocolHead_t *head)
{
    protocolCommon_t ack;
	
#if USE_MYPROTOCOL_DEBUG
    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: gizProtocolCommonAck data is empty \n");
        return false;
    }
#endif
	
    memcpy((uint8 *)&ack, (uint8 *)head, sizeof(protocolHead_t));
    ack.head.cmd = ack.head.cmd+1;
    ack.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
    ack.sum = gizProtocolSum((uint8 *)&ack, sizeof(protocolCommon_t));

    GIZWITS_UART_WRITE((uint8 *)&ack, sizeof(protocolCommon_t));

    return true;
}

/**
 *******************************************************************************
 * @brief       ACK逻辑处理函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        发送后的协议数据进行超时判断，超时200ms进行重发，重发上限为三次
 *******************************************************************************
 */
static void gizProtocolAckHandle( void )
{
    if(1 == gizwitsProtocol.waitAck.flag)
    {
        if(SEND_MAX_NUM >= gizwitsProtocol.waitAck.num)
        {
            //300ms未收到ACK重发
            if(SEND_MAX_TIME < (gizGetTimerCount() - gizwitsProtocol.waitAck.sendTime))
            {
                gizProtocolResendData();
                gizwitsProtocol.waitAck.num++;
            }
        }
        else
        {
            memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
        }
    }
}

/**
 *******************************************************************************
 * @brief       协议4.1 WiFi模组请求设备信息
 * @param       [in/out]  head    待发送的协议报文数据
 * @return      [in/out]  bool    执行结果
 * @note        协议4.1 WiFi模组请求设备信息
 *******************************************************************************
 */
static bool gizProtocolGetDeviceInfo(protocolHead_t * head)
{
    protocolDeviceInfo_t deviceInfo;
	
#if USE_MYPROTOCOL_DEBUG
    if(NULL == head)
    {
        GIZWITS_LOG("gizProtocolGetDeviceInfo Error , Illegal Param\n");
        return false;
    }
#endif

    gizProtocolHeadInit((protocolHead_t *)&deviceInfo);
    deviceInfo.head.cmd = ACK_GET_DEVICE_INFO;
    deviceInfo.head.sn = head->sn;
    memcpy((uint8 *)deviceInfo.protocolVer, protocol_VERSION, 8);
    memcpy((uint8 *)deviceInfo.p0Ver, P0_VERSION, 8);
    memcpy((uint8 *)deviceInfo.softVer, SOFTWARE_VERSION, 8);
    memcpy((uint8 *)deviceInfo.hardVer, HARDWARE_VERSION, 8);
    memcpy((uint8 *)deviceInfo.productKey, PRODUCT_KEY, 32);
	memset((uint8 *)deviceInfo.devAttr, 0, 8);
    deviceInfo.devAttr[7] |= DEV_IS_GATEWAY<<0;
    deviceInfo.devAttr[7] |= (0x01<<1);
    deviceInfo.ninableTime = gizProtocolExchangeBytes(NINABLETIME);
    deviceInfo.head.len = gizProtocolExchangeBytes(sizeof(protocolDeviceInfo_t)-4);
    deviceInfo.sum = gizProtocolSum((uint8 *)&deviceInfo, sizeof(protocolDeviceInfo_t));

    GIZWITS_UART_WRITE((uint8 *)&deviceInfo, sizeof(protocolDeviceInfo_t));

    return true;
}

/**
 *******************************************************************************
 * @brief       协议4.1 WiFi模组请求设备信息
 * @param       [in/out]  head     协议头指针
 * @param       [in/out]  error    非法消息通知类型
 * @return      [in/out]  bool     执行结果
 * @note        协议4.7 非法消息通知 的处理
 *******************************************************************************
 */
static bool gizProtocolErrorCmd(protocolHead_t *head,errorPacketsType_t error)
{
    protocolErrorType_t errorType;
	
#if USE_MYPROTOCOL_DEBUG
    if(NULL == head)
    {
        GIZWITS_LOG("gizProtocolErrorCmd Error , Illegal Param\n");
        return false;
    }
#endif
    gizProtocolHeadInit((protocolHead_t *)&errorType);
    errorType.head.cmd = ACK_ERROR_PACKAGE;
    errorType.head.sn = head->sn;
    
    errorType.head.len = gizProtocolExchangeBytes(sizeof(protocolErrorType_t)-4);
    errorType.error = error;
    errorType.sum = gizProtocolSum((uint8 *)&errorType, sizeof(protocolErrorType_t));
    
    GIZWITS_UART_WRITE((uint8 *)&errorType, sizeof(protocolErrorType_t));

    return true;
}

/**
 *******************************************************************************
 * @brief       对应协议 4.13 接收返回的网络时间 
 * @param       [in/out]  head    协议头地址
 * @return      [in/out]  bool    执行结果
 * @note        None
 *******************************************************************************
 */
static bool gizProtocolNTP(protocolHead_t *head)
{
#if USE_MYPROTOCOL_DEBUG
    if(NULL == head)
    {
        GIZWITS_LOG("ERROR: NTP is empty \n");
        return false;
    }
#endif
    memcpy((uint8 *)&gizwitsProtocol.TimeNTP,(uint8 *)&((protocolUTT_t *)head)->time, sizeof(gizwitsProtocol.TimeNTP));
    gizwitsProtocol.TimeNTP.year = gizProtocolExchangeBytes(gizwitsProtocol.TimeNTP.year);
    gizwitsProtocol.TimeNTP.ntp = gizExchangeWord(gizwitsProtocol.TimeNTP.ntp);

//    gizwitsProtocol.NTPEvent.event[gizwitsProtocol.NTPEvent.num] = WIFI_NTP;
//    gizwitsProtocol.NTPEvent.num++;
    
//    gizwitsProtocol.issuedFlag = GET_NTP_TYPE;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       设备MCU重置WiFi模组
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void gizProtocolReboot( void )
{
	GIZWITS_LOG("gizProtocolReboot\n");
	
    Onboard_wait(600);
   
    Onboard_soft_reset();
}

/**
 *******************************************************************************
 * @brief       协议 4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化
 * @param       [in/out]  status    WiFi模组工作状态
 * @return      [in/out]  bool      执行结果
 * @note        None
 *******************************************************************************
 */
static bool gizProtocolModuleStatus(protocolWifiStatus_t *status)
{
#if USE_MYPROTOCOL_DEBUG
    if(NULL == status)
    {
        GIZWITS_LOG("gizProtocolModuleStatus Error , Illegal Param\r\n");
        return false;
    }
#endif
    status->status.value = gizProtocolExchangeBytes(status->status.value);
   
    //OnBoarding mode status
    if(1 == status->status.types.onboarding)
    {
        if(1 == status->status.types.softap)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
            gizwitsProtocol.wifiStatusEvent.num++;
			gizwitsProtocol.wifiStatusData.softap = 1;
            GIZWITS_LOG("on softap \n");
        }
		else
		{
			gizwitsProtocol.wifiStatusData.softap = 0;
		}

        if(1 == status->status.types.station)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_AIRLINK;
            gizwitsProtocol.wifiStatusEvent.num++;
			gizwitsProtocol.wifiStatusData.station = 1;
            GIZWITS_LOG("on AirLink \n");
        }
		else
		{
			gizwitsProtocol.wifiStatusData.station = 0;
		}
		
		gizwitsProtocol.wifiStatusData.onboarding = 1;
    }
    else
    {
        if(1 == status->status.types.softap)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
            gizwitsProtocol.wifiStatusEvent.num++;
			gizwitsProtocol.wifiStatusData.softap = 1;
            GIZWITS_LOG("on softap \n");
        }
		else
		{
			gizwitsProtocol.wifiStatusData.softap = 0;
		}
		
        if(1 == status->status.types.station)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_STATION;
            gizwitsProtocol.wifiStatusEvent.num++;
			gizwitsProtocol.wifiStatusData.station = 1;
            GIZWITS_LOG("on AirLink \n");
        }
		else
		{
			gizwitsProtocol.wifiStatusData.station = 0;
		}
		
		gizwitsProtocol.wifiStatusData.onboarding = 0;
    }

    //binding mode status
    if(1 == status->status.types.binding)
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_BINDING;
        gizwitsProtocol.wifiStatusEvent.num++;
		gizwitsProtocol.wifiStatusData.binding = 1;
        GIZWITS_LOG("in binding \n");
    }
    else
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_BINDING;
        gizwitsProtocol.wifiStatusEvent.num++;
		gizwitsProtocol.wifiStatusData.binding = 0;
        GIZWITS_LOG("out binding \n");
    }

    //router status
    if(1 == status->status.types.con_route)
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_ROUTER;
        gizwitsProtocol.wifiStatusEvent.num++;
		gizwitsProtocol.wifiStatusData.con_route = 1;
        GIZWITS_LOG("connected router \n");
    }
    else
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_ROUTER;
        gizwitsProtocol.wifiStatusEvent.num++;
		gizwitsProtocol.wifiStatusData.con_route = 0;
        GIZWITS_LOG("disconnected router \n"); 
    }

    //M2M server status
    if(1 == status->status.types.con_m2m)
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_M2M;
        gizwitsProtocol.wifiStatusEvent.num++;
        gizwitsProtocol.wifiStatusData.con_m2m = 1;
        GIZWITS_LOG("connected m2m \n");
    }
    else
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_M2M;
        gizwitsProtocol.wifiStatusEvent.num++;
        gizwitsProtocol.wifiStatusData.con_m2m = 0;
        GIZWITS_LOG("disconnected m2m \n");
    }

    //APP status
    if(1 == status->status.types.app)
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_APP;
        gizwitsProtocol.wifiStatusEvent.num++;
        GIZWITS_LOG("app connect \n");
    }
    else
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_APP;
        gizwitsProtocol.wifiStatusEvent.num++;
        gizwitsProtocol.wifiStatusData.app = 0;
        GIZWITS_LOG("no app connect \n");
    }

   //test mode status
   if(1 == status->status.types.test)
   {
       gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_TESTMODE;
       gizwitsProtocol.wifiStatusEvent.num++;
	   gizwitsProtocol.wifiStatusData.test = 1;
       GIZWITS_LOG("WiFi status: in test mode\r\n");
   }
   else
   {
       gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_TESTMODE;
       gizwitsProtocol.wifiStatusEvent.num++;
	   gizwitsProtocol.wifiStatusData.test = 0;
       GIZWITS_LOG("WiFi status: out test mode\r\n");
   }
        
    gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_RSSI;
    gizwitsProtocol.wifiStatusEvent.num++;
    gizwitsProtocol.wifiStatusData.rssi = status->status.types.rssi;

    gizwitsProtocol.issuedFlag = 2;

    return true;
}

/**
 *******************************************************************************
 * @brief       协议下发数据返回ACK
 * @param       [in/out] head       协议头指针
 * @param       [in/out] data       数据地址
 * @param       [in/out] len        数据长度
 * @return      [in/out] bool       执行结果
 * @note        None
 *******************************************************************************
 */
static bool gizProtocolIssuedDataAck(protocolHead_t *head, uint8 *data, uint8 len)
{
    uint8 *ptrData = NULL;
    uint8 dataLen = 0;
    protocolReport_t protocolReport;
    protocolCommon_t protocolCommon;
    protocolP0Head_t *p0Head = (protocolP0Head_t *)head;
	
#if USE_MYPROTOCOL_DEBUG
    if((NULL == head)||(NULL == data))
    {
        GIZWITS_LOG("gizProtocolIssuedDataAck Error , Illegal Param\n");
        return false;
    }
#endif

    if(0 == len)
    {
        gizProtocolHeadInit((protocolHead_t *)&protocolCommon);
        protocolCommon.head.cmd = head->cmd+1;
        protocolCommon.head.sn = head->sn;
        protocolCommon.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
        protocolCommon.sum = gizProtocolSum((uint8 *)&protocolCommon, sizeof(protocolCommon_t));
        
        ptrData = (uint8 *)&protocolCommon;
        dataLen = sizeof(protocolCommon_t);
    }
    else
    {
        gizProtocolHeadInit((protocolHead_t *)&protocolReport);
        protocolReport.head.cmd = p0Head->head.cmd+1;
        protocolReport.head.sn = p0Head->head.sn;
        protocolReport.head.len = gizProtocolExchangeBytes(sizeof(protocolP0Head_t)+len+1-4);
        protocolReport.action = p0Head->action+1;

        memcpy((uint8 *)&protocolReport.reportData, data, len);
        protocolReport.sum = gizProtocolSum((uint8 *)&protocolReport, sizeof(protocolReport_t));
        
        ptrData = (uint8 *)&protocolReport;
        dataLen = sizeof(protocolReport_t);
    }
    
    GIZWITS_UART_WRITE(ptrData, dataLen);

    return true;
}

/**
 *******************************************************************************
 * @brief       上报数据
 * @param       [in/out]  action     PO cmd
 * @param       [in/out]  data       数据地址
 * @param       [in/out]  len        数据长度
 * @return      [in/out]  bool       执行结果
 * @note        None
 *******************************************************************************
 */
static bool gizReportData(uint8 action, uint8 *data, uint32 len)
{
    protocolReport_t protocolReport;
	
#if USE_MYPROTOCOL_DEBUG
    if(NULL == data)
    {
        GIZWITS_LOG("gizReportData Error , Illegal Param\n");
        return false;
    }
#endif

    gizProtocolHeadInit((protocolHead_t *)&protocolReport);
    protocolReport.head.cmd = CMD_REPORT_P0;
    protocolReport.head.sn = gizwitsProtocol.sn++;
    protocolReport.action = action;
    protocolReport.head.len = gizProtocolExchangeBytes(sizeof(protocolReport_t)-4);
    memcpy((gizwitsReport_t *)&protocolReport.reportData, (gizwitsReport_t *)data,len);
    protocolReport.sum = gizProtocolSum((uint8 *)&protocolReport, sizeof(protocolReport_t));
    
    GIZWITS_UART_WRITE((uint8 *)&protocolReport, sizeof(protocolReport_t));

    gizProtocolWaitAck((uint8 *)&protocolReport, sizeof(protocolReport_t));

    return true;
}

/**
 *******************************************************************************
 * @brief       接收协议报文并进行相应的协议处理
 * @param       [in/out]  inData     输入的协议数据
 * @param       [in/out]  inLen      输入数据的长度
 * @param       [in/out]  outData    输出的协议数据
 * @param       [in/out]  outLen     输出数据的长度
 * @return      [in/out]  bool       执行结果
 * @note        Wifi模组接收来自云端或APP端下发的相关协议数据发送到MCU端，经过协议报文解析后
 * @note        将相关协议数据传入次函数，进行下一步的协议处理
 *******************************************************************************
 */
static bool gizProtocolIssuedProcess(uint8 *inData, uint8 inLen, uint8 *outData, uint8 *outLen)
{
    protocolReport_t *protocolIssuedData = (protocolReport_t *)inData;
    uint8 issuedAction = 0;
    issuedAction = protocolIssuedData->action;
	
#if USE_MYPROTOCOL_DEBUG
    if((NULL == inData)||(NULL == outData)||(NULL == outLen))
    {
		GIZWITS_LOG("gizProtocolIssuedProcess Error , Illegal Param\n");
        return false;
    }
#endif  

    memset((uint8 *)&gizwitsProtocol.issuedProcessEvent, 0, sizeof(eventInfo_t));
    switch(issuedAction)
    {
        case ACTION_CONTROL_DEVICE:
            gizDataPoint2Event((gizwitsIssued_t *)(inData+sizeof(protocolP0Head_t)), &gizwitsProtocol.issuedProcessEvent,&gizwitsProtocol.gizCurrentDataPoint);
            gizwitsProtocol.issuedFlag = 1;
            outData = NULL;
            *outLen = 0;
            break;
        case ACTION_READ_DEV_STATUS:
            gizDataPoints2ReportData(&gizwitsProtocol.gizLastDataPoint,&gizwitsProtocol.reportData.devStatus);
            //memcpy((uint8 *)&gizwitsProtocol.gizLastDataPoint, (uint8 *)currentData, sizeof(dataPoint_t));

            memcpy(outData, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(gizwitsReport_t));
            *outLen = sizeof(gizwitsReport_t);
            break;
        case ACTION_W2D_TRANSPARENT_DATA:
//           memcpy(gizwitsProtocol.transparentBuff, inData+sizeof(protocolP0Head_t), inLen-sizeof(protocolP0Head_t)-1);
//           gizwitsProtocol.transparentLen = inLen - sizeof(protocolP0Head_t) - 1;
//           gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = TRANSPARENT_DATA;
//           gizwitsProtocol.issuedProcessEvent.num++;
//           gizwitsProtocol.issuedFlag = 3;
           break;
        default:
            break;
    }

    return true;
}

/**
 *******************************************************************************
 * @brief       gizwits协议初始化接口
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        用户调用该接口可以完成Gizwits协议相关初始化（包括协议相关定时器、串口的初始化）
 * @note        用户可在在此接口内完成数据点的初始化状态设置
 *******************************************************************************
 */
void gizwitsInit( void )
{
    pRb.rbCapacity = MAX_PACKAGE_LEN;
    pRb.rbBuff = (uint8 *)&gizwitsProtocol.protocolRxBuff;
    rbCreate(&pRb);

    memset((uint8 *)&gizwitsProtocol, 0, sizeof(gizwitsProtocol_t));
}

/**
 *******************************************************************************
 * @brief       WiFi配置接口
 * @param       [in/out]  mode    配置模式选择
 * @param                         0x00: 模组复位
 * @param						  0x01: SoftAp模式
 * @param						  0x02: AirLink模式
 * @param						  0x03: 产测模式模式
 * @param						  0x04: 允许用户绑定设备
 * @return      [in/out]  bool    执行结果
 * @note        用户可以调用该接口使WiFi模组进入相应的配置模式或者复位模组
 * @note        用户可在在此接口内完成数据点的初始化状态设置
 *******************************************************************************
 */
bool gizwitsSetMode(uint8 mode)
{
    protocolCfgMode_t cfgMode;
    protocolCommon_t setDefault;

    switch(mode)
    {
        case WIFI_RESET_MODE:
            gizProtocolHeadInit((protocolHead_t *)&setDefault);
            setDefault.head.cmd = CMD_SET_DEFAULT;
            setDefault.head.sn = gizwitsProtocol.sn++;
            setDefault.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
            setDefault.sum = gizProtocolSum((uint8 *)&setDefault, sizeof(protocolCommon_t));
            GIZWITS_UART_WRITE((uint8 *)&setDefault, sizeof(protocolCommon_t));
            gizProtocolWaitAck((uint8 *)&setDefault, sizeof(protocolCommon_t));   
            break;
        case WIFI_SOFTAP_MODE:
            gizProtocolHeadInit((protocolHead_t *)&cfgMode);
            cfgMode.head.cmd = CMD_WIFI_CONFIG;
            cfgMode.head.sn = gizwitsProtocol.sn++;
            cfgMode.cfgMode = mode;
            cfgMode.head.len = gizProtocolExchangeBytes(sizeof(protocolCfgMode_t)-4);
            cfgMode.sum = gizProtocolSum((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
            GIZWITS_UART_WRITE((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
            gizProtocolWaitAck((uint8 *)&cfgMode, sizeof(protocolCfgMode_t)); 
            break;
        case WIFI_AIRLINK_MODE:
            gizProtocolHeadInit((protocolHead_t *)&cfgMode);
            cfgMode.head.cmd = CMD_WIFI_CONFIG;
            cfgMode.head.sn = gizwitsProtocol.sn++;
            cfgMode.cfgMode = mode;
            cfgMode.head.len = gizProtocolExchangeBytes(sizeof(protocolCfgMode_t)-4);
            cfgMode.sum = gizProtocolSum((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
            GIZWITS_UART_WRITE((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
            gizProtocolWaitAck((uint8 *)&cfgMode, sizeof(protocolCfgMode_t)); 
            break;
        case WIFI_PRODUCTION_TEST:
            gizProtocolHeadInit((protocolHead_t *)&setDefault);
            setDefault.head.cmd = CMD_PRODUCTION_TEST;
            setDefault.head.sn = gizwitsProtocol.sn++;
            setDefault.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
            setDefault.sum = gizProtocolSum((uint8 *)&setDefault, sizeof(protocolCommon_t));
            GIZWITS_UART_WRITE((uint8 *)&setDefault, sizeof(protocolCommon_t));
            gizProtocolWaitAck((uint8 *)&setDefault, sizeof(protocolCommon_t));
            break;
        case WIFI_NINABLE_MODE:
            gizProtocolHeadInit((protocolHead_t *)&setDefault);
            setDefault.head.cmd = CMD_NINABLE_MODE;
            setDefault.head.sn = gizwitsProtocol.sn++;
            setDefault.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
            setDefault.sum = gizProtocolSum((uint8 *)&setDefault, sizeof(protocolCommon_t));
            GIZWITS_UART_WRITE((uint8 *)&setDefault, sizeof(protocolCommon_t));
            gizProtocolWaitAck((uint8 *)&setDefault, sizeof(protocolCommon_t));
            break;
        default:
            GIZWITS_LOG("gizwitsSetMode ERROR: CfgMode error!\n");
            break;
    }

    return true;
}

/**
 *******************************************************************************
 * @brief       MCU请求获取网络时间
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        用户可以调用该接口获取网络时间
 *******************************************************************************
 */
bool gizwitsGetNTP( void )
{
    protocolCommon_t packet;

    gizProtocolHeadInit((protocolHead_t *)&packet);
   
	packet.head.cmd = CMD_GET_NTP;
    packet.head.sn = gizwitsProtocol.sn++;
    packet.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
    packet.sum = gizProtocolSum((uint8 *)&packet, sizeof(protocolCommon_t));
    
	GIZWITS_UART_WRITE((uint8 *)&packet, sizeof(protocolCommon_t));
    
    gizProtocolWaitAck((uint8 *)&packet, sizeof(protocolCommon_t));

    return true;
}
 
/**
 *******************************************************************************
 * @brief       设置APP为连接状态
 * @param       [in/out]  void
 * @return      [in/out]  bool    连接状态
 * @note        用户可以调用检测WIFI模块与云端之间的连接状态
 *******************************************************************************
 */
bool setGizwitsAPPStatus( void )
{
    gizwitsProtocol.wifiStatusData.app = 1;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       获取APP连接状态
 * @param       [in/out]  void
 * @return      [in/out]  bool    连接状态
 * @note        用户可以调用检测WIFI模块与APP之间的连接状态
 *******************************************************************************
 */
bool getGizwitsAPPStatus( void )
{
    return (gizwitsProtocol.wifiStatusData.app == 1) ? (true):(false);
}

/**
 *******************************************************************************
 * @brief       设置M2M为连接状态
 * @param       [in/out]  void
 * @return      [in/out]  bool    连接状态
 * @note        用户可以调用检测WIFI模块与云端之间的连接状态
 *******************************************************************************
 */
bool setGizwitsM2MStatus( void )
{
    gizwitsProtocol.wifiStatusData.app = 1;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       获取M2M连接状态
 * @param       [in/out]  void
 * @return      [in/out]  bool    连接状态
 * @note        用户可以调用检测WIFI模块与云端之间的连接状态
 *******************************************************************************
 */
bool getGizwitsM2MStatus( void )
{
    return (gizwitsProtocol.wifiStatusData.con_m2m == 1) ? (true):(false);
}

/**
 *******************************************************************************
 * @brief       刷新本地时间数据
 * @param       [in/out]  *packet    带有时间信息的数据包
 * @return      [in/out]  bool       返回状态
 * @note        用户可以调用该接口刷新本地时间
 *******************************************************************************
 */
bool gizUpdateTime( protocolUTT_t *packet )
{
    gizwitsProtocol.TimeNTP.year   = packet->time.year;
    gizwitsProtocol.TimeNTP.month  = packet->time.month;
    gizwitsProtocol.TimeNTP.day    = packet->time.day;
    gizwitsProtocol.TimeNTP.hour   = packet->time.hour;
    gizwitsProtocol.TimeNTP.minute = packet->time.minute;
    gizwitsProtocol.TimeNTP.second = packet->time.second;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       获取网络时间
 * @param       [in/out]  void 
 * @return      [in/out]  user_time    设备时间
 * @note        用户可以调用该接口刷新本地时间
 *******************************************************************************
 */
user_time gizwitsNTPConverUserTime( void )
{
	user_time time;
	
	time.year   = gizwitsProtocol.TimeNTP.year;
    time.month  = gizwitsProtocol.TimeNTP.month;
    time.day    = gizwitsProtocol.TimeNTP.day;
    time.hour   = gizwitsProtocol.TimeNTP.hour;
    time.minute = gizwitsProtocol.TimeNTP.minute;
    time.second = gizwitsProtocol.TimeNTP.second;
    time.week   = app_cal_week(&time);
	
    return time;
}

/**
 *******************************************************************************
 * @brief       写入时间
 * @param       [in/out]  void 
 * @return      [in/out]  user_time    设备时间
 * @note        用户可以调用该接口刷新本地时间
 *******************************************************************************
 */
bool gizwitsWrTime( user_time *time )
{
	gizwitsProtocol.TimeNTP.year   = time->year;
    gizwitsProtocol.TimeNTP.month  = time->month;
    gizwitsProtocol.TimeNTP.day    = time->day;
    gizwitsProtocol.TimeNTP.hour   = time->hour;
    gizwitsProtocol.TimeNTP.minute = time->minute;
    gizwitsProtocol.TimeNTP.second = time->second;
    gizwitsProtocol.TimeNTP.ntp    =  TimeConverTimeStamp(time);
	
    return true;
}

/**
 *******************************************************************************
 * @brief       机智云发送数据函数
 * @param       [in/out]  *packet 发送的数据包 
 * @return      [in/out]  bool    发送状态
 * @note        用户可以调用该接口刷新本地时间
 *******************************************************************************
 */
bool gizwitsSendData( void *packet )
{
#if USE_MYPROTOCOL_DEBUG
   if( packet == NULL )
   {
       return false;
   }
#endif

    gizDataPoints2ReportData((dataPoint_t *)packet,&gizwitsProtocol.reportData.devStatus);

    gizReportData(ACTION_REPORT_DEV_STATUS, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
//    gizReportData(ACTION_D2W_TRANSPARENT_DATA, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
    
	memcpy((uint8 *)&gizwitsProtocol.gizLastDataPoint, packet, sizeof(dataPoint_t));
        
    return true;
}

/**
 *******************************************************************************
 * @brief       协议处理函数
 * @param       [in/out]  void
 * @return      [in/out]  bool         返回状态
 * @note        该函数中完成了相应协议数据的处理及数据主动上报的相关操作
 *******************************************************************************
 */
bool gizwitsHandle( void )
{
	int8 ret = 0;
    uint16 protocolLen = 0;
    uint8 ackLen = 0;
    protocolHead_t *recvHead = NULL;

    /*重发机制*/
    gizProtocolAckHandle();
    ret = gizProtocolGetOnePacket(&pRb, (uint8 *)&gizwitsProtocol.protocolPacket, &protocolLen);

    if(0 == ret)
    {
        GIZWITS_LOG("Get Packet!\n");

        recvHead = (protocolHead_t *)gizwitsProtocol.protocolPacket;
        switch (recvHead->cmd)
        {
            case CMD_GET_DEVICE_INTO:
                gizProtocolGetDeviceInfo(recvHead);
                break;
            case CMD_ISSUED_P0:
                if(gizProtocolIssuedProcess((uint8 *)&gizwitsProtocol.protocolPacket, protocolLen, (uint8 *)&gizwitsProtocol.protocolAckBuff, &ackLen) == true)
                {
                    gizProtocolIssuedDataAck(recvHead, (uint8 *)&gizwitsProtocol.protocolAckBuff, ackLen);
                }
                break;
            case CMD_HEARTBEAT:
                gizProtocolCommonAck(recvHead);
                break;
            case CMD_WIFISTATUS:
                gizProtocolCommonAck(recvHead);
                gizProtocolModuleStatus((protocolWifiStatus_t *)recvHead);
                break;
            case ACK_REPORT_P0:
            case ACK_WIFI_CONFIG:
            case ACK_SET_DEFAULT:
                gizProtocolWaitAckCheck(recvHead);
                break;
            case CMD_MCU_REBOOT:
                gizProtocolCommonAck(recvHead);
                gizProtocolReboot();
                break;
            case CMD_ERROR_PACKAGE:
                GIZWITS_LOG("I SEND ERROR PACKET TO GIZWITS!\n");
                break;
			case ACK_PRODUCTION_TEST:
                gizProtocolWaitAckCheck(recvHead);
                GIZWITS_LOG("Ack PRODUCTION_MODE success \n");
                break;  
            case ACK_GET_NTP:
                gizProtocolWaitAckCheck(recvHead);
                gizProtocolNTP(recvHead);
                GIZWITS_LOG("Ack GET_UTT success \n");
                break;   
            default:
                gizProtocolErrorCmd(recvHead,ERROR_CMD);
                GIZWITS_LOG("ERROR: cmd code error!\n");
                break;
        }
    }
    else if(-2 == ret)
    {
        //校验失败，报告异常
        recvHead = (protocolHead_t *)gizwitsProtocol.protocolPacket;
        gizProtocolErrorCmd(recvHead,ERROR_ACK_SUM);
        GIZWITS_LOG("ERROR: check sum error!\n");
        return false;
    }
    
	switch(gizwitsProtocol.issuedFlag)
    {
        case ACTION_CONTROL_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8 *)&gizwitsProtocol.gizCurrentDataPoint, sizeof(dataPoint_t));
            memset((uint8 *)&gizwitsProtocol.issuedProcessEvent,0x0,sizeof(gizwitsProtocol.issuedProcessEvent));  
            break;
        case WIFI_STATUS_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.wifiStatusEvent, (uint8 *)&gizwitsProtocol.wifiStatusData, sizeof(moduleStatusInfo_t));
            memset((uint8 *)&gizwitsProtocol.wifiStatusEvent,0x0,sizeof(gizwitsProtocol.wifiStatusEvent));
            break;
        case ACTION_W2D_TRANSPARENT_TYPE:
//            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
//            gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8 *)gizwitsProtocol.transparentBuff, gizwitsProtocol.transparentLen);
            break;
        case GET_NTP_TYPE:
//            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
//            gizwitsEventProcess(&gizwitsProtocol.NTPEvent, (uint8 *)&gizwitsProtocol.TimeNTP, sizeof(gizwitsProtocol.TimeNTP));
//            memset((uint8 *)&gizwitsProtocol.NTPEvent,0x0,sizeof(gizwitsProtocol.NTPEvent));
            break;      
    }

    // if(0 == (gizGetTimerCount() % (100)))
    // {
        // GIZWITS_LOG("Info: 600S report data\n");
        // gizDataPoints2ReportData(currentData,&gizwitsProtocol.reportData.devStatus);
        // gizReportData(ACTION_REPORT_DEV_STATUS, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
        // memcpy((uint8 *)&gizwitsProtocol.gizLastDataPoint, (uint8 *)currentData, sizeof(dataPoint_t));
    // }    
	
    if( gizwitsProtocol.wifiStatusData.con_m2m == 1 )
    {
        if(0 == (gizGetTimerCount() % (UPDATE_NTP_TIME)))
        {
            gizwitsGetNTP();
        }
    }

    return true;
}

/** @}*/     /** myprotocol模块 */

/**********************************END OF FILE*********************************/
