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

#include "TTBuffer.h"
#include <TTEScheduler.h>
#include <TTBufferEmpty_m.h>
#include "ApplicationBase.h"

#include <ModuleAccess.h>

using namespace CoRE4INET;

Define_Module(TTBuffer);

TTBuffer::TTBuffer()
{
    actionTimeEvent = NULL;
}

TTBuffer::~TTBuffer()
{
}

int TTBuffer::numInitStages() const
{
    return 2;
}

void TTBuffer::initialize(int stage)
{
    CTBuffer::initialize(stage);
    if (stage == 0)
    {
        nextAction = (uint64_t) par("sendWindowStart").longValue();
    }
    if (stage == 1)
    {
        ev << "Initialize TTBuffer" << endl;

        //Register Event
        Scheduled::initialize();

        actionTimeEvent = new SchedulerActionTimeEvent("TTBuffer Scheduler Event", ACTION_TIME_EVENT);
        //Dirty FIX
        //TODO find out what is wrong here!
        //if(actionTimeEvent->isScheduled())
        //    cancelEvent(actionTimeEvent);
        actionTimeEvent->setAction_time((uint32_t) par("sendWindowStart").longValue());
        actionTimeEvent->setDestinationGate(gate("schedulerIn"));

        if ((uint32_t) par("sendWindowStart").longValue() >= (uint32_t) period->par("cycle_ticks").longValue())
        {
            opp_error("The send window (%d ticks) starts outside of the period (%d ticks)",
                    par("sendWindowStart").longValue(), period->par("cycle_ticks").longValue());
        }

        nextAction = period->registerEvent(actionTimeEvent);

        if (ev.isGUI())
        {
            //Update displaystring
            getDisplayString().setTagArg("i", 0, "buffer/empty");
        }
        return;
    }
}

void TTBuffer::handleMessage(cMessage *msg)
{
    bool arrivedOnSchedulerIn = msg->arrivedOn("schedulerIn");

    CTBuffer::handleMessage(msg);

    if (arrivedOnSchedulerIn && msg->getKind() == ACTION_TIME_EVENT && destinationGates.size() > 0)
    {
        EtherFrame *outgoingMessage = getFrame();
        //Send Message
        for (std::list<cGate*>::iterator destGate = destinationGates.begin(); destGate != destinationGates.end();
                ++destGate)
        {
            if (outgoingMessage)
            {
                sendDirect(outgoingMessage->dup(), *destGate);
            }
            else
            {
                sendDirect(new TTBufferEmpty("TT Buffer Empty"), *destGate);
            }
        }
        if (gate("out")->isConnected())
        {
            if (outgoingMessage)
            {
                send(outgoingMessage->dup(), "out");
            }
            else
            {
                send(new TTBufferEmpty("TT Buffer Empty"), "out");
            }
        }
        if (outgoingMessage)
        {
            recordPacketSent(outgoingMessage);
            delete outgoingMessage;
        }
        //Reregister scheduler
        static_cast<SchedulerActionTimeEvent *>(msg)->setNext_cycle(true);
        nextAction = period->registerEvent(static_cast<SchedulerActionTimeEvent *>(msg));
    }
}

void TTBuffer::handleParameterChange(const char* parname)
{
    CTBuffer::handleParameterChange(parname);

    if (actionTimeEvent)
        actionTimeEvent->setAction_time((uint32_t) par("sendWindowStart").longValue());
}

uint64_t TTBuffer::nextSendWindowStart()
{
    return nextAction;
}