#include <cfsmss/core/State.hpp>

using namespace cfsmss;

std::pair<StateId, Msg>
State::GetTran(Msg msg) const
{
    auto it = m_msg2trans.find(msg);
    if (it == m_msg2trans.end()) // no such tran
        return {NO_STATE, NO_MSG};
    StateId sId = it->second.s2_id;
    Msg sMsg = it->second.msgOut;
    return {sId, sMsg};
}
