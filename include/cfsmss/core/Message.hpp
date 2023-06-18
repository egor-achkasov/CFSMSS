#ifndef MESSAGE_H
#define MESSAGE_H

#include <cfsmss/core/TypeId.hpp> // for FSM_id

#include <cstdint> // for uint32_t
#include <utility> // for pair

namespace cfsmss {

typedef uint32_t Msg;
static const Msg NO_MSG = -1;
typedef std::pair<FSM_id, Msg> SendMsg;
static const SendMsg NO_SENDMSG = {NO_FSM, NO_MSG};

/*
struct Msg {
    FSM_id senFSM_id;
    FSM_id recFSM_id;
    uint32_t val;

    bool operator==(const Msg& o)
    {
        return senFSM_id == o.senFSM_id
        && recFSM_id == o.recFSM_id
        && val == o.val;
    }
};
static const Msg NO_MSG = {NO_FSM, NO_FSM, -1};
*/

}

#endif
