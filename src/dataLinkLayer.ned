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
simple DataLinkLayer
{
    parameters:
        double	ackTimeoutFactor = default(10);		//< The ACK timeout value in seconds = ackTimeoutFactor x timeToTransmitDataFrame 
        int  	maxNumQueuedMsgs = default(16);		//< Message buffer limit
        int  	ackFrameLength @unit(bits) = default(50b);
        int 	dataFrameLength @unit(bits) = default(1000b);
        int 	statusFrameLength @unit(bits) = default(50b);
        bool 	useAdaptivePolling = default(false);
    gates:
        input fromNetwork;
        output toNetwork;
        output toPhysical;
        input fromPhysical;
}
