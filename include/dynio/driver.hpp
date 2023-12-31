// dynio: Dynamic I/O Drivers
//
// Copyright (c) 2023 Justin Singh-Mohudpur <justin@justinsingh.me>
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef DYNIO_DRIVER_HPP
#define DYNIO_DRIVER_HPP

#include <memory>
#include <string>
#include <vector>

#include <dlfcn.h>

#include "driver.h"

namespace dynio {

struct dynamic
{};

template<typename Tp>
struct driver;

template<>
struct driver<dynamic>
{
    using type                       = dynamic;
    using size_type                  = std::size_t;
    using pointer                    = std::shared_ptr<dyn_driver>;

    driver()                         = delete;
    driver(const driver&)            = default;
    driver& operator=(const driver&) = default;
    driver(driver&& drv)             = default;
    driver& operator=(driver&& drv)  = default;

    driver(
      dyn_driver*               drv,
      dyn_driver_deregistration dtor,
      std::shared_ptr<void>     handle
    )
      : instance_(drv, deleter{ dtor })
      , handle_(handle){};

    driver(
      dyn_driver_registration   ctor,
      dyn_driver_deregistration dtor,
      std::shared_ptr<void>     handle
    )
      : instance_(nullptr)
    {
        dyn_driver* ptr = nullptr;
        ctor(&ptr);
        instance_ = { ptr, deleter{ dtor } };
        handle_   = handle;
    };

    driver(pointer ptr, std::shared_ptr<void> handle)
      : instance_(ptr)
      , handle_(handle){};

    template<typename Model>
    driver<Model> cast() noexcept
    {
        driver<Model> drv{ std::move(instance_), handle_ };
        handle_ = nullptr;
        return drv;
    }

    const char* name() const noexcept
    {
        return instance_->name;
    }

    dyn_driver_option* option(const std::string& name) noexcept
    {
        return instance_->option(instance_.get(), name.c_str());
    }

    const dyn_driver_option* option(const std::string& name) const noexcept
    {
        return instance_->option(instance_.get(), name.c_str());
    }

    template<typename Tp>
    Tp read(std::vector<byte_t>& input)
    {
        return *static_cast<Tp*>(read(input));
    }

    void* read(std::vector<byte_t>& input)
    {
        void* output = nullptr;
        instance_->read(instance_.get(), input.data(), input.size(), &output);
        return output;
    }

    template<typename Tp>
    std::vector<byte_t> write(Tp& input)
    {
        return write(&input);
    }

    std::vector<byte_t> write(void* input)
    {
        byte_t*   output = nullptr;
        size_type size;
        instance_->write(instance_.get(), input, &output, &size);
        return { output, output + size };
    }

  private:
    struct deleter
    {
        deleter(dyn_driver_deregistration dtor)
          : dtor_(dtor){};

        deleter() = default;

        void operator()(dyn_driver* d)
        {
            if (dtor_ && d)
                dtor_(&d);
        }

      private:
        dyn_driver_deregistration dtor_ = nullptr;
    };

    pointer               instance_;
    std::shared_ptr<void> handle_;
};

template<>
struct driver<void>
{
    using type = void;
};

template<typename Tp>
struct driver : private driver<dynamic>
{
    using type      = Tp;
    using base_type = driver<dynamic>;
    using size_type = base_type::size_type;
    using pointer   = base_type::pointer;

    driver()        = delete;

    using base_type::driver;
    using base_type::operator=;
    using base_type::name;
    using base_type::option;

    Tp read(std::vector<byte_t>& input)
    {
        return base_type::read<Tp>(input);
    }

    std::vector<byte_t> write(Tp& input)
    {
        return base_type::write<Tp>(input);
    }
};

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

#endif // DYNIO_DRIVER_HPP
