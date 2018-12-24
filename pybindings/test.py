import cpglib
import unittest

class MatsuSignalCalcs(unittest.TestCase):
    def runTest(self):
        mats_state = cpglib.signal_calcs()
        next_val = cpglib.signal_calcs.next_val_rk(1, 2, 3, 4, 5, 6, 7, mats_state.internals)
        assert round(next_val, 2) == -1.62, 'Unexpected RK next step value'

class XORRand(unittest.TestCase):
    def runTest(self):
        assert cpglib.XORRand.next() is not None
        assert type(cpglib.XORRand.next()) is int
        assert cpglib.XORRand.next_sig_val() is not None
        assert type(cpglib.XORRand.next_sig_val()) is float

class MatsuNode(unittest.TestCase):
    def setUp(self):
        self.node = cpglib.MatsuNode()

    def runTest(self):
        # Test some converted properties
        self.node.identifier = 2
        assert self.node.identifier is 2

        # Test enum classes
        assert self.node.get_signal_state() == cpglib.MatsuNodeSignalState.nonSignificant

        assert self.node.synch_mode == cpglib.MatsuNodeSynchMode.free
        self.node.synch_mode = cpglib.MatsuNodeSynchMode.synchOnce
        assert self.node.synch_mode == cpglib.MatsuNodeSynchMode.synchOnce

        # Test overloaded member
        assert self.node.get_output(False) == self.node.get_output(0, False)
        assert self.node.get_output(False) != self.node.get_output(1, False)

        # Test vector indexing
        iterable = False
        try:
            iter(self.node.get_input_ids())
        except TypeError:
            pass
        else:
            iterable = True
        assert iterable, "get_input_ids doesnt provide an iterator"

class MatsuokaEngine(unittest.TestCase):
    def setUp(self):
        self.engine = cpglib.MatsuokaEngine()

    def runTest(self):
        # Test callback propagation from py fn to cpp fn pointer
        callback_fn = lambda int_arg, float_arg: int_arg+float_arg
        callback = False
        try:
            self.engine.set_event_callback(callback_fn)
        except:
            pass
        else:
            callback = True
        assert callback, "set_event_callback didn't receive python function"

        # Test Initialised engine using one other, using sample_rate property
        assert self.engine.sample_rate == 1000
        self.engine.sample_rate = 2000
        self.engine.do_queued_actions()
        assert self.engine.sample_rate == 2000
        new_engine = cpglib.MatsuokaEngine(self.engine)
        assert self.engine.sample_rate == 2000

if __name__ == "__main__":
    print('\nTesting bindings...\n')
    unittest.main()
