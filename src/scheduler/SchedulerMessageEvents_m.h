//
// Generated file, do not edit! Created by opp_msgc 4.2 from scheduler/SchedulerMessageEvents.msg.
//

#ifndef _SCHEDULERMESSAGEEVENTS_M_H_
#define _SCHEDULERMESSAGEEVENTS_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "SchedulerEvent.h"
// }}


namespace TTEthernetModel {

/**
 * Class generated from <tt>scheduler/SchedulerMessageEvents.msg</tt> by opp_msgc.
 * <pre>
 * message SchedulerActionTimeEvent extends SchedulerEvent {
 *     unsigned int action_time;
 * }
 * </pre>
 */
class SchedulerActionTimeEvent : public ::TTEthernetModel::SchedulerEvent
{
  protected:
    unsigned int action_time_var;

    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const SchedulerActionTimeEvent&);

  public:
    SchedulerActionTimeEvent(const char *name=NULL, int kind=0);
    SchedulerActionTimeEvent(const SchedulerActionTimeEvent& other);
    virtual ~SchedulerActionTimeEvent();
    SchedulerActionTimeEvent& operator=(const SchedulerActionTimeEvent& other);
    virtual SchedulerActionTimeEvent *dup() const {return new SchedulerActionTimeEvent(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual unsigned int getAction_time() const;
    virtual void setAction_time(unsigned int action_time_var);
};

inline void doPacking(cCommBuffer *b, SchedulerActionTimeEvent& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, SchedulerActionTimeEvent& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>scheduler/SchedulerMessageEvents.msg</tt> by opp_msgc.
 * <pre>
 * message SchedulerTimerEvent extends SchedulerEvent {
 *     unsigned int timer;
 * }
 * </pre>
 */
class SchedulerTimerEvent : public ::TTEthernetModel::SchedulerEvent
{
  protected:
    unsigned int timer_var;

    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const SchedulerTimerEvent&);

  public:
    SchedulerTimerEvent(const char *name=NULL, int kind=0);
    SchedulerTimerEvent(const SchedulerTimerEvent& other);
    virtual ~SchedulerTimerEvent();
    SchedulerTimerEvent& operator=(const SchedulerTimerEvent& other);
    virtual SchedulerTimerEvent *dup() const {return new SchedulerTimerEvent(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual unsigned int getTimer() const;
    virtual void setTimer(unsigned int timer_var);
};

inline void doPacking(cCommBuffer *b, SchedulerTimerEvent& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, SchedulerTimerEvent& obj) {obj.parsimUnpack(b);}

}; // end namespace TTEthernetModel

#endif // _SCHEDULERMESSAGEEVENTS_M_H_
