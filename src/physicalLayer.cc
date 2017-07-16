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

#include "physicalLayer.h"
#include "Definitions.h"
#include "dataLinkLayer.h"
#include "dataFrame_m.h"

Define_Module(PhysicalLayer);

bool PhysicalLayer::isTransmitting()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->isTxActive;
}

/**
 * Return the Euclidean integer distance between this node and the one specified.
 */
int PhysicalLayer::distanceTo(PhysicalLayer* nodePtr)
{
    Enter_Method_Silent();  // Omnet requires this

    return sqrt((x_pos - nodePtr->x_pos) * (x_pos - nodePtr->x_pos) + (y_pos - nodePtr->y_pos) * (y_pos - nodePtr->y_pos));
}

/**
 * Estimates and returns the PER for this node transmitting to a neighbour that is txDist away
 */
double  PhysicalLayer::estimatePER(double txDist)
{
    Enter_Method_Silent();  // Omnet requires this

    double halfPRange = 0.75 * (double)this->tx_range;
    if (txDist <= halfPRange)
    {
        return ( 0.5 * pow(txDist / halfPRange, this->attenuationFactor) );
    }
    else
        return ( 1.0 - 0.5 * pow(2.0 - txDist / halfPRange, this->attenuationFactor) );
}

int PhysicalLayer::getTxRange()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->tx_range;
}

int PhysicalLayer::getTxRate()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->tx_rate;
}

int PhysicalLayer::getXPos()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->x_pos;
}

int PhysicalLayer::getYPos()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->y_pos;
}

/**
 * Returns proportion of battery capacity remaining.
 */
double PhysicalLayer::getEnergyRemaining()
{
    Enter_Method_Silent();  // Omnet requires this

    return this->energyRemaining;
}

void PhysicalLayer::initialize()
{
    this->isTxActive = false;
    this->tx_range = this->par("transmissionRange");
    this->tx_rate = this->par("transmissionRate");

    this->battCapacityMax = this->par("batteryCapacityMax");
    this->battCapacityNow = this->par("batteryCapacityNow");
    this->energyRemaining = 1.0;
    this->minEnergyRequired = this->par("minBatteryLevel");
    this->energyCost_Tx = this->par("energyCostTransmitting");
    this->energyCost_Rx = this->par("energyCostReceiving");

    this->dataLinkLayerPtr = this->getParentModule()->getSubmodule("mac");
    this->radioInGatePtr = this->gate("radioIn");
    this->rxNoise =  this->par("receivingNoise"); //((double) this->par("receivingNoise")) / ((double) this->dataLinkLayerPtr->par("dataFrameLength"));
    this->attenuationFactor =  this->par("attenuationFactor");

    // Pseudo-Random placement using a grid layout
    //this->_place();
    this->_placeCircularly();

    // Create and set display string for this SensorNode module
    sprintf(this->radioRangeString, "%d", this->tx_range);
    sprintf(this->dispString, "p=%d,%d;i=device/accesspoint;is=vs", this->x_pos, this->y_pos);
    // Hacky Code to set up base station specific params @HACK
    if (this->getParentModule()->getIndex() == (this->numSensorNodes / 2))
    {
        this->battCapacityMax = this->battCapacityMax * 1000;
        this->battCapacityNow = this->battCapacityMax;
        sprintf(this->dispString, "p=%d,%d;i=device/antennatower", this->x_pos, this->y_pos);
    }
    this->getParentModule()->setDisplayString(this->dispString);


    // Schedule second initialization phase (once node locations have been set)
    scheduleAt(simTime() + 1.0, new cMessage("PostInitialize", SMSGKIND_INIT));
}

void PhysicalLayer::handleMessage(cMessage *msg)
{
    // Execute post-initialization phase
    if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_INIT)
    {
        this->_determineNodesInRadioRadio();

        delete msg;
    }
    // An active transmission has completed
    else if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_TXCOMPLETE)
    {
        delete this->transmissionQueue.front();
        this->transmissionQueue.pop();

        // Complete transmission sequence
        if (this->transmissionQueue.empty())
            scheduleAt(simTime() + ((double) this->tx_range) / CONST_SPEEDOFLIGHT, new cMessage("MediumNowFree", SMSGKIND_MEDIUMFREE));  // Flag medium as free after last signal has propagated
        // Continue transmission sequence
        else
            this->_transmitNext();

       delete msg;
    }
    // All transmissions have completed and signals have propagated
    else if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_MEDIUMFREE)
    {
        this->isTxActive = false;
        this->getParentModule()->getDisplayString().setTagArg("r", 0, "0");

        delete msg;
    }

    // Received Message from Data Link Layer
    else if (msg->getSenderModule() == this->dataLinkLayerPtr)
    {
        this->transmissionQueue.push(check_and_cast<cPacket*>(msg));

        // Begin transmission sequence
        if (this->isTxActive == false)
        {
            this->isTxActive = true;
            this->getParentModule()->getDisplayString().setTagArg("r", 0, this->radioRangeString);
            this->_transmitNext();
        }
    }

    // Received Message from radioIn
    else if (msg->getArrivalGate() == this->radioInGatePtr)
    {
        this->_receiveMessage(check_and_cast<cPacket*>(msg));
    }

    // Something else happened - Ignore it
    else
        delete msg;
}


/**
 * Pseudo-Random placement using a grid layout.
 */
void PhysicalLayer::_place()
{
    cModule* nodePtr = this->getParentModule();
    cModule* networkPtr = nodePtr->getParentModule();

    int mapWidth = networkPtr->par("width");
    int mapHeight = networkPtr->par("height");
    int nodesWide = networkPtr->par("nodesWide");
    int nodesHigh = networkPtr->par("nodesHigh");

    int regionWidth = mapWidth / nodesWide;
    int regionHeight = mapHeight / nodesHigh;
    //int x_offset = intuniform(regionWidth / 6, 5 * regionWidth / 6);
    //int y_offset = intuniform(regionHeight / 6, 5 * regionHeight / 6);

    // TEST
    double theta = uniform(0, 2*PI);
    double r = triang(0,1.0,1.0);
    r = pow(r, exp(r));
    double lFactor;
    if ( theta <= (2.0 * PI / 8.0) || theta >= (2.0 * PI * 7.0 / 8.0) || (theta >= PI * 3.0 / 4.0 && theta <= 2.0 * PI * 5.0 / 8.0) )
        lFactor = 1.0 / cos(theta);
    else
        lFactor = 1.0 / sin(theta);
    if (lFactor < 0.0)
        lFactor *= -1.0;
    int centre_x_offset = regionWidth / 2;
    int centre_y_offset = regionHeight / 2;
    int x_offset = centre_x_offset + cos(theta) * r * 0.5 * regionWidth * lFactor;
    int y_offset = centre_y_offset + sin(theta) * r * 0.5 * regionHeight * lFactor;
    // TEST

    this->col_index = nodePtr->getIndex() % nodesWide;
    this->row_index = nodePtr->getIndex() / nodesHigh;
    this->numSensorNodes = nodesWide * nodesHigh;

    this->x_pos = this->col_index * regionWidth + x_offset;
    this->y_pos = this->row_index * regionHeight + y_offset;
    //this->x_pos = x_offset;
    //this->y_pos = y_offset;
}

/**
 * Pseudo-Random placement using a grid layout and circularish placement.
 */
void PhysicalLayer::_placeCircularly()
{
    cModule* nodePtr = this->getParentModule();
    cModule* networkPtr = nodePtr->getParentModule();

    int mapWidth = networkPtr->par("width");
    int mapHeight = networkPtr->par("height");
    int nodesWide = networkPtr->par("nodesWide");
    int nodesHigh = networkPtr->par("nodesHigh");

    int regionWidth = mapWidth / nodesWide;
    int regionHeight = mapHeight / nodesHigh;

    double theta = uniform(0, 2*PI);
    double r = triang(0,1.0,1.0);
    r = pow(r, exp(r));
    int centre_x_offset = regionWidth / 2;
    int centre_y_offset = regionHeight / 2;
    int x_offset = centre_x_offset + cos(theta) * r * 0.5 * regionWidth;
    int y_offset = centre_y_offset + sin(theta) * r * 0.5 * regionHeight;

    this->col_index = nodePtr->getIndex() % nodesWide;
    this->row_index = nodePtr->getIndex() / nodesHigh;
    this->numSensorNodes = nodesWide * nodesHigh;

    this->x_pos = this->col_index * regionWidth + x_offset;
    this->y_pos = this->row_index * regionHeight + y_offset;
    //this->x_pos = x_offset;
    //this->y_pos = y_offset;
}

/**
 * Determine which nodes are within radio transmission range of this node and populate the
 * nodesInRadioRange list accordingly.
 */
void PhysicalLayer:: _determineNodesInRadioRadio()
{
    cModule* nodePtr = this->getParentModule();
    cModule* networkPtr = nodePtr->getParentModule();

    int nodesWide = networkPtr->par("nodesWide");
    int nodesHigh = networkPtr->par("nodesHigh");

    PhysicalLayer* nPtr;
    int distance;
    int numNodes = nodesWide * nodesHigh;

    for (int i = 0; i < numNodes; ++i)
    {
        nPtr = check_and_cast<PhysicalLayer*>(networkPtr->getSubmodule("sn", i)->getSubmodule("physical"));
        distance = distanceTo(nPtr);

        if (distance <= tx_range && nPtr != this)
            nodesInRadioRange.push_back(std::make_pair(nPtr, distance));
    }

    nodesInRadioRange.sort([](const std::pair<PhysicalLayer*, int>& p1, const std::pair<PhysicalLayer*, int>& p2) {return p1.second < p2.second;});
}

/**
 * Transmit "wirelessly" the next queued msg to all neighbours in radio range.
 */
void PhysicalLayer::_transmitNext()
{
    cPacket* mPtr = this->transmissionQueue.front();
    simtime_t tx_duration = ((double) mPtr->getBitLength()) / ((double) this->tx_rate);

    // Schedule event to signal transmission has completed
    scheduleAt(simTime() + tx_duration, new cMessage("TransmissionCompleted", SMSGKIND_TXCOMPLETE));

    // Send a copy of the message to each node in radio range
    for (auto p : this->nodesInRadioRange)
    {
        this->sendDirect(mPtr->dup(), ((double) p.second) / CONST_SPEEDOFLIGHT, tx_duration, p.first->getParentModule(), "radioIn");
    }

    // Consume battery energy
    this->battCapacityNow -= ((double) mPtr->getBitLength()) / ((double) this->tx_rate) / CONST_SEC_PER_HOUR * this->energyCost_Tx;
    this->energyRemaining = this->battCapacityNow / this->battCapacityMax;

    // Check battery level and send notification if empty
    if (this->energyRemaining < this->minEnergyRequired)
    {
        this->sendDirect(new cMessage("BatteryDrained", EVENTMSG_BATTERYEMPTY), this->getParentModule()->getParentModule()->getSubmodule("monitor"), "eventIn");
    }
}

/**
 * Receive the message provided by:
 *  - consuming energy
 *  - applying error model
 *  - passing the message up to the DataLink layer
 */
void PhysicalLayer::_receiveMessage(cPacket* mPtr)
{
    // Consume battery energy
    this->battCapacityNow -= ((double) mPtr->getBitLength()) / ((double) this->tx_rate) / CONST_SEC_PER_HOUR * this->energyCost_Rx;
    this->energyRemaining = this->battCapacityNow / this->battCapacityMax;

    // Check battery level and send notification if empty
    if (this->energyRemaining < this->minEnergyRequired)
    {
        this->sendDirect(new cMessage("BatteryDrained", EVENTMSG_BATTERYEMPTY), this->getParentModule()->getParentModule()->getSubmodule("monitor"), "eventIn");
    }

    // Error model @TODO
    //double dist = (double)this->distanceTo((PhysicalLayer*)mPtr->getSenderModule());
    //double distScaledProbFactor = pow(dist / ((double)this->tx_range), 3);
    //double p = this->rxNoise * ((double) mPtr->getBitLength()) * distScaledProbFactor;
    double p = this->_determinePER(mPtr);
    EV << "per: " << p << endl;
    if ( uniform(0.0,1.0) < p )
    {
        delete mPtr;
        return;
    }

    // Pass the message up to the data link layer
    this->send(mPtr, "toDataLink");
}

/**
 * Calculate and return the Packet Error Rate for this RECEIVED packet.
 * If the packet is not a DataFrame then we just return 0.
 */
double  PhysicalLayer::_determinePER(cPacket* mPtr)
{
    if (dynamic_cast<DataFrame*>(mPtr) == NULL)
        return 0.0;

    double dist = (double)this->distanceTo((PhysicalLayer*)mPtr->getSenderModule());
    double halfPRange = 0.75 * (double)((PhysicalLayer*)mPtr->getSenderModule())->getTxRange();
    if (dist <= halfPRange)
    {
        return ( 0.5 * pow(dist / halfPRange, this->attenuationFactor) );
    }
    else
        return ( 1.0 - 0.5 * pow(2.0 - dist / halfPRange, this->attenuationFactor) );
}
