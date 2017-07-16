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
simple ApplicationLayer
{
    parameters:
        double	startTime = default(10.0);			//< Time when data frames will start being generated
        double	dataGenerationRate = default(0);	//< Number of data frames to generate at this node per second
        
        // Data Frame info
        int		protocol = default(0);					//< Protocol to use when routing the frame: 0 = GreedyGeo, 1 = GreedyGradient, 2 = EnergyAware, 3 = EnergySmart, 4 = QSmart
        int		txAttemptsAllowedPerHop = default(5);	//< Max number of transmission attempts allowed for a data frame at each hop  
        
    gates:
        output toNetwork;
        input fromNetwork;
}
