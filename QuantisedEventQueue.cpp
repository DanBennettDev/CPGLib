#include "QuantisedEventQueue.h"



QuantisedEventQueue::QuantisedEventQueue(unsigned sampleRate, float tempo)
    :   _grid24(0, 24, sampleRate, tempo), 
        _grid32(0,32,sampleRate,tempo),
    _mark24(0, 0),
    _mark32(0, 0)
{
    _tempo = tempo;
    _sampleRate = sampleRate;
    setNoteDelays();
    _quantiseAmount = 1;
	_nullEvent.nodeID = -1;
	

    setNodeGrid(0, gridType::unQuantised);
    for (int i = 1; i < MAX_NODES; i++) {
        setNodeGrid(i, gridType::_32nd);
        setNodeGridMultiple(i, 8);
    }
}

QuantisedEventQueue::QuantisedEventQueue(const QuantisedEventQueue &rhs)
    : _grid24(0, 24, rhs.getSampleRate(), rhs.getTempo()),
    _grid32(0, 32, rhs.getSampleRate(), rhs.getTempo()),
    _mark24(0, 0),
    _mark32(0, 0)
{
    _tempo = rhs.getTempo();
    _sampleRate = rhs.getSampleRate();
    setNoteDelays();
}




void QuantisedEventQueue::addEvent(outputEvent e)
{
    if (nodes[e.nodeID].grid!= gridType::unQuantised) {
        unsigned mult = nodes[e.nodeID].multiple;
        unsigned off = nodes[e.nodeID].offset;
        queuedEvent qe ;
        qe.event = e;
        // add to correct place in relevant queue
        switch (nodes[e.nodeID].grid) {
            case gridType::_32nd:
            {
				_grid32.setQuantiseAmount(nodes[e.nodeID].amount * _quantiseAmount);
				qe.queueMarker = _grid32.getNoteCoordinate(mult, off);
				_eventQueue32.add(qe);
                return;
            }
            case gridType::_24th:
            {
				_grid24.setQuantiseAmount(nodes[e.nodeID].amount * _quantiseAmount);
				qe.queueMarker = _grid24.getNoteCoordinate(mult, off);
				_eventQueue24.add(qe);
                return;
            }
        }

    } else {
		queuedEvent qe;
		qe.event = e;
		qe.queueMarker = _grid32.getNoteCoordinateUnquantised();
		_eventQueue32.add(qe);
    }
}



void QuantisedEventQueue::tick()
{
    _mark32 = _grid32.tick();
    _mark24 = _grid24.tick();
}


QuantisedEventQueue::outputEvent QuantisedEventQueue::getNote()
{
	outputEvent *e = _eventQueue24.getEvent(_mark24);
	if (e != nullptr) { return *e; };
	e = _eventQueue32.getEvent(_mark32);
	if (e != nullptr) { 
		if (e->nodeID == 0) {
			syncToBarStart();
		}
		return *e; 
	};


    return _nullEvent;
}

void QuantisedEventQueue::setSampleRate(unsigned sampleRate)
{
    _sampleRate = sampleRate;
    _grid24.setSampleRate(sampleRate);
    _grid32.setSampleRate(sampleRate);
    setNoteDelays();

}

void QuantisedEventQueue::setTempo(float tempo)
{
    float changeTempo = (tempo/_tempo)-1;
	if (changeTempo > TEMPOCHANGETHRESHOLD || (-1*changeTempo) > TEMPOCHANGETHRESHOLD) {
		_tempo = tempo;
		_grid24.setTempo(tempo);
		_grid32.setTempo(tempo);
		setNoteDelays();
	}
}

void QuantisedEventQueue::setQuantiseAmount(float amount)
{
    if (amount < 0) {
        amount = 0;
    } else if (amount > 1) {
        amount = 1;
    }
    _quantiseAmount = amount;
    _grid24.setQuantiseAmount(amount);
    _grid32.setQuantiseAmount(amount);
}


void QuantisedEventQueue::setQuantiseAmount(unsigned node, float amount)
{
	if (amount < 0) {
		amount = 0;
	}
	else if (amount > 1) {
		amount = 1;
	}
	nodes[node].amount = amount;
}



float QuantisedEventQueue::getQuantiseAmount()
{
    return _quantiseAmount;
}

float QuantisedEventQueue::getQuantiseAmount(unsigned node)
{
	return nodes[node].amount;
}



unsigned QuantisedEventQueue::getSampleRate() const
{
    return _sampleRate;
}

float QuantisedEventQueue::getTempo() const
{
    return _tempo;
}

void QuantisedEventQueue::setNodeGrid(unsigned nodeID, gridType grid)
{
    if (nodeID == 0) {
        // node 0 must remain free running - it syncs the rest
        return;
    }
    nodes[nodeID].grid = grid;
}


void QuantisedEventQueue::setNodeGridMultiple(unsigned nodeID, unsigned mult)
{
    nodes[nodeID].multiple = mult;
}

void QuantisedEventQueue::setNodeGridOffset(unsigned nodeID, unsigned off)
{
    nodes[nodeID].offset = off;
}


QuantisedEventQueue::gridType QuantisedEventQueue::getNodeGrid(unsigned nodeID)
{
    return nodes[nodeID].grid;
}

unsigned QuantisedEventQueue::getNodeGridMultiple(unsigned nodeID)
{
    return nodes[nodeID].multiple;
}

unsigned QuantisedEventQueue::getNodeGridOffset(unsigned nodeID)
{
    return nodes[nodeID].offset;

}

unsigned QuantisedEventQueue::getNodeBarDivision(unsigned nodeID)
{
    float grid;
    if (nodes[nodeID].grid == gridType::_24th) {
        grid = 24;
    } else if (nodes[nodeID].grid == gridType::_32nd) {
        grid = 32;
    } else {
        return 0;
    }

    return (unsigned)(grid / nodes[nodeID].multiple);
}



void QuantisedEventQueue::syncToBarStart()
{
        _grid24.resetToBarStart();
        _grid32.resetToBarStart();
}


void QuantisedEventQueue::setNoteDelays()
{
    // all notes are delayed a 1/4 note - this allows free and 
    // quantised notes to be played alongside each other
    _delayNotes =  (unsigned)((_sampleRate * 60 ) / (_tempo * 4));
    _grid24.setDelay(_delayNotes);
    _grid32.setDelay(_delayNotes);
}

unsigned QuantisedEventQueue::getNoteDelay() const
{
	return _delayNotes;
}