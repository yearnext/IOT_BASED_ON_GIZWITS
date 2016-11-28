/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits 控制协议处理,及平台相关的硬件初始化 
* @author       Gizwits
* @date         2016-09-05
* @version      V03010101
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#include <string.h>
#include "gizwits_product.h"
#include "myprotocol.h"
#include "devicelist.h"

/** 用户区当前设备状态结构体*/
extern dataPoint_t currentDataPoint;

/**@name Gizwits 用户API接口
* @{
*/

/**
* @brief 事件处理接口

* 说明：

* 1.用户可以对WiFi模组状态的变化进行自定义的处理

* 2.用户可以在该函数内添加数据点事件处理逻辑，如调用相关硬件外设的操作接口

* @param[in] info : 事件队列
* @param[in] data : 协议数据
* @param[in] len : 协议数据长度
* @return NULL
* @ref gizwits_protocol.h
*/
int8 gizwitsEventProcess(eventInfo_t *info, uint8 *data, uint8 len)
{
  uint8 i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)data;
//  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)data;

  if((NULL == info) || (NULL == data))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {
      case EVENT_PACKET:
        GIZWITS_LOG("Evt: EVENT_PACKET\n");
//        memcpy((uint8 *)&currentDataPoint.valuePacket,(uint8 *)&dataPointPtr->valuePacket,sizeof(currentDataPoint.valuePacket));
        Gizwits_Message_Handler((uint8 *)&currentDataPoint.valuePacket,(uint8 *)&dataPointPtr->valuePacket);
        break;

      case WIFI_SOFTAP:
        break;
      case WIFI_AIRLINK:
        break;
      case WIFI_STATION:
        break;
      case WIFI_CON_ROUTER:
        break;
      case WIFI_DISCON_ROUTER:
            break;
      case WIFI_CON_M2M:
        break;
      case WIFI_DISCON_M2M:
        break;
      case WIFI_RSSI:
        break;
      case WIFI_CON_APP:
      {
        uint8 num = Get_DeviceNum_ForList();
        MYPROTOCOL_SEND_MSG(MYPROTOCOL_DIR_D2W,NULL,create_devicelist_update_packet,(void *)&num);
        
        GIZWITS_LOG("DEVICE NUM IS ");
        num += 0x30;
        GIZWITS_LOG(&num);
        GIZWITS_LOG("! \n");
        break;
      }
      case WIFI_DISCON_APP:
        break;
//      case TRANSPARENT_DATA:
//        break;
      default:
        break;
    }
  }

  return 0;
}
