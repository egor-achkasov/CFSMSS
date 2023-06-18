#include <cfsmss/IO/Readers.hpp>

#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cctype> // for isdigit
#include <list>

using namespace cfsmss;

inline bool
isid(const std::string& word)
{
    for (char c : word)
        if (!(c >= '0' && c <= '9'))
            return false;
    return true;
}

/// \return i: names[i] == name
/// \return -1 if not found
inline size_t
id_by_name(const std::vector<std::string>& names, const std::string& name)
{
    for (size_t i = 0; i != names.size(); ++i)
        if (names[i] == name)
            return i;
    return -1;
}

inline void
atosize_t(const std::string& word, size_t& out)
{
    std::sscanf(word.c_str(), "%zu", &out);
}

CFSMS
TextReader::read_cfsms_file(const std::string& path)
{
    std::ifstream ifs(path);
    if (!ifs.is_open()) return CFSMS();
    std::string line, word;
    size_t num_fsms, num_msgs;
    CFSMS::Names fsm_names, msg_names;

    // fsms
    std::getline(ifs, line);
    std::istringstream iss(line);
    iss >> word;
    if (word != "fsms") return CFSMS();
    iss >> num_fsms;
    if (!num_fsms) return CFSMS();
    if (!iss.eof()){ // file contains names
        fsm_names.reserve(num_fsms);
        for (size_t i = 0; i != num_fsms; ++i){
            iss >> word;
            fsm_names.push_back(word);
        }
    }

    // msgs
    std::getline(ifs, line);
    iss = std::istringstream(line);
    iss >> word;
    if (word != "msgs") return CFSMS();
    iss >> num_msgs;
    if (!num_msgs) return CFSMS();
    if (!iss.eof()) {
        msg_names.reserve(num_msgs);
        for (size_t i = 0; i != num_msgs; ++i) {
            iss >> word;
            msg_names.push_back(word);
        }
    }

    // for each fsm
    std::vector<FSM> fsms;
    fsms.reserve(num_fsms);
    for (size_t fsm_id = 0; fsm_id != num_fsms; ++fsm_id){
        size_t num_states = 0;
        FSM::States states;
        std::vector<State::Msg2Trans> states_msg2trans;
        std::vector<std::string> state_names;
        StateId initStateId = NO_STATE;
        FSM_id initMemSlot = NO_FSM;
        
        // s
        std::getline(ifs, line);
        iss = std::istringstream(line);
        iss >> word;
        if (word != "s") return CFSMS();
        iss >> num_states;
        if (!num_states) return CFSMS();
        states.reserve(num_states);
        states_msg2trans.resize(num_states);
        if (!iss.eof()) { // read names
            state_names.resize(num_states);
            for (size_t i = 0; i != num_states; ++i)
                iss >> state_names[i];
        }

        // n0
        std::getline(ifs, line);
        iss = std::istringstream(line);
        iss >> word;
        if (word != "n0") return CFSMS();
        iss >> word;
        if (isid(word)) atosize_t(word, initStateId);
        else initStateId = id_by_name(state_names, word);
        if (initStateId >= num_states) return CFSMS();

        // m0
        std::getline(ifs, line);
        iss = std::istringstream(line);
        iss >> word;
        if (word != "m0") return CFSMS();
        iss >> word;
        if (isid(word)) atosize_t(word, initStateId);
        else initMemSlot = id_by_name(fsm_names, word);
        if (initMemSlot >= num_fsms) return CFSMS();

        // p
        std::getline(ifs, line);
        iss = std::istringstream(line);
        size_t num_trans;
        iss >> word;
        if (word != "p") return CFSMS();
        iss >> num_trans;
        if (!num_trans) return CFSMS();

        // transitions
        StateId s1, s2;
        Msg mIn, mOut;
        std::string word;
        for (size_t i = 0; i != num_trans; ++i) {
            std::getline(ifs, line);
            iss = std::istringstream(line);
            // starting state (s1)
            iss >> word;
            if (isid(word)) atosize_t(word, s1);
            else s1 = id_by_name(state_names, word);
            // incoming msg (mIn)
            iss >> word;
            size_t temp; // magic var for casting size_t to uint32_t
            if (isid(word)) atosize_t(word, temp);
            else temp = id_by_name(msg_names, word);
            mIn = temp;
            // end state (s2)
            iss >> word;
            if (isid(word)) atosize_t(word, s2);
            else s2 = id_by_name(state_names, word);
            // outgoing msg (mOut)
            if (!iss.eof()) {
                iss >> word;
                if (isid(word)) atosize_t(word, temp);
                else temp = id_by_name(msg_names, word);
                mOut = temp;
            } else mOut = NO_MSG;

            states_msg2trans[s1][mIn] = {mOut, s2};
        }

        // construct states
        for (StateId s_id = 0; s_id != num_states; ++s_id)
            states.push_back(State(state_names[s_id], std::move(states_msg2trans[s_id])));
        // construct fsm
        fsms.push_back(FSM(fsm_names[fsm_id], states, initStateId, initMemSlot));
    }

    return CFSMS(std::move(fsms), std::move(msg_names));
}

std::map<size_t, std::list<SendMsg> >
TextReader::read_msgs_file(const std::string& path,
const std::vector<std::string>& fsm_names,
const std::vector<std::string>& msg_names)
{
    std::ifstream ifs(path);
    if (!ifs.is_open()) return {};
    std::map<size_t, std::list<SendMsg> > res;
    std::string line, word;

    // external_msgs
    std::getline(ifs, line);
    std::stringstream iss(line);
    iss >> word;
    if (word != "external_msgs") return {};

    // Send msgs
    size_t step_n;
    FSM_id fsm_id;
    Msg msg;
    while (!ifs.eof()) {
        std::getline(ifs, line);
        if (line == "") continue;
        std::stringstream iss(line);
        // step_n
        iss >> word;
        atosize_t(word, step_n);
        // fsm_id
        iss >> word;
        if (isid(word)) atosize_t(word, fsm_id);
        else fsm_id = id_by_name(fsm_names, word);
        // msg
        iss >> word;
        size_t temp;
        if (isid(word)) atosize_t(word, temp);
        else temp = id_by_name(msg_names, word);
        msg = temp;

        res[step_n].push_back({fsm_id, msg});
    } 

    return res;
}