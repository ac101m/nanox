/*************************************************************************************/
/*      Copyright 2010 Barcelona Supercomputing Center                               */
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

#ifndef NANOS_CHPL_H
#define NANOS_CHPL_H

#define CHPL_TASKS nanos

#include <stdbool.h>

#ifdef __cplusplus
#define _Bool bool
extern "C" {
#endif

typedef int chpl_taskID_t;
#define chpl_nullTaskID 0

typedef void * chpl_mutex_t;

typedef struct {
   bool is_full;
   void *empty;
   void *full;
} chpl_sync_aux_t;

typedef struct {
   bool is_full;
   void *full;
} chpl_single_aux_t;

#include <chpltypes.h>
#include <chpltasks_func_names.h>
#include <chpltasks.h>

#ifdef __cplusplus
}
#endif

#endif