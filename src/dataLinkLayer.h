//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __INTELLIGENTROUTING_DATALINKLAYER_H_
#define __INTELLIGENTROUTING_DATALINKLAYER_H_

#define POLLS_PER_DATAFRAME 2.0
#define TIMEOUT_MULTIPLIER 25.0

#include "physicalLayer.h"
#include "ackFrame_m.h"
#include "dataFrame_m.h"

#include <omnetpp.h>
#include <list>
#include <queue>
#include <deque>

/**
 * Represents the Data Link layer of a WSN node.
 */
class DataLinkLayer : public cSimpleModule
{
public:
            int     getNumFreeSlotsInQueue();
            int     getBitLengthDataFrame();
            int     getBitLengthAckFrame();
            void    requestToBroadcastStatusFrame();
protected:
    virtual void    initialize();
    virtual void    handleMessage(cMessage *msg);
private:
            void    _determineNodesInInterferenceRange();
            bool    _isMediumFree();
            void    _printQSize();

            void    _performTxSequence();
            void    _scheduleAck(DataFrame* msgToAck);
            void    _scheduleTxAttempt();

            void    _receiveMsgFromNetworkLayer(cPacket* msg);
            void    _receiveMsgFromPhysicalLayer(cPacket* msg);
            void    _handleAck(AckFrame* ackMsg);
            void    _handleAckTimeout(cMessage* timeoutMsg);
            bool    _isDataFrameQueued(int treeId);


protected:
    // Node Knowledge
    cModule*    physicalLayerPtr;
    cModule*    networkLayerPtr;
    int         maxNumQueuedMsgs;
    int         bitLengthAckFrame;
    int         bitLengthDataFrame;
    int         bitLengthStatusFrame;
    bool        isPolling;
    bool        useAdaptivePolling;
    double      pollingTimerBaseDuration;
    double      ackTimeoutDuration;
    std::queue<AckFrame*>   ackMsgsQueue;
    std::deque<DataFrame*>  unsentMsgsDeque;
    std::list<DataFrame*>   waitingAckMsgsList;

    // Simulator Knowledge
    std::list<PhysicalLayer*> nodesInInterferenceRange;
    char qSizeInfo[16];
};

#endif
