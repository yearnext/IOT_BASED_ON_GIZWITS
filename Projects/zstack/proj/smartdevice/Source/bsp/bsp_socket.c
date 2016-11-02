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

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_SOCKET)
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 继电器数值与PWM占空比之间转换
#define Socket_Value_Conversion(n) ( 0xFF - (n) )
#define Get_Socket_Value()         ( Get_TIM4_CH0_Duty() )

/* Private typedef -----------------------------------------------------------*/
// 继电器控制命令
#define SOCKET_TICK_CMD            (0x00)
#define SOCKET_R_VALUE_CMD         (0x01)
#define SOCKET_W_VALUE_CMD         (0x02)
#define SOCKET_R_SINGLE_TIMER_CMD  (0x03)
#define SOCKET_W_SINGLE_TIMER_CMD  (0x04)
#define SOCKET_R_CIRCUL_TIMER_CMD  (0x05)
#define SOCKET_W_CIRCUL_TIMER_CMD  (0x06)

typedef struct
{
    uint8 value;
}SOCKET_VALUE_CMD_DATA;

/* Private variables ---------------------------------------------------------*/
static DEVICE_SOCKET_SAVE_DATA socket;

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
    
    user_data.cmd = SOCKET_R_VALUE_CMD;
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
        user_data.cmd = SOCKET_R_SINGLE_TIMER_CMD;
    }
    else if( timer == 1 )
    {
        user_data.cmd = SOCKET_R_CIRCUL_TIMER_CMD;
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
    if( Device_FirstWrite_Check() == false \
        || osal_nv_read(DEVICE_SOCKET_SAVE_ID,0,DEVICE_SOCKET_DATA_SIZE,(void *)&socket) != SUCCESS )
    {
        memset(&socket,0,sizeof(socket));
        socket.status.now = SOCKET_OFF_CMD;
        socket.status.last = SOCKET_ON_CMD;
        
        osal_nv_item_init(DEVICE_SOCKET_SAVE_ID,DEVICE_SOCKET_DATA_SIZE,NULL);
        osal_nv_write(DEVICE_SOCKET_SAVE_ID,0,DEVICE_SOCKET_DATA_SIZE,(void *)&socket);
    }
    
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
    TIM4_CH0_UpdateDuty( Socket_Value_Conversion(socket.status.now) );
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
    return Socket_Value_Conversion( get_socket_value() );
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
        if( socket.status.now == 0xFE )
        {
            socket.status.now = 0xFF;
        }
        
        socket.status.last = socket.status.now;
        socket.status.now = value;
        
        set_socket_value( socket.status.now );
        
        osal_nv_write(DEVICE_SOCKET_SAVE_ID,\
                      (uint16)(&((DEVICE_SOCKET_SAVE_DATA *)0)->status),\
                      sizeof(socket.status),\
                      (void *)&socket.status);
        
        if( socket.status.now == 0xFF )
        {
            socket.status.now = 0xFE;
        }
        
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
void socket_switch_headler( void )
{
    uint8 temp = 0;
    
    if( socket.status.now == 0xFE )
    {
        socket.status.now = 0xFF;
    }
    
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
    
    if( socket.status.now == 0xFF )
    {
        socket.status.now = 0xFE;
    }
    
    report_socket_value_data();
}

/**
 *******************************************************************************
 * @brief       插座工作处理函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void socket_working_headler( void )
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
        case SOCKET_TICK_CMD:
            break;
        case SOCKET_R_VALUE_CMD:
        {
            report_socket_value_data();
            break;
        }
        case SOCKET_W_VALUE_CMD:
        {
            socket_control_handler(data->data[0]);
            report_socket_value_data();
            break;
        }
        case SOCKET_R_SINGLE_TIMER_CMD:
        {
            report_timer_data(0);
            break;
        }
        case SOCKET_W_SINGLE_TIMER_CMD:
        {
            memcpy(&socket.timer[0],data->data,sizeof(socket.timer[0]));
            report_timer_data(0);
            break;
        }
        case SOCKET_R_CIRCUL_TIMER_CMD:
        {
            report_timer_data(1);
            break;
        }
        case SOCKET_W_CIRCUL_TIMER_CMD:
        {
            memcpy(&socket.timer[1],data->data,sizeof(socket.timer[1]));
            report_timer_data(1);
            break;
        }
        default:
            return false;
            break;
    }
    return true;
}

#endif

/** @}*/     /* 智能插座模块 */

/**********************************END OF FILE*********************************/
