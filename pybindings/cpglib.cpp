#include "../matsu_signal_calcs.h"
#include <boost/python.hpp>
 
using namespace boost::python;

BOOST_PYTHON_MODULE(cpglib)
{
    class_<matsuoka_internals>("matsuoka_internals")
        .def_readwrite("x1", &matsuoka_internals::x1)
        .def_readwrite("x2", &matsuoka_internals::x2)
        .def_readwrite("v1", &matsuoka_internals::v1)
        .def_readwrite("v2", &matsuoka_internals::v2)
        .def_readwrite("out", &matsuoka_internals::out)
    ;
}
