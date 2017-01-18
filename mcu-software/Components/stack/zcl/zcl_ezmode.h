/**************************************************************************************************
  Filename:       zcl_ezmode.h
  Revised:        $Date: 2014-12-03 14:48:39 -0800 (Wed, 03 Dec 2014) $
  Revision:       $Revision: 41325 $

  Description:    This file contains the ZCL EZ Mode definitions.


  Copyright 2013 - 2014 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef ZCL_EZMODE_H
#define ZCL_EZMODE_H

#ifdef ZCL_EZMODE

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * INCLUDES
 */
#include "zcl.h"
#if !defined ( ZCL_STANDALONE )
  #include "ZDObject.h"
#endif

/******************************************************************************
 * CONSTANTS
 */

// EZ-Mode error codes (EZMODE_STATE_FAILED)
#define EZMODE_ERR_SUCCESS                      0
#define EZMODE_ERR_BAD_PARAMETER                1  // invalid parameter
#define EZMODE_ERR_CANCELLED                    2  // Invoke called again
#define EZMODE_ERR_TIMEDOUT                     3  // didn't complete EZ mode in Windowtime.
#define EZMODE_ERR_NOMATCH                      4  // No match on EP

// EZ_Mode Timeouts
#define EZMODE_TIME                (3*60000)    // (EZModeTime) time to stay in EZ-mode 3 Minutes (in ms)
#define EZMODE_IDQUERYTIME            4000      // time between identify queries (ms)
#define EZMODE_AUTOCLOSETIME          4000      // time to stay in EZ-Mode after autoclose (after binding) (ms)

// EZ-Mode states
typedef uint8 zlcEZMode_State_t;
#define EZMODE_STATE_READY                      0   // Ez-mode ready to be invoked
#define EZMODE_STATE_JOINER                     2   // this node is joining a network
#define EZMODE_STATE_OPENER                     3   // already on a network, open it
#define EZMODE_STATE_IDENTIFYING                4   // open identify mode for EZMODE_IDWINDOWTIME
#define EZMODE_STATE_WAITING_IDENTIFYQUERYRSP   5   // Initiator only (waiting to find other nodes/EPs in EZ-Mode/Identify)
#define EZMODE_STATE_WAITING_MATCHDESCRSP       6   // Initiator only (waiting on Match Descriptor Response)
#define EZMODE_STATE_AUTOCLOSE                  7   // recieved IdentifyQuery or IdentifyQueryRsp. Wait a bit in case other side hasn't found us yet.
#define EZMODE_STATE_FINISH                     99  // done

// is this device a target or initiator (if both, set as initiator)
#define EZMODE_TARGET       0
#define EZMODE_INITIATOR    1

// for the commission state variable
#define EZMODE_COMMISSION_ON_NETWORK    0x01
#define EZMODE_COMMISSION_OPERATIONAL   0x02  // means bindings have completed

// EZ-Mode actions (may cause state change)
typedef uint8 zclEzMode_Action_t;
#define EZMODE_ACTION_PROCESS                   1   // time to process next state
#define EZMODE_ACTION_NETWORK_STARTED           2   // we've formed/joined the network
#define EZMODE_ACTION_IDENTIFY_QUERY            3   // received identify query while in identify mode
#define EZMODE_ACTION_IDENTIFY_QUERY_RSP        4   // received identify query rsp
#define EZMODE_ACTION_MATCH_DESC_RSP            5   // received match descriptor response
#define EZMODE_ACTION_TIMED_OUT                 99  // timed out

// EZ-Mode Invoke action bits (see HA 1.2 Spec), zcl_general.h, COMMAND_IDENTIFY_EZMODE_INVOKE
#define EZMODE_INVOKE_FACTORY_FRESH       0x01
#define EZMODE_INVOKE_NETWORK_STEERING    0x02
#define EZMODE_INVOKE_FINDING_BINDING     0x04
#define EZMODE_INVOKE_NORMAL              0x06    // invoke both network join and finding/binding

// EZ-Mode Commission State actions (for commission state bitmap)
#define EZMODE_COMMISSION_ACTION_NULL     0x00
#define EZMODE_COMMISSION_ACTION_SET      0x01    // or
#define EZMODE_COMMISSION_ACTION_CLEAR    0x02    // ~and

/*******************************************************************************
 * TYPEDEFS
 */

typedef struct
{
  uint8   err;
  uint8   ep;
  uint16  nwkaddr;
} zclEZMode_Finish_t;

typedef struct
{
  uint8   err;
} zclEZMode_AutoClose_t;

// data sent to the app callback function (depends on EZ-Mode state)
typedef union
{
  zclEZMode_AutoClose_t sAutoClose; // inform app we are autoclosing
  zclEZMode_Finish_t    sFinish;    // inform app we are finishing
} zclEZMode_CBData_t;

// used to notify applications of EZ-Mode state change
typedef void (*zclEZMode_NotifyCB_t)( zlcEZMode_State_t state, zclEZMode_CBData_t *pData );

// used to register EZ-Mode in app task init function
typedef struct
{
  byte *pTaskID;                  // task ID for the app task (where to set event bits)
                                  // For ZCL_STANDALONE, put pointer to endpoint here
  uint16 processEvt;              // what EVT bit does EZ-Mode use when it's time to process the next state?
  uint16 timeoutEvt;              // what EVT bit does EZ-Mode use to signal it's done?
  byte *pZclSeqNum;               // pointer to sequence # for ZCL
  zclEZMode_NotifyCB_t pfnNotifyCB;  // application notification callback
} zclEZMode_RegisterData_t;

// used to Invoke EZ-Mode on an endpoint
typedef struct
{
  uint8 endpoint;               // endpoint on which to invoke EZ-Mode
  uint8 onNetwork;              // (boolean) is the node already on a network?
  uint8 initiator;              // if initiator, it sends the IdentifyQuery, otherwise, waits for Identify Query
  uint8 numActiveInClusters;    // # of active input clusters (initiator only)
  uint16 *pActiveInClusterIDs;  // active input cluster IDs (initiator only)
  uint8 numActiveOutClusters;   // # of active output clusters (initiator only)
  uint16 *pActiveOutClusterIDs; // active output cluster IDs (initiator only)
} zclEZMode_InvokeData_t;

#if defined ( ZCL_STANDALONE )
// This structure must match ZDO_ActiveEndpointRsp_t in ZDObject.h
typedef struct
{
  uint8  status;
  uint16 nwkAddr;   // Network address of interest
  uint8  cnt;
  uint8  epList[];
} ZDO_MatchDescRsp_t;
#endif // ZCL_STANDALONE

// used by application to inform EZ-Mode of various application events

typedef union
{
  ZDO_MatchDescRsp_t      *pMatchDescRsp;
  zclIdentifyQueryRsp_t   *pIdentifyQueryRsp;
} zclEZMode_ActionData_t;

/******************************************************************************
 * FUNCTION MACROS
 */

/******************************************************************************
 * VARIABLES
 */

/******************************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      zcl_RegisterEZMode
 *
 * @brief   Called upon task initialation, to initialize EZ-Mode.
 *
 * @param   pData - task ID, App Callback routine, etc..
 *
 * @return  none
 */
void zcl_RegisterEZMode( zclEZMode_RegisterData_t const *pData );

/*********************************************************************
 * @fn      zcl_InvokeEZMode
 *
 * @brief   Used when invoking (or cancelling) EZ-Mode. It's a toggle.
 *
 * @param   pData - endpoint to invoke EZ-Mode on, active output clusters, etc...
 *
 * @return  none
 */
void zcl_InvokeEZMode( zclEZMode_InvokeData_t *pData );

/*********************************************************************
 * @fn      zcl_EZModeAction
 *
 * @brief   Called when some action has happened, such as identify query response.
 *
 * @param   action - the action that occurred
 *          pData - data associated with the action
 *
 * @return  none
 */
void zcl_EZModeAction(zclEzMode_Action_t action, zclEZMode_ActionData_t *pData);


/*********************************************************************
 * @fn      zcl_EZModeSendMatchDescReq
 *
 * @brief   Call to send a ZDP Match Descriptor Request
 *
 *   NOTE: Do not call this function outside of zcl_ezmode.c.
 *   NOTE2:  For ZCL_STANDALONE, this function needs to be implemented
 *           elsewhere.
 *
 * @param   srcID - source ID
 * @param   dstAddr - destination address
 * @param   nwkAddr - network address of interest
 * @param   NumInClusters - number of input clusters
 * @param   InClusterList - input cluster ID list
 * @param   NumOutClusters - number of output clusters
 * @param   OutClusterList - output cluster ID list
 *
 * @return  afStatus_t
 */
extern afStatus_t zcl_EZModeSendMatchDescReq( uint8 srcID,
                                              zAddrType_t *dstAddr,
                                              uint16 nwkAddr,
                                              uint8  NumInClusters,
                                              uint16 *InClusterList,
                                              uint8  NumOutClusters,
                                              uint16 *OutClusterList );

/*********************************************************************
 * @fn      zcl_EZModeStartTimer
 *
 * @brief   This function is called to start a timer to expire in n mSecs.
 *          When the timer expires, call zcl_EZModeAction().
 *
 *   NOTE: Do not call this function outside of zcl_ezmode.c.
 *   NOTE2:  For ZCL_STANDALONE, this function needs to be implemented
 *           elsewhere.
 *
 * @param   uint8 taskID - task id to set timer for
 * @param   uint16 event_id - event to be notified with
 * @param   uint32 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
extern afStatus_t zcl_EZModeStartTimer( uint8 task_id, uint16 event_id,
                                uint32 timeout_value );


/*********************************************************************
 * @fn      zcl_EZModeStopTimer
 *
 * @brief   This function is called to cancel a timer.
 *
 *   NOTE: Do not call this function outside of zcl_ezmode.c.
 *   NOTE2:  For ZCL_STANDALONE, this function needs to be implemented
 *           elsewhere.
 *
 * @param   uint8 taskID - task id to set timer for
 * @param   uint16 event_id - event to be notified with
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
extern afStatus_t zcl_EZModeStopTimer( uint8 task_id, uint16 event_id );

/*********************************************************************
 * @fn          zcl_EZModePermitJoiningRequest
 *
 * @brief       Call to set the permit joining for device and network.
 *
 *   NOTE: Do not call this function outside of zcl_ezmode.c.
 *   NOTE2:  For ZCL_STANDALONE, this function needs to be implemented
 *           elsewhere.
 *
 * @param       srcID - source task ID sending,
 *              for ZCL_STANDALONE, use source endpoint
 * @param       dstAddr - destination address of the message (short addr only),
 *                    NULL to send to local device only.
 * @param       duration - Permit duration
 *
 * @return      none
 */
extern void zcl_EZModePermitJoiningRequest( uint8 srcID,
                                            zAddrType_t *dstAddr,
                                            uint8 duration );

/*********************************************************************
 * @fn          zcl_EZModeStartDevice
 *
 * @brief       Call to start a device joining.
 *
 *   NOTE: Do not call this function outside of zcl_ezmode.c.
 *   NOTE2:  For ZCL_STANDALONE, this function needs to be implemented
 *           elsewhere.
 *
 * @param       srcID - source task ID sending,
 *              for ZCL_STANDALONE, use source endpoint
 * @param       startDelay - timeDelay to start device (in milliseconds)
 *
 * @return      none
 */
extern void zcl_EZModeStartDevice( uint8 srcID, uint16 startDelay );

/*********************************************************************
 * @fn      zcl_EZModeBindAddEntry()
 *
 * @brief   This function is used to Add an entry to the binding table
 *
 *   NOTE: Do not call this function outside of zcl_ezmode.c.
 *   NOTE2:  For ZCL_STANDALONE, this function needs to be implemented
 *           elsewhere.
 *
 * @param       srcEpInt - source endpoint
 * @param       dstAddr - destination Address
 * @param       dstEpInt - destination endpoint
 * @param       numClusterIds - number of cluster Ids in the list
 * @param       clusterIds - pointer to the Object ID list
 *
 * @return  pointer to binding table entry, NULL if not added
 */
extern void zcl_EZModeBindAddEntry( uint8 srcEpInt,
                              zAddrType_t *dstAddr, uint8 dstEpInt,
                              uint8 numClusterIds, uint16 *clusterIds );

/*********************************************************************
 * @fn      zcl_EZModeGetNwkAddr()
 *
 * @brief   This function is used to Add an entry to the binding table
 *
 *   NOTE: Do not call this function outside of zcl_ezmode.c.
 *   NOTE2:  For ZCL_STANDALONE, this function needs to be implemented
 *           elsewhere.
 *
 * @param       none
 *
 * @return  16 bit network address
 */
extern uint16 zcl_EZModeGetNwkAddr( void );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif
#endif // ZCL_EZMODE

#endif /* ZCL_EZMODE_H */
