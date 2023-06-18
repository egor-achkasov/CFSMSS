#include <cfsmss/core/FSM.hpp>

using namespace cfsmss;
    
std::vector<std::string>
FSM::GetStateNames() const
{
    std::vector<std::string> names;
    names.reserve(m_states.size());
    for (auto& s : m_states)
        names.push_back(s.name);
    return names;
}

bool
FSM::Go(FSM_id sender_id, Msg msg, SendMsg& sendMsg)
{
    State* curState = &m_states[m_curStateId];
    auto p = curState->GetTran(msg);
    if (p.first == NO_STATE) { // no such transition
        sendMsg = NO_SENDMSG;
        return false;
    }
    m_curStateId = p.first;

    // construct send message
    sendMsg = {m_memorySlot, p.second};
    if (sender_id != NO_FSM)
        m_memorySlot = sender_id;
    return true;
}
