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

package core4inet.applications.trafficsource.AS6802;

import core4inet.applications.trafficsource.base.TrafficSourceAppBase;

//
// Abstract base traffic source application used for various traffic generators.
// Uses CTFrame with encapsulated cPacket as payload
//
// @warning Since this is an abstract module please do not instantiate it!
//
// @see ~TrafficSourceAppBase, ~TTTrafficSourceApp, ~RCTrafficSourceApp
//
// @author Till Steinbach
//
simple CTTrafficSourceAppBase extends TrafficSourceAppBase
{
    parameters:
        @class(AbstractSimpleModule);
        @display("i=block/source");

        //Critical traffic identifier of the message that is being generated (-1) means don't set ID
        int ct_id = default(-1);
        //Oscillator of Scheduler this app is assigned to
        string oscillator = default("^.scheduler.oscillator");
        //Timer of Scheduler this app is assigned to
        string timer = default("^.scheduler.timer");
}
