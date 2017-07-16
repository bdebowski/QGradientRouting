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

//
// TODO generated message class
//
packet StatusFrame {
    int 	senderId;				// Id of node broadcasting this message
    double 	energyRemaining;		// Sending node's residual energy level
    double	qEValue;				// Sending node's most up to date QE-Value
    double	qBValue;				// Sending node's most up to date QB-Value
    double	numTxToBaseStation;		// Sending node's best case estimated average number of transmission to reach base station
    int		numHopsToBaseStation;	// Sending node's best known hop distance to base station
}
