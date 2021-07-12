#ifndef SCHEINERMAN_STREAM_PROVIDERS_GENERATE_H
#define SCHEINERMAN_STREAM_PROVIDERS_GENERATE_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

template<typename T, typename Generator>
class Generate : public StreamProvider<T> {

public:
    Generate(Generator&& generator)
        : generator_(generator) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        try {
            current_ = std::make_shared<T>(generator_());
            return true;
        }
        catch (const std::out_of_range& e) {
            // out_of_range indicates end of stream
            return false;
        }
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "[generated stream]\n";
        return PrintInfo::Source();
    }

private:
    Generator generator_;
    std::shared_ptr<T> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
