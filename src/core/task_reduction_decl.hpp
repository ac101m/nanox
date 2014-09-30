/*************************************************************************************/
/*      Copyright 2012 Barcelona Supercomputing Center                               */
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

#ifndef _NANOS_TASK_REDUCTION_DECL_H
#define _NANOS_TASK_REDUCTION_DECL_H

class TaskReduction {
   public: /* data types */
      typedef void ( *initializer_t ) ( void *omp_priv,  void* omp_orig );
      typedef void ( *reducer_t ) ( void *obj1, void *obj2 );
      typedef std::vector<char> storage_t;
   private: /* data members */
      void                            *_original;
      void                            *_dependence;
      unsigned                         _depth;
      initializer_t                    _initializer;
      reducer_t                        _reducer;
      reducer_t                        _reducer_orig_var;
      storage_t                        _storage;
      size_t                           _size;
      size_t                           _threads;
      void                            *_min;
      void                            *_max;
   private: /* private methods */
      TaskReduction ( const TaskReduction &tr ) {}
   public: /* public methods */
      TaskReduction ( void *orig, void *dep, initializer_t init, reducer_t red, reducer_t red_orig_var, size_t size, size_t threads, unsigned depth ) : _original(orig), _dependence(dep), _depth(depth), _initializer(init), _reducer(red), _reducer_orig_var(red_orig_var),
                      _storage((size<<8)*threads), _size(size<<8), _threads(threads), _min(NULL), _max(NULL)
      {
         _min = & _storage[0];
         _max = & _storage[_size*threads];
         size_t i;
         for ( i=0; i<threads; i++) {
             //printf("storage thread[%d] = %p \n", (unsigned int)i,  &_storage[i*_size]);
             _initializer( &_storage[i*_size], _original );
         }
      }

     ~TaskReduction ( )
      {
      }
      void * have ( const void *ptr, size_t id ) ;
      void * have_dependence ( const void *ptr, size_t id ) ;
      void * finalize ( void );
      unsigned getDepth(void) const ;
};

#endif
