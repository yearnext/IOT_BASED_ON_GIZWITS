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
#include "AF.h"
#include "onBoard.h"
#include <string.h>
#include "aps_groups.h"
#include "gizwits_protocol.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
 * @name Zigbee通讯配置
 * @{
 */

/** 簇的数量 */
#define DeviceClustersNum        (1)
#define DeviceCommClustersID     (0x0001)
#define DeviceEndPoint           (20)
#define DeviceDeviceID           (0x0001)
#define DeviceVersion            (0x00)
#define DeviceFlags              (0)
#define DeviceProfileID          (0x0F08)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** 端点描述符 */
static endPointDesc_t DeviceEpDesc;

/** 簇表 */
static const cId_t DeviceInClusterList[DeviceClustersNum] =
{
    DeviceCommClustersID,
};

/** 简单描述符 */
static const SimpleDescriptionFormat_t DeviceSimpleDesc =
{
    DeviceEndPoint, 
    DeviceProfileID,
    DeviceDeviceID,
    DeviceVersion,
    DeviceFlags,                  
    DeviceClustersNum,
    (cId_t *)&DeviceInClusterList,
    DeviceClustersNum,
    (cId_t *)&DeviceInClusterList,
};

/** 发送ID */
static uint8 DeviceTransID = 0;

/** 设备组别 */
static aps_Group_t DeviceGroup;

///** 目标地址 */
//static afAddrType_t DeviceDstAddr;

/**@} */

//static afAddrType_t MYPROTOCOL_DST_ADDR;

// 数据缓冲区
//static MYPROTOCOL_FORMAT_t tx_packet;
//static MYPROTOCOL_FORMAT_t rx_packet;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @name MYPROTOCOL 硬件通讯层
 * @{
 */
 
#if USE_MYPROTOCOL_DEBUG

/**
 *******************************************************************************
 * @brief       初始化函数
 * @param       [in/out]  *task_id    任务ID
 * @return      [in/out]  bool        返回状态
 * @note        None
 *******************************************************************************
 */
bool MyprotocolInit( uint8 *taskId )
{
	if( taskId == NULL )
	{
		return false;
	}
	
	/** 初始化发送ID */
    DeviceTransID = 0;
	
//	/** 初始化目标接收地址 */
//	memset(&DeviceDstAddr.addr,0,sizeof(DeviceDstAddr.addr));
//	DeviceDstAddr.addrMode = afAddr64Bit;
//    DeviceDstAddr.endPoint = DeviceEndPoint;
	
    /** 注册AF层应用对象 */
    DeviceEpDesc.endPoint = DeviceEndPoint;
    DeviceEpDesc.simpleDesc = (SimpleDescriptionFormat_t *)&DeviceSimpleDesc;
    DeviceEpDesc.task_id = taskId;
    DeviceEpDesc.latencyReq = noLatencyReqs;
    afRegister(&DeviceEpDesc);

    /** 创建一个组表 */
    DeviceGroup.ID = 0x0001;
    osal_memcpy( DeviceGroup.name, "Group 1", 7 );
    aps_AddGroup(DeviceEndPoint, &DeviceGroup);
	
	return true;
}

/**
 *******************************************************************************
 * @brief       输出调试信息程序
 * @param       [in/out]  *log    调试信息
 * @param       [in/out]  size    调试信息大小
 * @return      [in/out]  bool    返回状态
 * @note        None
 *******************************************************************************
 */
void MyprotocolPutLog( uint8 *log, uint16 size )
{
#if MYPROTOCOL_DEVICE_IS_COORD
    HalUARTWrite(HAL_UART_PORT_1,log,size);
#else
    HalUARTWrite(HAL_UART_PORT_0,log,size);
#endif
}

#else
/**
 *******************************************************************************
 * @brief       初始化函数
 * @param       [in/out]  *task_id    任务ID
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void MyprotocolInit( uint8 *taskId )
{
	/** 初始化发送ID */
    DeviceTransID = 0;
	
	/** 初始化目标接收地址 */
	memset(&DeviceDstAddr.addr.extaddr,0,sizeof(DeviceDstAddr.addr.extaddr));
	DeviceDstAddr.addrMode = afAddr64Bit;
    DeviceDstAddr.endPoint = DeviceEndPoint;
	
    /** 注册AF层应用对象 */
    DeviceEpDesc.endPoint = DeviceEndPoint;
    DeviceEpDesc.simpleDesc = (SimpleDescriptionFormat_t *)&DeviceSimpleDesc;
    DeviceEpDesc.task_id = taskId;
    DeviceEpDesc.latencyReq = noLatencyReqs;
    afRegister(&DeviceEpDesc);

    /** 创建一个组表 */
    DeviceGroup.ID = 0x0001;
    osal_memcpy( DeviceGroup.name, "Group 1", 7 );
    aps_AddGroup(DeviceEndPoint, &DeviceGroup);
}

#endif

#if 0
/**
 *******************************************************************************
 * @brief       计算用户数据的大小
 * @param       [in/out]  packet    数据包地址
 * @return      [in/out]  uint8     数据大小
 * @note        已进行加速化处理
 *******************************************************************************
 */
static uint8 MyprotocolCalUserDataSize( MYPROTOCOL_USER_DATA_t *packet )
{
    return MYPROTOCOL_USER_DATA_DATA_OFFSET + packet->len;
}

/**
 *******************************************************************************
 * @brief       计算整个数据包的大小
 * @param       [in/out]  packet    数据包地址
 * @return      [in/out]  uint8     数据大小
 * @note        已进行加速化处理
 *******************************************************************************
 */
static uint8 MyprotocolCalPacketDataSize( MYPROTOCOL_FORMAT_t *packet )
{
    return MYPROTOCOL_USER_DATA_OFFSET + MyprotocolCalUserDataSize( &packet->user_data );
}

/**
 *******************************************************************************
 * @brief       计算数据包的校验和
 * @param       [in/out]  packet    数据包地址
 * @return      [in/out]  checksum  校验和
 * @note        已进行加速化处理
 *******************************************************************************
 */
static uint8 MyprotocolCalChecksum( uint8 *packet )
{
    uint8 i;
    
    /** 计算有效数据数量 */
    uint8 num = MyprotocolCalPacketDataSize((MYPROTOCOL_FORMAT_t *)packet);
    
    uint8 checksum;

#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("cal check sum func intput param is invalid! \r\n");
        return 0;
    }
#endif

    // for( i=0, checksum=0; i<MYPROTOCOL_PACKET_SIZE-1; i++ )
    // {
        // checksum += packet[i];
    // }
    
    for( i=0, checksum=0; i<num; i++ )
    {
        checksum += packet[i];
    }
    
    return checksum;
}
#else 
/**
 *******************************************************************************
 * @brief       计算数据包的校验和
 * @param       [in/out]  packet    数据包地址
 * @return      [in/out]  checksum  校验和
 * @note        None
 *******************************************************************************
 */
static uint8 MyprotocolCalChecksum( uint8 *packet )
{
    uint8 i;
    uint8 checksum;

    for( i=0, checksum=0; i<MYPROTOCOL_PACKET_SIZE-1; i++ )
    {
        checksum += packet[i];
    }
    
    return checksum;
}

#endif
/**
 *******************************************************************************
 * @brief       校验数据包
 * @param       [in/out]  packet    数据包
 * @return      [in/out]  bool      校验结果
 * @note        None
 *******************************************************************************
 */
bool MyprotocolPacketCheck( void *packet )
{
    if( packet == NULL )
    {
#if USE_MYPROTOCOL_DEBUG
        MYPROTOCOL_LOG("check packet is invalid! \r\n");
#endif
        return false;
    }

    if( (((MYPROTOCOL_FORMAT_t *)packet)->sum) != MyprotocolCalChecksum((uint8 *)packet) )
    {
#if USE_MYPROTOCOL_DEBUG
        MYPROTOCOL_LOG("the packet's check sum is error! \r\n");
#endif
        return false;
    }
    
    return true;
} 

/**
 *******************************************************************************
 * @brief       检测W2D数据包发送目标是否为本机
 * @param       [in/out]  recPacket    数据包
 * @return      [in/out]  bool         校验结果
 * @note        None
 *******************************************************************************
 */
bool MyprotocolW2DRecDeviceCheck( MYPROTOCOL_FORMAT_t *recPacket )
{
#if MYPROTOCOL_DEVICE_IS_COORD
    return ( recPacket->device.device == MYPROTOCOL_DEVICE_COORD ) ? (true) : (false);
#else
    return (memcmp( &recPacket->device.mac, &aExtendedAddress, sizeof(aExtendedAddress) ) == 0) ? (true) : (false);
#endif
} 

/**
 *******************************************************************************
 * @brief       检测D2D数据包发送目标是否为本机
 * @param       [in/out]  recPacket    数据包
 * @return      [in/out]  bool         校验结果
 * @note        None
 *******************************************************************************
 */
bool MyprotocolD2DRecDeviceCheck( MYPROTOCOL_FORMAT_t *recPacket )
{
    if( recPacket->commtype == MYPROTOCOL_H2S_WAIT \
        || recPacket->commtype == MYPROTOCOL_H2S_ACK \
        || recPacket->commtype == MYPROTOCOL_S2H_WAIT \
        || recPacket->commtype == MYPROTOCOL_S2H_ACK )
    {
        return true;
    }
    
    return false;
} 
        
/**
 *******************************************************************************
 * @brief       发送D2D数据包
 * @param       [in/out]  ctx       上下文数据
 * @param       [in/out]  packet    创建数据包函数   
 * @return      [in/out]  bool      程序运行状态
 * @note        None
 *******************************************************************************
 */
bool MyprotocolD2DSendData( void *ctx, void *packet )
{
  afAddrType_t DeviceDstAddr;
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("MyprotocolD2DSendData intput param packet is invalid! \r\n");
        return false;
    }
#endif
        
    memset(&DeviceDstAddr,0,sizeof(DeviceDstAddr));
   
    if( ctx != NULL )
    {
        memcpy(&DeviceDstAddr.addr.extAddr,(uint8 *)ctx,sizeof(DeviceDstAddr.addr.extAddr));
        DeviceDstAddr.addrMode = afAddr64Bit;
    }
    else
    {
        DeviceDstAddr.addr.shortAddr = 0x0000;
        DeviceDstAddr.addrMode = afAddr16Bit;
    }
    
	DeviceDstAddr.endPoint = DeviceEndPoint;
    
    return AF_DataRequest(&DeviceDstAddr,
					      &DeviceEpDesc,
					      DeviceCommClustersID,
					      MYPROTOCOL_PACKET_SIZE,
					      (uint8 *)packet,
					      &DeviceTransID,
					      AF_DISCV_ROUTE,
					      AF_DEFAULT_RADIUS);
}

#if MYPROTOCOL_DEVICE_IS_COORD
/**
 *******************************************************************************
 * @brief       发送D2W数据包
 * @param       [in/out]  ctx       上下文数据
 * @param       [in/out]  packet    创建数据包函数   
 * @return      [in/out]  bool      程序运行状态
 * @note        None
 *******************************************************************************
 */
bool MyprotocolD2WSendData( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("MyprotocolD2WSendData intput param packet is invalid! \r\n");
        return false;
    }
#endif
	return gizwitsSendData(packet);
}

#endif

/**
 *******************************************************************************
 * @brief       Myprotocol 发送数据函数
 * @param       [in/out]  ctx            上下文数据(上一个数据包)
 * @param       [in/out]  dstaddr        发送目标的MAC地址  
 * @param       [in/out]  packet_func    创建数据包函数
 * @param       [in/out]  send_func      发送数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool MyprotocolSendData( void *ctx, void *dstaddr, packet_type packet_func, send_type send_func )
{
	MYPROTOCOL_FORMAT_t packet;
	
#if USE_MYPROTOCOL_DEBUG
    if( send_func == NULL )
    {
        MYPROTOCOL_LOG("MyprotocolSendData intput param send_func is invalid! \r\n");
        return false;
    }
#endif
    
    memset(&packet,0,sizeof(MYPROTOCOL_FORMAT_t));

    packet.device.device = MYPROTOCOL_DEVICE;
#if !MYPROTOCOL_DEVICE_IS_COORD
    memcpy(&packet.device.mac,&aExtendedAddress,sizeof(packet.device.mac));
#endif
    
    packet_func(ctx, &packet);
    
    packet.sum = MyprotocolCalChecksum((uint8 *)&packet);
     
    return send_func(dstaddr, &packet);
}

/**
 *******************************************************************************
 * @brief       Myprotocol 回复错误数据包
 * @param       [in/out]  recPacket      接收到的数据包
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool MyprotocolReplyErrPacket( MYPROTOCOL_FORMAT_t *recPacket )
{
    MYPROTOCOL_FORMAT_t packet;
    
#if USE_MYPROTOCOL_DEBUG
    if( recPacket == NULL )
    {
        MYPROTOCOL_LOG("MyprotocolReplyErrPacket intput param recPacket is invalid! \r\n");
        return false;
    }
#endif
    
//    memset(&packet,0,sizeof(MYPROTOCOL_FORMAT_t));

    packet.device.device = MYPROTOCOL_DEVICE;
    memcpy(&packet.device.mac,&aExtendedAddress,sizeof(packet.device.mac));
    
    if( recPacket->device.device != MYPROTOCOL_COMM_END \
        && recPacket->device.device != MYPROTOCOL_COMM_ERROR )
    {
        packet.commtype = MYPROTOCOL_COMM_ERROR;
    }
    else
    {
        return true;
    }
    
    packet.sum = MyprotocolCalChecksum((uint8 *)&packet);
    
    switch( recPacket->device.device )
    {
        // COORD 设备
        case MYPROTOCOL_W2D_WAIT:
        case MYPROTOCOL_D2W_ACK:
            return MyprotocolD2WSendData(NULL,&packet);
            break;
        case MYPROTOCOL_H2S_ACK:
        case MYPROTOCOL_H2S_WAIT:
            return MyprotocolD2DSendData(&recPacket->device.mac,&packet);
            break;
        default:
            return true;
            break;
    }
    
//    return false;
}

/**
 *******************************************************************************
 * @brief       Myprotocol 转发数据包函数
 * @param       [in/out]  ctx            上下文数据(上一个数据包)
 * @param       [in/out]  dstaddr        发送目标的MAC地址  
 * @param       [in/out]  packet_func    创建数据包函数
 * @param       [in/out]  send_func      发送数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool MyprotocolForwardData( void *dstaddr, void *packet, send_type send_func )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("MyprotocolSendData intput param packet is invalid! \r\n");
        return false;
    }
    else if( send_func == NULL )
    {
        MYPROTOCOL_LOG("MyprotocolSendData intput param send_func is invalid! \r\n");
        return false;
    }
    else
    {
        
    }
#endif

    return send_func(dstaddr, packet);
}

/**
 *******************************************************************************
 * @brief       Myprotocol 接收数据函数
 * @param       [in/out]  ctx            上下文数据(上一个数据包)
 * @param       [in/out]  receive        数据包解析函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool MyprotocolReceiveData( void *ctx, void *packet, receive_type error, receive_type receive )
{
    if( MyprotocolPacketCheck(packet) == false )
    {
        error(ctx, packet);
        return false;
    }
    else
    {
        receive(ctx, packet);
    }
    
    return true;
}

/**@} */     /** MYPROTOCOL 硬件层 */

/**
 * @name MYPROTOCOL 数据包函数
 * @{
 */
 
/**
 *******************************************************************************
 * @brief       通讯错误数据包
 * @param       [in/out]  ctx            上下文数据(上一个数据包)
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createCommErrorPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("comm error packet intput param is invalid! \r\n");
        return false;
    }
#endif 

    (((MYPROTOCOL_FORMAT_t *)packet)->commtype) = MYPROTOCOL_COMM_ERROR;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       通讯错误数据包
 * @param       [in/out]  ctx            上下文数据(上一个数据包)
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createCommEndPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("comm error packet intput param is invalid! \r\n");
        return false;
    }
#endif 

    (((MYPROTOCOL_FORMAT_t *)packet)->commtype) = MYPROTOCOL_COMM_END;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       设备发送心跳包
 * @param       [in/out]  ctx            上下文数据(上一个数据包)
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createDeviceTickPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("tick packet intput param is invalid! \r\n");
        return false;
    }
#endif    

    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_S2H_WAIT;
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.cmd = MYPROTOCOL_TICK_CMD; 
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.len = 0;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       网关响应设备心跳包
 * @param       [in/out]  ctx            上下文数据(上一个数据包)
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createDeviceTickAckPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("tick ack packet is invalid! \r\n");
        return false;
    }
#endif    

    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype      = MYPROTOCOL_S2H_ACK;
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.cmd = MYPROTOCOL_TICK_CMD; 
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.len = 0;
    ((MYPROTOCOL_FORMAT_t *)(packet))->device.device = ((MYPROTOCOL_FORMAT_t *)(ctx))->device.device;
    memcpy(&((MYPROTOCOL_FORMAT_t *)(packet))->device.mac, &((MYPROTOCOL_FORMAT_t *)(ctx))->device.mac,sizeof(((MYPROTOCOL_FORMAT_t *)(ctx))->device.mac));
    
//    memcpy(&(((MYPROTOCOL_FORMAT_t *)packet)->device), ctx, MyprotocolCalUserDataSize((MYPROTOCOL_USER_DATA_t *)ctx) );
    
    return true;
}

/**
 *******************************************************************************
 * @brief       从机向主机发送等待应答数据包
 * @param       [in/out]  ctx            用户数据
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createS2HWaitPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("S2H ack packet intput param packet is invalid! \r\n");
        return false;
    }
#endif 

    if( ctx != NULL )
    {
        memcpy(&((MYPROTOCOL_FORMAT_t *)packet)->user_data, ctx, sizeof(((MYPROTOCOL_FORMAT_t *)packet)->user_data) );
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_S2H_WAIT;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       从机向主机发送应答数据包
 * @param       [in/out]  ctx            用户数据
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createS2HAckPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("S2H ack packet intput param packet is invalid! \r\n");
        return false;
    }
#endif 

    if( ctx != NULL )
    {
        memcpy(&((MYPROTOCOL_FORMAT_t *)packet)->user_data, ctx, sizeof(((MYPROTOCOL_FORMAT_t *)packet)->user_data) );
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_S2H_ACK;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       主机向从机发送等待应答数据包
 * @param       [in/out]  ctx            用户数据
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createH2SWaitPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("H2S wait packet intput param packet is invalid! \r\n");
        return false;
    }
#endif 
    
    if( ctx != NULL )
    {
        memcpy(&((MYPROTOCOL_FORMAT_t *)packet)->user_data, ctx, sizeof(((MYPROTOCOL_FORMAT_t *)packet)->user_data) );
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_H2S_WAIT;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       主机向从机发送应答数据包
 * @param       [in/out]  ctx            用户数据
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createH2SAckPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("H2S ack packet intput param packet is invalid! \r\n");
        return false;
    }
#endif 

    if( ctx != NULL )
    {
        memcpy(&((MYPROTOCOL_FORMAT_t *)packet)->user_data, ctx, sizeof(((MYPROTOCOL_FORMAT_t *)packet)->user_data) );
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_H2S_ACK;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       设备向APP发送等待应答数据包
 * @param       [in/out]  ctx            用户数据
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createD2WWaitPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("D2W wait packet intput param packet is invalid! \r\n");
        return false;
    }
#endif 

    if( ctx != NULL )
    {
        memcpy(&((MYPROTOCOL_FORMAT_t *)packet)->user_data, ctx, sizeof(((MYPROTOCOL_FORMAT_t *)packet)->user_data) );
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_D2W_WAIT;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       设备向APP发送应答数据包
 * @param       [in/out]  ctx            用户数据
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createD2WAckPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("D2W ack packet intput param packet is invalid! \r\n");
        return false;
    }
#endif 

    if( ctx != NULL )
    {
        memcpy(&((MYPROTOCOL_FORMAT_t *)packet)->user_data, ctx, sizeof(((MYPROTOCOL_FORMAT_t *)packet)->user_data) );
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_D2W_ACK;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       APP向设备发送等待应答数据包
 * @param       [in/out]  ctx            用户数据
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createW2DWaitPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("W2D wait packet intput param packet is invalid! \r\n");
        return false;
    }
#endif 

    if( ctx != NULL )
    {
        memcpy(&((MYPROTOCOL_FORMAT_t *)packet)->user_data, ctx, sizeof(((MYPROTOCOL_FORMAT_t *)packet)->user_data) );
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_W2D_WAIT;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       APP向设备发送应答数据包
 * @param       [in/out]  ctx            用户数据
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createW2DAckPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("W2D ack packet intput param packet is invalid! \r\n");
        return false;
    }
#endif 

    if( ctx != NULL )
    {
        memcpy(&((MYPROTOCOL_FORMAT_t *)packet)->user_data, ctx, sizeof(((MYPROTOCOL_FORMAT_t *)packet)->user_data) );
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype = MYPROTOCOL_W2D_ACK;
    
    return true;
}

/**@} */     /** MYPROTOCOL 数据包函数 */

/** @}*/     /** myprotocol模块 */

/**********************************END OF FILE*********************************/
