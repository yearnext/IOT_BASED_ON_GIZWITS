 /**
 ******************************************************************************
  * @file       bsp_coord.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      bsp_coord 源文件
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
 * @defgroup COORD模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_coord.h"
#include "myprotocol.h"
#include "gizwits_protocol.h"
#include "devicelist.h"
#include "OnBoard.h"
#include <string.h>

/* Exported macro ------------------------------------------------------------*/
/**
 * @name Coord设备控制命令
 * @{
 */
#define COORD_LIST_CHANGE_CMD    (0x10)
#define COORD_RD_DEVICE_NUM_CMD  (0x11)
#define COORD_RD_DEVICE_INFO_CMD (0x12)
#define COORD_RD_LIST_ERROR_CMD  (0x13)
/**
 * @}
 */

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       初始化coord模块
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bspCoordInit( void )
{
    gizwitsInit();
}

/**
 *******************************************************************************
 * @brief       网关响应设备信息
 * @param       [in/out]  ctx            设备ID
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createDeviceListChangePacket( void *ctx, void *packet )
{    
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("Device Num Ack Packet info is invalid! \r\n");
        return false;
    }
#endif    

    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype          = MYPROTOCOL_D2W_WAIT;
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.cmd     = COORD_LIST_CHANGE_CMD; 
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[0] = deviceNumGet();
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.len     = 1;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       网关响应设备数量
 * @param       [in/out]  ctx            上下文数据(上一个数据包)
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createDeviceNumAckPacket( void *ctx, void *packet )
{
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("Device Num Ack Packet info is invalid! \r\n");
        return false;
    }
#endif    

    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype          = MYPROTOCOL_W2D_ACK;
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.cmd     = COORD_RD_DEVICE_NUM_CMD; 
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[0] = deviceNumGet();
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.len     = 1;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       网关响应设备信息
 * @param       [in/out]  ctx            设备ID
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createDeviceInfoAckPacket( void *ctx, void *packet )
{
    uint8 id = *((uint8 *)(ctx));
    MYPROTOCOL_DEVICE_INFO_t info; 
    MYPROTOCOL_DEVICE_INFO_t *device = &info;
    
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("Device Num Ack Packet info is invalid! \r\n");
        return false;
    }
    
    if( ctx == NULL )
    {
        MYPROTOCOL_LOG("Device Num is invalid! \r\n");
        return false;
    }
#endif    

    memset(&info, 0, sizeof(MYPROTOCOL_DEVICE_INFO_t));
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype      = MYPROTOCOL_W2D_ACK;
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.cmd = COORD_RD_DEVICE_INFO_CMD; 
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[0] = id;   
    
    if( deviceInfoGet(id, &device) == true )
    {
        memcpy(&(((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[1]), device, sizeof(MYPROTOCOL_DEVICE_INFO_t));
    }
    else
    {
        memcpy(&(((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[1]), &info, sizeof(MYPROTOCOL_DEVICE_INFO_t));
    }

    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.len = sizeof(MYPROTOCOL_DEVICE_INFO_t)+1;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       网关响应设备信息
 * @param       [in/out]  ctx            设备ID
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createDeviceRdTimeAckPacket( void *ctx, void *packet )
{    
    user_time nowTime = gizwitsNTPConverUserTime();
    
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("Device Num Ack Packet info is invalid! \r\n");
        return false;
    }
#endif    

    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype          = MYPROTOCOL_S2H_ACK;
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.cmd     = MYPROTOCOL_RD_TIME_CMD; 
    memcpy(&((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data, &nowTime, sizeof(user_time));
    
    if( getGizwitsM2MStatus() == true )
    {
        ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[sizeof(user_time)] = 0x01;
    }
    else
    {
        ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[sizeof(user_time)] = 0x00;
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.len = sizeof(user_time)+1;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       网关响应设备信息
 * @param       [in/out]  ctx            设备ID
 * @param       [in/out]  packet         创建数据包函数
 * @return      [in/out]  bool           程序运行状态
 * @note        None
 *******************************************************************************
 */
bool createAPPRdTimeAckPacket( void *ctx, void *packet )
{    
    user_time nowTime = gizwitsNTPConverUserTime();
    
#if USE_MYPROTOCOL_DEBUG
    if( packet == NULL )
    {
        MYPROTOCOL_LOG("Device Num Ack Packet info is invalid! \r\n");
        return false;
    }
#endif    

    ((MYPROTOCOL_FORMAT_t *)(packet))->commtype          = MYPROTOCOL_W2D_ACK;
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.cmd     = MYPROTOCOL_RD_TIME_CMD; 
    memcpy(&((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data, &nowTime, sizeof(user_time));
    
    if( getGizwitsM2MStatus() == true )
    {
        ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[sizeof(user_time)] = 0x01;
    }
    else
    {
        ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.data[sizeof(user_time)] = 0x00;
    }
    
    ((MYPROTOCOL_FORMAT_t *)(packet))->user_data.len = sizeof(user_time)+1;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       复位WIFI模块数据按键处理函数
 * @param       [in/out]  msg     按键信息
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void coordRstWIFIKeyHandler( key_message_t msg )
{
    switch( msg )
    {
        case KEY_MESSAGE_LONG_PRESS_EDGE:
#if USE_MYPROTOCOL_DEBUG
            MYPROTOCOL_LOG("coord press k1 to rst wifi mode \r\n");
#endif
            gizwitsSetMode(0x00);
            break;
        default:
            break;
    }
}

/**
 *******************************************************************************
 * @brief       刷新网络时间按键处理函数
 * @param       [in/out]  msg    按键信息
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void coordUpdateTimeKeyHandler( key_message_t msg )
{
    switch( msg )
    {
        case KEY_MESSAGE_RELEASE_EDGE:
#if USE_MYPROTOCOL_DEBUG
            MYPROTOCOL_LOG("coord press k2 to update time \r\n");
#endif
            gizwitsGetNTP();
            break;
        default:
            break;
    }
}

#if 0
/**
 *******************************************************************************
 * @brief       coord D2D信息处理
 * @param       [in]   recPacket    信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
static void coordD2DMessageHandler( MYPROTOCOL_FORMAT_t *recPacket )
{
    if( MyprotocolD2DRecDeviceCheck(recPacket) == true )
    {
        switch(recPacket->commtype)
        {
            case MYPROTOCOL_S2H_WAIT:
                switch( recPacket->user_data.cmd )
                {
                    case MYPROTOCOL_TICK_CMD:
                    {            
                        switch( recPacket->device.device )
                        {
                            case MYPROTOCOL_DEVICE_LIGHT:
                                MYPROTOCOL_LOG("Coord get a tick from LIGHT!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_SOCKET:
                                MYPROTOCOL_LOG("Coord get a tick from SOCKET!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_CURTAIN:
                                MYPROTOCOL_LOG("Coord get a tick from CURTAIN!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_HT_SENSOR:
                                MYPROTOCOL_LOG("Coord get a tick from HT_SENSOR!\r\n");
                                break;
                            default:
                                MYPROTOCOL_LOG("Coord get a tick from NO SUPPORT DEVICE!\r\n");
                                break;
                        }
        
                        addDeviceTick(&recPacket->device);
                        MyprotocolSendData(recPacket,&recPacket->device.mac, createDeviceTickAckPacket, MyprotocolD2DSendData);
                        break;
                    }
                    case MYPROTOCOL_RD_TIME_CMD:
                    {
                        MyprotocolSendData(NULL, NULL, DeviceRdTimeAckPacket, MyprotocolD2WSendData);
                        break;
                    }
                    default:
                        break;
                }
                break;
            case MYPROTOCOL_H2S_ACK:
                break;
            default:
                break;
        }
    }
    else
    {
        // 转发数据包
        MyprotocolForwardData(NULL, recPacket, MyprotocolD2WSendData);
#if USE_MYPROTOCOL_DEBUG
        MYPROTOCOL_LOG("coord get packet from device!");
#endif 
    }
}

/**
 *******************************************************************************
 * @brief       coord W2D信息处理
 * @param       [in]   recPacket    信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
static void coordW2DMessageHandler( MYPROTOCOL_FORMAT_t *recPacket )
{
    if( MyprotocolW2DRecDeviceCheck(recPacket) == true )
    {
        switch(recPacket->commtype)
        {
            case MYPROTOCOL_W2D_WAIT:
                switch( recPacket->user_data.cmd )
                {
                    case MYPROTOCOL_RESET_CMD:
                    case MYPROTOCOL_REBOOT_CMD:
                    {
                        MyprotocolSendData(recPacket, NULL, createW2DAckPacket, MyprotocolD2WSendData);
                        gizProtocolReboot();
                        break;
                    }
                    case MYPROTOCOL_UPDATE_TIME_CMD:
                    {    
                        gizwitsGetNTP();
                        MyprotocolSendData(NULL, NULL, DeviceRdTimeAckPacket, MyprotocolD2WSendData);
                        break;
                    }
                    case MYPROTOCOL_RD_TIME_CMD:
                    {
                        MyprotocolSendData(NULL, NULL, DeviceRdTimeAckPacket, MyprotocolD2WSendData);
                        break;
                    }
                    case MYPROTOCOL_WR_TIME_CMD:
                    {
                        gizwitsWrTime((user_time *)(&recPacket->user_data.data));
                        MyprotocolSendData(recPacket, NULL, createW2DAckPacket, MyprotocolD2WSendData);
                         break;
                    } 
                    case COORD_RD_DEVICE_NUM_CMD:
                    {
#if USE_MYPROTOCOL_DEBUG
                        MYPROTOCOL_LOG("app get device num!");
#endif 
                        MyprotocolSendData(NULL, NULL, DeviceNumAckPacket, MyprotocolD2WSendData);
                        break;
                    }
                    case COORD_RD_DEVICE_INFO_CMD:
                    {
#if USE_MYPROTOCOL_DEBUG
                        MYPROTOCOL_LOG("app get device info!");
#endif 
                        MyprotocolSendData((void *)&recPacket->user_data.data, NULL, DeviceInfoAckPacket, MyprotocolD2WSendData);
                        break;
                    } 
                    default:
                        break;
                }
                break;
            case MYPROTOCOL_D2W_ACK:
                break;
            default:
                break;
        }
    }
    else
    {
        // 转发数据包
        MyprotocolForwardData(&recPacket->device.mac, recPacket, MyprotocolD2DSendData);
        MyprotocolSendData(recPacket, NULL, createW2DAckPacket, MyprotocolD2WSendData);
#if USE_MYPROTOCOL_DEBUG
        MYPROTOCOL_LOG("coord forward packet to else device!");
#endif 
    }
}

/**
 *******************************************************************************
 * @brief       coord  信息处理
 * @param       [in]   recPacket    信息
 * @return      [out]  bool         处理状态
 * @note        None
 *******************************************************************************
 */
bool coordMessageHandler( void *recPacket )
{
    MYPROTOCOL_FORMAT_t *packet = (MYPROTOCOL_FORMAT_t *)recPacket;
    
#if USE_MYPROTOCOL_DEBUG
    if( recPacket == NULL )
    {
        MYPROTOCOL_LOG("coord message handler error, the input param is invaild!");
        return false;
    }    
    
    if( MyprotocolPacketCheck(packet) == false )
    {
        MyprotocolReplyErrPacket(packet);
        return true;
    }
#endif
    switch( packet->commtype )
    {
        case MYPROTOCOL_W2D_WAIT:
        case MYPROTOCOL_D2W_ACK:
            coordW2DMessageHandler(recPacket);
            break;
        case MYPROTOCOL_D2W_WAIT:
        case MYPROTOCOL_H2S_WAIT:
        case MYPROTOCOL_H2S_ACK:
        case MYPROTOCOL_S2H_WAIT:
        case MYPROTOCOL_S2H_ACK:
            coordD2DMessageHandler(recPacket);
            break;
        default:
            break;
    }

    return true;
}

#endif

/**
 *******************************************************************************
 * @brief       coord  信息处理
 * @param       [in]   recPacket    信息
 * @return      [out]  bool         处理状态
 * @note        None
 *******************************************************************************
 */
bool coordMessageHandler( MYPROTOCOL_FORMAT_t *recPacket )
{
    switch( recPacket->commtype )
    {
        case MYPROTOCOL_W2D_WAIT:
        {
            if( MyprotocolW2DRecDeviceCheck(recPacket) == true )
            {
                switch( recPacket->user_data.cmd )
                {
                    case MYPROTOCOL_RESET_CMD:
                    case MYPROTOCOL_REBOOT_CMD:
                    {
                        MyprotocolSendData(recPacket, NULL, createW2DAckPacket, MyprotocolD2WSendData);
                        gizProtocolReboot();
                        break;
                    }
                    case MYPROTOCOL_UPDATE_TIME_CMD:
                    {    
                        gizwitsGetNTP();
                        MyprotocolSendData(NULL, NULL, createDeviceRdTimeAckPacket, MyprotocolD2WSendData);
                        break;
                    }
                    case MYPROTOCOL_RD_TIME_CMD:
                    {
                        MyprotocolSendData(NULL, NULL, createAPPRdTimeAckPacket, MyprotocolD2WSendData);
                        break;
                    }
                    case MYPROTOCOL_WR_TIME_CMD:
                    {
                        gizwitsWrTime((user_time *)(&recPacket->user_data.data));
                        MyprotocolSendData(recPacket, NULL, createW2DAckPacket, MyprotocolD2WSendData);
                         break;
                    } 
                    case COORD_RD_DEVICE_NUM_CMD:
                    {
#if USE_MYPROTOCOL_DEBUG
                        MYPROTOCOL_LOG("app get device num!");
#endif 
                        MyprotocolSendData(NULL, NULL, createDeviceNumAckPacket, MyprotocolD2WSendData);
                        break;
                    }
                    case COORD_RD_DEVICE_INFO_CMD:
                    {
#if USE_MYPROTOCOL_DEBUG
                        MYPROTOCOL_LOG("app get device info!");
#endif 
                        MyprotocolSendData((void *)&recPacket->user_data.data, NULL, createDeviceInfoAckPacket, MyprotocolD2WSendData);
                        break;
                    } 
                    default:
                        break;
                }
            }
            else
            {
                // 转发数据包
                MyprotocolForwardData(&recPacket->device.mac, recPacket, MyprotocolD2DSendData);
                MyprotocolSendData(recPacket, NULL, createW2DAckPacket, MyprotocolD2WSendData);
#if USE_MYPROTOCOL_DEBUG
                MYPROTOCOL_LOG("coord forward recPacket to else device!");
#endif 
            }
            
            break;
        }
        case MYPROTOCOL_S2H_WAIT:
        {
            if( MyprotocolD2DRecDeviceCheck(recPacket) == true )
            {
                switch( recPacket->user_data.cmd )
                {
                    case MYPROTOCOL_TICK_CMD:
                    {          
#if USE_MYPROTOCOL_DEBUG
                        switch( recPacket->device.device )
                        {
                            case MYPROTOCOL_DEVICE_LIGHT:
                                MYPROTOCOL_LOG("Coord get a tick from LIGHT!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_SOCKET:
                                MYPROTOCOL_LOG("Coord get a tick from SOCKET!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_CURTAIN:
                                MYPROTOCOL_LOG("Coord get a tick from CURTAIN!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_HT_SENSOR:
                                MYPROTOCOL_LOG("Coord get a tick from HT_SENSOR!\r\n");
                                break;
                            default:
                                MYPROTOCOL_LOG("Coord get a tick from NO SUPPORT DEVICE!\r\n");
                                break;
                        }
#endif
                        addDeviceTick(&recPacket->device);
                        MyprotocolSendData(recPacket,&recPacket->device.mac, createDeviceTickAckPacket, MyprotocolD2DSendData);
                        break;
                    }
                    case MYPROTOCOL_RD_TIME_CMD:
                    {
#if USE_MYPROTOCOL_DEBUG
                        switch( recPacket->device.device )
                        {
                            case MYPROTOCOL_DEVICE_LIGHT:
                                MYPROTOCOL_LOG("device LIGHT get net time!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_SOCKET:
                                MYPROTOCOL_LOG("device SOCKET get net time!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_CURTAIN:
                                MYPROTOCOL_LOG("device CURTAIN get net time!\r\n");
                                break;
                            case MYPROTOCOL_DEVICE_HT_SENSOR:
                                MYPROTOCOL_LOG("device HT_SENSOR get net time!\r\n");
                                break;
                            default:
                                MYPROTOCOL_LOG("no support device get net time!\r\n");
                                break;
                        }
#endif
                        addDeviceTick(&recPacket->device);
                        MyprotocolSendData(NULL, &recPacket->device.mac, createDeviceRdTimeAckPacket, MyprotocolD2DSendData);
                        break;
                    }
                    default:
                        break;
                }
            }
//            else
//            {
//                addDeviceTick(&recPacket->device);
//                // 转发数据包
//                MyprotocolForwardData(NULL, recPacket, MyprotocolD2WSendData);
//#if USE_MYPROTOCOL_DEBUG
//                MYPROTOCOL_LOG("coord get recPacket from device!");
//#endif 
//            }
            
            break;
        }
        case MYPROTOCOL_D2W_WAIT:
                addDeviceTick(&recPacket->device);
                // 转发数据包
                MyprotocolForwardData(NULL, recPacket, MyprotocolD2WSendData);
#if USE_MYPROTOCOL_DEBUG
                MYPROTOCOL_LOG("coord get recPacket from device!");
#endif 
            break;
        case MYPROTOCOL_D2W_ACK:
//                // 转发数据包
//                MyprotocolForwardData(&recPacket->device.mac, recPacket, MyprotocolD2DSendData);
//                MyprotocolSendData(recPacket, NULL, createW2DAckPacket, MyprotocolD2WSendData);
//#if USE_MYPROTOCOL_DEBUG
//                MYPROTOCOL_LOG("coord forward recPacket to else device!");
//#endif 
            break;
        case MYPROTOCOL_H2S_ACK:
            break;
        case MYPROTOCOL_S2H_ACK:
            break;
        default:
            break;
    }

    return true;
}

/** @}*/     /* Coord模块 */

/**********************************END OF FILE*********************************/
