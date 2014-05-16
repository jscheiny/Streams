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

class EmptyStreamException : public StreamException {

public:
    explicit EmptyStreamException(const std::string& term)
          : StreamException(build_message(term)) {}

private:
    static std::string build_message(const std::string& term) {
        std::stringstream message;
        message << "No terminal result for operation Stream::" << term;
        return message.str();
    }
};

class VacantStreamException : public StreamException {

public:
    explicit VacantStreamException(const std::string& method)
        : StreamException(build_message(method)) {}

private:
    static std::string build_message(const std::string& method) {
        std::stringstream message;
        message << "Cannot call Stream::" << method << " on a vacant stream";
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
