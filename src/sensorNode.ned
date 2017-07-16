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

module SensorNode
{
    //types:
    //parameters:
    
    gates:
       input radioIn @directIn;
    submodules:
       app: ApplicationLayer;	//< Generates and consumes network traffic
       routing: NetworkLayer;	//< Routing happens here
       mac: DataLinkLayer;		//< Collision Avoidance, Message Queueing, ACK Handling
       physical: PhysicalLayer;	//< Energy model and Packet Error model
    connections:
       app.toNetwork --> routing.fromApplication;
       app.fromNetwork <-- routing.toApplication;
       routing.toDataLink --> mac.fromNetwork;
       routing.fromDataLink <-- mac.toNetwork;
       mac.toPhysical --> physical.fromDataLink;
       mac.fromPhysical <-- physical.toDataLink;
       physical.radioIn <-- radioIn;
}
