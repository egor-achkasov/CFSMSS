#ifndef STATE_H
#define STATE_H

namespace cfsmss {
    class State;
}

#include <cfsmss/core/Message.hpp>
#include <cfsmss/core/Transition.hpp>

#include <string>
#include <map>

namespace cfsmss {

class State
{
public:
    typedef std::map<Msg, Transition> Msg2Trans;

    State(std::string& name, Msg2Trans&& msg2trans)
    : name(name), m_msg2trans(msg2trans) {};

    /// \return pair of a next stateId and a message to send
    /// \return {NO_STATE, NO_MSG} if there is no transition for the msg
    /// \return {StateId, NO_MSG} if such a transition exists but it does not contain a send message
    std::pair<StateId, Msg> GetTran (Msg msg) const; 

    std::string name;

private:
    Msg2Trans m_msg2trans;
};

}
#endif
