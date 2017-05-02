/* Python interface to record targets.

   Copyright 2017 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef GDB_PY_RECORD_H
#define GDB_PY_RECORD_H

#include "inferior.h"
#include "python-internal.h"
#include "record.h"

/* Python Record object.  */

typedef struct
{
  PyObject_HEAD

  /* The ptid this object refers to.  */
  ptid_t ptid;

  /* The current recording method.  */
  enum record_method method;
} recpy_record_object;

/* Create a new gdb.RecordGap object.  */
extern PyObject *recpy_gap_new (int reason_code, const char *reason_string,
				Py_ssize_t number);

#endif /* GDB_PY_RECORD_H */
