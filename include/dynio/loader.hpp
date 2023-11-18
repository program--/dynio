// dynio: Dynamic I/O Drivers
//
// Copyright (c) 2023 Justin Singh-Mohudpur <justin@justinsingh.me>
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <system_error>
#ifndef DYNIO_LOADER_HPP
#define DYNIO_LOADER_HPP

#include "driver.hpp"

#include <dlfcn.h>
#include <fstream>

namespace dynio {

template<typename Tp = dynamic>
driver<Tp> load_driver(const std::string& path)
{
    if (!std::ifstream(path).good())
        throw std::runtime_error("failed to load library `" + path + "` (error: file not accessible/found)");

    char* err;
    std::cout << "Loading library\n";
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if ((err = dlerror()) != nullptr || handle == nullptr)
        throw std::runtime_error("failed to load library `" + path + "` (error: " + err + ")");

    std::cout << "Loading register\n";
    auto drv_register_fn = (dyn_driver_registration*)dlsym(handle, "register_driver");
    if ((err = dlerror()) != nullptr || drv_register_fn == nullptr)
        throw std::runtime_error("failed to load registration function from `" + path + "` (error: " + err + ")");

    std::cout << "Loading deregister\n";
    auto drv_deregister_fn = (dyn_driver_deregistration*) dlsym(handle, "deregister_driver");
    if ((err = dlerror()) != nullptr || drv_deregister_fn == nullptr)
        throw std::runtime_error("failed to load deregistration function from `" + path + "` (error: " + err + ")");

    std::cout << "Loaded\n";
    return {
        *drv_register_fn,
        *drv_deregister_fn,
        std::shared_ptr<void>(handle, dlclose)
    };
}

} // namespace dynio

#endif // DYNIO_LOADER_HPP
