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

package intelligentrouting;

//
// TODO auto-generated module
//
simple PhysicalLayer
{
    parameters:
        int transmissionRange @unit(m) = default(100m);
        int transmissionRate @unit(bps) = default(250000bps);
        double batteryCapacityMax = default(1);			//< in mWh
        double batteryCapacityNow = default(1);			//< in mWh
        double minBatteryLevel = default(0.001);			//< minimum proportion of capacity required for node to function
        double energyCostTransmitting = default(15.0);	//< in mW
        double energyCostReceiving = default(10.0);		//< in mW
        double receivingNoise = default(0.0);			//< 
        double attenuationFactor = default(7);
        
    gates:
        input fromDataLink;
        output toDataLink;
        input radioIn;
}
