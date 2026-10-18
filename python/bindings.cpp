#include <kkv/kkv.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;
PYBIND11_MODULE(_kkv, m) {
    m.doc() = "Python bindings for KKV";

    
    m.def("tryParseAsNum", KKV);
}