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
#include "timer_config.h"
#include <string.h>
#include "app_save.h"
#include "app_timer.h"
#include "bsp_socket.h"
#include "Onboard.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 继电器数值与PWM占空比之间转换
#define Socket_Value_Conversion(n) ( 0xFF - (n) )
#define Get_Socket_Value()         ( Get_TIM4_CH0_Duty() )

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

// 灯的开启关闭亮度
#define SOCKET_ON_CMD        (255)
#define SOCKET_OFF_CMD       (0)

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
//    enum
//    {
//        LIGHT_NO_LOAD_SET = 0x00,
//        LIGHT_LOAD_SET    = 0x01,
//    }load_set;
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
static void report_socket_value_data( void )
{
    MYPROTOCOL_USER_DATA user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA));
    
    user_data.cmd = RD_SOCKET_STATE;
    user_data.data[0] = socket.status.now;
    user_data.len = 1;
    MYPROTOCO_S2H_MSG_SEND(create_d2w_wait_packet,&user_data);
}

/**
 *******************************************************************************
 * @brief       上报插座定时器的数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void report_timer_data( uint8 timer )
{
    MYPROTOCOL_USER_DATA user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA));
    
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
    
    MYPROTOCO_S2H_MSG_SEND(create_d2w_wait_packet,&user_data);
}

/**
 *******************************************************************************
 * @brief       上报插座定时器的数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void report_socket_loadset_data( void )
{
    MYPROTOCOL_USER_DATA user_data;
    memset(&user_data,0,sizeof(MYPROTOCOL_USER_DATA));
    
    user_data.cmd = RD_SOCKET_LOAD_SET;
    memcpy(&user_data.data,&socket.load_set,sizeof(socket.load_set));
    user_data.len = sizeof(socket.load_set);
    
    MYPROTOCO_S2H_MSG_SEND(create_d2w_wait_packet,&user_data);
}

/**
 *******************************************************************************
 * @brief       存储定时器数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static bool save_socket_timer_data( uint8 timer, uint8 *data )
{
    if( timer >= SOCKET_USE_TIMER_NUM )
    {
        return false;
    }
    
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
static bool save_socket_loadset_data( uint8 data )
{
    socket.load_set = data;
    
    osal_nv_write(DEVICE_LIGHT_SAVE_ID,
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
static void socket_rst_set( void )
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
void bsp_socket_init( void )
{
    Timer4_PWM_Init( TIM4_CH0_PORT_P2_0 );
    set_socket_value(SOCKET_OFF_CMD);
    
    // FLASH 数据初始化
    Device_Load_LastData(DEVICE_SOCKET_SAVE_ID,DEVICE_SOCKET_DATA_SIZE,(void *)&socket,socket_rst_set);
    
    set_socket_value(socket.status.now);
}

/**
 *******************************************************************************
 * @brief       插座数值设置函数
 * @param       [in/out]  uint8    电灯的亮度
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void set_socket_value( uint8 value )
{
    if( value == 0 )
    {
        TIM4_CH0_UpdateDuty( Socket_Value_Conversion( SOCKET_OFF_CMD ) );
    }
    else
    {
        TIM4_CH0_UpdateDuty( Socket_Value_Conversion( SOCKET_ON_CMD) );
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
uint8 get_socket_value( void )
{
    if( Socket_Value_Conversion(get_socket_value()) == SOCKET_OFF_CMD )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 ******************************************************************************
 * @brief       插座数值设置函数
 * @param       [in/out]  void
 * @return      [in/out]  value    插座的数值
 * @note        None 
 *******************************************************************************
 */
void socket_control_handler( uint8 value )
{
    if( value != get_socket_value() )
    {
        socket.status.last = socket.status.now;
        socket.status.now = value;
        
        set_socket_value( socket.status.now );
        
        osal_nv_write(DEVICE_SOCKET_SAVE_ID,\
                      (uint16)&socket.status-(uint16)&socket,\
                      sizeof(socket.status),\
                      (void *)&socket.status);
    }
}

/**
 *******************************************************************************
 * @brief       插座按键处理
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void socket_switch_handler( void )
{
    uint8 temp = 0;

    if( socket.status.now == SOCKET_OFF_CMD )
    {
        if( socket.status.last != socket.status.now )
        {
            temp = socket.status.now;
            socket.status.now = socket.status.last;
            socket.status.last = temp;
        }
        else
        {
            socket.status.last = socket.status.now;
            socket.status.now = SOCKET_ON_CMD;
        }
    }
    else
    {
        socket.status.last = socket.status.now;
        socket.status.now = SOCKET_OFF_CMD;
    }
    
    set_socket_value( socket.status.now );
    
    osal_nv_write(DEVICE_SOCKET_SAVE_ID,\
                 ((uint16)&socket.status - (uint16)&socket),\
                 sizeof(socket.status),\
                 (void *)&socket.status); 

    report_socket_value_data();
}

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_SOCKET)
/**
 *******************************************************************************
 * @brief       按键处理
 * @param       [in]   message    按键信息
 * @return      [out]  void
 * @note        None
 *******************************************************************************
 */
void key_switch_key_handler( key_message_t message )
{
    switch (message)
    {
		case KEY_MESSAGE_PRESS_EDGE:
            socket_switch_handler();
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
void key_reset_key_handler( key_message_t message )
{
    switch (message)
    {
		case KEY_MESSAGE_PRESS_EDGE:
			break;
		default:
			break;
    }
}

#endif

/**
 *******************************************************************************
 * @brief       插座工作处理函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void socket_working_handler( void )
{
    uint8 i;
    
    for( i=0; i<SIMPLE_DEVICE_TIMER_NUM; i++ )
    {
        device_timer_handler((DEVICE_TIMER*)&socket.timer[i],socket_control_handler);
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
bool socket_cmd_resolve( MYPROTOCOL_USER_DATA *data )
{    
    switch( data->cmd )
    {
        case DEVICE_TICK:
            break;
        case DEVICE_RESET:
            socket_rst_set();
        case DEVICE_REBOOT:
            Onboard_soft_reset();
            break;
        case RD_SOCKET_STATE:
            report_socket_value_data();
            break;
        case WR_SOCKET_STATE:
            socket_control_handler(data->data[0]);
            report_socket_value_data();
            break;
        case RD_SOCKET_SINGLE_TIMER:
            report_timer_data(0);
            break;
        case WR_SOCKET_SINGLE_TIMER:
            save_socket_timer_data(0,data->data);
            report_timer_data(0);
            break;
        case RD_SOCKET_CIRCUL_TIMER:
            report_timer_data(1);
            break;
        case WR_SOCKET_CIRCUL_TIMER:
            save_socket_timer_data(1,data->data);
            report_timer_data(1);
            break;
        case RD_SOCKET_LOAD_SET:
            report_socket_loadset_data();
            break;
        case WR_SOCKET_LOAD_SET:
            save_socket_loadset_data(data->data[0]);
            report_socket_loadset_data();
            break;
        default:
            return false;
            break;
    }
    return true;
}

/** @}*/     /* 智能插座模块 */

/**********************************END OF FILE*********************************/
