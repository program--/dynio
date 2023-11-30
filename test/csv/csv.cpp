#include "csv.hpp"

#include <sstream>
#include <algorithm>
#include <cstring>

namespace example {

auto csv::header() const noexcept -> const std::vector<std::string>&
{
    return header_;
}

auto csv::row(size_type n) const -> const std::span<const std::string>
{
    return { &rows_.at(n * header_.size()), header_.size() };
}

void csv::read(std::istream& stream, char delim)
{
    // Read header
    std::string header;
    std::getline(stream, header);
    std::stringstream header_stream{ header };
    for (std::string column; std::getline(header_stream, column, delim);) {
        header_.emplace_back(std::move(column));
    }

    // Read contents
    for (std::string row; std::getline(stream, row);) {
        std::stringstream row_stream{ row };
        for (std::string column; std::getline(row_stream, column, delim);) {
            rows_.emplace_back(std::move(column));
        }
    }
};

void csv::read(std::span<byte_t> input, char delim)
{
    std::istringstream stream{ std::string{ input.begin(), input.end() } };
    read(stream, delim);
}

void csv::write(std::vector<byte_t>& output, char delim)
{

    const std::string delim_   = { delim };
    const std::string newline_ = "\n";

    const auto        append =
      [](const std::string& word, std::vector<byte_t>& output) {
          std::transform(
            std::begin(word),
            std::end(word),
            std::back_inserter(output),
            [](auto c) { return c; }
          );
      };

    size_type counter = 0;

    for (const auto& column : header_) {
        append(column, output);

        counter++;
        if (counter == header_.size()) {
            append(newline_, output);
            counter = 0;
        } else {
            append(delim_, output);
        }
    }

    for (const auto& data : rows_) {
        append(data, output);

        counter++;
        if (counter == header_.size()) {
            append(newline_, output);
            counter = 0;
        } else {
            append(delim_, output);
        }
    }
}

void read_csv(
  struct dyn_driver* self,
  byte_t*            input,
  size_t             input_size,
  void*              output
)
{
    auto csv_      = static_cast<csv*>(output);
    char delim     = ',';
    auto delim_opt = self->option(self, "delimiter");
    if (delim_opt != nullptr) {
        delim = (char)((uintptr_t)delim_opt->value);
    }

    std::span<byte_t> input_span{ input, input_size };
    csv_->read(input_span, delim);
    output = csv_;
}

void write_csv(
  struct dyn_driver* self,
  void*              input,
  byte_t*            output,
  size_t*            output_size
)
{
    auto csv_      = static_cast<csv*>(input);
    char delim     = ',';
    auto delim_opt = self->option(self, "delimiter");
    if (delim_opt != nullptr) {
        delim = (char)((uintptr_t)delim_opt->value);
    }

    std::vector<byte_t> output_;
    csv_->write(output_, delim);

    std::memcpy(output, output_.data(), output_.size());
    *output_size = output_.size();
}

dyn_driver_option* option_csv(struct dyn_driver* self, const char* name)
{
    if (std::string{ name } == "delimiter") {
        return static_cast<dyn_driver_option*>(self->internals);
    } else {
        return nullptr;
    }
}

} // namespace example

// Exports, !!REQUIRED!!
extern "C" void register_driver(dyn_driver** drv)
{
    auto opt = new dyn_driver_option{ dyn_driver_option_t::DRV_OPT_CHAR,
                                      "delimiter",
                                      (void*)',' };

    *drv     = new dyn_driver{ .name      = "csv",
                               .option    = example::option_csv,
                               .read      = example::read_csv,
                               .write     = example::write_csv,
                               .internals = opt };
}

extern "C" void deregister_driver(dyn_driver** drv)
{
    if (drv != nullptr) {
        if ((*drv)->internals != nullptr) {
            delete static_cast<dyn_driver_option*>((*drv)->internals);
            (*drv)->internals = nullptr;
        }

        delete *drv;
        *drv = nullptr;
    }
}
