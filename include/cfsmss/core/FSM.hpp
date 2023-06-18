#ifndef FSM_H
#define FSM_H

#include <cfsmss/core/State.hpp>
#include <cfsmss/core/Transition.hpp>
#include <cfsmss/core/Message.hpp>

#include <vector>
#include <string>
#include <queue>

namespace cfsmss {

class FSM
{
public:
    typedef std::vector<State> States;

    FSM(std::string& name, States& states, StateId initStateId, FSM_id initMemSlot)
    : name(name), m_states(states), m_curStateId(initStateId), m_memorySlot(initMemSlot) {}

    /// Go on a transition specified by message.
    /// \param sender_id Id of a sending FSM. NO_FSM if sent by user.
    /// \param msg Message that should trigger transition
    /// \param sendMsg OUTPUT. reciever FSM id and a message to send are written here. NO_SENDMSG if nothing to send.
    /// \return true if transition happened, false otherwise
    bool Go(FSM_id sender_id, Msg msg, SendMsg& sendMsg);
    StateId GetCurStateId() const { return m_curStateId; }
    std::size_t GetNumStates() const { return m_states.size(); }
    std::vector<std::string> GetStateNames() const;
    FSM_id GetLastSenderId() const { return m_memorySlot; }

    std::string name;

private:
    States      m_states;
    StateId     m_curStateId;
    FSM_id      m_memorySlot;
};

}
#endif
