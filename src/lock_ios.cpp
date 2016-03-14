// lock_ios.cpp - Per-stream synchronization.
// Copyright 2016 by David Krauss.
// This source is released under the MIT license, http://opensource.org/licenses/MIT

#include "lock_ios.h"

#include <ios>

namespace s6_lock_ios {
namespace impl {

void manip::acquire( std::ios_base & s ) const {
    static int const ios_index = std::ios_base::xalloc();
    void *& ptr = s.pword( ios_index );
    if ( ! ptr ) {
        s.register_callback( + []( std::ios_base::event e, std::ios_base & s, int ) {
            void *& ptr = s.pword( ios_index );
            if ( e == std::ios_base::erase_event ) {
                // When the stream is deleted, destroy the mutex.
                delete static_cast< std::recursive_mutex * >( ptr );
                ptr = nullptr; // Do not double delete after copyfmt.
                
            } else if ( e == std::ios_base::copyfmt_event ) {
                ptr = nullptr; // Do not copy a mutex.
                // Note, copyfmt does not copy rdbuf or otherwise promote races.
            }
        }, 0 );
        ptr = new std::recursive_mutex;
    }
    l = ios_lock{ * static_cast< std::recursive_mutex * >( ptr ) };
}

}
}
