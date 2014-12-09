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

#include "CoRE4INET_RCTrafficSourceApp.h"

//CoRE4INET
#include "CoRE4INET_Timer.h"
//INET
#include "ModuleAccess.h"

namespace CoRE4INET {

Define_Module(RCTrafficSourceApp);

RCTrafficSourceApp::RCTrafficSourceApp()
{
    this->tick = 0;
}

void RCTrafficSourceApp::initialize()
{
    CTTrafficSourceAppBase::initialize();

    if (par("enabled").boolValue())
    {
        Timer *timer = dynamic_cast<Timer*>(findModuleWhereverInNode("timer", getParentModule()));
        if(!timer)
        {
            throw cRuntimeError("Cannot find timer module. Timer is required to run RCTrafficSourceApp");
        }
        SchedulerTimerEvent *event = new SchedulerTimerEvent("API Scheduler Task Event", TIMER_EVENT);
        tick = findModuleWhereverInNode("oscillator", getParentModule())->par("tick").doubleValue();
        event->setTimer((uint64_t) (par("interval").doubleValue() / tick));
        event->setDestinationGate(gate("schedulerIn"));
        timer->registerEvent(event);
    }
}

void RCTrafficSourceApp::handleMessage(cMessage *msg)
{

    CTApplicationBase::handleMessage(msg);
    if (msg->arrivedOn("schedulerIn"))
    {
        sendMessage();

        Timer *timer = dynamic_cast<Timer*>(msg->getSenderModule());
        ASSERT(timer);
        SchedulerTimerEvent *event = (SchedulerTimerEvent *) msg;
        event->setTimer((uint64_t) (par("interval").doubleValue() / tick));
        timer->registerEvent(event);
    }
    else
    {
        delete msg;
    }
}

} //namespace
