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

#include "Definitions.h"
#include "networkLayer.h"
#include "applicationLayer.h"
#include "dataLinkLayer.h"
#include "physicalLayer.h"
#include "dataFrame_m.h"
#include "statusFrame_m.h"

Define_Module(NetworkLayer);

int NetworkLayer::getNumHopsToBaseStation()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->myInfo->numHopsToBaseStation;
}

double  NetworkLayer::getNumTxToBaseStation()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->myInfo->numTxToBaseStation;
}

double NetworkLayer::getQEValue()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->myInfo->qeValue;
}

double NetworkLayer::getQBValue()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->myInfo->qbValue;
}

void NetworkLayer::initialize()
{
    this->appLayerPtr = this->getParentModule()->getSubmodule("app");
    this->dataLinkLayerPtr = this->getParentModule()->getSubmodule("mac");
    this->physicalLayerPtr = this->getParentModule()->getSubmodule("physical");

    this->_qDiscountedRewardFactor = this->par("qDiscountedRewardFactor");
    this->_qEnergyDiffScalingFactor = this->par("qEnergyDiffScalingFactor");
    this->_qNumTxDiffScalingFactor = this->par("qNumTxDiffScalingFactor");
    this->_qLearnRate = this->par("qLearnRate");
    this->_qWeightEnergy = this->par("qWeightEnergy");

    // Schedule second initialization phase (once node locations have been set)
    scheduleAt(simTime() + 1.0, new cMessage("PostInitialize", SMSGKIND_INIT));

    // Schedule Network Discovery Phase @HACK
    int nHigh = this->getParentModule()->getParentModule()->par("nodesHigh");
    int nWide = this->getParentModule()->getParentModule()->par("nodesWide");
    if (this->getParentModule()->getIndex() == (nWide * nHigh / 2) )
        scheduleAt(simTime() + 2.0, new cMessage("NetworkDiscovery", SMSGKIND_NETWORKDISCOVERY));

}

void NetworkLayer::handleMessage(cMessage *msg)
{
    // Execute post-initialization phase
    if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_INIT)
    {
        this->_initializeNodeInfo();

        delete msg;
    }
    // Begin Network Discovery phase with this node as origin
    else if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_NETWORKDISCOVERY)
    {
        ((DataLinkLayer*)this->dataLinkLayerPtr)->requestToBroadcastStatusFrame();

        delete msg;
    }

    // Message is from application layer
    else if (msg->getSenderModule() == this->appLayerPtr)
    {
        this->_forwardMsg(check_and_cast<cPacket*>(msg));
    }

    // Message is from data link layer
    else if (msg->getSenderModule() == this->dataLinkLayerPtr)
    {
        this->_receiveFromDataLinkLayer(check_and_cast<cPacket*>(msg));
    }

    // something else happened - ignore it
    else
        delete msg;
}

/**
 * Determine 1-hop neighbours of this node and populate neighboursList with NodeInfo.
 * Also populate NodeInfo for known destinations.
 */
void NetworkLayer::_initializeNodeInfo()
{
    cModule* nodePtr = this->getParentModule();
    cModule* networkPtr = nodePtr->getParentModule();

    int nodesWide = networkPtr->par("nodesWide");
    int nodesHigh = networkPtr->par("nodesHigh");

    NetworkLayer* nPtr;
    NetworkLayer* this_nPtr = check_and_cast<NetworkLayer*>(nodePtr->getSubmodule("routing"));
    NodeInfo* niPtr;
    int distance;
    int numNodes = nodesWide * nodesHigh;

    // Node Info for myself
    this->myInfo = new NodeInfo();
    this->myInfo->id = this->getParentModule()->getId();
    this->myInfo->x_pos = ((PhysicalLayer*)this->physicalLayerPtr)->getXPos();
    this->myInfo->y_pos = ((PhysicalLayer*)this->physicalLayerPtr)->getYPos();
    this->myInfo->energyRemaining = ((PhysicalLayer*)this->physicalLayerPtr)->getEnergyRemaining();
    this->myInfo->numHopsToBaseStation = CONST_LARGE_INTVAL;
    this->myInfo->numTxToBaseStation = CONST_LARGE_INTVAL;
    this->myInfo->qeValue = 1.0;
    this->myInfo->qbValue = 1.0;
    if (this->getParentModule()->getIndex() == (numNodes / 2) )
    {
        this->myInfo->numHopsToBaseStation = 0;
        this->myInfo->numTxToBaseStation = 0.0;
    }

    // Node Info for 1-hop neighbours
    for (int i = 0; i < numNodes; ++i)
    {
        nPtr = check_and_cast<NetworkLayer*>(networkPtr->getSubmodule("sn", i)->getSubmodule("routing"));
        distance = ((PhysicalLayer*)this_nPtr->physicalLayerPtr)->distanceTo( (PhysicalLayer*)nPtr->physicalLayerPtr );

        if (distance <= ((PhysicalLayer*)this_nPtr->physicalLayerPtr)->getTxRange() && nPtr != this_nPtr)
        {
            niPtr = new NodeInfo();
            niPtr->id = nPtr->getParentModule()->getId();
            niPtr->x_pos = ((PhysicalLayer*)nPtr->physicalLayerPtr)->getXPos();
            niPtr->y_pos = ((PhysicalLayer*)nPtr->physicalLayerPtr)->getYPos();
            niPtr->energyRemaining = ((PhysicalLayer*)nPtr->physicalLayerPtr)->getEnergyRemaining();
            niPtr->numHopsToBaseStation = CONST_LARGE_INTVAL;
            niPtr->numTxToBaseStation = CONST_LARGE_INTVAL;
            niPtr->qeValue = 1.0;
            niPtr->qbValue = 1.0;
            this->neighboursList.push_back(niPtr);
        }
    }

    // Node Info for "base station" destination
    nPtr = check_and_cast<NetworkLayer*>(networkPtr->getSubmodule("sn", numNodes/2)->getSubmodule("routing"));
    niPtr = new NodeInfo();
    niPtr->id = nPtr->getParentModule()->getId();
    niPtr->x_pos = ((PhysicalLayer*)nPtr->physicalLayerPtr)->getXPos();
    niPtr->y_pos = ((PhysicalLayer*)nPtr->physicalLayerPtr)->getYPos();
    niPtr->energyRemaining = ((PhysicalLayer*)nPtr->physicalLayerPtr)->getEnergyRemaining();
    niPtr->numHopsToBaseStation = 0;
    niPtr->numTxToBaseStation = 0.0;
    niPtr->qeValue = 1.0;
    niPtr->qbValue = 1.0;
    this->knownDestinationsMap[niPtr->id] = niPtr;
}

/**
 * Returns the Euclidean distance between the two nodes specified.
 */
int NetworkLayer::_distanceBetween(NodeInfo* niPtr1, NodeInfo* niPtr2)
{
    return sqrt((niPtr1->x_pos - niPtr2->x_pos) * (niPtr1->x_pos - niPtr2->x_pos) + (niPtr1->y_pos - niPtr2->y_pos) * (niPtr1->y_pos - niPtr2->y_pos));
}

/**
 * Determines forwarding node for the msg and sets msg header data accordingly.
 * Passes the msg to data link layer.
 */
void NetworkLayer::_forwardMsg(cPacket* msg)
{
    int destinationId;
    int forwardingNodeId;
    int protocol;

    // Figure out the destination node id and routing protocol
    if (dynamic_cast<DataFrame*>(msg) != NULL)
    {
        destinationId = ((DataFrame*)msg)->getDestinationId();
        protocol = ((DataFrame*)msg)->getProtocol();
    }

    // Figure out the forwarding node id using the routing protocol specified
    if (protocol == PROTOCOL_GREEDYGRADIENT)
        forwardingNodeId = this->_determineForwardingNodeId_GreedyGradient(destinationId);
    else if (protocol == PROTOCOL_ENERGYAWAREGRADIENT)
        forwardingNodeId = this->_determineForwardingNodeId_EnergyAwareGradient(destinationId);
    else if (protocol == PROTOCOL_ENERGYSMARTGRADIENT)
        forwardingNodeId = this->_determineForwardingNodeId_EnergySmartGradient(destinationId);
    else if (protocol == PROTOCOL_ENERGYAWAREHOPS)
        forwardingNodeId = this->_determineForwardingNodeId_EnergyAwareHops(destinationId);
    else if (protocol == PROTOCOL_ENERGYSMARTHOPS)
        forwardingNodeId = this->_determineForwardingNodeId_EnergySmartHops(destinationId);
    else if (protocol == PROTOCOL_QESMART)
        forwardingNodeId = this->_determineForwardingNodeId_QESmart(destinationId);
    else if (protocol == PROTOCOL_QBSMART)
        forwardingNodeId = this->_determineForwardingNodeId_QBSmart(destinationId);
    else
        forwardingNodeId = this->_determineForwardingNodeId_GreedyGeo(destinationId);   //< Default protocol

    // If we can't forward the message then drop it
    if (forwardingNodeId == this->myInfo->id)
    {
        this->getParentModule()->bubble("Message Dropped (Can't Forward)");
        EV << "Message Dropped (Can't Forward)" << endl;
        delete msg;
        return;
    }
    // Otherwise, update the corresponding message fields
    else if (dynamic_cast<DataFrame*>(msg) != NULL)
    {
       ((DataFrame*)msg)->setNextHopId(forwardingNodeId);
       ((DataFrame*)msg)->setSenderId(this->getParentModule()->getId());
    }

    // Pass down to data link layer to be forwarded
    send(msg, "toDataLink");
}

/**
 * Determine Forwarding Node Id using Greedy Geometric Routing.
 */
int NetworkLayer:: _determineForwardingNodeId_GreedyGeo(int destinationId)
{
    // Check if we know this destination.  If we dont, we return our id.
    if (this->knownDestinationsMap.count(destinationId) == 0)
        return this->getParentModule()->getId();

    // Otherwise, proceed as normal
    int forwardingNodeId = this->myInfo->id;
    double progress;
    double bestProgress = 0.0;
    double myDistanceToDest = (double)this->_distanceBetween(this->myInfo, this->knownDestinationsMap[destinationId]);

    for (auto ni : this->neighboursList)
    {
        double neighbourDistanceToDest = (double)this->_distanceBetween(ni, this->knownDestinationsMap[destinationId]);
        double estimatedSuccessRate = 1.0 - ((PhysicalLayer*)this->physicalLayerPtr)->estimatePER((double)this->_distanceBetween(this->myInfo, ni));
        progress = estimatedSuccessRate * (myDistanceToDest - neighbourDistanceToDest);

        if (progress > bestProgress)
        {
            bestProgress = progress;
            forwardingNodeId = ni->id;
        }
    }

    return forwardingNodeId;
}

/**
 * Determine Forwarding Node Id using Greedy Gradient Routing.
 */
int NetworkLayer::_determineForwardingNodeId_GreedyGradient(int destinationId)
{
    // Check if we know this destination.  If we dont, we return our id.
    if (this->knownDestinationsMap.count(destinationId) == 0)
        return this->getParentModule()->getId();

    // Otherwise, proceed as normal
    int forwardingNodeId = this->myInfo->id;
    double txProgress;
    double bestTxProgress = 0.0;

    // Select the node with which we can achieve the most average transmission progress towards the destination
    for (auto ni : this->neighboursList)
    {
        double estimatedSuccessRate = 1.0 - ((PhysicalLayer*)this->physicalLayerPtr)->estimatePER((double)this->_distanceBetween(this->myInfo, ni));
        txProgress = estimatedSuccessRate * (this->myInfo->numTxToBaseStation - ni->numTxToBaseStation);

        if (txProgress > bestTxProgress)
        {
            bestTxProgress = txProgress;
            forwardingNodeId = ni->id;
        }
    }

    return forwardingNodeId;
}

/**
 * Determine Forwarding Node Id using Energy Aware Gradient Routing.
 */
int NetworkLayer::_determineForwardingNodeId_EnergyAwareGradient(int destinationId)
{
    // Check if we know this destination.  If we dont, we return our id.
    if (this->knownDestinationsMap.count(destinationId) == 0)
        return this->getParentModule()->getId();

    // Otherwise, proceed as normal
    int forwardingNodeId = this->myInfo->id;
    double energyTxProgress;
    double bestEnergyTxProgress = 0.0;

    // Select the node with which we can achieve the most average transmission progress towards the destination
    for (auto ni : this->neighboursList)
    {
        double estimatedSuccessRate = 1.0 - ((PhysicalLayer*)this->physicalLayerPtr)->estimatePER((double)this->_distanceBetween(this->myInfo, ni));
        energyTxProgress = estimatedSuccessRate * (this->myInfo->numTxToBaseStation - ni->numTxToBaseStation) * ni->energyRemaining;

        if (energyTxProgress > bestEnergyTxProgress)
        {
            bestEnergyTxProgress = energyTxProgress;
            forwardingNodeId = ni->id;
        }
    }

    return forwardingNodeId;
}

/**
 * Determine Forwarding Node Id using Energy Smart Gradient Routing.
 */
int NetworkLayer::_determineForwardingNodeId_EnergySmartGradient(int destinationId)
{
    // Check if we know this destination.  If we dont, we return our id.
    if (this->knownDestinationsMap.count(destinationId) == 0)
        return this->getParentModule()->getId();

    // Otherwise, proceed as normal
    int forwardingNodeETPId = this->myInfo->id;
    int forwardingNodeESDId = -1;
    double bestEnergyTxProgress = 0.0;
    double bestEnergySuccessDistProgress = 0.15; //< Used as best value tracker AND min threshold
    double minSuccessThreshold = 0.6;

    // Select the node with which we can achieve the most average transmission progress towards the destination
    for (auto ni : this->neighboursList)
    {
        double estimatedSuccessRate = 1.0 - ((PhysicalLayer*)this->physicalLayerPtr)->estimatePER((double)this->_distanceBetween(this->myInfo, ni));
        double energyTxProgress = estimatedSuccessRate * (this->myInfo->numTxToBaseStation - ni->numTxToBaseStation) * ni->energyRemaining;

        // Priority is any node that:
        // a) has numTxToBase <= our numTxToBase
        // b) has estimatedSuccessRate >= minSuccesThreshold
        // c) and txDistance x estimatedSuccessRate x energyDif(us, them) > energyDifThreshold
        if (ni->numTxToBaseStation <= this->myInfo->numTxToBaseStation && estimatedSuccessRate >= minSuccessThreshold)
        {
            double txDistance = (double)this->_distanceBetween(this->myInfo, ni);
            double energyDif = ni->energyRemaining - this->myInfo->energyRemaining;
            double energySuccessDistProgress = txDistance * estimatedSuccessRate * pow(energyDif, 0.5);

            if (energyDif < 0)
                energySuccessDistProgress *= -1.0;

            if (energySuccessDistProgress > bestEnergySuccessDistProgress)
            {
                bestEnergySuccessDistProgress = energySuccessDistProgress;
                forwardingNodeESDId = ni->id;
            }
        }

        // If a node as defined above does not exist then we pick as with Energy Aware Gradient
        if (energyTxProgress > bestEnergyTxProgress)
        {
            bestEnergyTxProgress = energyTxProgress;
            forwardingNodeETPId = ni->id;
        }
    }

    if (forwardingNodeESDId > -1)
        return forwardingNodeESDId;
    else
        return forwardingNodeETPId;
}

/**
 * Determine Forwarding Node Id using Hop based Energy Aware Routing.
 */
int NetworkLayer::_determineForwardingNodeId_EnergyAwareHops(int destinationId)
{
    // Check if we know this destination.  If we dont, we return our id.
    if (this->knownDestinationsMap.count(destinationId) == 0)
        return this->getParentModule()->getId();

    int forwardingNodeId = this->myInfo->id;
    double bestEnergy = 0.0;

    // Iterate 1-hop neighbours and find highest energy neighbour that is 1-hop nearer to base station than us
    for (auto ni : this->neighboursList)
    {
        if (ni->numHopsToBaseStation < this->myInfo->numHopsToBaseStation)
        {
            if (ni->energyRemaining > bestEnergy)
            {
                forwardingNodeId = ni->id;
                bestEnergy = ni->energyRemaining;
            }
        }
    }

    return forwardingNodeId;
}

/**
 * Determine Forwarding Node Id using Hop based Energy Smart Routing.
 */
int NetworkLayer::_determineForwardingNodeId_EnergySmartHops(int destinationId)
{
    // Check if we know this destination.  If we dont, we return our id.
    if (this->knownDestinationsMap.count(destinationId) == 0)
        return this->getParentModule()->getId();

    int forwardingNodeId = this->myInfo->id;
    int forwardingNodeIdSameDist = -1;
    double bestEnergy = 0.0;
    double bestEnergySameDist = 0.0;

    // Iterate 1-hop neighbours and find highest energy neighbour that is 1-hop nearer to base station than us, or
    // find a neighbour with same hop distance to base as us and with 1.05 times our energy
    for (auto ni : this->neighboursList)
    {
        // Find highest energy node at same hop distance (if it has more than our energy level + 0.1)
        if (ni->numHopsToBaseStation == this->myInfo->numHopsToBaseStation && (ni->energyRemaining - this->myInfo->energyRemaining) > 0.1)//  > (1.1 * this->myInfo->energyRemaining) && ni->energyRemaining > 0)
        {
            //EV << "Energy Smart" << endl;
            if (ni->energyRemaining > bestEnergySameDist)
            {
                forwardingNodeIdSameDist = ni->id;
                bestEnergySameDist = ni->energyRemaining;
            }
        }
        // Find highest energy node that is nearer than us
        if (ni->numHopsToBaseStation < this->myInfo->numHopsToBaseStation)
        {
            if (ni->energyRemaining > bestEnergy)
            {
                forwardingNodeId = ni->id;
                bestEnergy = ni->energyRemaining;
            }
        }
    }

    if (forwardingNodeIdSameDist != -1)
        return forwardingNodeIdSameDist;
    else
        return forwardingNodeId;
}

/**
 * Determine Forwarding Node Id using QLearning variant of EnergySmart hops based routing.
 */
int NetworkLayer::_determineForwardingNodeId_QESmart(int destinationId)
{
    // Check if we know this destination.  If we dont, we return our id.
    if (this->knownDestinationsMap.count(destinationId) == 0)
        return this->getParentModule()->getId();

    int     forwardingNodeId = this->myInfo->id;
    double  bestNewQEValue = -1.0;
    double  newQEValue;

    for (auto ni : this->neighboursList)
    {
        // Determine our new Q E Value if we forward to this neighbour
        newQEValue = _qeValueIfForwardTo(ni);

        // Pick neighbour yielding highest new Q Value
        if (newQEValue > bestNewQEValue)
        {
            forwardingNodeId = ni->id;
            bestNewQEValue = newQEValue;
        }
    }

    // Update our QEValue
    this->myInfo->qeValue = (1.0 - this->_qLearnRate) * this->myInfo->qeValue + this->_qLearnRate * bestNewQEValue;

    return forwardingNodeId;
}

/**
 * Determine Forwarding Node Id using QLearning for bandwidth based routing.
 */
int NetworkLayer::_determineForwardingNodeId_QBSmart(int destinationId)
{
    // Check if we know this destination.  If we dont, we return our id.
    if (this->knownDestinationsMap.count(destinationId) == 0)
        return this->getParentModule()->getId();

    int     forwardingNodeId = this->myInfo->id;
    double  bestNewQBValue = -1.0;
    double  newQBValue;

    for (auto ni : this->neighboursList)
    {
        // Determine our new Q E Value if we forward to this neighbour
        newQBValue = _qbValueIfForwardTo(ni);

        // Pick neighbour yielding highest new Q Value
        if (newQBValue > bestNewQBValue)
        {
            forwardingNodeId = ni->id;
            bestNewQBValue = newQBValue;
        }
    }

    // Update our QEValue
    this->myInfo->qbValue = (1.0 - this->_qLearnRate) * this->myInfo->qbValue + this->_qLearnRate * bestNewQBValue;

    return forwardingNodeId;
}

/**
 * Calculate the QE-Value of forwarding packet to node specified.
 */
double  NetworkLayer::_qeValueIfForwardTo(NodeInfo* nPtr)
{
    double qeValue;
    double rewardTotal;
    double rewardProgress;
    double rewardEnergy;
    double numTxDiff;
    double energyDiff;
    double estimatedPER;

    // Determine the reward for progress to basestation
    numTxDiff = this->myInfo->numTxToBaseStation - nPtr->numTxToBaseStation;
    rewardProgress = ( tanh(numTxDiff * PI * this->_qNumTxDiffScalingFactor) + 1.0 ) / 2.0;

    // Determine the reward for energy difference
    energyDiff = nPtr->energyRemaining - this->myInfo->energyRemaining;
    rewardEnergy = ( tanh(energyDiff * PI * this->_qEnergyDiffScalingFactor) + 1.0 ) / 2.0;

    // Determine total reward via weighted sum of partial rewards
    rewardTotal = this->_qWeightEnergy * rewardEnergy + (1.0 - this->_qWeightEnergy) * rewardProgress;

    // Determine Q Value of this action as weighted sum of reward and destination node Q Value
    qeValue = this->_qDiscountedRewardFactor * nPtr->qeValue + (1.0 - this->_qDiscountedRewardFactor) * rewardTotal;

    // Scale by a factor of 1 - PER
    estimatedPER = ((PhysicalLayer*)this->physicalLayerPtr)->estimatePER((double)this->_distanceBetween(this->myInfo, nPtr));
    qeValue = qeValue * (1.0 - estimatedPER);

    return qeValue;
}

/**
 * Calculate the QB-Value of forwarding packet to node specified.
 */
double  NetworkLayer::_qbValueIfForwardTo(NodeInfo* nPtr)
{
    double qeValue;
    double rewardTotal;
    double rewardProgress;
    double rewardEnergy;
    double numTxDiff;
    double energyDiff;
    double estimatedPER;

    // Determine the reward for progress to basestation
    numTxDiff = this->myInfo->numTxToBaseStation - nPtr->numTxToBaseStation;
    rewardProgress = ( tanh(numTxDiff * PI * this->_qNumTxDiffScalingFactor) + 1.0 ) / 2.0;

    // Determine the reward for energy difference
    energyDiff = nPtr->energyRemaining - this->myInfo->energyRemaining;
    rewardEnergy = ( tanh(energyDiff * PI * this->_qEnergyDiffScalingFactor) + 1.0 ) / 2.0;

    // Determine total reward via weighted sum of partial rewards
    rewardTotal = this->_qWeightEnergy * rewardEnergy + (1.0 - this->_qWeightEnergy) * rewardProgress;

    // Determine Q Value of this action as weighted sum of reward and destination node Q Value
    qeValue = this->_qDiscountedRewardFactor * nPtr->qeValue + (1.0 - this->_qDiscountedRewardFactor) * rewardTotal;

    // Scale by a factor of 1 - PER
    estimatedPER = ((PhysicalLayer*)this->physicalLayerPtr)->estimatePER((double)this->_distanceBetween(this->myInfo, nPtr));
    qeValue = qeValue * (1.0 - estimatedPER);

    return qeValue;
}

/**
 * Receives message passed up from data link layer.
 */
void NetworkLayer::_receiveFromDataLinkLayer(cPacket* msg)
{
    // Received Data Frame
    if (dynamic_cast<DataFrame*>(msg) != NULL)
    {
        DataFrame* dMsg = check_and_cast<DataFrame*>(msg);

        // If this node is the message destination, pass the message up to App layer
        if (this->getParentModule()->getId() == dMsg->getDestinationId())
        {
            this->send(msg, "toApplication");
        }
        // Otherwise, forward the message
        else
        {
            this->_forwardMsg(msg);
        }
    }
    // Received Status Frame
    else if (dynamic_cast<StatusFrame*>(msg) != NULL)
    {
        StatusFrame* sMsg = check_and_cast<StatusFrame*>(msg);
        double roundingError = 0.02;

        // Update status info for the neighbour who sent the frame
        for (auto p : this->neighboursList)
        {
            if (sMsg->getSenderId() == p->id)
            {
                p->energyRemaining = sMsg->getEnergyRemaining();
                p->numHopsToBaseStation = sMsg->getNumHopsToBaseStation();
                p->numTxToBaseStation = sMsg->getNumTxToBaseStation();
                p->qeValue = sMsg->getQEValue();
                p->qbValue = sMsg->getQBValue();

                // Update our own hop distance if it is different now
                if (p->numHopsToBaseStation < (this->myInfo->numHopsToBaseStation - 1) )
                {
                    this->myInfo->numHopsToBaseStation = p->numHopsToBaseStation + 1;

                    // Inform other nodes around us of our new hop distance
                    ((DataLinkLayer*)this->dataLinkLayerPtr)->requestToBroadcastStatusFrame();
                }
                // Update our own estimated best case average number of transmissions to base station if it is different now
                double estimatedSuccessRate = 1.0 - ((PhysicalLayer*)this->physicalLayerPtr)->estimatePER((double)this->_distanceBetween(this->myInfo, p));
                double estimatedNumTx = 1.0 / estimatedSuccessRate;
                if ( (p->numTxToBaseStation + estimatedNumTx + roundingError) < this->myInfo->numTxToBaseStation )
                {
                    this->myInfo->numTxToBaseStation = p->numTxToBaseStation + estimatedNumTx;

                    // Inform other nodes around us of our new estimated best case average number of transmissions to base station
                    ((DataLinkLayer*)this->dataLinkLayerPtr)->requestToBroadcastStatusFrame();
                }

                break;
            }
        }

        // Update our own energyRemaining info
        this->myInfo->energyRemaining = ((PhysicalLayer*)this->physicalLayerPtr)->getEnergyRemaining();

        // @info we update our qValue when we pick a forwarding node using the QSmart protocol

        delete sMsg;
    }
    // Received something else - discard it
    else
    {
        delete msg;
    }
}

void NetworkLayer::_receiveFromAppLayer(cPacket* msg)
{
    this->_forwardMsg(msg);
}
