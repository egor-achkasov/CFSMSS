#ifndef TYPEID_H
#define TYPEID_H

#include <cstddef> // for size_t

namespace cfsmss {
    typedef std::size_t FSM_id;
    static const FSM_id NO_FSM = -1;
    typedef std::size_t StateId;
    static const StateId NO_STATE = -1;
}
#endif