//  MIT License
//
//  Copyright (c) 2020, The Regents of the University of California,
// through Lawrence Berkeley National Laboratory (subject to receipt of any
// required approvals from the U.S. Dept. of Energy).  All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

/** \file manager.cpp
 * This file defines the extern init for manager
 *
 */

#include "timemory/manager.hpp"
#include "timemory/timemory.hpp"

#if defined(TIMEMORY_EXTERN_INIT)

//======================================================================================//
#    if !defined(_WINDOWS)

using manager_pointer_t = std::shared_ptr<tim::manager>;
namespace
{
static manager_pointer_t timemory_master_manager_instance =
    tim::manager::master_instance();
}
#    endif
//======================================================================================//

::tim::manager*
timemory_manager_master_instance()
{
    using manager_t     = tim::manager;
    static auto& _pinst = tim::get_shared_ptr_pair<manager_t>();
    return _pinst.first.get();
}

extern "C"
{
    __library_ctor__ void timemory_library_constructor()
    {
        auto library_ctor = tim::get_env<bool>("TIMEMORY_LIBRARY_CTOR", true);
        if(!library_ctor)
            return;

#    if defined(DEBUG)
        auto _debug   = tim::settings::debug();
        auto _verbose = tim::settings::verbose();

        if(_debug || _verbose > 3)
            printf("[%s]> initializing manager...\n", __FUNCTION__);
#    endif

        static auto              _master = tim::manager::master_instance();
        static thread_local auto _worker = tim::manager::instance();

        if(!_master)
            _master = tim::manager::master_instance();

        if(_worker != _master)
            printf("[%s]> tim::manager :: master != worker : %p vs. %p\n", __FUNCTION__,
                   (void*) _master.get(), (void*) _worker.get());

#    if defined(DEBUG)
        if(_debug || _verbose > 3)
            printf("[%s]> initializing storage...\n", __FUNCTION__);
#    endif

        std::atexit(tim::timemory_finalize);
        // initialize storage
        tim::settings::initialize_storage();
        // using tuple_type = tim::available_tuple<tim::complete_tuple_t>;
        // tim::manager::get_storage<tuple_type>::initialize(_master);
    }

    __library_dtor__ static void timemory_library_destructor()
    {
        /*
        auto library_dtor = tim::get_env<bool>("TIMEMORY_LIBRARY_DTOR", true);
        if(!library_dtor)
            return;

        auto _master = timemory_manager_master_instance();
        if(_master)
            _master->finalize();
        */
    }
}

//======================================================================================//

namespace tim
{
//======================================================================================//

std::atomic<int32_t>&
manager::f_manager_instance_count()
{
    static std::atomic<int32_t> _instance(0);
    return _instance;
}

//======================================================================================//
// number of threads counter
//
std::atomic<int32_t>&
manager::f_thread_counter()
{
    static std::atomic<int32_t> _instance(0);
    return _instance;
}

//======================================================================================//
// get either master or thread-local instance
//
manager::pointer_t
manager::instance()
{
    static thread_local auto _inst = get_shared_ptr_pair_instance<manager>();
    return _inst;
}

//======================================================================================//
// get master instance
//
manager::pointer_t
manager::master_instance()
{
    static auto _pinst = get_shared_ptr_pair_master_instance<manager>();
    return _pinst;
}

}  // namespace tim

#endif  // defined(TIMEMORY_EXTERN_INIT)
