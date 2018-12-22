#include "../matsu_signal_calcs.h"
#include "../XORRand.h"
#include "../matsuNode.h"
#include "../CPG.h"
#include "../QuantisedEventQueue.h"
#include "../MatsuokaEngine.h"

#include <functional>
#include <string>
#include <vector>

// Wrap the matsu_signal_calcs.h C header in something Boost-friendly
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
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
 
using namespace boost::python;

// Delay python obj to std::function MatsuokaEngine.h
void MatsuokaEngine_setEventCallback(MatsuokaEngine& self, object pycallback)
{
  return self.setEventCallback(pycallback);
}

BOOST_PYTHON_MODULE(cpglib)
{
    /* matsu_signal_calcs.h
     *
     */
    class_<matsuoka_internals>("matsuoka_internals")
        .def_readwrite("x1", &matsuoka_internals::x1)
        .def_readwrite("x2", &matsuoka_internals::x2)
        .def_readwrite("v1", &matsuoka_internals::v1)
        .def_readwrite("v2", &matsuoka_internals::v2)
        .def_readwrite("out", &matsuoka_internals::out)
    ;

    class_<signal_calcs>("signal_calcs")
        .def_readwrite("internals", &signal_calcs::internals)
        .def("next_val_rk", &signal_calcs::next_val_rk)
        .staticmethod("next_val_rk");
    ;

    /* XORRand.h
     *
     */
    class_<XORRand>("XORRand")
        .def("next_sig_val", &XORRand::nextSigVal)
        .staticmethod("next_sig_val")
        .def("next", &XORRand::next)
        .staticmethod("next")
    ;

    /* matsuNode.h
     *
     */
    enum_<MatsuNode::signalState>("MatsuNodeSignalState")
        .value("nonSignificant", MatsuNode::signalState::nonSignificant)
        .value("zeroXup", MatsuNode::signalState::zeroXup)
        .value("zeroXdown", MatsuNode::signalState::zeroXdown)
        .value("firstPeak", MatsuNode::signalState::firstPeak)
        .value("firstTrough", MatsuNode::signalState::firstTrough)
        .export_values()
    ;

    enum_<MatsuNode::matsuParam>("MatsuNodeParam")
        .value("T1", MatsuNode::matsuParam::T1)
        .value("T2", MatsuNode::matsuParam::T2)
        .value("T", MatsuNode::matsuParam::T)
        .value("T2OverT1", MatsuNode::matsuParam::T2OverT1)
        .value("C1", MatsuNode::matsuParam::C1)
        .value("C2", MatsuNode::matsuParam::C2)
        .value("C", MatsuNode::matsuParam::C)
        .value("B", MatsuNode::matsuParam::B)
        .value("G", MatsuNode::matsuParam::G)
        .export_values()
    ;

    enum_<MatsuNode::matsuInternal>("MatsuNodeInternal")
        .value("X1", MatsuNode::matsuInternal::X1)
        .value("X2", MatsuNode::matsuInternal::X2)
        .value("V1", MatsuNode::matsuInternal::V1)
        .value("V2", MatsuNode::matsuInternal::V2)
        .export_values()
    ;

    enum_<MatsuNode::synchMode>("MatsuNodeSynchMode")
        .value("free", MatsuNode::synchMode::free)
        .value("synchOnce", MatsuNode::synchMode::synchOnce)
        .value("synchLock", MatsuNode::synchMode::synchLock)
        .export_values()
    ;

    // Manage overloaded functions, and ignore default args for ease
    double (MatsuNode::*get_output1)(bool) const = &MatsuNode::getOutput;
    double (MatsuNode::*get_output2)(unsigned, bool) const = &MatsuNode::getOutput;

    void (MatsuNode::*set_params1)(double, double, double, double, double, double)
            = &MatsuNode::setParams;
    void (MatsuNode::*set_params2)(double, double, double, double, double)
            = &MatsuNode::setParams;

    void (MatsuNode::*mnReset1)() = &MatsuNode::reset;
    void (MatsuNode::*mnReset2)(double, double, double, double) = &MatsuNode::reset;

    // Manage types that need support for python iteration
    class_<std::vector<unsigned>>("std::vector<unsigned>")
        .def(vector_indexing_suite<std::vector<unsigned>>())
    ;

    // NOTE: hierarchy management functions aren't included, for now
    class_<MatsuNode>("MatsuNode")
        .def(init<unsigned>())
        .def(init<unsigned, double, double, double, double, double>())
        .def(init<unsigned, double, double, double, double, double, double>())

        .add_property("identifier", &MatsuNode::getIdentifier,
                                    &MatsuNode::setIdentifier)
        .add_property("delay_line_length", &MatsuNode::getDelayLineLength,
                                           &MatsuNode::setDelayLineLength)
        .add_property("freq_compensation", &MatsuNode::getFreqCompensation,
                                           &MatsuNode::setFreqCompensation)
        .add_property("active", &MatsuNode::isActive,
                                &MatsuNode::setIsActive)
        .add_property("synch_mode", &MatsuNode::getSynchMode,
                                    &MatsuNode::setSynchMode)

        .def("do_calc_step", &MatsuNode::doCalcStep)
        .def("do_auxiliary_step_actions", &MatsuNode::doAuxiliaryStepActions)
        .def("get_input", &MatsuNode::getInput)
        .def("get_output", get_output1)
        .def("get_output", get_output2)
        .def("get_last_maxima", &MatsuNode::getLastMaxima)
        .def("get_last_minima", &MatsuNode::getLastMinima)
        .def("get_signal_state", &MatsuNode::getSignalState)
        .def("get_internal", &MatsuNode::getInternal)
        .def("get_param", &MatsuNode::getParam)
        .def("get_frequency", &MatsuNode::getFrequency)
        .def("get_input_ids", &MatsuNode::getInputIds)
        .def("get_input_weight", &MatsuNode::getInputWeight)
        .def("get_input_delay", &MatsuNode::getInputDelay)
        .def("get_input_node", &MatsuNode::getInputNode,
             return_internal_reference<>())
        .def("get_self_noise_amount", &MatsuNode::getSelfNoiseAmount)
        .def("get_output_delay", &MatsuNode::getOutputDelay)
        .def("is_input", &MatsuNode::isInput)
        .def("set_t", &MatsuNode::set_t)
        .def("set_t1", &MatsuNode::set_t1)
        .def("set_t2", &MatsuNode::set_t2)
        .def("set_t2_over_t1", &MatsuNode::set_t2_over_t1)
        .def("set_b", &MatsuNode::set_b)
        .def("set_g", &MatsuNode::set_g)
        .def("set_c", &MatsuNode::set_c)
        .def("set_c1", &MatsuNode::set_c1)
        .def("set_c2", &MatsuNode::set_c2)
        .def("set_params", set_params1)
        .def("set_params", set_params2)
        .def("set_frequency", &MatsuNode::setFrequency)
        .def("set_driven_mode", &MatsuNode::setDrivenMode)
        .def("drive_output", &MatsuNode::driveOutput)
        .def("set_input_weight", &MatsuNode::setInputWeight)
        .def("set_input_delay", &MatsuNode::setInputDelay)
        .def("set_self_noise_amount", &MatsuNode::setSelfNoiseAmount)
        .def("add_input", &MatsuNode::addInput)
        .def("remove_input", &MatsuNode::removeInput)
        .def("clear_inputs", &MatsuNode::clearInputs)
        .def("set_external_input", &MatsuNode::setExternalInput)
        .def("calc_freq_compensation", &MatsuNode::calcFreqCompensation)
        .def("reset_frequency_compensation", &MatsuNode::resetFreqCompensation)
        .def("reset", mnReset1)
        .def("reset", mnReset2)
        .def("reset_change_flag_params", &MatsuNode::resetChangeFlag_Params)
        .def("reset_change_flag_inputs", &MatsuNode::resetChangeFlag_Inputs)
        .def("has_changed_params", &MatsuNode::hasChanged_Params)
        .def("has_changed_inputs", &MatsuNode::hasChanged_Inputs)
        .def("set_node_output_delay", &MatsuNode::setNodeOutputDelay)
        .def("clone", &MatsuNode::clone)
    ;

    /* QuantisedEventQueue.h
     *
     */
    class_<QuantisedEventQueue::outputEvent>("OutputEvent")
        .def_readwrite("node_id", &QuantisedEventQueue::outputEvent::nodeID)
        .def_readwrite("velocity", &QuantisedEventQueue::outputEvent::velocity)
    ;

    enum_<QuantisedEventQueue::gridType>("QuantisedEventQueueGridType")
        .value("unQuantised", QuantisedEventQueue::gridType::unQuantised)
        .value("_24th", QuantisedEventQueue::gridType::_24th)
        .value("_32nd", QuantisedEventQueue::gridType::_32nd)
        .export_values()
    ;

    /* CPG.h
     *
     */
    class_<CPG::input>("CPGInput")
        .def_readwrite("source_id", &CPG::input::sourceID)
        .def_readwrite("weight", &CPG::input::weight)
        .def_readwrite("phase", &CPG::input::phase)
    ;
    class_<std::vector<CPG::input>>("std::vector<CPG::input>")
        .def(vector_indexing_suite<std::vector<CPG::input>>())
    ;

    /* MatsuokaEngine.h
     *
     */
    enum_<MatsuokaEngine::externalSync>("MatsuokaEngineExternalSync")
        .value("none", MatsuokaEngine::externalSync::none)
        .value("driving", MatsuokaEngine::externalSync::driving)
        .value("reseting", MatsuokaEngine::externalSync::reseting)
        .export_values()
    ;

    void (MatsuokaEngine::*step1)() = &MatsuokaEngine::step;
    void (MatsuokaEngine::*step2)(unsigned) = &MatsuokaEngine::step;

    bool (MatsuokaEngine::*load_conn1)(std::string)
        = &MatsuokaEngine::loadConnectionWeightCurve;
    bool (MatsuokaEngine::*load_conn2)(std::vector<float>, std::vector<float>)
        = &MatsuokaEngine::loadConnectionWeightCurve;

    void (MatsuokaEngine::*meReset1)() = &MatsuokaEngine::reset;
    void (MatsuokaEngine::*meReset2)(unsigned, double, double, double, double)
        = &MatsuokaEngine::reset;

    void (MatsuokaEngine::*set_conn1)(unsigned, unsigned, double)
        = &MatsuokaEngine::setConnection;
    void (MatsuokaEngine::*set_conn2)(unsigned, unsigned, double, double)
        = &MatsuokaEngine::setConnection;

    void (MatsuokaEngine::*setQuantiseAmt1)(float)
        = &MatsuokaEngine::setQuantiseAmount;
    void (MatsuokaEngine::*setQuantiseAmt2)(unsigned, float)
        = &MatsuokaEngine::setQuantiseAmount;

    float (MatsuokaEngine::*getQuantiseAmt1)() = &MatsuokaEngine::getQuantiseAmount;
    float (MatsuokaEngine::*getQuantiseAmt2)(unsigned) = &MatsuokaEngine::getQuantiseAmount;

    class_<MatsuokaEngine>("MatsuokaEngine")
        .def(init<unsigned>())
        .def(init<unsigned, bool, bool, bool>())
        .def(init<const MatsuokaEngine&>())
        .add_property("frequency_compensation", &MatsuokaEngine::getFrequencyCompensation,
                      &MatsuokaEngine::setFreqCompensation)
        .add_property("sample_rate", &MatsuokaEngine::getSampleRate,
                      &MatsuokaEngine::setSampleRate)
        .def("step", step1)
        .def("step", step2)
        .def("step_barebones", &MatsuokaEngine::stepBareBones)
        .def("get_node_list", &MatsuokaEngine::getNodeList)
        .def("node_exists", &MatsuokaEngine::nodeExists)
        .def("get_node", &MatsuokaEngine::getNode,
             return_value_policy<copy_const_reference>())
        .def("set_connection_weight_scaling", &MatsuokaEngine::setConnectionWeightScaling)
        .def("load_connection_weight_curve", load_conn1)
        .def("load_connection_weight_curve", load_conn2)
        .def("set_unity_connection_weight", &MatsuokaEngine::setUnityConnectionWeight)
        .def("get_node_frequency", &MatsuokaEngine::getNodeFrequency)
        .def("get_node_parent", &MatsuokaEngine::getNodeParent)
        .def("get_node_children", &MatsuokaEngine::getNodeChildren)
        .def("get_node_output", &MatsuokaEngine::getNodeOutput)
        .def("get_node_phase_offset", &MatsuokaEngine::getNodePhaseOffset)
        .def("get_node_self_noise", &MatsuokaEngine::getNodeSelfNoise)
        .def("get_node_input_phase", &MatsuokaEngine::getNodeInputPhase)
        .def("calibrate", &MatsuokaEngine::calibrate)
        .def("calc_frequency_compensation", &MatsuokaEngine::calcFrequencyCompensation)
        .def("quit_frequency_compensation_calcs",
                &MatsuokaEngine::quitFrequencyCompensationCalcs)
        .def("add_child", &MatsuokaEngine::addChild)
        .def("add_node", &MatsuokaEngine::addNode)
        .def("reset", meReset1)
        .def("reset", meReset2)
        .def("set_connection", set_conn1)
        .def("set_connection", set_conn1)
        .def("set_connection_phase_offset", &MatsuokaEngine::setConnectionPhaseOffset)
        .def("remove_connection", &MatsuokaEngine::removeConnection)
        .def("delete_node", &MatsuokaEngine::deleteNode)
        .def("set_param_t2_over_t1", &MatsuokaEngine::setParam_t2Overt1)
        .def("set_param_c", &MatsuokaEngine::setParam_c)
        .def("set_param_b", &MatsuokaEngine::setParam_b)
        .def("set_param_g", &MatsuokaEngine::setParam_g)
        .def("set_node_frequency", &MatsuokaEngine::setNodeFrequency)
        .def("set_node_phase_offset", &MatsuokaEngine::setNodePhaseOffset)
        .def("set_node_self_noise", &MatsuokaEngine::setNodeSelfNoise)
        .def("set_node_synch_mode", &MatsuokaEngine::setNodeSynchMode)
        .def("set_node_external_input", &MatsuokaEngine::setNodeExternalInput)
        .def("reset_node_change_flag_params", &MatsuokaEngine::resetNodeChangeFlag_Params)
        .def("reset_node_change_flag_inputs", &MatsuokaEngine::resetNodeChangeFlag_Inputs)
        .def("set_node_quantiser_grid", &MatsuokaEngine::setNodeQuantiser_Grid)
        .def("set_node_quantiser_multiple", &MatsuokaEngine::setNodeQuantiser_Multiple)
        .def("set_node_quantiser_offset", &MatsuokaEngine::setNodeQuantiser_Offset)
        .def("get_node_quantiser_multiple", &MatsuokaEngine::getNodeQuantiser_Multiple)
        .def("get_node_quantiser_offset", &MatsuokaEngine::getNodeQuantiser_Offset)
        .def("get_node_quantiser_bardivision", &MatsuokaEngine::getNodeQuantiser_BarDivision)
        .def("set_quantise_amount", setQuantiseAmt1)
        .def("set_quantise_amount", setQuantiseAmt2)
        .def("get_quantise_amount", getQuantiseAmt1)
        .def("get_quantise_amount", getQuantiseAmt2)
        .def("clear", &MatsuokaEngine::clear)
        .def("do_queued_actions", &MatsuokaEngine::doQueuedActions)
        .def("set_shutdown", &MatsuokaEngine::setShutdown)
        .def("set_pause", &MatsuokaEngine::setPause)
        .def("is_shutting_down", &MatsuokaEngine::isShuttingDown)
        .def("is_idle", &MatsuokaEngine::isIdle)
        .def("get_engine_step_counter", &MatsuokaEngine::getEngineStepCounter)
        .def("set_driving_input", &MatsuokaEngine::setDrivingInput)
        .def("set_driven", &MatsuokaEngine::setDriven)
        .def("zero_sync", &MatsuokaEngine::zeroSync)
        .def("set_event_callback", &MatsuokaEngine_setEventCallback)
        .def("get_inputs", &MatsuokaEngine::getInputs)
        .def("get_events", &MatsuokaEngine::getEvents)
        .def("get_node_quantiser_grid", &MatsuokaEngine::getNodeQuantiser_Grid)
    ;
}
