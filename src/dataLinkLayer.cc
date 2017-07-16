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

#include "dataLinkLayer.h"
#include "Definitions.h"
#include "physicalLayer.h"
#include "networkLayer.h"
#include "statusFrame_m.h"
#include <algorithm>

Define_Module(DataLinkLayer);

/**
 * Returns number of free message slots in Queue.
 */
int DataLinkLayer::getNumFreeSlotsInQueue()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->maxNumQueuedMsgs - (this->unsentMsgsDeque.size() + this->waitingAckMsgsList.size());
}

int DataLinkLayer::getBitLengthDataFrame()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->bitLengthDataFrame;
}

int DataLinkLayer::getBitLengthAckFrame()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->bitLengthAckFrame;
}

/**
 * Requests the Data Link Layer to generate and broadcast a Status Frame for this node.
 */
void DataLinkLayer::requestToBroadcastStatusFrame()
{
    Enter_Method_Silent();  // Omnet requires this

    // A status frame is sent out during each transmission sequence (even if there are no other messages to send).
    this->_scheduleTxAttempt();
}

void DataLinkLayer::initialize()
{
    this->physicalLayerPtr = this->getParentModule()->getSubmodule("physical");
    this->networkLayerPtr = this->getParentModule()->getSubmodule("routing");
    this->bitLengthAckFrame = this->par("ackFrameLength");
    this->bitLengthDataFrame = this->par("dataFrameLength");
    this->bitLengthStatusFrame = this->par("statusFrameLength");
    this->maxNumQueuedMsgs = this->par("maxNumQueuedMsgs");
    this->isPolling = false;
    this->useAdaptivePolling = this->par("useAdaptivePolling");
    this->pollingTimerBaseDuration = ((double) this->bitLengthDataFrame) / ((double) this->getParentModule()->getSubmodule("physical")->par("transmissionRate")) / POLLS_PER_DATAFRAME;
    //this->ackTimeoutDuration = TIMEOUT_MULTIPLIER * ((double) this->bitLengthDataFrame) / ((double) this->getParentModule()->getSubmodule("physical")->par("transmissionRate"));
    this->ackTimeoutDuration = (double)this->par("ackTimeoutFactor") * ((double) this->bitLengthDataFrame) / ((double) this->getParentModule()->getSubmodule("physical")->par("transmissionRate"));

    // Print Buffer size to display
    this->_printQSize();

    // Schedule second initialization phase (once node locations have been set)
    scheduleAt(simTime() + 1.0, new cMessage("PostInitialize", SMSGKIND_INIT));

}

void DataLinkLayer::handleMessage(cMessage *msg)
{
    // Execute post-initialization phase
    if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_INIT)
    {
        this->_determineNodesInInterferenceRange();

        delete msg;
    }
    // Polling medium for chance to transmit
    else if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_TXATTEMPT)
    {
        // Toggle polling flag to off.  If we wish to keep polling we must re-enable it.
        this->isPolling = false;

        // Medium is free: transmit stuff
        if (this->_isMediumFree())
        {
            this->_performTxSequence();
        }
        // Schedule next transmission attempt
        else
        {
            this->_scheduleTxAttempt();
        }

        delete msg;
    }
    // An ACK timeout has triggered
    else if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_ACKTIMEOUT)
    {
        this->_handleAckTimeout(msg);
        delete msg;
    }

    // Message is from network layer
    else if (msg->getSenderModule() == this->networkLayerPtr)
    {
        this->_receiveMsgFromNetworkLayer(check_and_cast<cPacket*>(msg));
    }

    // Message is from physical layer
    else if (msg->getSenderModule() == this->physicalLayerPtr)
    {
        this->_receiveMsgFromPhysicalLayer(check_and_cast<cPacket*>(msg));
    }

    // something else happened - ignore it
    else
    {
        delete msg;
    }
}

/**
 * Determine which nodes are within radio transmission interference range of this node (including hidden node problem)
 * and populate the nodesInInterferenceRange list accordingly.
 */
void DataLinkLayer:: _determineNodesInInterferenceRange()
{
    cModule* nodePtr = this->getParentModule();
    cModule* networkPtr = nodePtr->getParentModule();

    int nodesWide = networkPtr->par("nodesWide");
    int nodesHigh = networkPtr->par("nodesHigh");

    PhysicalLayer* nPtr;
    PhysicalLayer* this_nPtr = check_and_cast<PhysicalLayer*>(nodePtr->getSubmodule("physical"));
    int distance;
    int numNodes = nodesWide * nodesHigh;

    for (int i = 0; i < numNodes; ++i)
    {
        nPtr = check_and_cast<PhysicalLayer*>(networkPtr->getSubmodule("sn", i)->getSubmodule("physical"));
        distance = this_nPtr->distanceTo(nPtr);

        if (distance <= (2 * this_nPtr->getTxRange()) && nPtr != this_nPtr)
            this->nodesInInterferenceRange.push_back(nPtr);
    }
}

/**
 * Returns true if no nodes within interference range are transmitting and no signals are propagating.
 */
bool DataLinkLayer::_isMediumFree()
{
    for (auto p : this->nodesInInterferenceRange)
    {
        if (p->isTransmitting() == true)
            return false;
    }
    return true;
}

/**
 * Prints the q size info to the gui
 */
void DataLinkLayer::_printQSize()
{
    int numQueued = this->unsentMsgsDeque.size() + this->waitingAckMsgsList.size();

    sprintf(this->qSizeInfo, "q:%d", numQueued);
    this->getParentModule()->getDisplayString().setTagArg("t", 0, this->qSizeInfo);
}

/**
 * Performs a single transmission sequence.
 */
void DataLinkLayer::_performTxSequence()
{
    DataFrame* dMsg;
    StatusFrame* sMsg;

    // Begin sequence by sending all queued ACK Frames
    while (this->ackMsgsQueue.empty() == false)
    {
        this->send(this->ackMsgsQueue.front(), "toPhysical");
        this->ackMsgsQueue.pop();
    }
    // Send a single queued DataFrame
    if (this->unsentMsgsDeque.empty() == false)
    {
        dMsg = this->unsentMsgsDeque.front();
        this->unsentMsgsDeque.pop_front();
        dMsg->setTxAttemptsMadeThisHop(dMsg->getTxAttemptsMadeThisHop() + 1);
        this->send(dMsg->dup(), "toPhysical");

        // Put the sent message into the waiting for ack list and schedule a timeout for retransmission
        this->waitingAckMsgsList.push_back(dMsg);
        cMessage* ackTimeoutMsg = new cMessage("TimeoutWaitingForAck", SMSGKIND_ACKTIMEOUT);
        ackTimeoutMsg->setContextPointer(dMsg);
        this->scheduleAt(simTime() + this->ackTimeoutDuration, ackTimeoutMsg);
    }
    // Create and broadcast a StatusFrame for this node
    sMsg = new StatusFrame();
    sMsg->setBitLength(this->bitLengthStatusFrame);
    sMsg->setSenderId(this->getParentModule()->getId());
    sMsg->setNumHopsToBaseStation( ((NetworkLayer*)this->networkLayerPtr)->getNumHopsToBaseStation() );
    sMsg->setNumTxToBaseStation( ((NetworkLayer*)this->networkLayerPtr)->getNumTxToBaseStation() );
    sMsg->setEnergyRemaining( ((PhysicalLayer*)this->physicalLayerPtr)->getEnergyRemaining() );
    sMsg->setQEValue( ((NetworkLayer*)this->networkLayerPtr)->getQEValue() );
    sMsg->setQBValue( ((NetworkLayer*)this->networkLayerPtr)->getQBValue() );
    this->send(sMsg, "toPhysical");

    // If unsent messages queue is not empty we keep polling the medium
    if (this->unsentMsgsDeque.empty() == false)
        this->_scheduleTxAttempt();
}

/**
 * Schedules an ACK message to be sent out on the next transmission.
 */
void DataLinkLayer::_scheduleAck(DataFrame* msgToAck)
{
    AckFrame* ackMsg = new AckFrame();
    ackMsg->setBitLength(this->bitLengthAckFrame);
    ackMsg->setSenderId(this->getParentModule()->getId());

    ackMsg->setTargetId(msgToAck->getSenderId());
    ackMsg->setMsgToAckId(msgToAck->getTreeId());

    this->ackMsgsQueue.push(ackMsg);

    this->_scheduleTxAttempt();
}

/**
 * Schedule a transmission attempt.
 */
void DataLinkLayer::_scheduleTxAttempt()
{
    simtime_t pollTimer;
    double qScaledPollTimerDuration = this->pollingTimerBaseDuration * ( 1.0 + (double)(this->maxNumQueuedMsgs - this->unsentMsgsDeque.size()) ) / (double)(this->maxNumQueuedMsgs);
    double ackScaledPollTimerDuration = this->pollingTimerBaseDuration / (double)(this->ackMsgsQueue.size() + 1.0);
    double ackScaledPollTimerDuration2 = this->pollingTimerBaseDuration * 2.0 / (2.0 + ( ((double)this->ackMsgsQueue.size()) / ((double)this->maxNumQueuedMsgs) ));
    double ackScaledPollTimerDuration3 = this->pollingTimerBaseDuration / (1.0 + pow(2 * ((double)this->ackMsgsQueue.size()) / ((double)this->maxNumQueuedMsgs), 2.0));
    double combinedScaledPollTimerDuration = 0.5 * qScaledPollTimerDuration + 0.5 * ackScaledPollTimerDuration;
    double combinedScaledPollTimerDuration2 = std::min(qScaledPollTimerDuration, ackScaledPollTimerDuration3);

    if (this->isPolling == false)
    {
        this->isPolling = true;

        if (this->useAdaptivePolling)
        {
            //pollTimer = uniform(combinedScaledPollTimerDuration * 0.5, combinedScaledPollTimerDuration * 1.5); 2.34
            //pollTimer = uniform(this->pollingTimerBaseDuration * 0.5, this->pollingTimerBaseDuration * 1.5); 2.43
            //pollTimer = this->pollingTimerBaseDuration; 2.30
            //pollTimer = combinedScaledPollTimerDuration; 2.27
            //pollTimer = combinedScaledPollTimerDuration2; // 2.285
            //pollTimer = uniform(combinedScaledPollTimerDuration * 0.8, combinedScaledPollTimerDuration * 1.2); //2.287 : 2.217
            //pollTimer = uniform(combinedScaledPollTimerDuration * 0.95, combinedScaledPollTimerDuration * 1.05); // 2.30
            pollTimer = uniform(combinedScaledPollTimerDuration2 * 0.8, combinedScaledPollTimerDuration2 * 1.2); // 2.271
        }
        else
        {
            pollTimer = this->pollingTimerBaseDuration;
        }
        this->scheduleAt(simTime() + pollTimer, new cMessage("TxAttempt", SMSGKIND_TXATTEMPT));
    }
}

/**
 * Receives message from network layer.
 */
void DataLinkLayer::_receiveMsgFromNetworkLayer(cPacket* msg)
{
    // Check if there is room in the queue.  If there is not, discard the message.
    // Ideally, this won't happen here but we check just in case.
    if (this->getNumFreeSlotsInQueue() > 0)
    {
        DataFrame* dMsg = check_and_cast<DataFrame*>(msg);
        dMsg->setTxAttemptsMadeThisHop(0);
        this->unsentMsgsDeque.push_back(dMsg);
        this->_printQSize();

        this->_scheduleTxAttempt();
    }
    else
    {
        EV << "Queue is full when receiving message from network layer." << endl;
        delete msg;
    }
}

/**
 * Receives message from physical layer.
 */
void DataLinkLayer::_receiveMsgFromPhysicalLayer(cPacket* msg)
{
    // Received Data Frame
    if (dynamic_cast<DataFrame*>(msg) != NULL)
    {
        DataFrame* dMsg = check_and_cast<DataFrame*>(msg);

        // Message was intended for us
        if (this->getParentModule()->getId() == dMsg->getNextHopId())
        {
            // Check if we already received this message (sender could be waiting on ACK)
            if (this->_isDataFrameQueued(dMsg->getTreeId()) == true)
            {
                this->_scheduleAck(dMsg);
                delete dMsg;
            }
            // Check if we have room in Queue to store the message
            else if (this->getNumFreeSlotsInQueue() > 0)
            {
                EV << "Message Received from: " << dMsg->getSenderId() << endl;
                //this->getParentModule()->bubble("Message Received");

                // Create and Queue ACK
                this->_scheduleAck(dMsg);

                // Pass up to Network Layer
                this->send(dMsg, "toNetwork");
            }
            // If we dont have room then discard the message
            else
            {
                this->getParentModule()->bubble("Message Dropped (Buffer Full)");
                delete dMsg;
            }
        }
        // Message not intended for us.  Discard it.
        else
            delete dMsg;
    }
    // Received ACK
    else if (dynamic_cast<AckFrame*>(msg) != NULL)
    {
        AckFrame* ackMsg = check_and_cast<AckFrame*>(msg);

        // Message was intended for us
        if (this->getParentModule()->getId() == ackMsg->getTargetId())
        {
            this->_handleAck(ackMsg);

            delete ackMsg;
        }
        // Message not intended for us.  Discard it.
        else
            delete ackMsg;
    }
    // Received StatusFrame
    else if (dynamic_cast<StatusFrame*>(msg) != NULL)
    {
        // Pass up to Network Layer
        this->send(msg, "toNetwork");
    }
    // Received something else.  Discard it.
    else
        delete msg;
}

/**
 * Handle the arriving ACK message.
 */
void DataLinkLayer::_handleAck(AckFrame* ackMsg)
{
    EV << "ACK Received from: " << ackMsg->getSenderId() << endl;
    //this->getParentModule()->bubble("ACK Received");

    // Find the data frame awaiting this ACK and remove it
    for (auto p : this->waitingAckMsgsList)
    {
        if (p->getTreeId() == ackMsg->getMsgToAckId())
        {
            this->waitingAckMsgsList.remove(p);
            this->_printQSize();
            break;
        }
    }
    // Data Frame might have moved to our send queue so check there as well
    for (std::deque<DataFrame*>::iterator it = this->unsentMsgsDeque.begin(); it != this->unsentMsgsDeque.end(); ++it)
    {
        if ((*it)->getTreeId() == ackMsg->getMsgToAckId())
        {
            this->unsentMsgsDeque.erase(it);
            this->_printQSize();
            break;
        }
    }
}

/**
 * Handles an ACK timeout event.
 */
void DataLinkLayer::_handleAckTimeout(cMessage* timeoutMsg)
{
    bool toBeDeleted = true;
    DataFrame* dMsgWaitingAck = (DataFrame*)timeoutMsg->getContextPointer();

    // Check if this message is still waiting to be acknowledged.  If not we delete it from memory and ignore this timeout.
    for (auto p : this->waitingAckMsgsList)
    {
        if (p->getId() == dMsgWaitingAck->getId())
        {
            EV << "ACK Timeout" << endl;
            this->getParentModule()->bubble("Timeout ACK");

            this->waitingAckMsgsList.remove(p);

            // Check if we want to attempt another transmission for this message
            if (p->getTxAttemptsMadeThisHop() < p->getTxAttemptsAllowedPerHop())
            {
                this->unsentMsgsDeque.push_front(p);
                this->_scheduleTxAttempt();
                toBeDeleted = false;
            }

            this->_printQSize();
            break;
        }
    }

    // Message was already acknowleged
    if (toBeDeleted)
        delete dMsgWaitingAck;
}

/**
 * Returns true if a DataFrame of this treeId is in either the unsent or waitingACK queue
 */
bool DataLinkLayer::_isDataFrameQueued(int treeId)
{
    for (auto p : this->unsentMsgsDeque)
    {
        if (p->getTreeId() == treeId)
            return true;
    }
    for (auto p : this->waitingAckMsgsList)
    {
        if (p->getTreeId() == treeId)
            return true;
    }

    return false;
}
