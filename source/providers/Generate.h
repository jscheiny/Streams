#ifndef SCHEINERMAN_STREAM_PROVIDERS_GENERATE_H
#define SCHEINERMAN_STREAM_PROVIDERS_GENERATE_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

template<typename Generator>
class generate {

public:
    using element = std::result_of_t<Generator()>;

    generate(Generator&& generator)
        : generator_(generator) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        current_ = std::make_shared<element>(generator_());
        return true;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "[generated stream]\n";
        return print_info::source();
    }

private:
    Generator generator_;
    std::shared_ptr<element> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
