/**************************************************************************************************
  Filename:       zcl_ezmode.c
  Revised:        $Date: 2014-12-03 14:48:39 -0800 (Wed, 03 Dec 2014) $
  Revision:       $Revision: 41325 $

  Description:    Zigbee Cluster Library - EZ Mode


  Copyright 2013-2014 Texas Instruments Incorporated. All rights reserved.

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

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ezmode.h"

#if !defined ( ZCL_STANDALONE )
  #include "OSAL.h"
  #include "zcl_ha.h"
#endif


#ifdef ZCL_EZMODE

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL PROTOTYPES
 */
static void zcl_SetEZModeError( uint8 errorCode );
static void zcl_SetEZModeState( zlcEZMode_State_t newState );
static void zcl_ProcessEZMode( void );


/*********************************************************************
 * GLOBAL VARIABLES
 */

// internal EZ-Mode state machine
uint8  zclEZModeRegistered;
uint8  zclEZModeErr;
uint8  zclEZModeState;
uint8  zclEZModeOpener;
uint8  zclEZModeMatched;              // we were matched by a remote node
uint16 zclEZModeQueryRspNwkAddr;      // short address (on QueryRsp)
uint8  zclEZModeQueryRspEP;           // endpoint (on QueryRsp)
zclEZMode_RegisterData_t zclEZModeRegisterData; // registered once on init
zclEZMode_InvokeData_t   zclEZModeInvokeData; // user's configuration parameters
                                              // what endpoint, initiator, etc.

/*********************************************************************
 * GLOBAL FUNCTIONS
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
void zcl_RegisterEZMode( zclEZMode_RegisterData_t const *pData )
{
  // make a copy of the data. Cannot fail.
  zcl_memcpy( &zclEZModeRegisterData, (void *)pData,
              sizeof(zclEZMode_RegisterData_t) );
  zclEZModeRegistered = TRUE;
}

/*********************************************************************
 * @fn      zcl_InvokeEZMode
 *
 * @brief   Called to invoke EZ-Mode on an endpoint. This is a toggle
 *          (will cancel if EZ-Mode currently enabled). Note: there is only 1
 *          state machine. EZ-Mode can only be invoked on 1 endpoint at a time.
 *
 * @param   none
 *
 * @return  none
 */
void zcl_InvokeEZMode( zclEZMode_InvokeData_t *pData )
{
  // if not registered, do nothing
  if(!zclEZModeRegistered)
  {
    return;
  }

  // there is only 1 EZ-Mode state machine. If already in EZ-Mode, cancel it
  if(zclEZModeState != EZMODE_STATE_READY)
  {
    zcl_SetEZModeError ( EZMODE_ERR_CANCELLED );
    // needed to shut down timers, turn off joining, etc...
    zcl_SetEZModeState ( EZMODE_STATE_FINISH );
    return;
  }

  // copy the data, so we remember which endpoint, etc...
  zcl_memcpy( &zclEZModeInvokeData, pData, sizeof(zclEZMode_InvokeData_t) );

  // start with no error, and no QueryResponses in our list
  zcl_SetEZModeError ( EZMODE_ERR_SUCCESS );
  zclEZModeOpener = zclEZModeMatched = 0;

  // if already on network, just go to identify state
  if ( zclEZModeInvokeData.onNetwork )
  {
    zcl_SetEZModeState( EZMODE_STATE_OPENER );
  }

  // not already on network, form/join a network
  else
  {
    zcl_SetEZModeState( EZMODE_STATE_JOINER );
  }

  // start a total timeout for EZ_Mode (will cancel if not finished in this time)
  zcl_EZModeStartTimer( *zclEZModeRegisterData.pTaskID,
                     zclEZModeRegisterData.timeoutEvt, EZMODE_TIME );
}

/*********************************************************************
 * @fn      zcl_EZModeAction
 *
 * @brief   Called when the application needs to inform EZ-Mode of some action
 *          (now on the network, identify mode query, etc...)
 *
 * @param   action - which action has taken place
 *          pData  - the data unique to the action
 *
 * @return  none
 */
void zcl_EZModeAction(zclEzMode_Action_t action, zclEZMode_ActionData_t *pData)
{
  ZDO_MatchDescRsp_t *pMatchDescRsp;
  zAddrType_t dstAddr;

  // not in the EZ-Mode state machine, so do nothing
  if( zclEZModeState == EZMODE_STATE_READY )
    return;

  switch ( action )
  {
    case EZMODE_ACTION_PROCESS:
      zcl_ProcessEZMode();  // process next state
    break;

    case EZMODE_ACTION_NETWORK_STARTED:
      // once on the network, time to go on to the identify state
      if( zclEZModeState == EZMODE_STATE_JOINER )
      {
        // set local permit joining on locally only for joiners (openers turn it on across the network)
        zcl_EZModePermitJoiningRequest( *zclEZModeRegisterData.pTaskID, NULL,
                                       (byte)(EZMODE_TIME / 1000) ); // in seconds
        zcl_SetEZModeState( EZMODE_STATE_IDENTIFYING );
      }
    break;

    // received identify query
    case EZMODE_ACTION_IDENTIFY_QUERY:

      // targets just go to autoclose once they have been identified
      if ( !zclEZModeInvokeData.initiator )
      {
        zcl_SetEZModeState( EZMODE_STATE_AUTOCLOSE );
      }
    break;

    // received identify query response
    case EZMODE_ACTION_IDENTIFY_QUERY_RSP:

      if ( pData->pIdentifyQueryRsp->srcAddr->addr.shortAddr
          != zcl_EZModeGetNwkAddr() )
      {
        // remember the node we found via identify query
        zclEZModeQueryRspNwkAddr = pData->pIdentifyQueryRsp->srcAddr->addr.shortAddr;
        zclEZModeQueryRspEP = pData->pIdentifyQueryRsp->srcAddr->endPoint;

        // initiate match descriptor request on the remote node
        dstAddr.addrMode = Addr16Bit;
        dstAddr.addr.shortAddr = zclEZModeQueryRspNwkAddr;
        zcl_EZModeSendMatchDescReq( *zclEZModeRegisterData.pTaskID,
                          &dstAddr,
                          zclEZModeQueryRspNwkAddr,
                          zclEZModeInvokeData.numActiveOutClusters,
                          zclEZModeInvokeData.pActiveOutClusterIDs,
                          zclEZModeInvokeData.numActiveInClusters,
                          zclEZModeInvokeData.pActiveInClusterIDs );
        zcl_SetEZModeState( EZMODE_STATE_WAITING_MATCHDESCRSP );
      }
    break;

    // received match descriptor response, see if active clusters match
    case EZMODE_ACTION_MATCH_DESC_RSP:

      pMatchDescRsp = pData->pMatchDescRsp;
      if ( ( pMatchDescRsp && pMatchDescRsp->status == ZSuccess ) && ( pMatchDescRsp->cnt>0 ) )
      {
        zclEZModeMatched = TRUE;

        dstAddr.addr.shortAddr = zclEZModeQueryRspNwkAddr;
        dstAddr.addrMode = Addr16Bit;

        // bind each matching input cluster
        if ( zclEZModeInvokeData.numActiveInClusters )
        {
          zcl_EZModeBindAddEntry( zclEZModeInvokeData.endpoint, &dstAddr, zclEZModeQueryRspEP,
                        zclEZModeInvokeData.numActiveInClusters, zclEZModeInvokeData.pActiveInClusterIDs );
        }

        // bind each matching output cluster
        if ( zclEZModeInvokeData.numActiveOutClusters )
        {
          zcl_EZModeBindAddEntry( zclEZModeInvokeData.endpoint, &dstAddr, zclEZModeQueryRspEP,
                        zclEZModeInvokeData.numActiveOutClusters, zclEZModeInvokeData.pActiveOutClusterIDs );
        }
      }

      // time to close (wait a bit before finishing, to allow for multiple initiators)
      zcl_SetEZModeState( EZMODE_STATE_AUTOCLOSE );
    break;

    // timed out of EZ-Mode
    case EZMODE_ACTION_TIMED_OUT:
      // timed out
      if(zclEZModeState != EZMODE_STATE_READY)
      {
        zcl_SetEZModeError( EZMODE_ERR_TIMEDOUT );
        zcl_SetEZModeState( EZMODE_STATE_FINISH );
      }
    break;
  }   // switch ( action )

}

/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      zcl_SetEZModeState
 *
 * @brief   Move on to new state after a short wait.
 *
 * @param   none
 *
 * @return  none
 */
static void zcl_SetEZModeState( zlcEZMode_State_t newState )
{
  zclEZModeState = newState;
  zcl_EZModeStartTimer( *zclEZModeRegisterData.pTaskID, zclEZModeRegisterData.processEvt, 5 );
}

/*********************************************************************
 * @fn      zclSampleSw_SetEZModeError
 *
 * @brief   Called to set error code that will be reported on finish. Starts as EZMODE_ERR_SUCCESS.
 *
 * @param   none
 *
 * @return  none
 */
static void zcl_SetEZModeError( uint8 errorCode )
{
  zclEZModeErr = errorCode;
}

/*********************************************************************
 * @fn      zcl_ProcessEZMode
 *
 * @brief   Called when EZ-Mode changes state. See EZMODE_STATE_xxxx in zcl_ezmode.h
 *
 * @param   none
 *
 * @return  status
 */
static void zcl_ProcessEZMode( void )
{
  zAddrType_t dstAddr;
  afAddrType_t afDstAddr;
  zclEZMode_CBData_t cbData;

  dstAddr.addr.shortAddr = 0xfffc;        // all routers (for PermitJoin) devices
  dstAddr.addrMode = AddrBroadcast;

  afDstAddr.addr.shortAddr = 0xffff;      // all devices (for IdentifyQuery)
  afDstAddr.addrMode = afAddrBroadcast;
  afDstAddr.endPoint = 0xff;

  switch(zclEZModeState)
  {
    // openers will broadcast permit joining
    case EZMODE_STATE_OPENER:
      zclEZModeOpener = 1;

      // enable joining both locally and over-the-air
      zcl_EZModePermitJoiningRequest( *zclEZModeRegisterData.pTaskID, &dstAddr,
                                     (byte)(EZMODE_TIME / 1000) );

      // then go to identifying state
      zcl_SetEZModeState(EZMODE_STATE_IDENTIFYING);
    break;

    // joiners will try to join the network, and if success will go to identifying state
    case EZMODE_STATE_JOINER:
      zclEZModeOpener = 0;
      zcl_EZModeStartDevice(*zclEZModeRegisterData.pTaskID, 0);   // see ZDO_STATE_CHANGE in zclSampleSw_event_loop()
    break;

    // go into identify state
    case EZMODE_STATE_IDENTIFYING:

      // tell app to go into identify mode
      if ( zclEZModeRegisterData.pfnNotifyCB )
      {
        (*zclEZModeRegisterData.pfnNotifyCB)( zclEZModeState, NULL );
      }

      // initiators start looking for other nodes in identify mode
      if ( zclEZModeInvokeData.initiator )
      {
        zcl_SetEZModeState ( EZMODE_STATE_WAITING_IDENTIFYQUERYRSP );
      }
    break;

    // timeout out with no query response, send another
    case EZMODE_STATE_WAITING_IDENTIFYQUERYRSP:
      // ZStatus_t zclGeneral_SendIdentifyQuery( uint8 srcEP, afAddrType_t *dstAddr, uint8 disableDefaultRsp, uint8 seqNum );
      // NOTE: Ensure that Identify Cluster is enabled to use this function for EZ-Mode
      zclGeneral_SendIdentifyQuery( zclEZModeInvokeData.endpoint, &afDstAddr, TRUE, (*zclEZModeRegisterData.pZclSeqNum)++ );

      // wait some time before sending out the next IdentifyQuery, will stop when we get a response
      zcl_EZModeStartTimer( *zclEZModeRegisterData.pTaskID, zclEZModeRegisterData.processEvt, EZMODE_IDQUERYTIME );
      break;

    // waiting for simple descriptor response
    case EZMODE_STATE_WAITING_MATCHDESCRSP:
    break;

    // if waiting on autoclose, then we're done. Go to success.
    case EZMODE_STATE_AUTOCLOSE:

      // special case: if 2 initators, we only fail if no match from either side
      if( zclEZModeInvokeData.initiator && !zclEZModeMatched )
      {
        zcl_SetEZModeError ( EZMODE_ERR_NOMATCH );
      }

      // if user specified callback, call on AutoClose
      if ( zclEZModeRegisterData.pfnNotifyCB )
      {
        cbData.sAutoClose.err = zclEZModeErr;
        (*zclEZModeRegisterData.pfnNotifyCB)( zclEZModeState, &cbData );
      }

      // no longer will timeout, since cannot fail
      zcl_EZModeStopTimer( *zclEZModeRegisterData.pTaskID, zclEZModeRegisterData.timeoutEvt );

      // wait a little to turn off identify mode, to give time for the other side to discover
      // in case of complex devices (both target/initiator)
      zcl_EZModeStartTimer( *zclEZModeRegisterData.pTaskID, zclEZModeRegisterData.processEvt, EZMODE_AUTOCLOSETIME );

      // go to finish state after autoclose. Don't use zcl_SetEZModeState() because we don't want it to happen immediately
      zclEZModeState = EZMODE_STATE_FINISH;
    break;

    case EZMODE_STATE_FINISH:

      // no longer will timeout, since we're done
      zcl_EZModeStopTimer( *zclEZModeRegisterData.pTaskID, zclEZModeRegisterData.timeoutEvt );

      // if we opened the network, close it now (turn off joining)
      if ( zclEZModeOpener )
      {
        zcl_EZModePermitJoiningRequest( *zclEZModeRegisterData.pTaskID, &dstAddr, 0 );
      }

      // if user callback, inform them of the finish, which will also turn off identify
      if ( zclEZModeRegisterData.pfnNotifyCB )
      {
        cbData.sFinish.err = zclEZModeErr;
        cbData.sFinish.ep = zclEZModeQueryRspEP;
        cbData.sFinish.nwkaddr = zclEZModeQueryRspNwkAddr;
        (*zclEZModeRegisterData.pfnNotifyCB)( zclEZModeState, &cbData );
      }

      // done, back to ready state
      zclEZModeState = EZMODE_STATE_READY;
    break;
  }
}

#if !defined ( ZCL_STANDALONE )
/*********************************************************************
 * @fn      zcl_EZModeSendMatchDescReq
 *
 * @brief   Call to send a ZDP Match Descriptor Request
 *
 * @param   srcID - source task ID sending,
 *          for ZCL_STANDALONE, use source endpoint
 * @param   dstAddr - destination address
 * @param   nwkAddr - network address of interest
 * @param   NumInClusters - number of input clusters
 * @param   InClusterList - input cluster ID list
 * @param   NumOutClusters - number of output clusters
 * @param   OutClusterList - output cluster ID list
 *
 * @return  afStatus_t
 */
afStatus_t zcl_EZModeSendMatchDescReq( uint8 srcID,
                                       zAddrType_t *dstAddr,
                                       uint16 nwkAddr,
                                       uint8  NumInClusters,
                                       uint16 *InClusterList,
                                       uint8  NumOutClusters,
                                       uint16 *OutClusterList )
{
  (void)srcID;
  return ZDP_MatchDescReq( dstAddr,
                           nwkAddr,
                           ZCL_HA_PROFILE_ID,
                           NumInClusters,
                           InClusterList,
                           NumOutClusters,
                           OutClusterList,
                           FALSE );
}

/*********************************************************************
 * @fn      zcl_EZModeStartTimer
 *
 * @brief   This function is called to start a timer to expire in n mSecs.
 *          When the timer expires, call zcl_EZModeAction().
 *
 * @param   uint8 taskID - task id to set timer for
 * @param   uint16 event_id - event to be notified with
 * @param   uint32 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
afStatus_t zcl_EZModeStartTimer( uint8 task_id, uint16 event_id,
                                  uint32 timeout_value )
{
  return osal_start_timerEx( task_id, event_id, timeout_value );
}

/*********************************************************************
 * @fn      zcl_EZModeStopTimer
 *
 * @brief   This function is called to cancel a timer.
 *
 * @param   uint8 taskID - task id to set timer for
 * @param   uint16 event_id - event to be notified with
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
afStatus_t zcl_EZModeStopTimer( uint8 task_id, uint16 event_id )
{
  return osal_stop_timerEx( task_id, event_id );
}

/*********************************************************************
 * @fn      zcl_EZModePermitJoiningRequest
 *
 * @brief   Call to set the permit joining for device and network.
 *
 * @param   srcID - source task ID sending,
 *          for ZCL_STANDALONE, use source endpoint
 * @param   dstAddr - destination address of the message (short addr only),
 *                    NULL to send to local device only.
 * @param   duration - Permit duration
 *
 * @return  none
 */
void zcl_EZModePermitJoiningRequest( uint8 srcID, zAddrType_t *dstAddr,
                                    uint8 duration )
{
  zAddrType_t tmpAddr;

  (void)srcID;

  tmpAddr.addrMode = Addr16Bit;

  if ( dstAddr == NULL )
  {
    tmpAddr.addr.shortAddr = NLME_GetShortAddr();
  }
  else
  {
    tmpAddr.addr.shortAddr = dstAddr->addr.shortAddr;
  }

  // Trust Center significance is always true
  ZDP_MgmtPermitJoinReq( &tmpAddr, duration, TRUE, FALSE );
}

/*********************************************************************
 * @fn      zcl_EZModeStartDevice
 *
 * @brief   Call to start a device joining.
 *
 * @param   srcID - source task ID sending,
 *          for ZCL_STANDALONE, use source endpoint
 * @param   startDelay - timeDelay to start device (in milliseconds)
 *
 * @return      none
 */
void zcl_EZModeStartDevice( uint8 srcID, uint16 startDelay )
{
  (void)srcID;
  ZDOInitDevice( startDelay );
}

/*********************************************************************
 * @fn      zcl_EZModeBindAddEntry()
 *
 * @brief   This function is used to Add an entry to the binding table
 *
 * @param   srcEpInt - source endpoint
 * @param   dstAddr - destination Address
 * @param   dstEpInt - destination endpoint
 * @param   numClusterIds - number of cluster Ids in the list
 * @param   clusterIds - pointer to the Object ID list
 *
 * @return  pointer to binding table entry, NULL if not added
 */
void zcl_EZModeBindAddEntry( uint8 srcEpInt,
                             zAddrType_t *dstAddr, uint8 dstEpInt,
                             uint8 numClusterIds, uint16 *clusterIds )
{
  if ( pbindAddEntry )
  {
    if ( pbindAddEntry( srcEpInt, dstAddr, dstEpInt, numClusterIds, clusterIds ) )
    {
      ZDApp_NVUpdate();
    }
  }
}

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
uint16 zcl_EZModeGetNwkAddr( void )
{
  return ( NLME_GetShortAddr() );
}
#endif // !ZCL_STANDALONE

#endif // ZCL_EZMODE
