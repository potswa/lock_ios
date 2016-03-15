`lock_ios`
==========

An iostreams manipulator to provide thread safety to individual I/O streams.

*by David Krauss (potatoswatter)*
<!-- language: lang-cxx -->

Usage
=====

    #include "lock_ios.h"
    
    using namespace s6_lock_ios;
    
    int main() {
        std::cout << mutex_init; // Or << mutex_init_own.
        
        std::thread t( print_one_thing );
        print_two_things();
    }
    
    void print_one_thing() {
        std::cout << lock_ios() << "One thing.\n";
        // cout is unlocked at the end of the statement. (The lock is a temporary object.)
    }
    
    void print_two_things() {
        ios_lock lock; // Same type as std::unique_lock< std::recursive_mutex >.
        
        std::cout << lock_ios( lock ) << "The first thing.\n";
        std::cout << The second thing.\n";
    } // cout is unlocked when lock is destroyed or lock.unlock() is called.

There is one mutex per stream, created by `mutex_init`. It is not a global lock.

The mutex is "recursive," so a function may lock a stream which is already locked
by a calling function.

Input streams are supported as well.
(For example, `getline( std::cin >> lock_ios() )` is reasonable usage.)

## Setup

Prepare a stream for sharing by applying either `mutex_init` or `mutex_init_own`.
This will create and attach an internal mutex.

`mutex_init` is safe for concurrency. It allows several contending threads to start sharing
the stream. This is achieved by a critical section.

`mutex_init_own` may be called by a thread before it starts sharing a stream with others.
Typically it is lock-free.

Some streams, such as `std::cerr` and `std::cin`, flush other streams via `std::ios::tie()`.
These setup routines clear `tie` associations.

Unlike most manipulators, these require that the stream is in a good state (per `!s.bad()`).
Upon failure, they set `badbit` and throw `std::ios_base::failure` (even if the stream has
not set `exceptions`).

These setup routines do not lock anything. `lock_ios` must be called subsequently to do so.

## I/O

Always lock a shared stream before any access. It's not just that output from racing
insertions appears mixed or interleaved. Any concurrent accesses incur data races, which
(aside from the four standard streams under `sync_with_stdio`) carry undefined behavior.

Unlike most manipulators, `lock_ios` works regardless of the stream's state.
As with any access, locking is necessary before checking `badbit` or `failbit`.

If `lock_ios` is attempted when no mutex has been initialized, a `system_error` is thrown
as if a `std::unique_lock` was locked without a mutex. (`badbit` is not set.)

## Limitations

This library requires the `ios_base::pword` function to be thread-safe, which is only
realistic as long as it does not allocate memory. User-defined manipulators, such as this
library, tend to cause it to allocate memory. To be safe, either avoid other such formatting
libraries, or ensure that each thread has been suitably exposed to the library before
concurrent use.

Note that such libraries using `pword` are inherently unsafe for concurrency. It is not
an incompatibility with this library, in particular.

## Caveats

Stream buffers are not locked individually. For example, both `std::cerr` and `std::clog`
may be locked at the same time by different threads, and they will race with each other.
(However, this is not typically true of `cerr` and `cout`, though they might funnel into
the same file once the data leaves the process.)

A lock operation is unsequenced with respect to evaluation of its statement's subexpressions.
Avoid interactions between the lock and the items after the `<<` or `>>` operator.

Be careful when locking several streams at the same time.
The locks should be acquired in a consistent order, or a deadlock could arise.

A named lock object only locks a single stream. Inserting `lock_ios(lock)` to a second
stream, when `lock` is already locking something, will first unlock `lock`.

Do not destroy a stream while it is locked.


Feedback
========

Please report issues [on GitHub](https://github.com/potswa/lock_ios/issues).

For other feedback, my email address may also be found [there](https://github.com/potswa).
