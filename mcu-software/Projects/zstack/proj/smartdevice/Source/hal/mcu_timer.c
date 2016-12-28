 /**
 ******************************************************************************
  * @file       mcu_timer.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      CC2530 定时器配置源文件
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
 * @defgroup CC2530 定时器配置模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "mcu_timer.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       定时器3 计时功能初始化
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        定时周期大约为1ms
 *******************************************************************************
 */
void Timer3_Init( void )
{
    T3CTL = 0xFC;
    IEN1 |= 0x08;
}
  
/**
 *******************************************************************************
 * @brief       定时器3输出比较功能初始化
 * @param       [in/out]  task_id    输出比较通道引脚
 * @return      [in/out]  false      配置失败
 * @return      [in/out]  true       配置成功
 * @note        PWM周期大约为1ms
 *******************************************************************************
 */
bool Timer3_PWM_Init( uint8 CHANNEL_PIN )
{
    switch( CHANNEL_PIN )
    {
        case TIM3_CH0_PORT_P1_3:
            // 配置定时器3的IO位置为备用位置1
            PERCFG &= ~0x20;
            // 配置端口1的外设优先级为定时器3优先
            P2SEL |= 0x20;
            // 配置P1_3为外设IO
            P1SEL |= 0x08;
            // 定时器输出比较配置
            T3CCTL0 = 0xAC;
            // 定时器比较寄存器配置
            T3CC0 = 0xFF;
            break;
        case TIM3_CH0_PORT_P1_6:
            // 配置定时器3的IO位置为备用位置1
            PERCFG |= 0x20;
            // 配置端口1的外设优先级为定时器3优先
            P2SEL |= 0x20;
            // 配置P1_6为外设IO
            P1SEL |= 0x40;
            // 定时器输出比较配置
            T3CCTL0 = 0xAC;
            // 定时器比较寄存器配置
            T3CC0 = 0xFF;
            break;
        case TIM3_CH1_PORT_P1_4:
            // 配置定时器3的IO位置为备用位置1
            PERCFG &= ~0x20;
            // 配置端口1的外设优先级为定时器3优先
            P2SEL |= 0x20;
            // 配置P1_4为外设IO
            P1SEL |= 0x10;
            // 定时器输出比较配置
            T3CCTL1 = 0xAC;
            // 定时器比较寄存器配置
            T3CC1 = 0xFF;
            break;
        case TIM3_CH1_PORT_P1_7:
            // 配置定时器3的IO位置为备用位置2
            PERCFG |= 0x20;
            // 配置端口1的外设优先级为定时器3优先
            P2SEL |= 0x20;
            // 配置P2_1为外设IO
            P1SEL |= 0x80;
            // 定时器输出比较配置
            T3CCTL1 = 0xAC;
            // 定时器比较寄存器配置
            T3CC1 = 0xFF;
            break;
        default:
            return false;
                break;
    }
    // 启动定时器3 128时钟分频
    T3CTL = 0xF4;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       定时器4 计时功能初始化
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        定时周期大约为1ms
 *******************************************************************************
 */
void Timer4_Init( void )
{
    T4CTL = 0xFC;
    IEN1 |= 0x10;
}

/**
 *******************************************************************************
 * @brief       定时器4输出比较功能初始化
 * @param       [in/out]  CHANNEL_PIN    输出比较通道引脚
 * @return      [in/out]  false          配置失败
 * @return      [in/out]  true           配置成功
 * @note        PWM周期大约为1ms
 *******************************************************************************
 */
bool Timer4_PWM_Init( uint8 CHANNEL_PIN )
{
    switch( CHANNEL_PIN )
    {
        case TIM4_CH0_PORT_P1_0:
            // 配置定时器3的IO位置为备用位置1
            PERCFG &= ~0x10;
            // 配置P1_0为外设IO
            P1SEL |= 0x01;
            // 定时器输出比较配置
            T4CCTL0 = 0xAC;
            // 定时器比较寄存器配置
            T4CC0 = 0xFF;
            break;
        case TIM4_CH0_PORT_P2_0:
            // 配置定时器3的IO位置为备用位置2
            PERCFG |= 0x10;
            // 配置P2_0为外设IO
            P2SEL |= 0x01;
            // 定时器输出比较配置
            T4CCTL0 = 0xAC;
            // 定时器比较寄存器配置
            T4CC0 = 0xFF;
            break;
        case TIM4_CH1_PORT_P1_1:
            // 配置定时器3的IO位置为备用位置1
            PERCFG &= ~0x10;
            // 配置P1_1为外设IO
            P1SEL |= 0x02;
            // 定时器输出比较配置
            T4CCTL1 = 0xAC;
            // 定时器比较寄存器配置
            T4CC1 = 0xFF;
            break;
        case TIM4_CH1_PORT_P2_1:
            // 配置定时器3的IO位置为备用位置2
            PERCFG |= 0x10;
            // 配置P2_1为外设IO
            P2SEL |= 0x02;
            // 定时器输出比较配置
            T4CCTL1 = 0xAC;
            // 定时器比较寄存器配置
            T4CC1 = 0xFF;
            break;
        default:
            return false;
                break;
    }
    // 启动定时器3 128时钟分频
    T4CTL = 0xF4;
    
    return true;
}

/**
 *******************************************************************************
 * @brief       定时器4输出比较功能复位
 * @param       [in/out]  CHANNEL_PIN    输出比较通道引脚
 * @return      [in/out]  false          配置失败
 * @return      [in/out]  true           配置成功
 * @note        默认复位IO端口为输出高电平状态
 *******************************************************************************
 */
bool Timer4_PWM_Deinit( uint8 CHANNEL_PIN, bool status )
{
    switch( CHANNEL_PIN )
    {
        case TIM4_CH0_PORT_P1_0:
            // 定时器输出比较配置
            T4CCTL0 = 0x00;
            // 定时器比较寄存器配置
            T4CC0 = 0x00;    
            // 配置P1_0为普通IO
            P1SEL &= ~0x01;
            // 配置P1_0为输出状态
            P1DIR |= 0x01;
            
            // 配置P1_0输出电平
            if( status == true )
            {
                P1_0  |= 0x01;
            }
            else
            {
                P1_0  &= ~0x01;
            }
            break;
        case TIM4_CH0_PORT_P2_0:
            // 定时器输出比较配置
            T4CCTL0 = 0x00;
            // 定时器比较寄存器配置
            T4CC0 = 0x00;      
            // 配置定时器3的IO位置为备用位置2
            PERCFG &= ~0x10;
            // 配置P2_0为普通IO
            P2SEL |= 0x01;
            // 配置P2_0为输出状态
            P2DIR |= 0x01;

            // 配置P2_0输出电平
            if( status == true )
            {
                P2_0  |= 0x01;
            }
            else
            {
                P2_0  &= ~0x01;
            }
            break;
        case TIM4_CH1_PORT_P1_1:
            // 定时器输出比较配置
            T4CCTL1 = 0x00;
            // 定时器比较寄存器配置
            T4CC1 = 0x00;      
            // 配置P1_1为普通IO
            P1SEL &= ~0x02;
            // 配置P1_1为输出状态
            P1DIR |= 0x02;

            // 配置P1_1输出电平
            if( status == true )
            {
                P1_1  |= 0x01;
            }
            else
            {
                P1_1  &= ~0x01;
            }
            break;
        case TIM4_CH1_PORT_P2_1:
            // 定时器输出比较配置
            T4CCTL1 = 0x00;
            // 定时器比较寄存器配置
            T4CC1 = 0x00;     
            // 配置定时器3的IO位置为备用位置2
            PERCFG &= ~0x10;
            // 配置P2_1为普通IO
            P2SEL &= ~0x02;
            // 配置P2_1为输出状态
            P2DIR |= 0x02;
            
            // 配置P2_1输出电平
            if( status == true )
            {
                P2_1  |= 0x01;
            }
            else
            {
                P2_1  &= ~0x01;
            }
            break;
        default:
            return false;
                break;
    }
    
    // 关闭定时器4
    T4CTL = 0x00;
    
    return true;
}

/** @}*/     /* CC2530 定时器配置模块 */

/**********************************END OF FILE*********************************/
