/**
 ******************************************************************************
  * @file       gizwits_product.h
  * @author     Gizwits
  * @par        Modify
                    yearnext
  * @version    V03010101
  * @date       2016年12月1日
  * @brief      gizwits_product 头文件
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GIZWITS_PRODUCT_H__
#define __GIZWITS_PRODUCT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ZComDef.h"
#include "gizwits_protocol.h"
    
/* Exported macro ------------------------------------------------------------*/
/**
* MCU软件版本号
*/
#define SOFTWARE_VERSION "03010101"
/**
* MCU硬件版本号
*/
#define HARDWARE_VERSION "03010100"

#ifndef SOFTWARE_VERSION
    #error "no define SOFTWARE_VERSION"
#endif

#ifndef HARDWARE_VERSION
    #error "no define HARDWARE_VERSION"
#endif

/**@} */

/**@name 设备状态数据上报间隔时间宏定义
* @{
*/
#define REPORT_TIME_MAX 6000 //6S
/**@} */

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**@name Gizwits 用户API接口
* @{
*/
extern bool gizwitsEventProcess(eventInfo_t *info, uint8 *data, uint8 len);
/**@} */

#ifdef __cplusplus
}
#endif

#endif      /** __GIZWITS_PROTOCOL_H__ */

/**********************************END OF FILE*********************************/
