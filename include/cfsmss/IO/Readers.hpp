#ifndef READER_H
#define READER_H

#include <cfsmss/core/CFSMS.hpp>
#include <cfsmss/core/Message.hpp> // for SendMsg

#include <string>
#include <map>
#include <list>

namespace cfsmss{

class TextReader {
public:
    static CFSMS read_cfsms_file(const std::string& path);
    static std::map<size_t, std::list<SendMsg> >
    read_msgs_file(const std::string& path,
        const std::vector<std::string>& fsm_names,
        const std::vector<std::string>& msg_names);
};

class BinaryReader {
public:
    static CFSMS read_file(const std::string& path);
    static std::map<size_t, std::list<SendMsg> >
    read_msgs_file(const std::string& path,
        const std::vector<std::string>& fsm_names,
        const std::vector<std::string>& msg_names);
};

}
#endif
