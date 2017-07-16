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
simple NetworkLayer
{
    parameters:
        double	qLearnRate = default(0.3);
    	double  qWeightEnergy = default(0.65); // 0.65
    	double  qDiscountedRewardFactor = default(0.7); //0.7
    	double  qEnergyDiffScalingFactor = default(2.0); //2.0
    	double  qNumTxDiffScalingFactor = default(0.5); //< May want to test a linear function for this instead of tanh
    gates:
        input fromApplication;
        output toApplication;
        output toDataLink;
        input fromDataLink;
}
