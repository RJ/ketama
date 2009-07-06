/**
 * $Id: ketamamodule.h 23 2007-07-13 01:32:00Z toxik $
 *
 * License: See ../LICENSE or
 *  svn://svn.audioscrobbler.net/misc/ketama/LICENSE
 */

#ifndef _PYKETAMA_H
#define _PYKETAMA_H

#include <Python.h>
#include <ketama.h>

/* Common idiom goes to macro, that's it. */
#define PY_DEF(name, self, args) static PyObject *(name)( \
    PyObject *(self), PyObject *(args))
#define PY_KDEF(name, self, args, kwds) static PyObject *(name)( \
    PyObject *(self), PyObject *(args) PyObject *(kwds))

PyObject *pyketama_error;

/* {{{ Continuum def */

typedef struct {
    PyObject_HEAD
    ketama_continuum cont;
    char *filename;
} pyketama_Continuum;

static void pyketama_Continuum_dealloc(pyketama_Continuum *self);
PyObject * pyketama_Continuum_new(PyTypeObject *type,
        PyObject *args, PyObject *kwds);
static int pyketama_Continuum_init(PyObject *self, PyObject *args,
        PyObject *kwds);
static PyObject *pyketama_Continuum_repr(PyObject *o);

PY_DEF(pyketama_Continuum_get_server, self, args);
PY_DEF(pyketama_Continuum_get_points, self, args);

/* get_server could use METH_O instead of METH_VARARGS, but ParseTuple is
 * a lot more flexible somehow.
 */
static PyMethodDef pyketama_Continuum_methods[] = {
    {"get_server", pyketama_Continuum_get_server, METH_VARARGS,
     "Return a tuple containing point on circle and address where hash is."},
    {"get_points", pyketama_Continuum_get_points, METH_NOARGS,
     "Return a list with all points and associated server."},
    {NULL}
};

/* See http://docs.python.org/api/type-structs.html for more information
 * regarding the struct's members.
 */
static PyTypeObject pyketama_ContinuumType = {
    PyObject_HEAD_INIT(NULL)
    0,
    "ketama.Continuum",                     /* tp_name */
    sizeof(pyketama_Continuum),             /* tp_basicsize */
    0,                                      /* tp_itemsize */

    /* Methods to implement standard operations */
    (destructor)pyketama_Continuum_dealloc, /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    (reprfunc)pyketama_Continuum_repr,      /* tp_repr */

    /* Method suites for standard classes */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */

    /* More standard operations (here for binary compatibility) */
    0,                                      /* tp_hash */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */

    /* Functions to access object as input/output buffer */
    0,                                      /* tp_as_buffer */

    /* Flags to define presence of optional/expanded features */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,

    /* Documentation string */
    "Ketama continuum type",

    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */

    /* Iterator API */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */

    /* Attribute descriptor and subclassing stuff */
    pyketama_Continuum_methods,             /* tp_methods */
    0,                                      /* tp_members */
    0,                                      /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    (initproc)pyketama_Continuum_init,      /* tp_init */
    0,                                      /* tp_alloc */
    pyketama_Continuum_new,                 /* tp_new */
    0,                                      /* tp_free */
    0,                                      /* tp_is_gc */
    0,                                      /* tp_bases */
    0,                                      /* tp_mro */
    0,                                      /* tp_cache */
    0,                                      /* tp_subclasses */
    0,                                      /* tp_weaklist */
};

/* }}} Continuum def */

PY_DEF(pyketama_hashi, self, args);

/* See comment at the pyketama_Continuum_methods definition.
 */
static PyMethodDef ketamaMethods[] = {
    {"hashi", pyketama_hashi, METH_VARARGS, "Calculates hash from input key"},
    {NULL}
};

PyMODINIT_FUNC initketama(void);

#endif

/* vim: ts=4 sts=4 expandtab
 */
