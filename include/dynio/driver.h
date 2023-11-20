// dynio: Dynamic I/O Drivers
//
// Copyright (c) 2023 Justin Singh-Mohudpur <justin@justinsingh.me>
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef DYNIO_DRIVER_H
#define DYNIO_DRIVER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Driver byte type
typedef uint8_t byte_t;

//! Driver Option Type
//!
//! An option can be one of:
//! - integer
//! - double
//! - boolean
//! - character
//! - string
//! - custom
//!
//! @note Custom options are implementation-defined.
//!       Thus, it is up to the implementor to clearly
//!       define the requirements for a custom option.
typedef enum dyn_driver_option_t
{
    DRV_OPT_INT,
    DRV_OPT_DOUBLE,
    DRV_OPT_BOOL,
    DRV_OPT_CHAR,
    DRV_OPT_STRING,
    DRV_OPT_CUSTOM
} dyn_driver_option_t;

//! Driver option
//!
//! A discriminated union-like struct defining an option for a driver.
typedef struct dyn_driver_option
{
    const dyn_driver_option_t type;
    const char*               name;
    void*                     value;
} dyn_driver_option;

typedef struct dyn_driver
{
    //! Name of this driver
    const char* name;

    //! driver::option - Get/Set an option for this driver.
    //!
    //! @param[in] self This instance of %driver.
    //! @param[in] option_name Name of the option to retrieve.
    //! @return driver_option*, a struct representing the option.
    dyn_driver_option* (*option)(
      struct dyn_driver* self,
      const char*        option_name
    );

    //! driver::read - Reads fom bytes to an output format.
    //!
    //! @param[in] self This instance of %driver.
    //! @param[in] input Bytes buffer to read from.
    //! @param[in] input_size Size of the given buffer.
    //! @param[out] output Output format.
    void (*read)(
      struct dyn_driver* self,
      byte_t*            input,
      size_t             input_size,
      void*              output
    );

    //! driver::write - Writes an input format to bytes.
    //!
    //! @param[in] self This instance of %driver.
    //! @param[in] input Input format.
    //! @param[out] output Bytes buffer to write to.
    //! @param[out] output_size Pointer to write size of buffer to.
    void (*write)(
      struct dyn_driver* self,
      void*              input,
      byte_t*            output,
      size_t*            output_size
    );

    //! User-internals for this driver
    void* internals;
} dyn_driver;

//! Driver registration alias
typedef void (*dyn_driver_registration)(dyn_driver** drv);

//! Driver registration function
void register_driver(dyn_driver**);
// extern dyn_driver_registration register_driver;

//! Driver deregistration alias
typedef void (*dyn_driver_deregistration)(dyn_driver** drv);

//! Driver deregistration function
void deregister_driver(dyn_driver**);
// extern dyn_driver_deregistration deregister_driver;

#ifdef __cplusplus
}
#endif
#endif // DYNIO_DRIVER_H
