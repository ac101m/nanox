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


#ifndef _GASNET_API
#define _GASNET_API

#include "network_decl.hpp"
#include "networkapi.hpp"
#include "simpleallocator.hpp"
#include "regiondirectory_decl.hpp"
#include "clusterinfo_decl.hpp"
#include "requestqueue_decl.hpp"
//#include "remoteworkgroup_decl.hpp"
#include <map>

extern "C" {
#include <gasnet.h>
}

namespace nanos {
namespace ext {

   class GASNetAPI : public NetworkAPI
   {
      private:
         static Network *_net;

         static RemoteWorkGroup *_rwgGPU;
         static RemoteWorkGroup *_rwgSMP;
         static NewRegionDirectory *_newMasterDir;
#ifndef GASNET_SEGMENT_EVERYTHING
         static SimpleAllocator *_thisNodeSegment;
#endif
         static SimpleAllocator *_packSegment;
         static std::set< void * > _waitingPutRequests;
         static std::set< void * > _receivedUnmatchedPutRequests;
         static Lock _waitingPutRequestsLock;
         static std::vector< SimpleAllocator * > _pinnedAllocators;
         static std::vector< Lock * > _pinnedAllocatorsLocks;
         static Atomic<unsigned int> *_seqN;

         // GASNet does not allow to send a message during the execution of an active message handler,
         // to handle put request, we have to enqueue the incoming request to be able to send the "real" put
         // later on (we do it during the GASNetAPI::poll).
         struct putReqDesc {
            unsigned int dest;
            void *origAddr;
            void *destAddr;
            std::size_t len;
            std::size_t count;
            std::size_t ld;
            void *tmpBuffer;
            unsigned int wdId;
            WD const *wd;
         };

         static RequestQueue< struct putReqDesc > _putReqs;
         static RequestQueue< std::pair< void *, WD const * > > _freeBufferReqs;
         static RequestQueue< std::pair<void *, unsigned int> > _workDoneReqs;

         static std::list<struct putReqDesc * > _delayedPutReqs;
         static Lock _delayedPutReqsLock;

         // NOT YET IMPLEMENTED struct delayedGetDesc {
         // NOT YET IMPLEMENTED    unsigned int dest;
         // NOT YET IMPLEMENTED    void *origTag;
         // NOT YET IMPLEMENTED    void *origAddr;
         // NOT YET IMPLEMENTED    void *destAddr;
         // NOT YET IMPLEMENTED    std::size_t len;
         // NOT YET IMPLEMENTED    std::size_t count;
         // NOT YET IMPLEMENTED    std::size_t ld;
         // NOT YET IMPLEMENTED    void *waitObj;
         // NOT YET IMPLEMENTED };
         // NOT YET IMPLEMENTED static std::list<struct delayedGetDesc * > _delayedGets;
         // NOT YET IMPLEMENTED static Lock _delayedGetsLock;

         static std::size_t rxBytes;
         static std::size_t txBytes;
         static std::size_t _totalBytes;
         

      public:
         void initialize ( Network *net );
         void finalize ();
         void poll ();
         void sendExitMsg ( unsigned int dest );
         void sendWorkMsg ( unsigned int dest, void ( *work ) ( void * ), unsigned int arg0, unsigned int arg1, unsigned int numPe, std::size_t argSize, char * arg, void ( *xlate ) ( void *, void * ), int arch, void *wd, std::size_t expectedData );
         void sendWorkDoneMsg ( unsigned int dest, void *remoteWdAddr, int peId);
         static void _sendWorkDoneMsg ( unsigned int dest, void *remoteWdAddr, int peId);
         void put ( unsigned int remoteNode, uint64_t remoteAddr, void *localAddr, std::size_t size, unsigned int wdId, WD const &wd );
         void putStrided1D ( unsigned int remoteNode, uint64_t remoteAddr, void *localAddr, void *localPack, std::size_t size, std::size_t count, std::size_t ld, unsigned int wdId, WD const &wd );
         void get ( void *localAddr, unsigned int remoteNode, uint64_t remoteAddr, std::size_t size, volatile int *requestComplete );
         void getStrided1D ( void *packedAddr, unsigned int remoteNode, uint64_t remoteTag, uint64_t remoteAddr, std::size_t size, std::size_t count, std::size_t ld, volatile int *requestComplete );
         void malloc ( unsigned int remoteNode, std::size_t size, void *waitObjAddr );
         void memFree ( unsigned int remoteNode, void *addr );
         void memRealloc ( unsigned int remoteNode, void *oldAddr, std::size_t oldSize, void *newAddr, std::size_t newSize );
         void nodeBarrier( void );
         
         void sendMyHostName( unsigned int dest );
         void sendRequestPut( unsigned int dest, uint64_t origAddr, unsigned int dataDest, uint64_t dstAddr, std::size_t len, unsigned int wdId, WD const &wd);
         void sendRequestPutStrided1D( unsigned int dest, uint64_t origAddr, unsigned int dataDest, uint64_t dstAddr, std::size_t len, std::size_t count, std::size_t ld, unsigned int wdId, WD const &wd );
         void setNewMasterDirectory(NewRegionDirectory *dir);
         std::size_t getMaxGetStridedLen() const;
         std::size_t getTotalBytes();
         static std::size_t getRxBytes();
         static std::size_t getTxBytes();
         SimpleAllocator *getPackSegment() const;
         void *allocateReceiveMemory( std::size_t len );
         void freeReceiveMemory( void * addr );

      private:
         static void getDataFromDevice( uint64_t addr, std::size_t len );
         static void invalidateDataFromDevice( uint64_t addr, std::size_t len );
         void _put ( unsigned int remoteNode, uint64_t remoteAddr, void *localAddr, std::size_t size, void *remoteTmpBuffer, unsigned int wdId, WD const &wd );
         void _putStrided1D ( unsigned int remoteNode, uint64_t remoteAddr, void *localAddr, void *localPack, std::size_t size, std::size_t count, std::size_t ld, void *remoteTmpBuffer, unsigned int wdId, WD const &wd );
         static void enqueuePutReq( unsigned int dest, void *origAddr, void *destAddr, std::size_t len, std::size_t count, std::size_t ld, void *tmpBuffer, unsigned int wdId, WD const *wd );
         static void enqueueDelayedPutReq( unsigned int dest, void *origAddr, void *destAddr, std::size_t len, std::size_t count, std::size_t ld, void *tmpBuffer, unsigned int wdId, WD const *wd );
         static void enqueueDelayedGet( unsigned int dest, void *origTag, void *origAddr, void *destAddr, std::size_t len, std::size_t count, std::size_t ld, void *waitObj );
         static void delayAmGet( unsigned int dest, void *origTag, void *origAddr, void *destAddr, std::size_t len, std::size_t count, std::size_t ld, void *waitObj ) ;
         void checkForPutReqs();
         static void sendWaitForRequestPut( unsigned int dest, uint64_t addr );
         static void print_copies( WD const *wd, int deps );
         static void enqueueFreeBufferNotify( void *bufferAddr, WD const *wd );
         static void checkForFreeBufferReqs();
         static void checkWorkDoneReqs();
         static void checkDeferredWorkReqs();
         static void sendFreeTmpBuffer( void *addr, WD const *wd );

         // Active Message handlers
         static void amFinalize( gasnet_token_t token );
         static void amFinalizeReply(gasnet_token_t token);
         static void amWork(gasnet_token_t token, void *arg, std::size_t argSize,
                             gasnet_handlerarg_t workLo,
                             gasnet_handlerarg_t workHi,
                             gasnet_handlerarg_t xlateLo,
                             gasnet_handlerarg_t xlateHi,
                             gasnet_handlerarg_t rmwdLo,
                             gasnet_handlerarg_t rmwdHi,
                             gasnet_handlerarg_t expectedDataLo,
                             gasnet_handlerarg_t expectedDataHi,
                             unsigned int dataSize, unsigned int wdId, unsigned int numPe, int arch, unsigned int seq );
         static void amWorkData(gasnet_token_t token, void *buff, std::size_t len,
               gasnet_handlerarg_t msgNum,
               gasnet_handlerarg_t totalLenLo,
               gasnet_handlerarg_t totalLenHi);
                  
         static void amWorkDone( gasnet_token_t token, gasnet_handlerarg_t addrLo, gasnet_handlerarg_t addrHi, gasnet_handlerarg_t peId );
         static void amMalloc( gasnet_token_t token, gasnet_handlerarg_t sizeLo, gasnet_handlerarg_t sizeHi,
                                gasnet_handlerarg_t waitObjAddrLo, gasnet_handlerarg_t waitObjAddrHi );
         static void amMallocReply( gasnet_token_t token, gasnet_handlerarg_t addrLo, gasnet_handlerarg_t addrHi,
                                      gasnet_handlerarg_t waitObjAddrLo, gasnet_handlerarg_t waitObjAddrHi );
         static void amFree( gasnet_token_t token, gasnet_handlerarg_t addrLo, gasnet_handlerarg_t addrHi );
         static void amRealloc( gasnet_token_t token, gasnet_handlerarg_t oldAddrLo, gasnet_handlerarg_t oldAddrHi,
                                    gasnet_handlerarg_t oldSizeLo, gasnet_handlerarg_t oldSizeHi,
                                    gasnet_handlerarg_t newAddrLo, gasnet_handlerarg_t newAddrHi,
                                    gasnet_handlerarg_t newSizeLo, gasnet_handlerarg_t newSizeHi);
         static void amMasterHostname( gasnet_token_t token, void *buff, std::size_t nbytes );
         static void amPut( gasnet_token_t token,
               void *buf,
               std::size_t len,
               gasnet_handlerarg_t origAddrLo,
               gasnet_handlerarg_t origAddrHi,
               gasnet_handlerarg_t tagAddrLo,
               gasnet_handlerarg_t tagAddrHi,
               gasnet_handlerarg_t totalLenLo,
               gasnet_handlerarg_t totalLenHi,
               gasnet_handlerarg_t wdId,
               gasnet_handlerarg_t wdLo,
               gasnet_handlerarg_t wdHi,
               gasnet_handlerarg_t first,
               gasnet_handlerarg_t last);
         static void amGet( gasnet_token_t token,
               gasnet_handlerarg_t destAddrLo,
               gasnet_handlerarg_t destAddrHi,
               gasnet_handlerarg_t origAddrLo,
               gasnet_handlerarg_t origAddrHi,
               gasnet_handlerarg_t tagAddrLo,
               gasnet_handlerarg_t tagAddrHi,
               gasnet_handlerarg_t lenLo,
               gasnet_handlerarg_t lenHi,
               gasnet_handlerarg_t totalLenLo,
               gasnet_handlerarg_t totalLenHi,
               gasnet_handlerarg_t waitObjLo,
               gasnet_handlerarg_t waitObjHi );
         static void amGetReply( gasnet_token_t token,
               void *buf,
               std::size_t len,
               gasnet_handlerarg_t waitObjLo,
               gasnet_handlerarg_t waitObjHi);
         static void amPutF( gasnet_token_t token,
               gasnet_handlerarg_t destAddrLo,
               gasnet_handlerarg_t destAddrHi,
               gasnet_handlerarg_t len,
               gasnet_handlerarg_t wordSize,
               gasnet_handlerarg_t valueLo,
               gasnet_handlerarg_t valueHi );
         static void amRequestPut( gasnet_token_t token,
               gasnet_handlerarg_t destAddrLo,
               gasnet_handlerarg_t destAddrHi,
               gasnet_handlerarg_t origAddrLo,
               gasnet_handlerarg_t origAddrHi,
               gasnet_handlerarg_t tmpBufferLo,
               gasnet_handlerarg_t tmpBufferHi,
               gasnet_handlerarg_t lenLo,
               gasnet_handlerarg_t lenHi,
               gasnet_handlerarg_t wdId,
               gasnet_handlerarg_t wdLo,
               gasnet_handlerarg_t wdHi,
               gasnet_handlerarg_t dst );
         static void amRequestPutStrided1D( gasnet_token_t token,
               gasnet_handlerarg_t destAddrLo,
               gasnet_handlerarg_t destAddrHi,
               gasnet_handlerarg_t origAddrLo,
               gasnet_handlerarg_t origAddrHi,
               gasnet_handlerarg_t tmpBufferLo,
               gasnet_handlerarg_t tmpBufferHi,
               gasnet_handlerarg_t lenLo,
               gasnet_handlerarg_t lenHi,
               gasnet_handlerarg_t count,
               gasnet_handlerarg_t ld,
               gasnet_handlerarg_t wdId,
               gasnet_handlerarg_t wdLo,
               gasnet_handlerarg_t wdHi,
               gasnet_handlerarg_t dst );
         static void amWaitRequestPut( gasnet_token_t token, 
               gasnet_handlerarg_t addrLo,
               gasnet_handlerarg_t addrHi );
         static void amFreeTmpBuffer( gasnet_token_t token, 
               gasnet_handlerarg_t addrLo,
               gasnet_handlerarg_t addrHi,
               gasnet_handlerarg_t wdLo,
               gasnet_handlerarg_t wdHi );
         static void amPutStrided1D( gasnet_token_t token,
               void *buf,
               std::size_t len,
               gasnet_handlerarg_t realTagLo,
               gasnet_handlerarg_t realTagHi,
               gasnet_handlerarg_t totalLenLo,
               gasnet_handlerarg_t totalLenHi,
               gasnet_handlerarg_t count,
               gasnet_handlerarg_t ld,
               gasnet_handlerarg_t wdId,
               gasnet_handlerarg_t wdLo,
               gasnet_handlerarg_t wdHi,
               gasnet_handlerarg_t firstMsg,
               gasnet_handlerarg_t lastMsg );
         static void amGetStrided1D( gasnet_token_t token,
               gasnet_handlerarg_t destAddrLo,
               gasnet_handlerarg_t destAddrHi,
               gasnet_handlerarg_t origAddrLo,
               gasnet_handlerarg_t origAddrHi,
               gasnet_handlerarg_t origTagLo,
               gasnet_handlerarg_t origTagHi,
               gasnet_handlerarg_t lenLo,
               gasnet_handlerarg_t lenHi,
               gasnet_handlerarg_t count,
               gasnet_handlerarg_t ld,
               gasnet_handlerarg_t waitObjLo,
               gasnet_handlerarg_t waitObjHi );
         static void amGetReplyStrided1D( gasnet_token_t token,
               void *buf,
               std::size_t len,
               gasnet_handlerarg_t waitObjLo,
               gasnet_handlerarg_t waitObjHi);
   };
}
}
#endif
