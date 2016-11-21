 /**
 ******************************************************************************
  * @file       hal_hlw8012.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      HLW8012 驱动源文件
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
 * @defgroup HLW8012 驱动模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "hal_hlw8012.h"
#include <ioCC2530.h>
#include "hal_board_cfg.h"
#include "OnBoard.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 硬件寄存定义
#define HLW8012_SEL_PORT     P1_1
#define HLW8012_SEL_BV       BV(1)
#define HLW8012_SEL_SEL      P1SEL
#define HLW8012_SEL_DIR      P1DIR
#define HLW8012_SEL_POLARITY ACTIVE_HIGH

#define HLW8012_CF0_PORT     P1_2
#define HLW8012_CF0_BV       BV(2)
#define HLW8012_CF0_SEL      P1SEL
#define HLW8012_CF0_DIR      P1DIR
#define HLW8012_CF0_POLARITY ACTIVE_HIGH

#define HLW8012_CF1_PORT     P1_3
#define HLW8012_CF1_BV       BV(3)
#define HLW8012_CF1_SEL      P1SEL
#define HLW8012_CF1_DIR      P1DIR
#define HLW8012_CF1_POLARITY ACTIVE_HIGH
// 硬件功能定义
#define HLW8012_SEL_PORT_WrMode() ( HLW8012_SEL_DIR |=  HLW8012_SEL_BV )
#define HLW8012_SEL_PORT_RdMode() ( HLW8012_SEL_DIR &= ~HLW8012_SEL_BV )
#define SET_HLW8012_SEL_PORT()    ( HLW8012_SEL_PORT = 1 )
#define RST_HLW8012_SEL_PORT()    ( HLW8012_SEL_PORT = 0 )
#define Read_HLW8012_SEL_PORT()   ( DHT11_POLARITY(HLW8012_SEL_PORT) )

#define HLW8012_CF0_PORT_WrMode() ( HLW8012_CF0_DIR |=  HLW8012_CF0_BV )
#define HLW8012_CF0_PORT_RdMode() ( HLW8012_CF0_DIR &= ~HLW8012_CF0_BV )
#define SET_HLW8012_CF0_PORT()    ( HLW8012_CF0_PORT = 1 )
#define RST_HLW8012_CF0_PORT()    ( HLW8012_CF0_PORT = 0 )
#define Read_HLW8012_CF0_PORT()   ( DHT11_POLARITY(HLW8012_CF0_PORT) )

#define HLW8012_CF1_PORT_WrMode() ( HLW8012_CF1_DIR |=  HLW8012_CF1_BV )
#define HLW8012_CF1_PORT_RdMode() ( HLW8012_CF1_DIR &= ~HLW8012_CF1_BV )
#define SET_HLW8012_CF1_PORT()    ( HLW8012_CF1_PORT = 1 )
#define RST_HLW8012_CF1_PORT()    ( HLW8012_CF1_PORT = 0 )
#define Read_HLW8012_CF1_PORT()   ( DHT11_POLARITY(HLW8012_CF1_PORT) )
// 延时功能定义
#define HLW8012_DELAY(n)     Onboard_wait(n)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       HLW8012初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void hal_hlw8012_init( void )
{
}

/** @}*/     /* HLW8012 驱动模块 */

/**********************************END OF FILE*********************************/
