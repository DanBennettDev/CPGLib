#pragma once
#include <cstdint>
#include <list>
#include <vector>
#include <array>
#include <mutex>
#include "QuantiseGrid_soft.h"
#include "core.h"

#define EVENTPOOLSIZE 1024

/*! \class QuantisedEventQueue
*  \brief Engine: Takes events as inputs and outputs them at the correct quantised interval
*   correct quantised interval depends on settings, quantiser grid size, etc.
*   introduces a delay. Push pull quantiser - moves incoming events to nearest beat
*/

class QuantisedEventQueue
{

public:
    using noteCoordinate = QuantiseGrid_soft::noteCoordinate;

    QuantisedEventQueue(unsigned sampleRate, float tempo);
    QuantisedEventQueue(const QuantisedEventQueue &rhs);

    /// struct specifying for the events the queue quantises
    struct outputEvent
    {
        unsigned nodeID;
        double velocity;

        bool operator < (const outputEvent &rhs) { return nodeID < rhs.nodeID; }
    };

    /// the types of grids available to be quantised to 
    enum class gridType
    {
        unQuantised, _24th, _32nd
    };


    // Thoughts - handling of multiple notes for same coord - take most recent
    //  build QuantisedNoteQueue to handle the note level side of this
    // allows setting of quantisation (or lack of) per node, plus divisions, courseness, offset

    /// move the system forwards 1 sample
    void tick();

    /// add an event to the system
    void addEvent(outputEvent e);

    /// set the sample rate of the system
    void setSampleRate(unsigned sampleRate);

    /// set the tempo in beats per minute
    void setTempo(float tempo);

    /// set the amount of quantisation (0=none, 1=strict)
    void setQuantiseAmount(float amount);
    

	void setQuantiseAmount(unsigned node, float amount);


	/// get the amount of quantisation (0=none, 1=strict)
    float getQuantiseAmount();
	float getQuantiseAmount(unsigned node);

    /// set the quantise grid type for the node
    void setNodeGrid(unsigned nodeID, gridType grid);
    /// sets the grid multiplier for the node (space between grid lines 
    /// that are valid for beat placement)
    void setNodeGridMultiple(unsigned nodeID, unsigned mult);

    /// sets the grid offset 
    void setNodeGridOffset(unsigned nodeID, unsigned off);


	unsigned getNoteDelay() const;

    unsigned getSampleRate() const;
    float getTempo() const;
    gridType getNodeGrid(unsigned nodeID);
    unsigned getNodeGridMultiple(unsigned nodeID);
    unsigned getNodeGridOffset(unsigned nodeID);
    unsigned getNodeBarDivision(unsigned nodeID);

    /// moves to the start of the next bar
    void syncToBarStart();

    /// returns any notes which are ready to be played (call every tick())
    outputEvent getNote();



private:

    struct queuedEvent
    {
        outputEvent event;
        noteCoordinate queueMarker;
		bool active;
		queuedEvent *next;

        queuedEvent():
            event(),
            queueMarker(0, 0),
			active{false},
			next {nullptr}
        {}
    } ;

    struct queuedEvent_free
    {
        outputEvent event;
        float countDown;
        float delta;

        queuedEvent_free() :
            event()
        {
            countDown = 1;
            delta = 1;
        }
    };


    struct nodeDetails
    {
        gridType grid;
        unsigned multiple;
        unsigned offset;
		float amount;


        nodeDetails()
        {
            grid = gridType::unQuantised;
            multiple = 1;
            offset = 0;
			amount = 1;
        }
    };

	class EventQueue
	{

	private:
		std::array<queuedEvent, EVENTPOOLSIZE> _pool;

		queuedEvent *_queueStart;
		queuedEvent *_queueEnd;
		queuedEvent *_cursor;

		queuedEvent* _getMem() {
			for (int i = 0; i < EVENTPOOLSIZE; i++) {
				if (!_pool[i].active) {
					return &_pool[i];
				}
			}
			return nullptr;
		}

		queuedEvent* _place(queuedEvent e) {
			queuedEvent* newEvent = _getMem();
			*newEvent = e;
			newEvent->active = true;
			newEvent->next = nullptr;

			if (e.queueMarker < _queueStart->queueMarker) {
				newEvent->next = _queueStart;
				_queueStart = newEvent;
				return _queueEnd;
			}

			_cursor = _queueStart;
			while (_cursor->next != nullptr && e.queueMarker  < _cursor->next->queueMarker) {
				_cursor = _cursor->next;
			}	
			newEvent->next = _cursor->next;
			_cursor->next = newEvent;
			if (newEvent->next == nullptr) {
				_queueEnd = newEvent;
			}
			return _queueEnd;
		}


	public:
		EventQueue() {
			_queueStart = nullptr;
			_queueEnd = nullptr;
			_cursor = nullptr;
		}

		void add(queuedEvent e) {
			e.next = nullptr;
			e.active = true;
			if (_queueStart == nullptr) {
				_pool[0] = e;
				_queueStart = &_pool[0];
				_queueEnd = &_pool[0];
			} else {
				_place(e);
			}
		}

		outputEvent* getEvent(noteCoordinate currTime) {
			if (_queueStart == nullptr || currTime < _queueStart->queueMarker ) {
				return nullptr;
			}
			outputEvent *ret = &_queueStart->event;
			_queueStart->active = false;
			_queueStart = _queueStart->next;
			return ret;
		}
	};



    void setNoteDelays();
    unsigned _delayNotes;
    float _tempo;
    float _quantiseAmount;
    unsigned _sampleRate;
    nodeDetails nodes[MAX_NODES];
	EventQueue _eventQueue24;
	EventQueue _eventQueue32;
    std::list<queuedEvent_free> _eventQueueFree;
	outputEvent _nullEvent;

    QuantiseGrid_soft _grid24;
    QuantiseGrid_soft _grid32;
    noteCoordinate _mark24;
    noteCoordinate _mark32;
    std::mutex _queue_mutex;


};