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

#ifndef __INTELLIGENTROUTING_NETWORKLAYER_H_
#define __INTELLIGENTROUTING_NETWORKLAYER_H_

#include <omnetpp.h>
#include <list>
#include <map>

struct NodeInfo
{
    int     id;
    int     x_pos;
    int     y_pos;
    int     numHopsToBaseStation;
    double  numTxToBaseStation;
    double  energyRemaining;
    double  qeValue;
    double  qbValue;
};

/**
 * Performs network layer related tasks of protocol stack.
 */
class NetworkLayer : public cSimpleModule
{
public:
            int     getNumHopsToBaseStation();
            double  getNumTxToBaseStation();
            double  getQEValue();
            double  getQBValue();
protected:
    virtual void    initialize();
    virtual void    handleMessage(cMessage *msg);

private:
            void    _initializeNodeInfo();
            int     _distanceBetween(NodeInfo* niPtr1, NodeInfo* niPtr2);

            void    _forwardMsg(cPacket* msg);
            int     _determineForwardingNodeId_GreedyGeo(int destinationId);
            int     _determineForwardingNodeId_GreedyGradient(int destinationId);
            int     _determineForwardingNodeId_EnergyAwareGradient(int destinationId);
            int     _determineForwardingNodeId_EnergySmartGradient(int destinationId);
            int     _determineForwardingNodeId_EnergyAwareHops(int destinationId);
            int     _determineForwardingNodeId_EnergySmartHops(int destinationId);
            int     _determineForwardingNodeId_QESmart(int destinationId);
            int     _determineForwardingNodeId_QBSmart(int destinationId);
            double  _qeValueIfForwardTo(NodeInfo* nPtr);
            double  _qbValueIfForwardTo(NodeInfo* nPtr);

            void    _receiveFromDataLinkLayer(cPacket* msg);
            void    _receiveFromAppLayer(cPacket* msg);


protected:
    // Node Knowledge
    cModule*    appLayerPtr;
    cModule*    dataLinkLayerPtr;
    cModule*    physicalLayerPtr;
    NodeInfo*   myInfo;
    std::list<NodeInfo*>        neighboursList;
    std::map<int, NodeInfo*>    knownDestinationsMap;

    // QLearning Params
    double      _qLearnRate;
    double      _qWeightEnergy;
    double      _qDiscountedRewardFactor;
    double      _qEnergyDiffScalingFactor;
    double      _qNumTxDiffScalingFactor;
};

#endif
