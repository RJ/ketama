/**
 * $Id: ketamamodule.c 22 2007-07-13 01:31:27Z toxik $
 *
 * ketama Python module.
 *
 * Definitions of C-level API calls to libketama
 *
 * libketama defines:
 * ketama_roll: initialize server list and return a continuum
 * ketama_destroy: free memory used by continuum
 * ketama_get_server: hash key up and return server to use from continuum
 * ketama_print_continuum: print out continuum
 * ketama_error: return current error string
 */

#include "ketamamodule.h"

/* {{{ Continuum type definition */

static void pyketama_Continuum_dealloc(pyketama_Continuum *self) {
    ketama_smoke(self->cont);
    free(self->filename);
    self->ob_type->tp_free((PyObject *)self);
}

PyObject *pyketama_Continuum_new(PyTypeObject *type,
        PyObject *args, PyObject *kwds) {
    /* Investigate: This seems like dull boiler-plate code. Needed? */
    pyketama_Continuum *pkc = (pyketama_Continuum *)type->tp_alloc(type, 0);

    /* Assures that a free() on uninitialized filename will not die. */
    pkc->filename = NULL;

    return (PyObject *)pkc;
}

static int pyketama_Continuum_init(PyObject *self, PyObject *args,
        PyObject *kwds) {
    char *filename;
    pyketama_Continuum *pkc = (pyketama_Continuum *)self;

    if (!PyArg_ParseTuple(args, "s", &filename)) {
        return -1;
    }

    /* libketama doesn't really handle long filenames. */
    if (strlen(filename) > 255) {
        PyErr_SetString(PyExc_ValueError,
            "filename must not be over 255 bytes long");
        return -1;
    }

    /* I'm not entirely sure if you need this, but I do it. */
    if(!(pkc->filename = strdup(filename))) {
        PyErr_NoMemory();
        return -1;
    }

    if (!ketama_roll(&(pkc->cont), pkc->filename)) {
        PyErr_SetString(pyketama_error, ketama_error());
        return -1;
    }

    return 0;
}

static PyObject *pyketama_Continuum_repr(PyObject *o) {
    PyObject *r;

    r = PyString_FromFormat("<%s from ", o->ob_type->tp_name);
    PyString_Concat(&r,PyObject_Repr(
        PyString_FromString(((pyketama_Continuum* )o)->filename)));
    if (r) {
        PyString_Concat(&r, PyString_FromString(">"));
    }

    /* Example: <ketama.Continuum from '/tmp/slist'> */
    return r;
}

PY_DEF(pyketama_Continuum_get_server, self, args) {
    char *key;
    mcs *r;
    PyObject *ret = NULL;

    if (PyArg_ParseTuple(args, "s", &key)) {
        /* ketama_get_server simply can not fail, so there's no point in
         * error checking.
         */
        r = ketama_get_server(key, ((pyketama_Continuum *)self)->cont);
        ret = Py_BuildValue("Is", r->point, r->ip);
    }

    return ret;
}

PY_DEF(pyketama_Continuum_get_points, self, args) {
    PyObject *ret = NULL;
    pyketama_Continuum *pkc = (pyketama_Continuum *)self;

    if (pkc->cont->array) {
        int i;
        mcs (*mcsarr)[pkc->cont->numpoints] = pkc->cont->array;

        ret = PyList_New(pkc->cont->numpoints);
        for (i = 0; i < pkc->cont->numpoints; i++) {
            PyList_SET_ITEM(ret, i,
                Py_BuildValue("Is", (*mcsarr)[i].point, (*mcsarr)[i].ip));
        }
    }

    return ret;
}

/* }}} Continuum type definition */

PY_DEF(pyketama_hashi, self, args) {
    char *data;
    PyObject *r = NULL;

    if (PyArg_ParseTuple(args, "s", &data)) {
        r = Py_BuildValue("I", ketama_hashi(data));
    }
}

PyMODINIT_FUNC initketama(void) {
    PyObject *m;

    if (PyType_Ready(&pyketama_ContinuumType) < 0) {
        return;
    }

    if (!(m = Py_InitModule3("ketama", ketamaMethods,
        "Python extension for calling libketama functions."))) {
        return;
    }

    pyketama_error = PyErr_NewException("ketama.KetamaError", NULL, NULL);

    Py_INCREF(pyketama_error);
    PyModule_AddObject(m, "KetamaError", pyketama_error);

    Py_INCREF(&pyketama_ContinuumType);
    PyModule_AddObject(m, "Continuum", (PyObject *)&pyketama_ContinuumType);
}

/* vim: ts=4 sts=4 expandtab
 */
