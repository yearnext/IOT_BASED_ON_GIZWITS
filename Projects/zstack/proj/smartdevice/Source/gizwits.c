 /**
 ******************************************************************************
  * @file       gizwits.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      gizwits 源文件
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
 * @defgroup gizwits模块
 * @{
 */

#if defined ( USE_GIZWITS_MOD )

/* Includes ------------------------------------------------------------------*/
#include "OSAL.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"
#include "ZGlobals.h"

#include "gizwits.h"
#include "myprotocol.h"

#include "onboard.h"

#include "MT.h"
#include "MT_UART.h"

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
 * @brief       OSAL任务初始化函数
 * @param       [in/out]  task_id    任务ID
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void app_gizwits_init( void )
{
    gizwitsInit();
    
    myprotocol_init();
    
    GIZWITS_LOG("Gizwits Init Finish!\n");
}

#endif
/** @}*/     /* gizwits模块 */

/**********************************END OF FILE*********************************/
