add_subdirectory(csv)

add_executable(csv_test_runner csv_test.cpp)
target_link_libraries(csv_test_runner PRIVATE dynio driver_csv)

add_test(
    NAME csv_test
    COMMAND csv_test_runner $<TARGET_FILE:driver_csv>
)
