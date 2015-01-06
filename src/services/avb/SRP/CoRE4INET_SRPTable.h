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
#ifndef __CoRE4INET_SRPTABLE_H_
#define __CoRE4INET_SRPTABLE_H_

//Std
#include <map>
#include <unordered_map>

//OMNeT++
#include "csimplemodule.h"
//INET
#include "MACAddress.h"
//CoRE4INET
#include "CoRE4INET_AVBDefs.h"
//Auto-generated Messages
#include "AVBDefs_m.h"

namespace CoRE4INET {

/**
 * @brief This module handles the mapping between ports and MAC addresses.
 *
 * See the NED definition for details.
 */
class SRPTable : public cSimpleModule
{
    public:
        /**
         * @brief Entry for Talker
         */
        class TalkerEntry : public cObject
        {
            public:
                uint64_t streamId;
                SR_CLASS srClass;               // Stream Reservation Class
                MACAddress address;             // The talkers address
                cModule *module;                // Input port or module
                size_t framesize;         // framesize in byte
                uint16_t intervalFrames;    // interval frames
                uint16_t vlan_id;         // VLAN identifier
                simtime_t insertionTime;        // Arrival time of SRP entry
                TalkerEntry()
                {
                    streamId = 0;
                    srClass = SR_CLASS_A;
                    module = NULL;
                    framesize = 0;
                    intervalFrames = 0;
                    vlan_id = VLAN_ID_DEFAULT;
                }
                TalkerEntry(uint64_t streamId, SR_CLASS srClass, MACAddress address, cModule *module,
                        size_t framesize, unsigned short intervalFrames, unsigned short vlan_id, simtime_t insertionTime) :
                        streamId(streamId), srClass(srClass), address(address), module(module), framesize(framesize), intervalFrames(
                                intervalFrames), vlan_id(vlan_id), insertionTime(insertionTime)
                {
                }
        };
        friend std::ostream& operator<<(std::ostream& os, const TalkerEntry& entry);

        /**
         * @brief Entry for Listener
         */
        class ListenerEntry : public cObject
        {
            public:
                uint64_t streamId;
                cModule *module;            // Listener port or module
                uint16_t vlan_id;     // VLAN identifier
                simtime_t insertionTime;    // Arrival time of SRP entry
                ListenerEntry()
                {
                    streamId = 0;
                    module = NULL;
                    vlan_id = VLAN_ID_DEFAULT;
                }
                ListenerEntry(uint64_t streamId, cModule *module, unsigned short vlan_id, simtime_t insertionTime) :
                        streamId(streamId), module(module), vlan_id(vlan_id), insertionTime(insertionTime)
                {
                }
        };
        friend std::ostream& operator<<(std::ostream& os, const ListenerEntry& entry);

        typedef std::unordered_map<uint64_t, TalkerEntry*> TalkerTable;
        typedef std::unordered_map<cModule*, ListenerEntry*> ListenerList;
        typedef std::unordered_map<uint64_t, ListenerList> ListenerTable;

        /**
         * map of talker entries for stream id
         */
        std::unordered_map<unsigned int, TalkerTable> talkerTables;
        /**
         * map of listener entries for stream id
         */
        std::unordered_map<unsigned int, ListenerTable> listenerTables;

        /**
         * Time when next entry is aging
         */
        simtime_t nextAging;
    protected:

        /**
         *  @brief Initialization, registers WATCH and updates display string
         */
        virtual void initialize();

        /**
         *  @brief Table does not receive messages, throws cRuntimeError when handleMessage is called
         */
        virtual void handleMessage(cMessage *msg) __attribute__ ((noreturn));

    public:
        /**
         *  @brief Constructor
         */
        SRPTable();
        /**
         *  @brief Destructor
         */
        ~SRPTable();

    public:
        /**
         * @brief For a known talker address and V-TAG it finds out the streamId
         *
         * @param talkerAddress address
         * @param vid VLAN ID
         * @return streamId related to talkerAddress
         */
        virtual uint64_t getStreamIdForTalkerAddress(MACAddress &talkerAddress, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         * @brief For a known talker address and V-TAG it finds out the SR-Class of the Stream
         *
         * @param talkerAddress address
         * @param vid VLAN ID
         * @return SR-Class related to the Stream of the talkerAddress
         */
        virtual SR_CLASS getSrClassForTalkerAddress(MACAddress &talkerAddress, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         * @brief For a known streamId and V-TAG it finds out the port where relay component should deliver the message
         *
         * @param streamId streamId
         * @param vid VLAN ID
         * @return listeners for the stream
         */
        virtual std::list<cModule*> getListenersForStreamId(uint64_t streamId, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         * @brief For a known talker address and V-TAG it finds out the port where relay component should deliver the message
         *
         * @param talkerAddress address
         * @param vid VLAN ID
         * @return listeners for the stream
         */
        virtual std::list<cModule*> getListenersForTalkerAddress(MACAddress &talkerAddress, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         * @brief Retrieve the module a message with a given streamId will come from (required for listener ready messages)
         *
         * @param streamId the streams id
         * @param vid VLAN ID
         * @return Output port for address, or -1 if unknown.
         */
        virtual cModule* getTalkerForStreamId(uint64_t streamId, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         * @brief Retrieve the required bandwidth for a module with registered listeners
         *
         * @param module the module registered as listener
         * @return bandwidth in bps
         */
        virtual unsigned long getBandwidthForModule(cModule *module);

        /**
         * @brief Retrieve the required bandwidth for a module with registered listeners per SR-Class
         *
         * @param module the module registered as listener
         * @param srClass the SR-Class (A or B)
         * @return bandwidth in bps
         */
        virtual unsigned long getBandwidthForModuleAndSRClass(cModule *module, SR_CLASS srClass);

        /**
         * @brief Retrieve the required bandwidth for a stream
         *
         * @param streamId the streams id
         * @param vid VLAN ID
         * @return bandwidth in bps
         */
        virtual unsigned long getBandwidthForStream(uint64_t streamId, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         * @brief Register a new streamId at talkerTable.
         * @return True if refreshed. False if it is new.
         */
        virtual bool updateTalkerWithStreamId(uint64_t streamId, cModule *module, MACAddress address, SR_CLASS srClass =
                SR_CLASS_A, size_t framesize = 0, uint16_t intervalFrames = 0, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         * @brief Unregister a streamId at talkerTable.
         * @return True if removed. False if not registered.
         */
        virtual bool removeTalkerWithStreamId(uint64_t streamId, cModule *module, MACAddress address, uint16_t vid =
                VLAN_ID_DEFAULT);

        /**
         * @brief Register a new streamId at listenerTable.
         * @return True if refreshed. False if it is new.
         */
        virtual bool updateListenerWithStreamId(uint64_t streamId, cModule *module, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         * @brief Unregister a streamId at listenerTable.
         * @return True if removed. False if not registered.
         */
        virtual bool removeListenerWithStreamId(uint64_t streamId, cModule *module, uint16_t vid = VLAN_ID_DEFAULT);

        /**
         *  @brief Prints cached data
         */
        virtual void printState();

        /**
         * @brief For lifecycle: clears all entries from the table.
         */
        virtual void clear();

        /**
         * @brief Remove entries that were not updated during agingTime if necessary
         */
        void removeAgedEntriesIfNeeded();

    protected:
        /**
         * @brief Remove entries that were not updated during agingTime
         */
        void removeAgedEntries();

        /**
         * @brief updates the displaystring of talkers and listeners
         */
        void updateDisplayString();
        /**
         * @brief get the number of registered talkers
         *
         * @return number of talkers
         */
        size_t getNumTalkerEntries();

        /**
         * @brief get the number of registered listeners
         *
         * @return number of listeners
         */
        size_t getNumListenerEntries();
};

}
#endif
