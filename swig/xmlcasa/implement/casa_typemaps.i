// Need to replicate
// IntVec
// StringVec
// DoubleVec
// BoolVec
// ComplexVec
//
// Shapes for all the vectors need to be added
//
// Quantities
// Quantity DoubleVec
%{
#include <string>
#include <vector>
#include <complex>
#include <xmlcasa/record.h>
#include <xmlcasa/swigconvert_python.h>

using casac::record;
using casac::variant;
using namespace casac;

%}
%typemap(in) string {
   $1 = string(PyString_AsString($input));
}
%typemap(in) StringVec {
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    for (int i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyString_Check(o))
        $1.value.push_back(PyString_AsString(PyList_GetItem($input,i)));
      else {
        PyErr_SetString(PyExc_TypeError,"list must contain strings");
        return NULL;
      }
    }
  } else {
    if(PyString_Check($input)){
       $1.value.push_back(PyString_AsString($input));
    } else {
       PyErr_SetString(PyExc_TypeError,"not a list");
       return NULL;
    }
  }
}

%typemap(in) complex {
}

%typemap(in) variant {
   $1 = pyobj2variant($input, true);
}

%typemap(in) record {
   if(PyDict_Check($input)){
      $1 = pyobj2variant($input, true).asRecord();      
   } else {
      PyErr_SetString(PyExc_TypeError,"not a dictionary");
   }
}

%typemap(in) BoolVec {
   $1 = new casac::BoolAry;
   if(pyarray_check($input)){
      numpy2vector($input, $1->value, $1->shape);
   } else {
      pylist2vector($input,  $1->value, $1->shape);
   }
}

%typemap(in) IntVec {
   $1 = new casac::IntAry;
   if(pyarray_check($input)){
      numpy2vector($input, $1->value, $1->shape);
   } else {
      pylist2vector($input,  $1->value, $1->shape);
   }
}

%typemap(in) DoubleVec {
   $1 = new casac::DoubleAry;
   if(pyarray_check($input)){
      numpy2vector($input, $1->value, $1->shape);
   } else {
      pylist2vector($input,  $1->value, $1->shape);
   }
}

%typemap(in) ComplexVec {
   $1 = new casac::ComplexAry;
   if(pyarray_check($input)){
      numpy2vector($input, $1->value, $1->shape);
   } else {
      pylist2vector($input,  $1->value, $1->shape);
   }
}

%typemap(out) complex {
}

%typemap(out) variant {
   $result = variant2pyobj($1);
}

%typemap(out) BoolVec {
   $result = ::map_array($1.value, $1.shape);
}

%typemap(out) IntVec {
   $result = ::map_array($1.value, $1.shape);
}

%typemap(out) ComplexVec {
   $result = ::map_array($1.value, $1.shape);
}

%typemap(out) DoubleVec {
   $result = ::map_array($1.value, $1.shape);
}

%typemap(out) StringVec {
   $result = ::map_array($1.value, $1.shape);
/*
   $result = PyList_New($1.size());
   for(int i=0;i<$1.size();i++)
      PyList_SetItem($result, i, PyString_FromString($1[i].c_str()));
*/
}

%typemap(out) RecordVec {
   $result = PyList_New($1.size());
   for(int i=0;i<$1.size();i++){
      const record &val = $1[i];
      PyObject *r = record2pydict(val);
      PyList_SetItem($result, i, r);
   }
}

%typemap(out) record {
   $result = PyDict_New();
   for(record::const_iterator iter = $1.begin(); iter != $1.end(); ++iter){
      const std::string &key = (*iter).first;
      const variant &val = (*iter).second;
      PyObject *v = variant2pyobj(val);
      PyDict_SetItem($result, PyString_FromString(key.c_str()), v);
      Py_DECREF(v);
   }
}
