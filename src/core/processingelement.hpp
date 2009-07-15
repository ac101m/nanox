#ifndef _NANOS_PROCESSING_ELEMENT
#define _NANOS_PROCESSING_ELEMENT

#include "workdescriptor.hpp"
#include "basethread.hpp"

namespace nanos
{

// forward definitions

   class SchedulingGroup;

   class ProcessingElement
   {

      private:
         int id;
         const Architecture *architecture;

         protected:
         virtual WorkDescriptor & getMasterWD () const = 0;
         virtual WorkDescriptor & getWorkerWD () const = 0;
	 //TODO: make this a vector (#6)
         BaseThread *workerThread;

      public:
         // constructors
         ProcessingElement ( int newId, const Architecture *arch ) : id ( newId ), architecture ( arch ), workerThread ( 0 ) {}

         // TODO: copy constructor
         ProcessingElement ( const ProcessingElement &pe );
         // TODO: assignment operations
         const ProcessingElement & operator= ( const ProcessingElement &pe );
         // destructor
         virtual ~ProcessingElement() {}

         /* get/put methods */
         int getId() const {
            return id;
         }

         const Architecture * getArchitecture () const {
            return architecture;
         }

         BaseThread & startThread ( WorkDescriptor &wd, SchedulingGroup *sg = 0 );
         virtual BaseThread & createThread ( WorkDescriptor &wd) = 0;
         BaseThread & associateThisThread (SchedulingGroup *sg);

         void startWorker ( SchedulingGroup *sg );
         void stopAll();
   };

   typedef class ProcessingElement PE;

};

#endif