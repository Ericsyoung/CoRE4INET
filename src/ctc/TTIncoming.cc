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

#include "TTIncoming.h"

#include <TTEScheduler.h>
#include <SchedulerMessageEvents_m.h>

namespace TTEthernetModel {

Define_Module( TTIncoming);

TTIncoming::TTIncoming()
{
    frame = NULL;
}

TTIncoming::~TTIncoming()
{
    if (frame != NULL)
    {
        delete frame;
    }
}

void TTIncoming::handleMessage(cMessage *msg)
{
    //Incoming Message
    if (msg->arrivedOn("in"))
    {
        TTEScheduler *tteScheduler = (TTEScheduler*) getParentModule()->getSubmodule("tteScheduler");
        //get current time in cylce
        unsigned int currentTicks = tteScheduler->getTicks();
        //Now check for correct arrival:
        if (frame != NULL)
        {
            ev.printf("Received frame before permanence point of previous frame \n");
            emit(ctDroppedSignal, 1);
            getDisplayString().setTagArg("i2", 0, "status/excl3");
            delete msg;
        }
        //Check too early
        else if (currentTicks < (unsigned int) par("receive_window_start").longValue())
        {
            ev.printf(
                    "Received frame in %s too early! Receive Time was %d Ticks, should have been between %d and %d! \n",
                    getName(), currentTicks, par("receive_window_start").longValue(),
                    par("receive_window_end").longValue());
            bubble("Frame to early");
            emit(ctDroppedSignal, 1);
            getDisplayString().setTagArg("i2", 0, "status/excl3");
            delete msg;
        }
        //Check too late
        else if (currentTicks > (unsigned int) par("receive_window_end").longValue())
        {
            ev.printf(
                    "Received frame in %s too late! Receive Time was %d Ticks, should have been between %d and %d! \n",
                    getName(), currentTicks, par("receive_window_start").longValue(),
                    par("receive_window_end").longValue());
            bubble("Frame to late");
            emit(ctDroppedSignal, 1);
            getDisplayString().setTagArg("i2", 0, "status/excl3");
            delete msg;
        }
        //Timing ok
        else
        {
            //delay for permanence_pit
            if(!hadError)
                getDisplayString().setTagArg("i2", 0, "status/hourglass");
            frame = (EtherFrame *) msg;
            SchedulerActionTimeEvent *event = new SchedulerActionTimeEvent("PIT Event", ACTION_TIME_EVENT);
            event->setAction_time(par("permanence_pit").doubleValue());
            event->setDestinationGate(gate("schedulerIn"));
            tteScheduler->registerEvent(event);
        }
    }
    else if (msg->arrivedOn("schedulerIn") && msg->getKind() == ACTION_TIME_EVENT)
    {
        delete msg;
        if(!hadError)
            getDisplayString().setTagArg("i2", 0, "");
        send(frame, "out");
        frame = NULL;
    }

}

} //namespace
