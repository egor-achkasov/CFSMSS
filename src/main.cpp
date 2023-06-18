#include <cfsmss/core/CFSMS.hpp>
#include <cfsmss/IO/Readers.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <string.h> // for strlen_s
#include <map>
#include <list>
#include <cassert>

using namespace cfsmss;

struct Flag {
    std::string short_name;
    std::string description;
};

std::size_t g_num_steps = 0;
std::string g_msgs_path = "";
bool g_print_fsm_names = false;
bool g_print_state_names = false;

std::vector<Flag> g_flags = {
    {"-h", "Print help and exit"},
    {"-s", "Set number of steps. Followed by by a number. Default is number_of_transimissions*number_of_states"},
    {"-m", "Path to a file with external messages."}
};

void print_help_and_exit()
{
    std::cout
        << "Usage: cfsmss [options] cfsm_system_file\n"
        << "\toptions:\n";
    for (auto& flag : g_flags) {
        std::cout << "\n\t\t"
            << flag.short_name << "\t"
            << flag.description;
    }
    std::cout << std::endl;
    exit(0);
}

void parse_flags(int argc, char* argv[])
{
    if (!argv) return;
    for (int i = 1; i < argc; ++i) {
        char* arg = argv[i];
        if (!arg) print_help_and_exit();
        std::size_t num_chars = strnlen(argv[i], 64);
        if (num_chars != 2) return;
        if (arg[0] != '-') return;
        switch (arg[1])
        {
        case 'h':
            print_help_and_exit();
            break;
        case 's':
        {
            int num_steps = std::stoi(argv[i+1]);
            if (num_steps < 1) print_help_and_exit();
            g_num_steps = (std::size_t)num_steps;
            ++i;
            break;
        }
        case 'm':
        {
            g_msgs_path = argv[i+1];
            ++i;
            break;
        }
        default:
            print_help_and_exit();
        }
    }
}

void
print_current_states(std::ostream& os, const CFSMS& cfsms,
    const std::vector<std::string>& fsm_names,
    const std::vector< std::vector<std::string> >& state_names)
{
    std::vector<StateId> curStateIds = cfsms.GetCurrentStateIds();
    assert(curStateIds.size() == fsm_names.size());
    for (size_t i = 0; i != curStateIds.size(); ++i)
        os << fsm_names[i] << ":\t"
           << state_names[i][curStateIds[i]] << std::endl;
}

void
print_channels(std::ostream& os, const CFSMS& cfsms,
    const std::vector<std::string>& msg_names)
{
    CFSMS::Channels chans = cfsms.GetChannels();
    size_t num_fsms = cfsms.GetNumFSMs();
    for (FSM_id i = 0; i != num_fsms; ++i) {
        for (FSM_id j = 0; j != num_fsms; ++j)
            os << "| "
               << (chans[i][j].empty() ? "-" : msg_names[chans[i][j].front()])
               << " : " << chans[i][j].size()
               << " |\t";
        os << std::endl;
    }
}

int main(int argc, char* argv[])
{
    // process args
    if (argc < 2) print_help_and_exit();
    parse_flags(argc, argv);

    // Init output
    std::ostream ostream(nullptr);
    ostream.rdbuf(std::cout.rdbuf());
    if (!ostream.good()) {
        std::cerr << "ERROR: Faild to initialize output stream." << std::endl;
        return(1);
    }

    // read given CFSMS
    CFSMS cfsms = TextReader::read_cfsms_file(argv[argc-1]);
    if (cfsms.empty()) {
        std::cerr << "CFSMS contains zero FSMs. Check the input file." << std::endl;
        return(0);
    }

    // read external messages
    auto stepId2FSM_id2Msg = TextReader::read_msgs_file(
        g_msgs_path, cfsms.GetFSM_Names(), cfsms.GetMsgNames());

    // get names
    std::vector<std::string> fsm_names = cfsms.GetFSM_Names();
    std::vector< std::vector<std::string> >
    state_names = cfsms.GetStateNames();
    std::vector<std::string> msg_names = cfsms.GetMsgNames();

    // push external messages
    auto msgs_list_id = stepId2FSM_id2Msg.find(0);
    if (msgs_list_id != stepId2FSM_id2Msg.end()) {
        for (auto sendMsg : msgs_list_id->second)
            cfsms.SendMsg(sendMsg);
        stepId2FSM_id2Msg.erase(msgs_list_id);
    }

    // print current states and channel content
    ostream << "--= INITIAL GLOBAL STATE" << " =--" << std::endl;
    ostream << "-=States:" << std::endl;
    print_current_states(ostream, cfsms, fsm_names, state_names);
    ostream << "-=Channels:" << std::endl;
    print_channels(ostream, cfsms, msg_names);

    // main program loop
    if (g_num_steps == 0)
        g_num_steps = cfsms.GetNumFSMs() * cfsms.GetNumStates();
    for (std::size_t step_n = 0; step_n != g_num_steps; ++step_n) {
        // perform step
        int rc = cfsms.Step();
        if (rc) {
            std::cerr << "ERROR: CFSMS::STEP error!" << std::endl;
            return(rc);
        }
        // push external messages
        auto msgs_list_id = stepId2FSM_id2Msg.find(step_n+1);
        if (msgs_list_id != stepId2FSM_id2Msg.end()) {
            for (auto sendMsg : msgs_list_id->second)
                cfsms.SendMsg(sendMsg);
            stepId2FSM_id2Msg.erase(msgs_list_id);
        }
        // print current states and channel content
        ostream << "--= STEP " << step_n << " =--" << std::endl;
        ostream << "-=States:" << std::endl;
        print_current_states(ostream, cfsms, fsm_names, state_names);
        ostream << "-=Channels:" << std::endl;
        print_channels(ostream, cfsms, msg_names);
    }

    return 0;
}