/*************************************************************************************/
/*      Copyright 2015 Barcelona Supercomputing Center                               */
/*                                                                                   */
/*      This file is part of the NANOS++ library.                                    */
/*                                                                                   */
/*      NANOS++ is free software: you can redistribute it and/or modify              */
/*      it under the terms of the GNU Lesser General Public License as published by  */
/*      the Free Software Foundation, either version 3 of the License, or            */
/*      (at your option) any later version.                                          */
/*                                                                                   */
/*      NANOS++ is distributed in the hope that it will be useful,                   */
/*      but WITHOUT ANY WARRANTY; without even the implied warranty of               */
/*      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                */
/*      GNU Lesser General Public License for more details.                          */
/*                                                                                   */
/*      You should have received a copy of the GNU Lesser General Public License     */
/*      along with NANOS++.  If not, see <http://www.gnu.org/licenses/>.             */
/*************************************************************************************/

#ifndef _NANOS_ATOMIC_DECL
#define _NANOS_ATOMIC_DECL

#include "compatibility.hpp"
#include "nanos-int.h"
#include <algorithm> // for min/max
#include <cstddef>

/* TODO: move to configure
#include <ext/atomicity.h>
#ifndef _GLIBCXX_ATOMIC_BUILTINS
#error "Atomic gcc builtins support is mandatory at this point"
#endif
*/

namespace nanos
{
   /** \brief Forwared declaration required by RecursiveLock */
   class BaseThread;
}

namespace nanos
{
   class BaseThread;
}


namespace nanos
{
   template<typename T>
   class Atomic
   {

      private:
#ifdef HAVE_NEW_GCC_ATOMIC_OPS
         T     _value;
#else
         volatile T     _value;
#endif

      public:
         // constructor
         Atomic () {}

         Atomic ( T init ) : _value( init ) {}

         // copy constructor
         Atomic ( const Atomic &atm ) : _value( atm._value ) {}

         // assignment operator
         Atomic & operator= ( const Atomic &atm );
         Atomic & operator= ( const T val );
         // destructor
         ~Atomic() {}

         T fetchAndAdd ( const T& val=1 );
         T addAndFetch ( const T& val=1 );
         T fetchAndSub ( const T& val=1 );
         T subAndFetch ( const T& val=1 );
         T value() const;

         //! pre-increment ++
         T operator++ ();
         T operator-- ();

         //! post-increment ++
         T operator++ ( int val );
         T operator-- ( int val );

         //! += operator
         T operator+= ( const T val );
         T operator+= ( const Atomic<T> &val );

         T operator-= ( const T val );
         T operator-= ( const Atomic<T> &val );

         //! equal operator
         bool operator== ( const Atomic<T> &val );
         bool operator!= ( const Atomic<T> &val );

         bool operator< (const Atomic<T> &val );
         bool operator> ( const Atomic<T> &val ) const;
         bool operator<= ( const Atomic<T> &val );
         bool operator>= ( const Atomic<T> &val );

         // other atomic operations

         //! compare and swap
         bool cswap ( const Atomic<T> &oldval, const Atomic<T> &newval );

#ifdef HAVE_NEW_GCC_ATOMIC_OPS
         // note that the caller becomes responsible of accessing the shared
         // storage in a non-racy way
         T& override ();
#else
         volatile T & override ();
#endif
   };

   void memoryFence ();

   template<typename T>
#ifdef HAVE_NEW_GCC_ATOMIC_OPS
   bool compareAndSwap( T *ptr, T oldval, T  newval );
#else
   bool compareAndSwap( volatile T *ptr, T oldval, T  newval );
#endif

   class Lock : public nanos_lock_t
   {
      private:
         typedef nanos_lock_state_t state_t;

         // disable copy constructor and assignment operator
         Lock( const Lock &lock );
         const Lock & operator= ( const Lock& );

      public:
         // constructor
         Lock( state_t init=NANOS_LOCK_FREE ) : nanos_lock_t( init ) {};

         // destructor
         ~Lock() {}

         void acquire ( void );
         void acquire_noinst ( void );
         bool tryAcquire ( void );
         void release ( void );

         state_t operator* () const;

         state_t getState () const;

         void operator++ ( int val );

         void operator-- ( int val );
   };

   class LockBlock
   {
     private:
       Lock & _lock;

       // disable copy-constructor
       explicit LockBlock ( const LockBlock & );

     public:
       LockBlock ( Lock & lock );
       ~LockBlock ( );

       void acquire();
       void release();
   };

   class LockBlock_noinst
   {
     private:
       Lock & _lock;

       // disable copy-constructor
       explicit LockBlock_noinst ( const LockBlock_noinst & );

     public:
       LockBlock_noinst ( Lock & lock );
       ~LockBlock_noinst ( );

       void acquire();
       void release();
   };

   class SyncLockBlock : public LockBlock
   {
     private:
       // disable copy-constructor
       explicit SyncLockBlock ( const SyncLockBlock & );

     public:
       SyncLockBlock ( Lock & lock );
       ~SyncLockBlock ( );
   };

   class RecursiveLock : public nanos_lock_t
   {

      private:
         BaseThread *_holderThread;
         std::size_t _recursionCount;
         
         typedef nanos_lock_state_t state_t;

         // disable copy constructor and assignment operator
         RecursiveLock( const RecursiveLock &lock );
         const RecursiveLock & operator= ( const RecursiveLock& );

      public:
         // constructor
         RecursiveLock( state_t init=NANOS_LOCK_FREE )
            : nanos_lock_t( init ), _holderThread( 0 ), _recursionCount( 0UL )
         {};

         // destructor
         ~RecursiveLock() {}

         void acquire ( void );
         bool tryAcquire ( void );
         void release ( void );

         state_t operator* () const;

         state_t getState () const;

         void operator++ ( int );

         void operator-- ( int );
   };

   class RecursiveLockBlock
   {
     private:
       RecursiveLock & _lock;

       // disable copy-constructor
       explicit RecursiveLockBlock ( const RecursiveLock & );

     public:
       RecursiveLockBlock ( RecursiveLock & lock );
       ~RecursiveLockBlock ( );

       void acquire();
       void release();
   };

   class SyncRecursiveLockBlock : public RecursiveLockBlock
   {
     private:
       // disable copy-constructor
       explicit SyncRecursiveLockBlock ( const SyncRecursiveLockBlock & );

     public:
       SyncRecursiveLockBlock ( RecursiveLock & lock );
       ~SyncRecursiveLockBlock ( );
   };

};

#endif
