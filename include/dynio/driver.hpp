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

#define DRIVER_EXPORT __attribute__((visibility("default")))

#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include "driver.h"

namespace dynio {

struct dynamic
{};

template<typename Tp>
struct driver;

template<>
struct driver<dynamic>
{
    using type      = dynamic;
    using size_type = std::size_t;
    using pointer   = std::unique_ptr<dyn_driver, dyn_driver_deregistration>;

    driver() = delete;

    driver(dyn_driver* drv, dyn_driver_deregistration dtor, std::shared_ptr<void> handle)
      : instance_(drv, dtor)
      , handle_(handle){};

    driver(dyn_driver_registration ctor, dyn_driver_deregistration dtor, std::shared_ptr<void> handle)
      : instance_(nullptr, nullptr)
    {
        dyn_driver* ptr = nullptr;
        std::cout << "Allocating\n";
        ctor(ptr);

        std::cout << "Passing\n";
        driver(ptr, dtor, handle);
    };

    driver(pointer&& ptr, std::shared_ptr<void> handle)
      : instance_(std::move(ptr))
      , handle_(handle){};

    template<typename Model>
    driver<Model> cast() noexcept {
        driver<Model> drv{std::move(instance_), handle_};
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
        Tp output;
        instance_->read(instance_.get(), input.data(), input.size(), &output);
        return output;
    }

    template<typename Tp>
    std::vector<byte_t> write(Tp& input)
    {
        byte_t* output = nullptr;
        size_type size;
        instance_->write(instance_.get(), &input, output, &size);
        return {output, output + size};
    }

  private:
    pointer               instance_;
    std::shared_ptr<void> handle_;
};

template<>
struct driver<void>
{ using type = void; };

template<typename Tp>
struct driver
  : private driver<dynamic>
{
    using type      = Tp;
    using base_type = driver<dynamic>;
    using size_type = base_type::size_type;
    using pointer   = base_type::pointer;

    driver() = delete;

    using base_type::driver;
    using base_type::name;
    using base_type::option;

    Tp read(std::vector<byte_t>& input)
    { return base_type::read<Tp>(input); }

    std::vector<byte_t> write(Tp& input)
    { return base_type::write<Tp>(input); }
};

} // namespace dynio

#endif // DYNIO_DRIVER_HPP
