# dynio: Dynamic I/O Drivers

## Usage

This library exposes a C header, `driver.h` for both producers and consumers.

### Producers (aka driver implementors)

At a minimum, driver implementations require:

1. The registration function, which allocates and assigns your new
   driver to passed pointer:
   ```c
   extern "C" void register_driver(dyn_driver**);
   ```

2. The deregistration function, which deallocates your created driver:
   ```c
   extern "C" void deregister_driver(dyn_driver**);
   ```

3. An implementation of your driver providing **3 functions**,
   - `dyn_driver::option`, which is a pointer to a function that
     returns a `dyn_driver_option`.
   - `dyn_driver::read`, which performs the reading portion of your driver.
   - `dyn_driver::write`, which performs the writing portion of your driver.

4. The source file containing the above 3 requirements to be compilable as a shared library. Drivers in dynio are loaded dynamically via `dlopen`.

Any internal state or functionality for your driver can be stored in the `dyn_driver::internals` void pointer.

See `test/csv/csv.{cpp/hpp}` for a producer view of how `example::csv` is implemented.

### Consumers (aka driver users)

Users of driver implementations are intended to be C++ users, and should use
`driver.hpp` (the C++ header), which provides a C++ abstraction of `dyn_driver`
and the template function `load_driver`, which handles loading from a dynio-based
shared-library driver.

See `test/csv_test.cpp` for a consumer view of how `example::csv` is loaded.

## License

Distributed under the [Boost Software License, Version 1.0](LICENSE).
