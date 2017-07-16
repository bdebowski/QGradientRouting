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

#ifndef __INTELLIGENTROUTING_PHYSICALLAYER_H_
#define __INTELLIGENTROUTING_PHYSICALLAYER_H_

//#include "dataLinkLayer.h"
#include <omnetpp.h>

#include <list>
#include <queue>
#include <utility>

/**
 * Represents the physical layer of a WSN node.
 */
class PhysicalLayer : public cSimpleModule
{
public:
            bool    isTransmitting();
            int     distanceTo(PhysicalLayer* nodePtr);
            double  estimatePER(double txDist);
            int     getTxRange();
            int     getTxRate();
            int     getXPos();
            int     getYPos();
            double  getEnergyRemaining();
protected:
    virtual void    initialize();
    virtual void    handleMessage(cMessage *msg);
private:
            void    _place();
            void    _placeCircularly();
            void    _determineNodesInRadioRadio();

            void    _transmitNext();

            void    _receiveMessage(cPacket* mPtr);
            double  _determinePER(cPacket* mPtr);

protected:
    // Node Knowledge
    double      energyRemaining;    //< proportion of battery capacity remaining
    double      minEnergyRequired;  //< minimum proportion required for node to operate
    double      battCapacityMax;    //< in mWh
    double      battCapacityNow;    //< in mWh
    double      energyCost_Tx;      //< in mW
    double      energyCost_Rx;      //< in mW
    int         tx_rate;            //< in bps
    int         tx_range;           //< in m
    bool        isTxActive;
    cModule*    dataLinkLayerPtr;
    cGate*      radioInGatePtr;
    std::queue<cPacket*>    transmissionQueue;

    // Simulator Knowledge
    int     x_pos;
    int     y_pos;
    int     row_index;
    int     col_index;
    int     numSensorNodes;
    double  rxNoise;
    double  attenuationFactor;
    char    dispString[64];
    char    radioRangeString[8];
    std::list< std::pair<PhysicalLayer*, int> > nodesInRadioRange;
};

#endif
