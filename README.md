`lock_ios`
==========

An iostreams manipulator to provide thread safety to individual I/O streams.

*by David Krauss (potatoswatter)*
<!-- language: lang-cxx -->

Usage
=====

    #include "lock_ios.h"
    
    using s6_lock_ios::lock_ios;
    
    void print_one_thing() {
        std::cout << lock_ios() << "One thing.\n";
        // cout is unlocked at the end of the statement. (The lock is a temporary object.)
    }
    
    void print_two_things() {
        s6_lock_ios::ios_lock lock; // Same type as std::unique_lock< std::recursive_mutex >.
        
        std::cout << lock_ios( lock ) << "The first thing.\n";
        std::cout << The second thing.\n";
    } // cout is unlocked when lock is destroyed or lock.unlock() is called.

There is one mutex per stream. It is not a global lock. It is dynamically allocated.

The mutex is "recursive," i.e. the critical sections may be nested,
or a function may lock a stream which a calling function has already locked.

Input streams are supported too.


## Caveats

Stream buffers are not locked individually. For example, both `std::cerr` and `std::clog`
may be locked at the same time by different threads, and they will race with each other.
(However, this is not typically true of `cerr` and `cout`, though they might funnel into
the same file once the data leaves the process.)

Be careful when locking several streams at the same time.
The locks should be acquired in a consistent order, or a deadlock could arise.

A named lock object only locks a single stream. Inserting `lock_ios(lock)` to a second
stream, when `lock` is already locking something, will first unlock `lock`.

Do not destroy a stream while it is locked.


Feedback
========

Please report issues [on GitHub](/potswa/lock_ios/issues).

For other feedback, my email address may also be found [there](/potswa).
