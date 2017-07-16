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

#ifndef __INTELLIGENTROUTING_NETWORKMONITOR_H_
#define __INTELLIGENTROUTING_NETWORKMONITOR_H_

#include <omnetpp.h>
#include <vector>
#include "applicationLayer.h"
#include "networkLayer.h"
#include "dataLinkLayer.h"
#include "physicalLayer.h"

/**
 * Captures state information about the node at a given point in time.
 */
struct NodeState
{
    NodeState* dup();

    double  energyLevel;
    double  avgEnergyUse;
    double  estimatedNumTxToBaseStation;
    //int     numPacketsReceived;
    //double  totalPacketDelay;
};

/**
 * Grants access to each layer of the node via a pointer.
 */
struct NodeHandle
{
    cModule*            nodePtr;
    ApplicationLayer*   appLayerPtr;
    NetworkLayer*       networkLayerPtr;
    DataLinkLayer*      dataLinkLayerPtr;
    PhysicalLayer*      physicalLayerPtr;
};

/**
 * This class monitors the network and its nodes, collecting information for experimental results.
 */
class NetworkMonitor : public cSimpleModule
{
protected:
    virtual void    initialize();
    virtual void    handleMessage(cMessage *msg);
            void    finish();
private:
            void    _observeNetworkState();
            void    _logNetworkState();

protected:
    bool        stopOnNodeFailure;  //< if true stops the simulation on receiving a node battery empty event.
    double      loggingInterval;
private:
    cModule*    _networkPtr;
    int         _numNodes;
    int         _nodesWide;
    int         _nodesHigh;
    std::vector< std::vector<NodeHandle*> > _networkHandle;         //< 2-D array of node handles.

    // Single time-point values
    int         _currentTotalNumPacketsReceived;
    simtime_t   _currentTotalPacketDelay;
    std::vector< std::vector<NodeState*> >  _currentNetworkState;   //< 2-D array of last observed node states.

    // Histograms
    std::vector< std::vector< std::vector<NodeState*> > >   _networkStateHistory;   //< 2-D array of last observed node states.
    std::vector<double>                                     _minNodeEnergyHistory;  //< each entry is the energy level of the lowest energy node at that time.
    std::vector<double>                                     _avgNetworkEnergyConsumptionHistory;    //< Total energy being consumed network wide averaged over loggingInterval.
    std::vector<double>                                     _avgPacketDelayHistory;
    std::vector<int>                                        _totNumPacketsReceivedHistory;
    std::vector<simtime_t>                                  _totPacketDelayHistory;
};

#endif
