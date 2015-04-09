#ifndef SCHEINERMAN_STREAM_STREAM_ERROR_H
#define SCHEINERMAN_STREAM_STREAM_ERROR_H

#include <string>
#include <sstream>

namespace stream {

class StreamException {

public:
    explicit StreamException(const std::string& msg) : message(msg) {}
    explicit StreamException(const char* msg) : message(msg) {}

    std::string what() const { return message; }

private:
    std::string message;

};

class EmptyStreamException : public StreamException {

public:
    explicit EmptyStreamException(const std::string& term)
          : StreamException(build_message(term)) {}

private:
    static std::string build_message(const std::string& term) {
        std::stringstream message;
        message << "No terminal result for operation " << term;
        return message.str();
    }
};

class VacantStreamException : public StreamException {

public:
    explicit VacantStreamException(const std::string& opname)
        : StreamException(build_message(opname)) {}

private:
    static std::string build_message(const std::string& opname) {
        std::stringstream message;
        message << "Cannot perform operation " << opname << " on a vacant stream.";
        return message.str();
    }
};

class ConsumedIteratorException : public StreamException {

public:
    explicit ConsumedIteratorException(const std::string& op)
        : StreamException(build_message(op)) {}

private:
    static std::string build_message(const std::string& op) {
        std::stringstream message;
        message << "Cannot perform " << op << " on consumed stream iterator.";
        return message.str();
    }
};

class StopStream : public StreamException {

public:
    StopStream() : StreamException("[End of stream]") {}

};

} /* namespace stream */

#endif
