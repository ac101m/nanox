/*************************************************************************************/
/*      Copyright 2013 Barcelona Supercomputing Center                               */
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

#ifndef _OpenCL_DEVICE_DECL
#define _OpenCL_DEVICE_DECL

#include "workdescriptor_decl.hpp"
#include "copydescriptor_decl.hpp"
#include "processingelement_fwd.hpp"

namespace nanos {

class OpenCLDevice : public Device
{
public:
   OpenCLDevice ( const char *name );

public:
//   static void *allocate( size_t size, ProcessingElement *pe, uint64_t tag = NULL  );
//
//   static void *realloc( void * address,
//                         size_t size,
//                         size_t ceSize,
//                         ProcessingElement *pe );
//
//   static void free( void *address, ProcessingElement *pe );
//
//   static bool copyDevToDev( void *addrDst,
//                             CopyDescriptor& dstCd,
//                             void* addrSrc,
//                             size_t size,
//                             ProcessingElement *peDst,
//                             ProcessingElement *peSrc );
//   
//   static bool copyIn( void *localDst,
//                       CopyDescriptor &remoteSrc,
//                       size_t size,
//                       ProcessingElement *pe );
//
//   static bool copyOut( CopyDescriptor &remoteDst,
//                        void *localSrc,
//                        size_t size,
//                        ProcessingElement *pe );
//
//   static void copyLocal( void *dst,
//                          void *src,
//                          size_t size,
//                          ProcessingElement *pe )
//   {
//       return;
//   }
//
//   static void syncTransfer( uint64_t hostAddress, ProcessingElement *pe );
//   
   void *memAllocate( std::size_t size, SeparateMemoryAddressSpace &mem, uint64_t targetHostAddr=0) const;
   std::size_t getMemCapacity( SeparateMemoryAddressSpace &mem ) const;
   void memFree( uint64_t addr, SeparateMemoryAddressSpace &mem ) const;
   void _getFreeMemoryChunksList( SeparateMemoryAddressSpace &mem, SimpleAllocator::ChunkList &list ) const;
   void _copyIn( uint64_t devAddr, uint64_t hostAddr, std::size_t len, SeparateMemoryAddressSpace &mem, DeviceOps *ops, Functor *f, WD const &wd, void *hostObject, reg_t hostRegionId ) const;
   void _copyOut( uint64_t hostAddr, uint64_t devAddr, std::size_t len, SeparateMemoryAddressSpace &mem, DeviceOps *ops, Functor *f, WD const &wd, void *hostObject, reg_t hostRegionId ) const;
   bool _copyDevToDev( uint64_t devDestAddr, uint64_t devOrigAddr, std::size_t len, SeparateMemoryAddressSpace &memDest, SeparateMemoryAddressSpace &memOrig, DeviceOps *ops, Functor *f, WD const &wd, void *hostObject, reg_t hostRegionId ) const ;
//   void _copyInStrided1D( uint64_t devAddr, uint64_t hostAddr, std::size_t len, std::size_t count, std::size_t ld, SeparateMemoryAddressSpace const &mem, DeviceOps *ops, Functor *f, WD const &wd ) ;
//   void _copyOutStrided1D( uint64_t hostAddr, uint64_t devAddr, std::size_t len, std::size_t count, std::size_t ld, SeparateMemoryAddressSpace const &mem, DeviceOps *ops, Functor *f, WD const &wd ) ;
//   bool _copyDevToDevStrided1D( uint64_t devDestAddr, uint64_t devOrigAddr, std::size_t len, std::size_t count, std::size_t ld, SeparateMemoryAddressSpace const &memDest, SeparateMemoryAddressSpace const &memOrig, DeviceOps *ops, Functor *f, WD const &wd ) const;



};

} // End namespace nanos.

#endif // _OpenCL_DEVICE_DECL
