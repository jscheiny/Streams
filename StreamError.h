#ifndef STREAM_ERROR_H
#define STREAM_ERROR_H

#include <string>
#include <sstream>

class StreamException {

public:
    explicit StreamException(const std::string& msg) : message(msg) {}
    explicit StreamException(const char* msg) : message(msg) {}

    std::string what() const { return message; }

private:
    std::string message;

};

class VacantStreamException : public StreamException {

public:
    explicit VacantStreamException(const std::string& method)
        : StreamException(build_message(method)) {}

private:
    static std::string build_message(const std::string& method) {
        std::stringstream message;
        message << "Cannot call Stream::" << method << " on an empty stream";
        return message.str();
    }
};

class ConsumsedIteratorException : public StreamException {

public:
    explicit ConsumsedIteratorException(const std::string& op)
        : StreamException(build_message(op)) {}

private:
    static std::string build_message(const std::string& op) {
        std::stringstream message;
        message << "Cannot perform " << op << " on consumed stream iterator.";
        return message.str();
    }
};

#endif
