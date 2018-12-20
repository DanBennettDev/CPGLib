import cpglib
import unittest

class MatsuSignalCalcs(unittest.TestCase):
    def runTest(self):
        mats_state = cpglib.signal_calcs()
        next_val = cpglib.signal_calcs.next_val_rk(1, 2, 3, 4, 5, 6, 7, mats_state.internals)
        assert round(next_val, 2) == -1.62, 'Unexpected RK next step value'

if __name__ == "__main__":
    # Collect everything in scope which isn't magic, for sanity checking bindings
    fns = (fn for fn in dir(cpglib) if fn[0:2] != fn[-2:] and fn[-2:] != '__')
    print('\nImported from cpglib:\n{}'.format('\n'.join(sorted(' - '+f for f in fns))))

    print('\nSanity checking bindings...\n')
    unittest.main()
