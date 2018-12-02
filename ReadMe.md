# A Static C++ Library for Creativity with Central Pattern Generators

For background see [this page](https://danbennettdev.github.io/projects/cpg_creativity.html)

The library is documented using doxygen - see index.html in ./doxygen/html/

## Overview

This is a static library which implements a Central Pattern Generator (%CPG) network - a neural network that generates adaptive rhythm. Using its default configuration this library will let you simulate a CPG network at a sample-rate (DEFAULTSAMPLERATE) suitable for generating event triggers and control-rate signals. It will generate an event-trigger once per cycle of each node in the network - you can then use these triggers in your application to drive visual or sonic events. The library allows you to build a rhythm-generating CPG network and interact with it in a fairly straightforward way. You can add and change connections between nodes, directly set the cycle frequencies of nodes, and apply rhythmic quantisation to the outputs. It is possible to use the library in many other ways - which will probably become apparent as you investigate further.

The library is mainly focused on creative use - e.g. for music. Several aspects of the library follow from this - for example, the frequency of the first node - node 0 - is taken as the root frequency or tempo of the system, and there are facilities to set the rates of other nodes relative to this - a setup useful for both rhythm generation and audio synthesis (though audio synthesis with this library is fairly computationally expensive).  %CPG networks are interesting for their generative and adaptive rhythmic properties. They demonstrate an "entrainment" behaviour: in the absence of an input they will repeat their own "natural" rhythmic firing pattern, based on system parameters, but will also synchronise, flexibly to incoming periodic signals. This gives them great potential when building systems for users to interact with computational rhythms. 

With the right facilities (as provided by this library) %CPGs can also be tuned manually by users, which also makes them suitable for building real-time, interactive, generative systems - such as my own "Neurythmic" system. In this case %CPGs are interesting for rhythm generation, since while their entrainment property helps ensure rhythmic coherence, they are not bound to a rigid rhythmic grid in the way that most rhythm sequencing systems are. It is easy to use these systems to create and manipulate expressive, asymmetrical, messy and evolving rhythms which still retain a sense of pattern. 

However, %CPG networks are fairly complex systems: each node of a %CPG network has about 6 parameters even before it is connected to other nodes - which adds at least 1 parameter per input. To indicate just two more complexities, there is no completely accurate way of predicting frequency from equation parameters, and the response of the system to inputs signals varies depending on the signal's waveshape and the relationship of the signal's frequency to its own frequency.  

This library simplifies interaction considerably - providing helper-methods and interfaces which allow for more intuitive calibration and control. It allows direct and accurate control of frequency, and intuitive control of connection weights. It also provides facilities for synchronising the network rigidly to an external rhythm, generating note-events from the behaviour of the network, and finally includes a flexible rhythmic quantiser, with variable "strength" which allows the rhythms generated to conform more or less precisely to a rhythmic grid.

In addition to these docs you can see <a href="http://www.nime.org/proceedings/2018/nime2018_paper0047.pdf">my paper on Neurythmic</a> - a rhythm sequencer built on top of this library. 

## Central Pattern Generators (CPGs)

The library is based on a %CPG called Matsuoka's Neural Oscillator - a model of a group of neurons which oscillates under the right conditions. This is a more detailed simulation of a biological neural network than you find in (for example) Deep Learning applications, and many of the concepts and practices of Deep Learning do not apply. The networks tend to be *much* smaller (<< 100 neurons), and are not commonly organised in as Deep learning networks tend to be - in layers with with feedforward connections between them. Instead more various topologies are found, often including internal feedback. 

Another disimilarity is that the "unit" of the network is not neccessarily a single neuron, but often a group of neurons which itself displays oscillatory behaviour. In this case, the library uses the two-neuron variant of Matsuoka's Oscillator as its basic unit. This is a model of two coupled neurons which oscillate at a stable frequency. This two-neuron network is treated as a unit or "node", and we interact with, and manipulate connections between these nodes.

The emphasis in usage is on "tuning" rather than "training". Possibly gradient descent could be used to train these networks, but their feedback relationships, and other mathematical dissimilarities mean that other techniques are generally used. These networks are commonly used in robotics for generating control signals for walking and other repetitive, but neccessarily adaptive, motor patterns. The literature suggests that the networks are tuned manually, interactively and intuitively. The fact that until relatively recently there was little guidance on this process, but work continued should indicate that this is not an unreasonable way to work. It is best to begin by forgetting what you know about neural networks from Machine Learning - though you may later want to introduce some concepts from that discipline.


One advantage of using Matsuoka's Neural Oscillator is that there is a large and easily searchable literature on its properties and use. This should be your first call if you wish to explore computational heuristics for tuning networks (for example swarm optimisation has been used).


# Setup and Config
The default constructor is a good starting point - it sets up the engine for generating rhythms and control signals. In this configuration the network expects to run at a relatively low sample rate (DEFAULTSAMPLERATE). It will generate a single note-trigger for each cycle of each node in the network, at the beginning of the cycle (positive zero crossing). The network is initialised with a single node - node 0, which will cycle at 1hz.

Having called the constructor you will likely want to do a little more set up. This guide assumes you do not change the equation parameters from default

1. In order to control the frequency of the nodes correctly you must calibrate the system to the equation parameters used,  by calling calibrate() 

2. Nodes respond to input input signals in a way that is dependent on the frequency of the input signal, and the frequency of the node. This can make control counter-intuitive. This library offers facilities to calibrate node-behaviour so that it is more consistent to input across a range of frequencies. This can be enabled by calling setConnectionWeightScaling(True). 

3. If you wish to run the system at a different sample rate (e.g. 44100 for sample rate, or 250 for less precise but more efficient control rate calculations), set the correct sample rate using setSampleRate();

## Advanced setup 

If you change the equation parameters from defaults you will need to do a few more things

1. If you wish to control nodes by setting their frequency directly, always call calibrate() after changing parameters to ensure control of frequency is accurate

2. Either call setConnectionWeightScaling(false), or else set a new curve for your parameters using loadConnectionWeightCurve(). See 2. above for more details. At present this library does not offer facilities for generating that new scaling curve for an arbitrary set of parameters. You must generate this curve yourself. One simple but fairly effective way is to simulate 2 nodes with input from one into the other. Raise the weight of the input signal until the node receiving the input fully entrains to the incoming signal (e.g. the point at which it cycles exactly once for every cycle of the parent node. It is worth allowing an adjustment period of e.g. 2 full cycles of the parent node). The X value of the point is the frequency of the incoming signal as a multiple of the frequency of the receiving node. The Y value is the weight at entrainment. Repeat this for a number of frequencies. The default curve uses values at 0.25f, 0.333f, 0.5f, 1.111f, 1.333f, 2.f, 3.f, 4.f, 6.f, 8.f


# Controlling the network / Queued Actions

This library is designed for cases where a control thread runs at a different rate to the signal generation. A common pattern in interactive GUIs and audio applications. To accomodate this, most control methods are not executed immediately, but added to a queue when called. They are only executed when doQueuedActions() is called. These methods are commented QUEUED ACTION. 

This allows a slower interface thread to dispatch control commands without blocking a faster signal-calculation thread (e.g. the audio vector thread in MAX/MSP). one good pattern of use (e.g. when the audio thread calculates a signal vector of multiple samples) is to call doQueuedAction() once per vector, before running the signal calculations.


# Running the network
The function step() calculates one sample of the network. This should thus be called as many times per second as your sample rate demands. 

Event triggers (e.g. to trigger notes) can be retrieved using getEvents() - you may wish to run this e.g. once per sample, or once per vector, depending on how accurate you need your event timing to be. If there are no events, a 0-length vector is returned. 

The current-sample output of each node can be retrieved using getNodeOutput().

If you wish to use external signal inputs for the network, you will need to call setNodeExternalInput() for each node you wish to send signal to, per sample.

If you wish to improve efficiency - e.g. to run at audio rate - and do not need many of the facilities the library offers (e.g. the quantiser, phase synchronisation, etc.) then you can call stepBareBones() instead, to only run the signal calculations.


# Events, and the Quantiser

The library provides a rhythm quantiser which allows for more conventional rhythms to be combined with the freer rhythms caharacteristic of CPG generation. The quantiser sits between the network's signals and event generation - so any changes you make that effect node signals will effect the quantiser. This allows the rhythmic output of that node to be forced to conform to a rhythmic grid. All quantiser settings can be set per node, which allows different rhythmic precisions, offsets, time-signatures, etc. to be used across the network for very flexible control of rhythm. The quantiser is synchronised to node 0's tempo. 

The strength of quantisation can be set per node too. At 0 no quantisation is applied, at 1 full quantisation is applied (events are moved to the nearest valid quantiser grid line). In between, events are moved a proportion of the distance towards the nearest valid quantiser grid line. This allows you to vary the amount to which the "expressive" character of the CPG's rhythm is expressed. 

By default the quantiser is off. 4/4 (32 steps per bar) and 3/4 (24 steps per bar) grids are available using setNodeQuantiser_Grid(). You can also set the grid multiple (e.g. for a 32 step bar, a multiple of 4 results in 32 / 4 = 8 steps per bar), for coarser or finer grids, and offset the grid by a number of units of the parent grid (e.g. for a 32 step grid, with a multiplier of 4, and an offset of 2, events will appear at divisions 2,6,10,14,18,22,24,28).


All events are delayed by a small amount relative to the network signals - this is done in order to minimise rhythmic distortion introduced by the quantiser, and to allow quantised and non-quantised events to be used alongside one-another. If you wish to use network signals alongside the events, you will need to set `matchQuantiser` to true, when getting the signals using getNodeOutput().

By default the quantiser is off for every node. It must be set per node.


