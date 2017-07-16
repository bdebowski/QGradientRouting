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

#include "networkMonitor.h"
#include "Definitions.h"

Define_Module(NetworkMonitor);

NodeState* NodeState::dup()
{
    NodeState* nsPtr = new NodeState();

    nsPtr->energyLevel = this->energyLevel;

    return nsPtr;
}

void NetworkMonitor::initialize()
{
    this->stopOnNodeFailure = this->par("stopOnNodeFailure");
    this->loggingInterval = this->par("loggingInterval");

    this->_networkPtr = this->getParentModule();
    this->_nodesWide = this->_networkPtr->par("nodesWide");
    this->_nodesHigh = this->_networkPtr->par("nodesHigh");
    this->_numNodes = this->_nodesWide * this->_nodesHigh;

    // Initialize the grid of node handles
    this->_networkHandle.resize(this->_nodesHigh);
    for (int i = 0; i < this->_nodesHigh; ++i)
    {
        for (int j = 0; j < this->_nodesWide; ++j)
        {
            int index = i * this->_nodesWide + j;
            this->_networkHandle[i].push_back(new NodeHandle());
            this->_networkHandle[i][j]->nodePtr = this->_networkPtr->getSubmodule("sn", index);
            this->_networkHandle[i][j]->appLayerPtr = check_and_cast<ApplicationLayer*>(this->_networkHandle[i][j]->nodePtr->getSubmodule("app"));
            this->_networkHandle[i][j]->networkLayerPtr = check_and_cast<NetworkLayer*>(this->_networkHandle[i][j]->nodePtr->getSubmodule("routing"));
            this->_networkHandle[i][j]->dataLinkLayerPtr = check_and_cast<DataLinkLayer*>(this->_networkHandle[i][j]->nodePtr->getSubmodule("mac"));
            this->_networkHandle[i][j]->physicalLayerPtr = check_and_cast<PhysicalLayer*>(this->_networkHandle[i][j]->nodePtr->getSubmodule("physical"));
            EV << this->_networkHandle[i][j]->nodePtr->getIndex() << ",";
        }
    }

    this->_currentTotalNumPacketsReceived = 0;
    this->_currentTotalPacketDelay = 0.0;

    // Initialize the current state of each node
    this->_currentNetworkState.resize(this->_nodesHigh);
    for (int i = 0; i < this->_nodesHigh; ++i)
    {
        for (int j = 0; j < this->_nodesWide; ++j)
        {
            this->_currentNetworkState[i].push_back(new NodeState());
            this->_currentNetworkState[i][j]->energyLevel = 0;
            this->_currentNetworkState[i][j]->estimatedNumTxToBaseStation = 0.0;
        }
    }

    // Log initial network sate
    //this->_observeNetworkState();
    //this->_logNetworkState();
    //scheduleAt(simTime() + this->loggingInterval, new cMessage("LogInterval", SMSGKIND_LOGINTERVAL));
    scheduleAt(simTime() + 1.5, new cMessage("LogInterval", SMSGKIND_LOGINTERVAL)); //@HACK - these types of init scheduling times need to be defined somewhere
}

void NetworkMonitor::handleMessage(cMessage *msg)
{
    // Time to log
    if (msg->isSelfMessage() && msg->getKind() == SMSGKIND_LOGINTERVAL)
    {
        this->_observeNetworkState();
        this->_logNetworkState();

        scheduleAt(simTime() + this->loggingInterval, new cMessage("LogInterval", SMSGKIND_LOGINTERVAL));
    }
    // Message is a battery empty event
    else if (msg->getKind() == EVENTMSG_BATTERYEMPTY && this->stopOnNodeFailure)
    {
        EV << "Stopping simulation on Battery Empty Event." << endl;
        this->_observeNetworkState();
        this->endSimulation();
    }
    // Something else - ignore it
    else
        delete msg;
}

void NetworkMonitor::finish()
{
    // Capture final network state
    this->_observeNetworkState();

    // Print Average Energy Use at each node at each log interval
    for (int t = 1; t < this->_networkStateHistory.size(); ++t)
    {
        EV << "Average Energy use at Each Node at t = " << t * this->loggingInterval << ":\n";
        for (int i = 0; i < this->_nodesHigh; ++i)
        {
            for (int j = 0; j < this->_nodesWide; ++j)
            {
                EV << "," << _networkStateHistory[t][i][j]->avgEnergyUse;
            }
            EV << "\n";
        }
    }

    // Print estimated number of transmissions to base station value at each node
    EV << "Estimated number of Tx to Base for Each Node:\n";
    for (int i = 0; i < this->_nodesHigh; ++i)
    {
        for (int j = 0; j < this->_nodesWide; ++j)
        {
            EV << "," << _currentNetworkState[i][j]->estimatedNumTxToBaseStation;
        }
        EV << "\n";
    }

    // Print remaining energy level at each node
    EV << "Remaining Energy at Each Node:\n";
    for (int i = 0; i < this->_nodesHigh; ++i)
    {
        for (int j = 0; j < this->_nodesWide; ++j)
        {
            EV << "," << _currentNetworkState[i][j]->energyLevel;
        }
        EV << "\n";
    }

    // Print min energy history
    EV << "Minimum Node Energy vs Time:\n";
    EV << "SimTime (s), EnergyLevel\n";
    for (int t = 0; t < this->_minNodeEnergyHistory.size(); ++t)
    {
        EV << "," << t * this->loggingInterval << "," << this->_minNodeEnergyHistory[t] << endl;
    }

    // Print Average network energy consumption history
    EV << "Average network Energy Consumption vs Time:\n";
    EV << "SimTime (s), EnergyUse\n";
    for (int t = 0; t < this->_avgNetworkEnergyConsumptionHistory.size(); ++t)
    {
        EV << "," << t * this->loggingInterval << "," << this->_avgNetworkEnergyConsumptionHistory[t] << endl;
    }

    // Print Average Packet Delay history
    EV << "Average Packet Delay vs Time:\n";
    EV << "SimTime (s), AvgDelay (s)\n";
    for (int t = 0; t < this->_avgPacketDelayHistory.size(); ++t)
    {
        EV << "," << t * this->loggingInterval << "," << this->_avgPacketDelayHistory[t] << endl;
    }

    // Print number packets received
    EV << "Total number of packets received: " << this->_currentTotalNumPacketsReceived << endl;
}

/**
 * Update the current state information for each node in the network.
 */
void NetworkMonitor::_observeNetworkState()
{
    int packetCount = 0;
    simtime_t packetDelay = 0.0;

    for (int i = 0; i < this->_nodesHigh; ++i)
    {
        for (int j = 0; j < this->_nodesWide; ++j)
        {
            this->_currentNetworkState[i][j]->energyLevel = this->_networkHandle[i][j]->physicalLayerPtr->getEnergyRemaining();
            this->_currentNetworkState[i][j]->estimatedNumTxToBaseStation = this->_networkHandle[i][j]->networkLayerPtr->getNumTxToBaseStation();
            packetCount += this->_networkHandle[i][j]->appLayerPtr->getNumPacketsReceived();
            packetDelay += this->_networkHandle[i][j]->appLayerPtr->getTotalPacketDelay();
        }
    }

    this->_currentTotalNumPacketsReceived = packetCount;
    this->_currentTotalPacketDelay = packetDelay;
}

/**
 * Record the current network state into the log history.
 */
void NetworkMonitor::_logNetworkState()
{
    // Copy current state into back of state history
    this->_networkStateHistory.push_back(this->_currentNetworkState);
    for (int i = 0; i < this->_nodesHigh; ++i)
    {
        for (int j = 0; j < this->_nodesWide; ++j)
        {
            this->_networkStateHistory.back()[i][j] = this->_networkStateHistory.back()[i][j]->dup();
        }
    }

    // Determine and log the min node energy
    double minNodeEnergy = 1.0;
    for (int i = 0; i < this->_nodesHigh; ++i)
    {
        for (int j = 0; j < this->_nodesWide; ++j)
        {
            if (this->_currentNetworkState[i][j]->energyLevel < minNodeEnergy)
            {
                minNodeEnergy = this->_currentNetworkState[i][j]->energyLevel;
            }
        }
    }
    this->_minNodeEnergyHistory.push_back(minNodeEnergy);

    // Total packets received log and total packet delay log
    this->_totNumPacketsReceivedHistory.push_back(this->_currentTotalNumPacketsReceived);
    this->_totPacketDelayHistory.push_back(this->_currentTotalPacketDelay);


    // Calculate differentials.  Set them to zero for the first log entry.
    if (this->_networkStateHistory.size() < 2)
    {
        for (int i = 0; i < this->_nodesHigh; ++i)
        {
            for (int j = 0; j < this->_nodesWide; ++j)
            {
                this->_networkStateHistory.back()[i][j]->avgEnergyUse = 0;
            }
        }

        this->_avgPacketDelayHistory.push_back(0.0);
        this->_avgNetworkEnergyConsumptionHistory.push_back(0.0);
    }
    else
    {
        int t_1 = this->_networkStateHistory.size() - 2;
        int t_2 = this->_networkStateHistory.size() - 1;
        double totalAvgEnergyUse = 0.0;

        for (int i = 0; i < this->_nodesHigh; ++i)
        {
            for (int j = 0; j < this->_nodesWide; ++j)
            {
                this->_networkStateHistory[t_2][i][j]->avgEnergyUse = (this->_networkStateHistory[t_1][i][j]->energyLevel - this->_networkStateHistory[t_2][i][j]->energyLevel) / this->loggingInterval;
                totalAvgEnergyUse += this->_networkStateHistory[t_2][i][j]->avgEnergyUse;
            }
        }

        double packetDelayDiff = (this->_totPacketDelayHistory[t_2] - this->_totPacketDelayHistory[t_1]).dbl();
        double numPacketsDiff = (double)(this->_totNumPacketsReceivedHistory[t_2] - this->_totNumPacketsReceivedHistory[t_1]);
        this->_avgPacketDelayHistory.push_back(packetDelayDiff / numPacketsDiff);
        this->_avgNetworkEnergyConsumptionHistory.push_back(totalAvgEnergyUse);
    }
}
