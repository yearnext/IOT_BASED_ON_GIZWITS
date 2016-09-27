/**
 ******************************************************************************
  * @file       smart_device.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      smart_device 头文件
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SMART_DEVICE_H__
#define __SMART_DEVICE_H__

/* Includes ------------------------------------------------------------------*/
#include "ZComDef.h"

/* Exported macro ------------------------------------------------------------*/
/** 设备端点 */
#define SmartDevice_EndPoint   (20)

/** 智能灯类型设备ID */
#define SmartLight_ProfileID           (0x0001)
/** 智能开关类型设备ID */
#define SmartSwitch_ProfileID          (0x0002)
/** 智能窗帘类型设备ID */
#define SmartCurtain_ProfileID         (0x0003)
/** 温湿度传感器类型设备ID*/
#define SmartHumitureSensor_ProfileID  (0x0004)
// 雨滴传感器类型设备ID */
#define SmartRainSensor_ProfileID      (0x0005)
/** 智能家居类型设备ID */
#define SmartHome_ProfileID            (0x00FA)

#define SmartDevice_ProfileID SmartHome_ProfileID
/** 设备ID */
#define SmartDevice_DeviceID (0x0001)
/** 设备版本号 */
#define SmartDevice_Version  (0x00)
#define SmartDevice_Flags    (0)
/** 簇的数量 */
#define SamrtDevice_ClustersNum (sizeof(zclSmartDevice_InClusterList)/sizeof(zclSmartDevice_InClusterList[0]))
#define SmartDevice_Report_ClustersID  (0x0001)

#define SmartDevice_Report_Time (3000)

#define SmartDevice_Report_Enent (0x0008)

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void SmartDevice_Init( byte task_id );

extern uint16 SamrtDevice_ProcessEven( uint8 task_id, uint16 events );
#endif      /* __SMART_LIGHT_H__ */

/**********************************END OF FILE*********************************/
