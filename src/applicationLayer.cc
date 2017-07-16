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

#include "applicationLayer.h"
#include "Definitions.h"
#include "dataFrame_m.h"
#include "NetworkLayer.h"
#include "DataLinkLayer.h"
#include "PhysicalLayer.h"

Define_Module(ApplicationLayer);

/**
 * Returns the total number of packets received at the application layer of this node.
 */
int ApplicationLayer::getNumPacketsReceived()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->_numPacketsReceived;
}

/**
 * Returns the total accumulated delay of all packets received at the application layer of this node.
 */
simtime_t ApplicationLayer::getTotalPacketDelay()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->_totalPacketDelay;
}

void ApplicationLayer::initialize()
{
    int nHigh = this->getParentModule()->getParentModule()->par("nodesHigh");   //@HACK
    int nWide = this->getParentModule()->getParentModule()->par("nodesWide");   //@HACK
    int destIndex = (nWide * nHigh / 2);

    this->startTime = this->par("startTime");
    this->dataGenerationRate = this->par("dataGenerationRate");
    this->protocol = this->par("protocol");
    this->txAttemptsAllowedPerHop = this->par("txAttemptsAllowedPerHop");

    this->_baseStationId = this->getParentModule()->getParentModule()->getSubmodule("sn", destIndex )->getId();
    this->_networkLayerPtr = this->getParentModule()->getSubmodule("routing");
    this->_dataLinkLayerPtr = this->getParentModule()->getSubmodule("mac");
    this->_physicalLayerPtr = this->getParentModule()->getSubmodule("physical");
    this->_numPacketsReceived = 0;
    this->_totalPacketDelay = 0;

    // Schedule first data frame
    if (this->dataGenerationRate > 0.0)
        scheduleAt(simTime() + this->startTime + uniform(0.0, 1.0 / this->dataGenerationRate), new cMessage("SendData", SMSGKIND_SENDDATA));
}

void ApplicationLayer::handleMessage(cMessage *msg)
{
    // Generate a Data Frame
    if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_SENDDATA)
    {
        DataFrame* dMsg = new DataFrame();
        dMsg->setBitLength( ((DataLinkLayer*)this->_dataLinkLayerPtr)->getBitLengthDataFrame() );
        dMsg->setSourceId(this->getParentModule()->getId());
        dMsg->setSenderId(this->getParentModule()->getId());
        dMsg->setDestinationId(this->_baseStationId);
        dMsg->setTxAttemptsAllowedPerHop(this->txAttemptsAllowedPerHop);
        dMsg->setProtocol(this->protocol);

        // Send it to the network layer to be forwarded
        this->send(dMsg, "toNetwork");

        // Schedule generating the next data frame
        double timeToWait = 1.0 / this->dataGenerationRate;
        scheduleAt(simTime() + timeToWait, new cMessage("SendData", SMSGKIND_SENDDATA));

        delete msg;
    }
    // Message is from network layer so we assume its a data frame for us
    else if (msg->getSenderModule() == this->_networkLayerPtr)
    {
        this->_numPacketsReceived += 1;
        this->_totalPacketDelay += simTime() - msg->getCreationTime();

        delete msg;
    }
    // Something else - ignore it
    else
        delete msg;
}
