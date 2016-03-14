// lock_ios.h - Per-stream synchronization.
// Copyright 2016 by David Krauss.
// This source is released under the MIT license, http://opensource.org/licenses/MIT

#ifndef INCLUDED_S6_LOCK_IOS_H
#define INCLUDED_S6_LOCK_IOS_H

#include <iosfwd>
#include <mutex>

namespace s6_lock_ios {

namespace impl {
class manip;
class temporary_lock;
}

typedef std::unique_lock< std::recursive_mutex > ios_lock;

// I/O manipulator to lock a given stream to a given, scoped object.
inline impl::manip lock_ios( ios_lock & l );

// I/O manipulator to lock a given stream until the next semicolon.
inline impl::manip lock_ios( impl::temporary_lock && l /* = {} */ );


namespace impl {

class manip {
    friend manip s6_lock_ios::lock_ios( ios_lock & );
    friend manip s6_lock_ios::lock_ios( temporary_lock && );
    
    ios_lock &l;
    
    manip( ios_lock & in_l ) : l( in_l ) {}
    
    void acquire( std::ios_base & ) const;

    template< typename type, typename traits >
    friend std::basic_istream< type, traits > & operator >> ( std::basic_istream< type, traits > & s, manip const & m ) {
        m.acquire( s );
        return s;
    }
    
    template< typename type, typename traits >
    friend std::basic_ostream< type, traits > & operator << ( std::basic_ostream< type, traits > & s, manip const & m ) {
        m.acquire( s );
        return s;
    }
};

class temporary_lock {
    friend manip s6_lock_ios::lock_ios( temporary_lock && );
    
    ios_lock l;
    
    temporary_lock() = default;
};

}

inline impl::manip lock_ios( ios_lock & l )
    { return { l }; }

inline impl::manip lock_ios( impl::temporary_lock && l = {} )
    { return { l.l }; }

}

#endif
