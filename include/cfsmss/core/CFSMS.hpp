#ifndef CFSMS_H
#define CFSMS_H

#include <cfsmss/core/FSM.hpp>
#include <cfsmss/core/Message.hpp> // for SendMsg

#include <vector>
#include <map>
#include <queue>

namespace cfsmss {

/*
* Communicating Finite State Machines System
* In terms of Brand's & Zafiropulo's 1983 paper,
* it is a "Protocol".
* Contains all the FSM and Channels
*/
class CFSMS
{
public:
    typedef std::vector<FSM> FSMs;
    typedef std::vector<std::string> Names;
    typedef std::queue<Msg> MsgQueue;
    typedef std::vector< std::vector<MsgQueue> > Channels;
    //typedef std::pair<FSM_id, Msg> ExternMsg;
    //typedef std::queue<ExternMsg> ExternMsgs;

    CFSMS(FSMs&& fsms = {}, Names&& msgs = {});

    /// @returns FSM id or NO_FSM if not found
    /// O(1)
    FSM_id GetFSM_id(const std::string& name) const;
    /// @return true if there are no FSMs in system, false otherwise
    /// O(1)
    bool empty() const { return m_fsms.empty(); } 
    /// O(1)
    std::size_t GetNumFSMs() const { return m_fsms.size(); }
    /// O(num_fsms)
    std::size_t GetNumStates() const;
    /// \return vector of cur state ids for each fsm
    /// O(num_fsms)
    std::vector<StateId> GetCurrentStateIds() const;
    /// \return channels matrix
    /// O(num_fsms^2)
    Channels GetChannels() const { return m_chans; }
    Names GetFSM_Names() const;
    Names GetMsgNames() const { return m_msgNames; }
    std::vector<Names> GetStateNames() const;

    /// Send a message to an FSM. Will be placed in a priority queue.
    void SendMsg(FSM_id r, Msg m) { m_chans[r][r].push(m); }
    /// Send a message to an FSM. Will be placed in a priority queue.
    void SendMsg(cfsmss::SendMsg sm) { m_chans[sm.first][sm.first].push(sm.second); }
    /// Send a message to an FSM from FSM s.
    void SendMsg(FSM_id s, cfsmss::SendMsg sm) { m_chans[s][sm.first].push(sm.second); }
    /// Send a message to an FSM by other FSM
    void SendMsg(FSM_id s, FSM_id r, Msg m) { m_chans[s][r].push(m); }
    /// Perform one simulational step.
    /// @returns 0 if no error detected, not 0 otherwise.
    int Step();

private:
    FSMs m_fsms;
    /// N by N matrix of queues of messages, where N is an amount of FSM.
    /// m_chans[i][j] is a queue of messages from FSM with id i to FSM with id j.
    /// m_chans[i][i] is a queue of external messages sent by user explicitly. Has a higher priority.
    Channels m_chans;
    Names m_msgNames;

    /// Preform simulation step for on FSM.
    /// store a msg to send in a second param
    /// \return true if a transition happened.
    /// \return false if none of the messages in channels has triggered a transition
    bool m_FSM_Step(FSM_id f,
        std::vector<cfsmss::SendMsg>& to_send);
};

}
#endif
