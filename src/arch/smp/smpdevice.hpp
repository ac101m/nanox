/*************************************************************************************/
/*      Copyright 2009 Barcelona Supercomputing Center                               */
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

#ifndef _SMP_DEVICE_DECL
#define _SMP_DEVICE_DECL

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "workdescriptor_decl.hpp"
#include "processingelement_fwd.hpp"
#include "copydescriptor.hpp"
#include "system_decl.hpp"
#include "smptransferqueue.hpp"
#include "globalregt.hpp"
#ifdef HAVE_MEMKIND_H
#include <memkind.h>
#endif


namespace nanos {

SMPDevice::SMPDevice ( const char *n ) : Device ( n ), _transferQueue(), _allocatedBytes(0) {}
SMPDevice::SMPDevice ( const SMPDevice &arch ) : Device ( arch ), _transferQueue(), _allocatedBytes(0) {}

/*! \brief SMPDevice destructor
 */
SMPDevice::~SMPDevice() {};

#define SMP_CAPACITY (1024 * 1024 * 1024 * 12UL)

void *SMPDevice::memAllocate( std::size_t size, SeparateMemoryAddressSpace &mem, WD const *wd, unsigned int copyIdx ) {
   void *retAddr = NULL;
    if ( _allocatedBytes.value() + size <= SMP_CAPACITY ) {
#ifdef MEMKIND_SUPPORT
   retAddr = memkind_malloc(MEMKIND_HBW, size);
   //retAddr = malloc(size);
#else
   retAddr = malloc(size);
//
//   SimpleAllocator *sallocator = (SimpleAllocator *) mem.getSpecificData();
//   sallocator->lock();
//   retAddr = sallocator->allocate( size );
//   if ( retAddr != NULL ) {
//      bzero( retAddr, size );
//   }
//   sallocator->unlock();
#endif
   }
   if ( retAddr != NULL ) _allocatedBytes += size;
   return retAddr;
}

void SMPDevice::memFree( uint64_t addr, SeparateMemoryAddressSpace &mem ) {
#ifdef MEMKIND_SUPPORT
   memkind_free( MEMKIND_HBW, (void *) addr );
#else
   free( (void *)addr );
//   SimpleAllocator *sallocator = (SimpleAllocator *) mem.getSpecificData();
//   sallocator->lock();
//   sallocator->free( (void *) addr );
//   sallocator->unlock();
#endif
}

void SMPDevice::_canAllocate( SeparateMemoryAddressSpace &mem, std::size_t *sizes, unsigned int numChunks, std::size_t *remainingSizes ) {
   SimpleAllocator *sallocator = (SimpleAllocator *) mem.getSpecificData();
   sallocator->canAllocate( sizes, numChunks, remainingSizes );
}

std::size_t SMPDevice::getMemCapacity( SeparateMemoryAddressSpace &mem ) {
   return SMP_CAPACITY;
//   SimpleAllocator *sallocator = (SimpleAllocator *) mem.getSpecificData();
//   return sallocator->getCapacity();
}

void SMPDevice::_copyIn( uint64_t devAddr, uint64_t hostAddr, std::size_t len, SeparateMemoryAddressSpace &mem, DeviceOps *ops, WD const *wd, void *hostObject, reg_t hostRegionId ) {
   if ( sys.getSMPPlugin()->asyncTransfersEnabled() ) {
      _transferQueue.addTransfer( ops, ((char *) devAddr), ((char *) hostAddr), len, 1, 0, true );
   } else {
      ops->addOp();
      NANOS_INSTRUMENT ( static InstrumentationDictionary *ID = sys.getInstrumentation()->getInstrumentationDictionary(); )
      NANOS_INSTRUMENT ( static nanos_event_key_t key = ID->getEventKey("cache-copy-in"); )
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseOpenBurstEvent( key, (nanos_event_value_t) len ); )
      if (sys._watchAddr != NULL ) {
         if ((uint64_t )sys._watchAddr >= hostAddr && (uint64_t )sys._watchAddr < hostAddr + len) {
            // *myThread->_file << "WATCH update dev: value " << *((double *) sys._watchAddr )<< std::endl;
            char buff[256];
            snprintf( buff, 256, "WATCH update dev: value %a", *((double *) sys._watchAddr ) );
            *myThread->_file << buff << std::endl;
         }
      }
      ::memcpy( (void *) devAddr, (void *) hostAddr, len );
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseCloseBurstEvent( key, (nanos_event_value_t) 0 ); )
      ops->completeOp();
   }
}

void SMPDevice::_copyOut( uint64_t hostAddr, uint64_t devAddr, std::size_t len, SeparateMemoryAddressSpace &mem, DeviceOps *ops, WD const *wd, void *hostObject, reg_t hostRegionId ) {
   if ( sys.getSMPPlugin()->asyncTransfersEnabled() ) {
      _transferQueue.addTransfer( ops, ((char *) hostAddr), ((char *) devAddr), len, 1, 0, true );
   } else {
      ops->addOp();
      NANOS_INSTRUMENT ( static InstrumentationDictionary *ID = sys.getInstrumentation()->getInstrumentationDictionary(); )
      NANOS_INSTRUMENT ( static nanos_event_key_t key = ID->getEventKey("cache-copy-out"); )
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseOpenBurstEvent( key, (nanos_event_value_t) len ); )
      if (sys._watchAddr != NULL ) {
         if ((uint64_t )sys._watchAddr >= hostAddr && (uint64_t )sys._watchAddr < hostAddr + len) {
            char buff[256];
            snprintf( buff, 256, "WATCH update host: old value %a", *((double *) sys._watchAddr ) );
            *myThread->_file << buff << std::endl;
            //*myThread->_file << "WATCH update host: old value " << *((double *) sys._watchAddr )<< std::endl;
         }
      }
      ::memcpy( (void *) hostAddr, (void *) devAddr, len );
      if (sys._watchAddr != NULL ) {
         if ((uint64_t )sys._watchAddr >= hostAddr && (uint64_t )sys._watchAddr < hostAddr + len) {
            char buff[256];
            //*myThread->_file << "WATCH update host: new value " << *((double *) sys._watchAddr )<< std::endl;
            snprintf( buff, 256, "WATCH update host: new value %a", *((double *) sys._watchAddr ) );
            *myThread->_file << buff << std::endl;
         }
      }
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseCloseBurstEvent( key, (nanos_event_value_t) 0 ); )
      ops->completeOp();
   }
}

bool SMPDevice::_copyDevToDev( uint64_t devDestAddr, uint64_t devOrigAddr, std::size_t len, SeparateMemoryAddressSpace &memDest, SeparateMemoryAddressSpace &memorig, DeviceOps *ops, WD const *wd, void *hostObject, reg_t hostRegionId ) {
   if ( sys.getSMPPlugin()->asyncTransfersEnabled() ) {
      _transferQueue.addTransfer( ops, ((char *) devDestAddr), ((char *) devOrigAddr), len, 1, 0, true );
   } else {
      ops->addOp();
      NANOS_INSTRUMENT ( static InstrumentationDictionary *ID = sys.getInstrumentation()->getInstrumentationDictionary(); )
      NANOS_INSTRUMENT ( static nanos_event_key_t key = ID->getEventKey("cache-copy-in"); )
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseOpenBurstEvent( key, (nanos_event_value_t) len ); )
      if (sys._watchAddr != NULL ) {
         global_reg_t reg( hostRegionId, sys.getHostMemory().getRegionDirectoryKey( (uint64_t) hostObject ) );
         uint64_t target_addr = reg.getKeyFirstAddress();
         if ((uint64_t )sys._watchAddr >= target_addr && (uint64_t )sys._watchAddr < target_addr + reg.getBreadth() ) {
            uint64_t offset = (uint64_t) sys._watchAddr - target_addr;
            char buff[256];
            snprintf( buff, 256, "WATCH update dev from dev: old value [ %p ] %a set value [ from %p ] %a", (void *)(devDestAddr + offset), *((double *) (devDestAddr + offset) ), (void *)(devOrigAddr + offset) , *((double *) (devOrigAddr + offset) ) );
            //*myThread->_file << "WATCH update dev from dev: old value [ " << (void *)(devDestAddr + offset) << " ] " << *((double *) (devDestAddr + offset) ) << " set value [from " << (void *)(devOrigAddr + offset) << " ] " << *((double *) (devOrigAddr + offset) )<< std::endl;
            *myThread->_file << buff << std::endl;
         }
      }
      ::memcpy( (void *) devDestAddr, (void *) devOrigAddr, len );
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseCloseBurstEvent( key, (nanos_event_value_t) 0 ); )
      ops->completeOp();
   }
   return true;
}

void SMPDevice::_copyInStrided1D( uint64_t devAddr, uint64_t hostAddr, std::size_t len, std::size_t numChunks, std::size_t ld, SeparateMemoryAddressSpace &mem, DeviceOps *ops, WD const *wd, void *hostObject, reg_t hostRegionId ) {
   if ( sys.getSMPPlugin()->asyncTransfersEnabled() ) {
      _transferQueue.addTransfer( ops, ((char *) devAddr), ((char *) hostAddr), len, numChunks, ld, true );
   } else {
      ops->addOp();
      NANOS_INSTRUMENT ( static InstrumentationDictionary *ID = sys.getInstrumentation()->getInstrumentationDictionary(); )
      NANOS_INSTRUMENT ( static nanos_event_key_t key = ID->getEventKey("cache-copy-in"); )
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseOpenBurstEvent( key, (nanos_event_value_t) 2 ); )
      for ( std::size_t count = 0; count < numChunks; count += 1) {
         ::memcpy( ((char *) devAddr) + count * ld, ((char *) hostAddr) + count * ld, len );
      }
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseCloseBurstEvent( key, (nanos_event_value_t) 0 ); )
      ops->completeOp();
   }
}

void SMPDevice::_copyOutStrided1D( uint64_t hostAddr, uint64_t devAddr, std::size_t len, std::size_t numChunks, std::size_t ld, SeparateMemoryAddressSpace &mem, DeviceOps *ops, WD const *wd, void *hostObject, reg_t hostRegionId ) {
   if ( sys.getSMPPlugin()->asyncTransfersEnabled() ) {
      _transferQueue.addTransfer( ops, ((char *) hostAddr), ((char *) devAddr), len, numChunks, ld, false );
   } else {
      ops->addOp();
      NANOS_INSTRUMENT ( static InstrumentationDictionary *ID = sys.getInstrumentation()->getInstrumentationDictionary(); )
      NANOS_INSTRUMENT ( static nanos_event_key_t key = ID->getEventKey("cache-copy-out"); )
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseOpenBurstEvent( key, (nanos_event_value_t) 2 ); )
      for ( std::size_t count = 0; count < numChunks; count += 1) {
         ::memcpy( ((char *) hostAddr) + count * ld, ((char *) devAddr) + count * ld, len );
      }
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseCloseBurstEvent( key, (nanos_event_value_t) 0 ); )
      ops->completeOp();
   }
}

bool SMPDevice::_copyDevToDevStrided1D( uint64_t devDestAddr, uint64_t devOrigAddr, std::size_t len, std::size_t numChunks, std::size_t ld, SeparateMemoryAddressSpace &memDest, SeparateMemoryAddressSpace &memOrig, DeviceOps *ops, WD const *wd, void *hostObject, reg_t hostRegionId ) {
   if ( sys.getSMPPlugin()->asyncTransfersEnabled() ) {
      _transferQueue.addTransfer( ops, ((char *) devDestAddr), ((char *) devOrigAddr), len, numChunks, ld, true );
   } else {
      ops->addOp();
      NANOS_INSTRUMENT ( static InstrumentationDictionary *ID = sys.getInstrumentation()->getInstrumentationDictionary(); )
      NANOS_INSTRUMENT ( static nanos_event_key_t key = ID->getEventKey("cache-copy-in"); )
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseOpenBurstEvent( key, (nanos_event_value_t) 2 ); )
      for ( std::size_t count = 0; count < numChunks; count += 1) {
         ::memcpy( ((char *) devDestAddr) + count * ld, ((char *) devOrigAddr) + count * ld, len );
      }
      NANOS_INSTRUMENT( sys.getInstrumentation()->raiseCloseBurstEvent( key, (nanos_event_value_t) 0 ); )
      ops->completeOp();
   }
   return true;
}

void SMPDevice::_getFreeMemoryChunksList( SeparateMemoryAddressSpace &mem, SimpleAllocator::ChunkList &list ) {
   SimpleAllocator *sallocator = (SimpleAllocator *) mem.getSpecificData();
   sallocator->getFreeChunksList( list );
}

void SMPDevice::tryExecuteTransfer() {
   if ( sys.getSMPPlugin()->asyncTransfersEnabled() ) {
      _transferQueue.tryExecuteOne();
   }
}

} // namespace nanos

#endif

