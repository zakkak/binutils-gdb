/* Python interface to record targets.

   Copyright 2016-2017 Free Software Foundation, Inc.

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

#include "defs.h"
#include "py-record.h"
#include "py-record-btrace.h"
#include "py-record-full.h"
#include "target.h"

/* Python Record type.  */

static PyTypeObject recpy_record_type = {
  PyVarObject_HEAD_INIT (NULL, 0)
};

/* Python RecordGap type.  */

PyTypeObject recpy_gap_type = {
  PyVarObject_HEAD_INIT (NULL, 0)
};

/* Python RecordGap object.  */
typedef struct
{
  PyObject_HEAD

  /* Reason code.  */
  int reason_code;

  /* Reason message.  */
  const char *reason_string;

  /* Element number.  */
  Py_ssize_t number;
} recpy_gap_object;

/* Implementation of record.method.  */

static PyObject *
recpy_method (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_FULL)
    return recpy_full_method (self, closure);

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_method (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.format.  */

static PyObject *
recpy_format (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_FULL)
    return recpy_full_format (self, closure);

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_format (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.goto (instruction) -> None.  */

static PyObject *
recpy_goto (PyObject *self, PyObject *value)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_goto (self, value);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.replay_position [instruction]  */

static PyObject *
recpy_replay_position (PyObject *self, void *closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_replay_position (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.instruction_history [list].  */

static PyObject *
recpy_instruction_history (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_instruction_history (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.function_call_history [list].  */

static PyObject *
recpy_function_call_history (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_function_call_history (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.begin [instruction].  */

static PyObject *
recpy_begin (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_begin (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Implementation of record.end [instruction].  */

static PyObject *
recpy_end (PyObject *self, void* closure)
{
  const recpy_record_object * const obj = (recpy_record_object *) self;

  if (obj->method == RECORD_METHOD_BTRACE)
    return recpy_bt_end (self, closure);

  return PyErr_Format (PyExc_NotImplementedError, _("Not implemented."));
}

/* Create a new gdb.RecordGap object.  */

PyObject *
recpy_gap_new (int reason_code, const char *reason_string, Py_ssize_t number)
{
  recpy_gap_object * const obj = PyObject_New (recpy_gap_object,
					       &recpy_gap_type);

  if (obj == NULL)
   return NULL;

  obj->reason_code = reason_code;
  obj->reason_string = reason_string;
  obj->number = number;

  return (PyObject *) obj;
}

/* Implementation of RecordGap.number [int].  */

static PyObject *
recpy_gap_number (PyObject *self, void *closure)
{
  const recpy_gap_object * const obj = (const recpy_gap_object *) self;

  return PyInt_FromSsize_t (obj->number);
}

/* Implementation of RecordGap.error_code [int].  */

static PyObject *
recpy_gap_reason_code (PyObject *self, void *closure)
{
  const recpy_gap_object * const obj = (const recpy_gap_object *) self;

  return PyInt_FromLong (obj->reason_code);
}

/* Implementation of RecordGap.error_string [str].  */

static PyObject *
recpy_gap_reason_string (PyObject *self, void *closure)
{
  const recpy_gap_object * const obj = (const recpy_gap_object *) self;

  return PyString_FromString (obj->reason_string);
}

/* Record method list.  */

static PyMethodDef recpy_record_methods[] = {
  { "goto", recpy_goto, METH_VARARGS,
    "goto (instruction|function_call) -> None.\n\
Rewind to given location."},
  { NULL }
};

/* Record member list.  */

static gdb_PyGetSetDef recpy_record_getset[] = {
  { "method", recpy_method, NULL, "Current recording method.", NULL },
  { "format", recpy_format, NULL, "Current recording format.", NULL },
  { "replay_position", recpy_replay_position, NULL, "Current replay position.",
    NULL },
  { "instruction_history", recpy_instruction_history, NULL,
    "List of instructions in current recording.", NULL },
  { "function_call_history", recpy_function_call_history, NULL,
    "List of function calls in current recording.", NULL },
  { "begin", recpy_begin, NULL,
    "First instruction in current recording.", NULL },
  { "end", recpy_end, NULL,
    "One past the last instruction in current recording.  This is typically \
the current instruction and is used for e.g. record.goto (record.end).", NULL },
  { NULL }
};

/* RecordGap member list.  */

static gdb_PyGetSetDef recpy_gap_getset[] = {
  { "number", recpy_gap_number, NULL, "element number", NULL},
  { "reason_code", recpy_gap_reason_code, NULL, "reason code", NULL},
  { "reason_string", recpy_gap_reason_string, NULL, "reason string", NULL},
  { NULL }
};

/* Sets up the record API in the gdb module.  */

int
gdbpy_initialize_record (void)
{
  recpy_record_type.tp_new = PyType_GenericNew;
  recpy_record_type.tp_flags = Py_TPFLAGS_DEFAULT;
  recpy_record_type.tp_basicsize = sizeof (recpy_record_object);
  recpy_record_type.tp_name = "gdb.Record";
  recpy_record_type.tp_doc = "GDB record object";
  recpy_record_type.tp_methods = recpy_record_methods;
  recpy_record_type.tp_getset = recpy_record_getset;

  recpy_gap_type.tp_new = PyType_GenericNew;
  recpy_gap_type.tp_flags = Py_TPFLAGS_DEFAULT;
  recpy_gap_type.tp_basicsize = sizeof (recpy_gap_object);
  recpy_gap_type.tp_name = "gdb.RecordGap";
  recpy_gap_type.tp_doc = "GDB recorded gap object";
  recpy_gap_type.tp_getset = recpy_gap_getset;

  if (PyType_Ready (&recpy_record_type) < 0
      || PyType_Ready (&recpy_gap_type) < 0)
    return -1;
  else
    return 0;
}

/* Implementation of gdb.start_recording (method) -> gdb.Record.  */

PyObject *
gdbpy_start_recording (PyObject *self, PyObject *args)
{
  const char *method = NULL;
  const char *format = NULL;
  PyObject *ret = NULL;

  if (!PyArg_ParseTuple (args, "|ss", &method, &format))
    return NULL;

  TRY
    {
      record_start (method, format, 0);
      ret = gdbpy_current_recording (self, args);
    }
  CATCH (except, RETURN_MASK_ALL)
    {
      gdbpy_convert_exception (except);
    }
  END_CATCH

  return ret;
}

/* Implementation of gdb.current_recording (self) -> gdb.Record.  */

PyObject *
gdbpy_current_recording (PyObject *self, PyObject *args)
{
  recpy_record_object *ret = NULL;

  if (find_record_target () == NULL)
    Py_RETURN_NONE;

  ret = PyObject_New (recpy_record_object, &recpy_record_type);
  ret->ptid = inferior_ptid;
  ret->method = target_record_method (inferior_ptid);

  return (PyObject *) ret;
}

/* Implementation of gdb.stop_recording (self) -> None.  */

PyObject *
gdbpy_stop_recording (PyObject *self, PyObject *args)
{
  PyObject *ret = NULL;

  TRY
    {
      record_stop (0);
      ret = Py_None;
      Py_INCREF (Py_None);
    }
  CATCH (except, RETURN_MASK_ALL)
    {
      gdbpy_convert_exception (except);
    }
  END_CATCH

  return ret;
}
