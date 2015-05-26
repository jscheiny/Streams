#ifndef SCHEINERMAN_STREAM_PROVIDERS_DYNAMIC_GROUP_H
#define SCHEINERMAN_STREAM_PROVIDERS_DYNAMIC_GROUP_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <vector>

namespace stream {
namespace provider {

template<typename Source>
class dynamic_group {

private:
    using source_elem = typename Source::element;

public:
    using element = std::vector<source_elem>;

    dynamic_group(std::unique_ptr<Source> source, size_t N)
        : source_(std::move(source))
        , N_(N) {}

    std::shared_ptr<std::vector<source_elem>> get() {
        return current_;
    }

    bool advance() {
        current_ = std::make_shared<std::vector<source_elem>>();
        current_->reserve(N_);
        for (int i = 0; i < N_; i++) {
            if (stream_advance(source_)) {
                current_->emplace_back(std::move(*source_->get()));
            } else {
                current_.reset();
                return false;
            }
        }
        return true;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Grouped[" << N_ << "]:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    std::shared_ptr<std::vector<source_elem>> current_;
    const size_t N_;

};

} /* namespace provider */
} /* namespace stream */

#endif
