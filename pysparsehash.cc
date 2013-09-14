#include <Python.h>
#include <sparsehash/sparse_hash_map> 

#ifdef KEY_INTEGER
typedef google::sparse_hash_map<PY_LONG_LONG, void*> hash_map;
#else
#include <string>
typedef google::sparse_hash_map<std::string, void*> hash_map;
#endif

typedef struct {
    PyObject_HEAD
	hash_map mapper;
} SparseHashMap;

static void
SparseHashMap_dealloc(PyObject* o)
{
    hash_map::iterator iter;
    SparseHashMap* self = (SparseHashMap*)o;

	for (iter = self->mapper.begin(); iter != self->mapper.end(); iter++) {
        PyObject *value;
        value = (PyObject*)((*iter).second);
        Py_XDECREF(value);
	}
    self->mapper.clear();
    self->mapper.~hash_map();
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
SparseHashMap_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SparseHashMap *self;

    self = (SparseHashMap *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

static int
SparseHashMap_init(SparseHashMap *self, PyObject *args, PyObject *kwds)
{
    new (&self->mapper) hash_map;
#ifdef KEY_INTEGER
    self->mapper.set_deleted_key(-1);
#else
    self->mapper.set_deleted_key("0xFF0xFF0xFF0xFF");
#endif
    return 0;
}

static PyObject *
SparseHashMap_erase(SparseHashMap* self, PyObject *args)
{
    size_t r;
#ifdef KEY_INTEGER
    PY_LONG_LONG key;
    hash_map::iterator iter;
    if (!PyArg_ParseTuple(args, "L", &key))
        return NULL;
#else
    const char *k;
    std::string key;
    hash_map::iterator iter;
    if (!PyArg_ParseTuple(args, "s", &k))
        return NULL;

    key = k;
#endif

    r = self->mapper.erase(key);
    return PyBool_FromLong(r);
}


static PyObject *
SparseHashMap_get(SparseHashMap* self, PyObject *args)
{
#ifdef KEY_INTEGER
    PY_LONG_LONG key;
    hash_map::iterator iter;
    if (!PyArg_ParseTuple(args, "L", &key))
        return NULL;
#else
    const char *k;
    std::string key;
    hash_map::iterator iter;
    if (!PyArg_ParseTuple(args, "s", &k))
        return NULL;

    key = k;
#endif

    iter = self->mapper.find(key);
    if (iter != self->mapper.end()) {
        PyObject *value;
        value = (PyObject*)((*iter).second);
        Py_INCREF(value);
        return value;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
SparseHashMap_set(SparseHashMap* self, PyObject *args)
{
    hash_map::iterator iter;
    PyObject *value;
#ifdef KEY_INTEGER
    PY_LONG_LONG key;
    if (!PyArg_ParseTuple(args, "LO", &key, &value))
        return NULL;
#else
    const char *k;
    std::string key;
    if (!PyArg_ParseTuple(args, "sO", &key, &value))
        return NULL;
    key = k;
#endif

    Py_INCREF(value);
    iter = self->mapper.find(key);
    if (iter != self->mapper.end()) {
        PyObject *v;
        v = (PyObject*)(*iter).second;
        Py_XDECREF(v);
        (*iter).second = value;
    } else {
        self->mapper[key] = value;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
SparseHashMap_clear(SparseHashMap* self) {
    hash_map::iterator iter;
	for (iter = self->mapper.begin(); iter != self->mapper.end(); iter++) {
        PyObject *value;
        value = (PyObject*)((*iter).second);
        Py_XDECREF(value);
	}
    self->mapper.clear();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef SparseHashMap_methods[] = {
    {
        "get", (PyCFunction)SparseHashMap_get, METH_VARARGS, "get key"
    },
    {
        "set", (PyCFunction)SparseHashMap_set, METH_VARARGS, "set key value"
    },
    {
        "erase", (PyCFunction)SparseHashMap_erase, METH_VARARGS, "erase key"
    },
    {
        "clear", (PyCFunction)SparseHashMap_clear, METH_NOARGS, "clear map"
    },
    {NULL}  
};

static PyTypeObject sparsehash_SparseHashMapType = {
    PyObject_HEAD_INIT(NULL)
    0,                                /*ob_size*/
    "sparsehash.SparseHashMap",       /*tp_name*/
    sizeof(SparseHashMap),            /*tp_basicsize*/
    0,                                /*tp_itemsize*/
    SparseHashMap_dealloc,            /*tp_dealloc*/
    0,                                /*tp_print*/
    0,                                /*tp_getattr*/
    0,                                /*tp_setattr*/
    0,                                /*tp_compare*/
    0,                                /*tp_repr*/
    0,                                /*tp_as_number*/
    0,                                /*tp_as_sequence*/
    0,                                /*tp_as_mapping*/
    0,                                /*tp_hash */
    0,                                /*tp_call*/
    0,                                /*tp_str*/
    0,                                /*tp_getattro*/
    0,                                /*tp_setattro*/
    0,                                /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,               /*tp_flags*/
    "SparseHashMap objects",          /* tp_doc */
    0,		                          /* tp_traverse */
    0,		                          /* tp_clear */
    0,		                          /* tp_richcompare */
    0,		                          /* tp_weaklistoffset */
    0,		                          /* tp_iter */
    0,		                          /* tp_iternext */
    SparseHashMap_methods,            /* tp_methods */
    0,                                /* tp_members */
    0,                                /* tp_getset */
	0,								  /* tp_base */
	0,								  /* tp_dict */
	0,								  /* tp_descr_get */
	0,								  /* tp_descr_set */
	0,								  /* tp_dictoffset */
    (initproc)SparseHashMap_init,     /* tp_init */
    0,                                /* tp_alloc */
    SparseHashMap_new,                /* tp_new */

};							           	

static PyMethodDef sparsehash_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initsparsehash(void) 
{
    PyObject* m;

    sparsehash_SparseHashMapType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&sparsehash_SparseHashMapType) < 0)
        return;

    m = Py_InitModule3("sparsehash", sparsehash_methods,
                       "Example module that creates an extension type.");

    Py_INCREF(&sparsehash_SparseHashMapType);
    PyModule_AddObject(m, "SparseHashMap", (PyObject *)&sparsehash_SparseHashMapType);
}
