#ifndef TRANSITION_H
#define TRANSITION_H

namespace cfsmss{
    class Transition;
}

#include <cfsmss/core/State.hpp>

#include <string>

namespace cfsmss {

/// Class for a transition from one state to anothe within a FSM.
///         
///   @----------------->@
/// s1_id   -msgOut    s2_id
class Transition
{
public:
    Msg msgOut;
    StateId s2_id;
};

}
#endif
