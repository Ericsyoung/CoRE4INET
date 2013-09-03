//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "AVBBuffer.h"
#include <CTFrame_m.h>
#include <AVBFrame_m.h>
#include "TTEApplicationBase.h"

namespace TTEthernetModel {

Define_Module(AVBBuffer);

simsignal_t AVBBuffer::creditSignal = SIMSIGNAL_NULL;

AVBBuffer::AVBBuffer()
{

}

AVBBuffer::~AVBBuffer()
{

}

int AVBBuffer::numInitStages() const
{
    return 1;
}

void AVBBuffer::initialize(int stage)
{
    Buffer::initialize(stage);

    if(stage==0)
    {
        credit = 0;
        maxCredit = 0;
        AVBReservation = 0;
        inTransmission = false;
        msgCnt = 0;
        newTime = simTime();
        oldTime = simTime();
        Wduration = 0;

        scheduler = (TTEScheduler*) getParentModule()->getSubmodule("scheduler");
        avbCTC = (AVBIncoming*)getParentModule()->getSubmodule("avbCTC");

        creditSignal = registerSignal("credit");

        WATCH(credit);
        WATCH(maxCredit);
        WATCH(AVBReservation);
        WATCH(inTransmission);
        WATCH(msgCnt);
        WATCH(Wduration);
    }
}

void AVBBuffer::handleMessage(cMessage *msg)
{
    Buffer::handleMessage(msg);

    if(avbCTC->getForwarding())
    {
        newTime = simTime();

        if(credit < 0)
        {
            idleSlope(newTime - oldTime);
        }

        if(msg->arrivedOn("in"))
        {
            msgCnt++;
            if(inTransmission)
            {
                interferenceSlope(newTime - oldTime);
                if(credit > maxCredit) maxCredit = credit;
            }
            else
            {
                if(credit >= 0)
                {
                    if(msgCnt > 0)
                    {
                        cMessage *outFrame = getFrame();
                        msgCnt--;
                        send(outFrame, "out");
                        inTransmission = true;
                    }
                    else
                    {
                        resetCredit();
                    }

                }
                else if(credit < 0)
                {
                    AVBReservation = avbCTC->getAVBPortReservation(this->getIndex());
                    Wduration = ((double)-credit)/(AVBReservation * 1024.00 * 1024.00);
                    SchedulerTimerEvent *event = new SchedulerTimerEvent("API Scheduler Task Event", TIMER_EVENT);
                    event->setTimer(ceil(Wduration / scheduler->par("tick").doubleValue()));
                    event->setDestinationGate(gate("schedulerIn"));
                    scheduler->registerEvent(event);
                }
            }
        }
        else if(msg->arrivedOn("schedulerIn"))
        {
            if(inTransmission)
            {
                interferenceSlope(newTime - oldTime);
                if(credit > maxCredit) maxCredit = credit;
            }
            else
            {
                if(credit >= 0)
                {
                    if(msgCnt > 0)
                    {
                        cMessage *outFrame = getFrame();
                        msgCnt--;
                        send(outFrame, "out");
                        inTransmission = true;
                    }
                    else
                    {
                        resetCredit();
                    }
                }
                else if(credit < 0)
                {
                    emit(creditSignal, credit);
                    AVBReservation = avbCTC->getAVBPortReservation(this->getIndex());
                    Wduration = ((double)-credit)/(AVBReservation * 1024.00 * 1024.00);
                    SchedulerTimerEvent *event = new SchedulerTimerEvent("API Scheduler Task Event", TIMER_EVENT);
                    event->setTimer(ceil(Wduration / scheduler->par("tick").doubleValue()));
                    event->setDestinationGate(gate("schedulerIn"));
                    scheduler->registerEvent(event);
                }
            }
            delete msg;
        }

        if(newTime >= oldTime)
            oldTime = simTime();
    }
}

void AVBBuffer::handleParameterChange(const char* parname)
{
    Buffer::handleParameterChange(parname);
}

void AVBBuffer::idleSlope(SimTime duration)
{
    if(duration >= 0)
    {
        Enter_Method("idleSlope()");
        AVBReservation = avbCTC->getAVBPortReservation(this->getIndex());
        credit += ceil( (AVBReservation * 1024.00 * 1024.00) * duration.dbl() );
        emit(creditSignal, credit);
        if(credit > 0 && msgCnt == 0 && !inTransmission) resetCredit();
    }
}

void AVBBuffer::interferenceSlope(SimTime duration)
{
    if(duration >= 0)
    {
        Enter_Method("interferenceSlope()");
        AVBReservation = avbCTC->getAVBPortReservation(this->getIndex());
        credit +=ceil( (AVBReservation * 1024.00 * 1024.00) * duration.dbl() );
        emit(creditSignal, credit);
    }
}

void AVBBuffer::sendSlope(SimTime duration)
{
    Enter_Method("sendSlope()");

    AVBReservation = avbCTC->getAVBPortReservation(this->getIndex());
    int portBandwith = avbCTC->getPortBandwith(this->getIndex());
    emit(creditSignal, credit);
    credit -= ceil( ( (portBandwith - AVBReservation) * 1024.00 * 1024.00) * duration.dbl() );
    inTransmission = false;
    if(msgCnt > 0)
    {
        if(credit < 0)
        {
            AVBReservation = avbCTC->getAVBPortReservation(this->getIndex());
            Wduration = duration.dbl();//((double)-credit)/(AVBReservation * 1024.00 * 1024.00) +
            SchedulerTimerEvent *event = new SchedulerTimerEvent("API Scheduler Task Event", TIMER_EVENT);
            event->setTimer(ceil(Wduration / scheduler->par("tick").doubleValue()));
            event->setDestinationGate(gate("schedulerIn"));
            scheduler->registerEvent(event);
        }
        else
        {
            cMessage *outFrame = getFrame();
            msgCnt--;
            send(outFrame, "out");
            inTransmission = true;
        }
    }
    else if(credit > 0)
    {
        resetCredit();
    }

    if(oldTime <= simTime())
        oldTime = simTime() + duration;
    else
        oldTime = oldTime + duration;
}

void AVBBuffer::refresh()
{
    newTime = simTime();


    if(credit < 0)
    {
        idleSlope(newTime - oldTime);
    }

    if(inTransmission)
    {
        interferenceSlope(newTime - oldTime);
        if(credit > maxCredit) maxCredit = credit;
    }

    if(newTime >= oldTime)
        oldTime = simTime();
}

int AVBBuffer::getCredit()
{
   //Enter_Method("getCredit()");

   return credit;
}

void AVBBuffer::resetCredit()
{
    Enter_Method("resetCredit");

    if(newTime >= oldTime)
    {
        credit = 0;
        emit(creditSignal, credit);
    }
}

int AVBBuffer::getMsgCount()
{
    return msgCnt;
}

} /* namespace TTEthernetModel */
