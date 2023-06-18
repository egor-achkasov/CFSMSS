#include <cfsmss/core/CFSMS.hpp>

#include <set>

using namespace cfsmss;

CFSMS::CFSMS(FSMs&& fsms, CFSMS::Names&& msg2name)
{
    m_fsms = std::move(fsms);
    m_msgNames = std::move(msg2name);
    m_chans.resize(m_fsms.size());
    for (auto& c : m_chans)
        c.resize(m_fsms.size());
}

FSM_id
CFSMS::GetFSM_id(const std::string& name) const
{
    for (FSM_id i = 0; i != m_fsms.size(); ++i)
        if (m_fsms[i].name == name)
            return i;
    return NO_FSM;
}

std::size_t
CFSMS::GetNumStates() const
{
    std::size_t result = 0;
    for (auto& fsm : m_fsms)
        result += fsm.GetNumStates();
    return result;
}

std::vector<StateId>
CFSMS::GetCurrentStateIds() const
{
    std::vector<StateId> stateIds;
    stateIds.reserve(m_fsms.size());
    for (auto& fsm : m_fsms)
        stateIds.push_back(fsm.GetCurStateId());
    return stateIds;
}

CFSMS::Names
CFSMS::GetFSM_Names() const
{
    Names names;
    names.reserve(m_fsms.size());
    for (auto& fsm : m_fsms)
        names.push_back(fsm.name);
    return names;
}

std::vector<CFSMS::Names>
CFSMS::GetStateNames() const
{
    std::vector<Names> names;
    names.reserve(m_fsms.size());
    for (auto& f : m_fsms)
        names.push_back(f.GetStateNames());
    return names;
}

// FSMStep
//=============================================================================

bool
CFSMS::m_FSM_Step(FSM_id i,
    std::vector<cfsmss::SendMsg>& to_send)
{
    cfsmss::SendMsg sendMsg = NO_SENDMSG;
    Msg msg = NO_MSG;
    bool is_accepted;
    FSM* fsm = &m_fsms[i];
    // Recieve priority message
    if (!m_chans[i][i].empty()){
        msg = m_chans[i][i].front();
        is_accepted = fsm->Go(NO_FSM, msg, sendMsg);
        if (is_accepted) { // if msg has triggered a transition
            m_chans[i][i].pop();
            to_send.push_back(sendMsg);
            return true;
        }
    }
    // Recieve messages from other channels
    // Round-Robin is used for channels equal priority
    size_t start_j = fsm->GetLastSenderId();
    size_t j = start_j;
    do {
        if (j == i) continue;
        if (m_chans[j][i].empty()) continue;
        msg = m_chans[j][i].front();
        is_accepted = fsm->Go(j, msg, sendMsg);
        if (!is_accepted) continue;
        m_chans[j][i].pop();
        // msg accepted and the transition occured.
        // now send the resulting msg.
        to_send.push_back(sendMsg);
        return true;
    } while((j = (j+1)%m_fsms.size()) != start_j);
    to_send.push_back(NO_SENDMSG);
    return false; // none of msgs in channels triggered a transition
}

int CFSMS::Step()
{
    // has at least one FSM transition happened?
    bool is_changed = false;
    std::vector<cfsmss::SendMsg> to_send;
    to_send.reserve(m_fsms.size());

    // accept msgs and perform transition
    for (FSM_id i = 0; i != m_fsms.size(); ++i)
        is_changed |= m_FSM_Step(i, to_send);
    // check if system's global state has changed
    if (!is_changed) return -1;
    
    // send msgs
    for (size_t f = 0; f != m_fsms.size(); ++f) {
        if (to_send[f].second == NO_MSG) continue;
        SendMsg(f, to_send[f]);
    }
    return 0;
}

//=============================================================================
