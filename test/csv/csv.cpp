#include "csv.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

namespace example {

auto csv::header() const noexcept
  -> const std::vector<std::string>&
{
    return header_;
}

auto csv::row(size_type n) const
  -> const std::span<const std::string>
{
    return { &rows_.at(n * header_.size()), header_.size() };
}

void csv::read(std::istream& stream, char delim)
{
    // Read header
    std::string header;
    std::getline(stream, header);
    std::stringstream header_stream{header};
    for (std::string column; std::getline(header_stream, column, delim);) {
        header_.emplace_back(std::move(column));
    }

    // Read contents
    for (std::string row; std::getline(stream, row);) {
        std::stringstream row_stream{row};
        for (std::string column; std::getline(row_stream, column, delim);) {
            rows_.emplace_back(std::move(column));
        }
    }
};

void csv::read(std::span<byte_t> input, char delim)
{
    std::istringstream stream{std::string{input.begin(), input.end()}};
    read(stream, delim);
}

void csv::write(std::vector<byte_t>& output, char delim)
{

    const std::string delim_ = { delim };
    const std::string newline_ = "\n";

    const auto append = [](const std::string& word, std::vector<byte_t>& output) {
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

void read_csv(struct dyn_driver* self, byte_t* input, size_t input_size, void* output)
{
    auto csv_ = new csv{};

    char delim = ',';
    auto delim_opt = self->option(self, "delimiter");
    if (delim_opt != nullptr) {
        delim = ((char*)delim_opt->value)[0];
    }

    std::span<byte_t> input_span {input, input_size};
    csv_->read(input_span, delim);
    output = csv_;
}

void write_csv(struct dyn_driver* self, void* input, byte_t* output, size_t* output_size)
{
    auto csv_ = static_cast<csv*>(input);

    char delim = ',';
    auto delim_opt = self->option(self, "delimiter");
    if (delim_opt != nullptr) {
        delim = ((char*)delim_opt->value)[0];
    }

    std::vector<byte_t> output_;
    csv_->write(output_, delim);

    std::memcpy(output, output_.data(), output_.size());
    *output_size = output_.size();
}

dyn_driver_option* option_csv(struct dyn_driver* self, const char* name)
{
    if (std::string{name} == "delimiter") {
        return static_cast<dyn_driver_option*>(self->internals);
    } else {
        return nullptr;
    }
}

void register_csv(dyn_driver* drv)
{
    std::cout << "Allocating option\n";
    auto opt = new dyn_driver_option{
        dyn_driver_option_t::DRV_OPT_CHAR,
        "delimiter",
        (void*)','
    };

    std::cout << "Allocating driver\n";
    drv = new dyn_driver{
        .name      = "csv",
        .option    = option_csv,
        .read      = read_csv,
        .write     = write_csv,
        .internals = opt
    };
}

void deregister_csv(dyn_driver* drv)
{
    std::cout << "Deregistering\n";

    if (drv != nullptr) {
        if (drv->internals != nullptr) {
            delete static_cast<dyn_driver_option*>(drv->internals);
            drv->internals = nullptr;
        }

        delete drv;
    }
}

}

// Exports, !!REQUIRED!!
extern "C" void register_driver(dyn_driver* drv)
{
    example::register_csv(drv);
}

extern "C" void deregister_driver(dyn_driver* drv)
{
    example::deregister_csv(drv);
}
