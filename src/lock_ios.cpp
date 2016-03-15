// lock_ios.cpp - Per-stream synchronization.
// Copyright 2016 by David Krauss.
// This source is released under the MIT license, http://opensource.org/licenses/MIT

#include "lock_ios.h"

#include <cassert>
#include <ios>
#include <stdexcept>
#include <system_error>

namespace s6_lock_ios {

namespace {
int ios_index() {
    static int value = std::ios_base::xalloc();
    return value;
}

bool good( std::ios_base const & s )
#ifdef _LIBCPP_VERSION
    { return s.good(); }
#else
    { return true; }
#endif
}

std::ios_base & mutex_init_own( std::ios_base & s ) {
    assert ( good( s ) && "mutex_init_own called on bad stream." );
    
    void *& ptr = s.pword( ios_index() );
    if ( ptr ) return s;
    
    s.register_callback( + []( std::ios_base::event e, std::ios_base & s, int ) {
        void *& ptr = s.pword( ios_index() );
        if ( e == std::ios_base::erase_event ) {
            // When the stream is terminated, destroy the mutex.
            delete static_cast< std::recursive_mutex * >( ptr );
            ptr = nullptr; // Do not double delete after copyfmt.
            
        } else if ( e == std::ios_base::copyfmt_event ) {
            ptr = nullptr; // Do not copy mutex access.
            // Note, copyfmt does not copy rdbuf or otherwise promote races.
        }
    }, 0 );
    
#ifdef _LIBCPP_VERSION
    try {
        if ( ! good( s ) ) throw std::bad_alloc{}; // pword or register_callback failed.
        ptr = new std::recursive_mutex; // Throw std::bad_alloc or std::system_error.
    } catch (...) {
        s.setf( std::ios_base::badbit );
        throw;
    }
#else
    ptr = new std::recursive_mutex;
    // Can't detect pword or register_callback failure; can't set badbit.
#endif
    
    return s;
}

std::ios_base & mutex_init( std::ios_base & s ) {
    static std::mutex critical;
    std::lock_guard< std::mutex > guard( critical );
    
    if ( ! s.pword( ios_index() ) ) {
        mutex_init_own( s );
    }
    return s;
}

namespace impl {
void manip::acquire( std::ios_base & s ) const {
    /*  Accessing pword here assumes that the implementation modifies nothing while
        retrieving a preexisting entry. */
    if ( void * ptr = s.pword( ios_index() ) ) {
        l = ios_lock( * static_cast< std::recursive_mutex * >( ptr ) );
    } else {
        // Imitate error handling of unique_lock::lock.
        throw std::system_error( std::make_error_code( std::errc::operation_not_permitted ), "shared stream is missing a mutex" );
    }
}
}
}
