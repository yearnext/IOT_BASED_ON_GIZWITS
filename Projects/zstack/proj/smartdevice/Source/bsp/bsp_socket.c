/**
 ******************************************************************************
  * @file       bsp_socket.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      插座配置 源头文件
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
 * @defgroup 插座配置模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_socket.h"
#include <string.h>
#include "app_save.h"
#include "bsp_socket.h"
#include "Onboard.h"
#include "app_time.h"

/* Exported macro ------------------------------------------------------------*/
// SOCKET 调试开关
#define USE_SOCKET_DEBUG (1)

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 插座控制端口寄存器定义
#define SOCKET_CONTROL_PORT     P2_0
#define SOCKET_CONTROL_BV       BV(0)
#define SOCKET_CONTROL_SEL      P2SEL
#define SOCKET_CONTROL_DIR      P2DIR
#define SOCKET_CONTROL_POLARITY ACTIVE_HIGH

// 插座控制端口功能配置
#define SOCKET_CONTROL_WrMode() ( SOCKET_CONTROL_SEL &= ~SOCKET_CONTROL_BV, SOCKET_CONTROL_DIR |=  SOCKET_CONTROL_BV )
#define SOCKET_CONTROL_RdMode() ( SOCKET_CONTROL_SEL &= ~SOCKET_CONTROL_BV, SOCKET_CONTROL_DIR &= ~SOCKET_CONTROL_BV )
#define SET_SOCKET_CONTROL()    ( SOCKET_CONTROL_PORT = SOCKET_CONTROL_POLARITY(1) )
#define CLR_SOCKET_CONTROL()    ( SOCKET_CONTROL_PORT = SOCKET_CONTROL_POLARITY(0) )
#define RD_SOCKET_CONTROL()     ( SOCKET_CONTROL_POLARITY(SOCKET_CONTROL_PORT) )

#define DEVICE_SOCKET_DATA_SIZE    (Cal_DataSize(socket))

/* Private typedef -----------------------------------------------------------*/
// 继电器控制命令
typedef enum
{
    RD_SOCKET_STATE        = 0x10,
    WR_SOCKET_STATE        = 0x11,
    RD_SOCKET_SINGLE_TIMER = 0x12,
    WR_SOCKET_SINGLE_TIMER = 0x13,
    RD_SOCKET_CIRCUL_TIMER = 0x14,
    WR_SOCKET_CIRCUL_TIMER = 0x15,
    RD_SOCKET_LOAD_SET     = 0x16,
    WR_SOCKET_LOAD_SET     = 0x17,
}DEVICE_LIGHT_CMD;

// 插座的开启关闭命令
#define SOCKET_ON_CMD        (SOCKET_CONTROL_POLARITY(1))
#define SOCKET_OFF_CMD       (SOCKET_CONTROL_POLARITY(0))
#define SOCKET_ON_STATE      (SOCKET_CONTROL_POLARITY(1))
#define SOCKET_OFF_STATE     (SOCKET_CONTROL_POLARITY(0))

// 加载掉电前的数据
#define SOCKET_NO_LOAD_SET   (0x00)
#define SOCKET_LOAD_SET      (0x01)

// 配置定时器使用数量
#define SOCKET_USE_TIMER_NUM (2)

/* Private variables ---------------------------------------------------------*/
// 插座存储数据
static struct _DEVICE_SOCKET_SAVE_DATA_
{
    // 状态数据
    struct
    {
        uint8 now;
        uint8 last;
    }status;
    
    // 定时器数据
    DEVICE_TIMER timer[SOCKET_USE_TIMER_NUM]; 
    
    // 初始化载入数据
    uint8 load_set;
}socket;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       上报插座当前信息
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportSocketState( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
        
#if USE_MYPROTOCOL_DEBUG
        MYPROTOCOL_LOG("device socket report state! \r\n");
#endif
    
    user_data.cmd = RD_SOCKET_STATE;
    user_data.data[0] = socket.status.now;
    user_data.len = 1;
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       上报插座定时器的数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportSocketTimerData( uint8 timer )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));

    if( timer == 0 )
    {
        user_data.cmd = RD_SOCKET_SINGLE_TIMER;
    }
    else if( timer == 1 )
    {
        user_data.cmd = RD_SOCKET_CIRCUL_TIMER;
    }
    else
    {
        return;
    }
    
    memcpy(&user_data.data,&socket.timer[timer],sizeof(DEVICE_TIMER));
    user_data.len = sizeof(DEVICE_TIMER);
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       上报插座定时器的数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void reportSocketLoadSetState( void )
{
    MYPROTOCOL_USER_DATA_t user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA_t));
    
    user_data.cmd = RD_SOCKET_LOAD_SET;
    memcpy(&user_data.data,&socket.load_set,sizeof(socket.load_set));
    user_data.len = sizeof(socket.load_set);
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData);
}

/**
 *******************************************************************************
 * @brief       存储定时器数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static bool saveSocketTimerData( uint8 timer, uint8 *data )
{
#if USE_SOCKET_DEBUG
    if( timer >= SOCKET_USE_TIMER_NUM )
    {
        return false;
    }
#endif
    
    memcpy(&socket.timer[timer],data,sizeof(socket.timer[timer]));
    
    osal_nv_write(DEVICE_SOCKET_SAVE_ID,
                  (uint32)&socket.timer[timer]-(uint32)&socket,
                  sizeof(socket.timer[timer]),
                  (void *)&socket.timer[timer]); 

    return true;
}

                  
/**
 *******************************************************************************
 * @brief       存储上电加载参数数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static bool saveSocketLoadData( uint8 data )
{
    socket.load_set = data;
    
    osal_nv_write(DEVICE_SOCKET_SAVE_ID,
                  (uint32)&socket.load_set-(uint32)&socket,
                  sizeof(socket.load_set),
                  (void *)&socket.load_set);     
    return true;
}

/**
 *******************************************************************************
 * @brief       初始化插座设置数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void rstSocketData( void )
{
    memset(&socket,0,sizeof(socket));
    socket.status.now = SOCKET_OFF_CMD;
    socket.status.last = SOCKET_ON_CMD;
    
    osal_nv_item_init(DEVICE_SOCKET_SAVE_ID,DEVICE_SOCKET_DATA_SIZE,NULL);
    osal_nv_write(DEVICE_SOCKET_SAVE_ID,0,DEVICE_SOCKET_DATA_SIZE,(void *)&socket);
}

/**
 *******************************************************************************
 * @brief       插座初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bspSocketInit( void )
{
    SOCKET_CONTROL_WrMode();
    setSocketState(SOCKET_OFF_CMD);
    
    // FLASH 数据初始化
    deviceLoadDownData(DEVICE_SOCKET_SAVE_ID,DEVICE_SOCKET_DATA_SIZE,(void *)&socket,rstSocketData);
    
    setSocketState(socket.status.now);
}

/**
 *******************************************************************************
 * @brief       插座数值设置函数
 * @param       [in/out]  uint8    插座状态
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void setSocketState( uint8 setCmd )
{
    if( setCmd == SOCKET_OFF_CMD )
    {
        CLR_SOCKET_CONTROL();
    }
    else
    {
        SET_SOCKET_CONTROL();
    }
}

/**
 *******************************************************************************
 * @brief       读取插座函数
 * @param       [in/out]  void
 * @return      [in/out]  uint8    插座的数值
 * @note        None
 *******************************************************************************
 */
uint8 getSocketState( void )
{
    return (RD_SOCKET_CONTROL() == SOCKET_ON_STATE) ? (SOCKET_ON_STATE) : (SOCKET_OFF_STATE);
}

/**
 ******************************************************************************
 * @brief       插座数值设置函数
 * @param       [in/out]  void
 * @return      [in/out]  value    插座的数值
 * @note        None 
 *******************************************************************************
 */
void socketControlHandler( uint8 state )
{
    if( state != getSocketState() )
    {
        socket.status.last = socket.status.now;
        socket.status.now = state;
        
        setSocketState( socket.status.now );
        
        osal_nv_write(DEVICE_SOCKET_SAVE_ID,\
                      (uint16)&socket.status-(uint16)&socket,\
                      sizeof(socket.status),\
                      (void *)&socket.status);
    }
    
    reportSocketState();
}

/**
 *******************************************************************************
 * @brief       插座按键处理
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void socketSwitchHandler( void )
{
    socket.status.last = socket.status.now;
    
    if( socket.status.now == SOCKET_OFF_CMD )
    {
        
        socket.status.now = SOCKET_ON_CMD;
    }
    else
    {
        socket.status.now = SOCKET_OFF_CMD;
    }
    
    socketControlHandler(socket.status.now);
}

/**
 *******************************************************************************
 * @brief       按键处理
 * @param       [in]   message    按键信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void socketSwitchKeyHandler( key_message_t message )
{
    switch (message)
    {
		case KEY_MESSAGE_PRESS_EDGE:
            socketSwitchHandler();
			break;
		default:
			break;
    }
}

/**
 *******************************************************************************
 * @brief       按键处理
 * @param       [in]   message    按键信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void socketRstKeyHandler( key_message_t message )
{
    switch (message)
    {
		case KEY_MESSAGE_LONG_PRESS_EDGE:
            rstSocketData();
            Onboard_soft_reset();
			break;
		default:
			break;
    }
}

/**
 *******************************************************************************
 * @brief       插座工作处理函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void socketWorkingHandler( void )
{
    uint8 i;
    
    for( i=0; i<SOCKET_USE_TIMER_NUM; i++ )
    {
        deviceTimerHandler((DEVICE_TIMER*)&socket.timer[i],socketControlHandler);
    }
}

/**
 *******************************************************************************
 * @brief       解析继电器命令数据
 * @param       [in/out]  *data    控制命令数据
 * @return      [in/out]  bool     解析状态
 * @note        None
 *******************************************************************************
 */
bool socketMessageHandler( MYPROTOCOL_FORMAT_t *recPacket )
{    
    switch( recPacket->user_data.cmd )
    {
        case MYPROTOCOL_TICK_CMD:
            break;
        case MYPROTOCOL_RESET_CMD:
            rstSocketData();
        case MYPROTOCOL_REBOOT_CMD:
            Onboard_soft_reset();
            break;
        case MYPROTOCOL_RD_TIME_CMD:
            if( recPacket->user_data.data[8] == 1 )
            {
                app_time_update((user_time *)&recPacket->user_data.data);
            }
            break;
        case RD_SOCKET_STATE:
            reportSocketState();
            break;
        case WR_SOCKET_STATE:
            socketControlHandler(recPacket->user_data.data[0]);
            reportSocketState();
            break;
        case RD_SOCKET_SINGLE_TIMER:
            reportSocketTimerData(0);
            break;
        case WR_SOCKET_SINGLE_TIMER:
            saveSocketTimerData(0,recPacket->user_data.data);
            reportSocketTimerData(0);
            break;
        case RD_SOCKET_CIRCUL_TIMER:
            reportSocketTimerData(1);
            break;
        case WR_SOCKET_CIRCUL_TIMER:
            saveSocketTimerData(1,recPacket->user_data.data);
            reportSocketTimerData(1);
            break;
        case RD_SOCKET_LOAD_SET:
            reportSocketLoadSetState();
            break;
        case WR_SOCKET_LOAD_SET:
            saveSocketLoadData(recPacket->user_data.data[0]);
            reportSocketLoadSetState();
            break;
        default:
            return false;
            break;
    }
    return true;
}

/** @}*/     /* 智能插座模块 */

/**********************************END OF FILE*********************************/
