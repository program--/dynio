#include <dynio/driver.hpp>

#include <span>

namespace example {

struct csv
{
    using size_type = std::size_t;

    const std::vector<std::string>&    header() const noexcept;
    const std::span<const std::string> row(size_type n) const;

    //
    friend void read_csv(
      struct dyn_driver* self,
      byte_t*            input,
      size_t             input_size,
      void*              output
    );

    friend void write_csv(
      struct dyn_driver* self,
      void*              input,
      byte_t*            output,
      size_t*            output_size
    );

  private:
    void read(std::istream& stream, char delim = ',');
    void read(std::span<byte_t> input, char delim = ',');
    void write(std::vector<byte_t>& output, char delim = ',');

    std::vector<std::string> header_;
    std::vector<std::string> rows_;
};

} // namespace example
