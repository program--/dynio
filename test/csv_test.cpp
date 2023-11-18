#include <dynio/loader.hpp>
#include "csv/csv.hpp"

#include <iostream>
#include <cassert>

int main(int argc, const char* argv[]) {
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

    // TODO:
    // std::cout << "Setting delimiter option\n";
    // assert(drv.option("delimiter") != nullptr);
    // drv.option("delimiter")->value = (void*)':';

    std::string example_csv_file = "COL1,COL2,COL3\n1,2,a\n3,4,b";
    std::vector<byte_t> bytes{example_csv_file.begin(), example_csv_file.end()};

    // FIXME: Segfault here
    std::cout << "Reading in-memory CSV file\n";
    auto parsed = drv.read(bytes);

    std::cout << "Getting header\n";
    decltype(auto) header = parsed.header();
    
    assert(header[0] == "COL1");
    assert(header[1] == "COL2");
    assert(header[2] == "COL3");

    std::cout << "Checking row 0\n";
    auto row = parsed.row(0);
    assert(row[0] == "1");
    assert(row[1] == "2");
    assert(row[2] == "a");

    std::cout << "Checking row 1\n";
    row = parsed.row(1);
    assert(row[0] == "3");
    assert(row[1] == "4");
    assert(row[2] == "b");

    std::cout << "Finished" << std::endl;

    return EXIT_SUCCESS;
}
