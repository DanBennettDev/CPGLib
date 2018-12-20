#include "../matsu_signal_calcs.h"

// Wrap the matsu_signal_calcs.h C header in Boost-friendly C++
struct signal_calcs
{
    matsuoka_internals internals;

	static double next_val_rk(double in, double t1, double t2,
		                     double c1, double c2, double b, double g,
		                     matsuoka_internals *state)
    {
        return  matsuoka_calc_nextVal_RK(in, t1, t2, c1, c2, b, g, state);
    }
};

#include <boost/python.hpp>
 
using namespace boost::python;

BOOST_PYTHON_MODULE(cpglib)
{
    class_<matsuoka_internals>("matsuoka_internals")
        .def_readwrite("x1", &matsuoka_internals::x1)
        .def_readwrite("x2", &matsuoka_internals::x2)
        .def_readwrite("v1", &matsuoka_internals::v1)
        .def_readwrite("v2", &matsuoka_internals::v2)
        .def_readwrite("out", &matsuoka_internals::out);

    class_<signal_calcs>("signal_calcs")
        .def_readwrite("internals", &signal_calcs::internals)
        .def("next_val_rk", &signal_calcs::next_val_rk)
        .staticmethod("next_val_rk");
    ;
}
