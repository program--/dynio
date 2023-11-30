#include <dynio/loader.hpp>
#include "csv/csv.hpp"

#include <iostream>
#include <cassert>

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        std::cout << "Number of args: " << argc << '\n';
        for (int i = 0; i < argc; i++) {
            std::cout << "(" << i << ") " << argv[i] << '\n';
        }
        std::cout << std::endl;
        assert(false);
    }

    std::cout << "Loading CSV driver\n";
    dynio::driver<example::csv> drv = dynio::load_driver<example::csv>(argv[1]);

    std::cout << "Setting delimiter option to ':'\n";
    assert(drv.option("delimiter") != nullptr);
    drv.option("delimiter")->value = (void*)':';

    // Begin test
    std::string         example_csv_file = "COL1:COL2:COL3\n1:2:a\n3:4:b";
    std::vector<byte_t> bytes{ example_csv_file.begin(),
                               example_csv_file.end() };

    std::cout << "Driver name: " << drv.name() << "\n";
    assert(std::string{ drv.name() } == "csv");

    std::cout << "Reading in-memory CSV file\n";
    example::csv parsed = drv.read(bytes);

    std::cout << "Getting header\n";
    decltype(auto) header = parsed.header();

    assert(header[0] == "COL1");
    assert(header[1] == "COL2");
    assert(header[2] == "COL3");

    std::cout << "Checking row 0\n";
    auto row1 = parsed.row(0);
    assert(row1[0] == "1");
    assert(row1[1] == "2");
    assert(row1[2] == "a");

    std::cout << "Checking row 1\n";
    auto row2 = parsed.row(1);
    assert(row2[0] == "3");
    assert(row2[1] == "4");
    assert(row2[2] == "b");

    std::cout << "Parsed CSV:\n"
              << header[0] << ", " << header[1] << ", " << header[2] << "\n"
              << row1[0] << ", " << row1[1] << ", " << row1[2] << "\n"
              << row2[0] << ", " << row2[1] << ", " << row2[2] << "\n";

    std::cout << "Finished" << std::endl;

    return EXIT_SUCCESS;
}
