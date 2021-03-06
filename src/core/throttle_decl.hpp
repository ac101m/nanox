/*************************************************************************************/
/*      Copyright 2009-2018 Barcelona Supercomputing Center                          */
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
/*      along with NANOS++.  If not, see <https://www.gnu.org/licenses/>.            */
/*************************************************************************************/

#ifndef __NANOS_THROTTLE_POLICY_DECL_H
#define __NANOS_THROTTLE_POLICY_DECL_H

namespace nanos {
   class ThrottlePolicy
   {
      private:
         /*! \brief ThrottlePolicy copy constructor (private)
          */
         ThrottlePolicy( const ThrottlePolicy & );
         /*! \brief ThrottlePolicy copy assignment opeator (private)
          */
         const ThrottlePolicy & operator=( const ThrottlePolicy & );
      public:
         /*! \brief ThrottlePolicy default constructor
          */
         ThrottlePolicy() {};
         /*! \brief ThrottlePolicy destructor
          */
         virtual ~ThrottlePolicy() {}

         virtual bool throttleIn( void )  = 0 ;
         virtual void throttleOut( void ) { /* empty function */ }
   };
} // namespace nanos

#endif
