// dynio: Dynamic I/O Drivers
//
// Copyright (c) 2023 Justin Singh-Mohudpur <justin@justinsingh.me>
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef DYNIO_LOADER_HPP
#define DYNIO_LOADER_HPP

#include "driver.hpp"

#include <dlfcn.h>

namespace dynio {

struct dynio_load_error : public std::runtime_error
{
    dynio_load_error(const std::string& path)
      : std::runtime_error("failed to load library \"" + path + "\""){};

    dynio_load_error(const std::string& path, const std::string& errmsg)
      : std::runtime_error(
          "failed to load library \"" + path + "\" (error: " + errmsg + ")"
        ){};
};

struct dynio_symbol_error : public std::runtime_error
{
    dynio_symbol_error(const std::string& symbol, const std::string& path)
      : std::runtime_error(
          "failed to load symbol `" + symbol + "` from \"" + path + "\""
        ){};

    dynio_symbol_error(
      const std::string& symbol,
      const std::string& path,
      const std::string& errmsg
    )
      : std::runtime_error(
          "failed to load symbol `" + symbol + "` from \"" + path +
          "\" (error: " + errmsg + ")"
        ){};
};

template<typename Tp = dynamic>
driver<Tp> load_driver(const std::string& path)
{
    char* err;

    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if ((err = dlerror()) != nullptr || handle == nullptr)
        throw dynio_load_error(path, err == nullptr ? "unknown error" : err);

    auto drv_register_fn =
      (dyn_driver_registration)dlsym(handle, "register_driver");
    if ((err = dlerror()) != nullptr || drv_register_fn == nullptr)
        throw dynio_symbol_error(
          "register_driver", path, err == nullptr ? "unknown error" : err
        );

    auto drv_deregister_fn =
      (dyn_driver_deregistration)dlsym(handle, "deregister_driver");
    if ((err = dlerror()) != nullptr || drv_deregister_fn == nullptr)
        throw dynio_symbol_error(
          "deregister_driver", path, err == nullptr ? "unknown error" : err
        );

    return { drv_register_fn,
             drv_deregister_fn,
             std::shared_ptr<void>(handle, dlclose) };
}

} // namespace dynio

#endif // DYNIO_LOADER_HPP
