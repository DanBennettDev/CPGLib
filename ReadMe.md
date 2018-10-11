## MatsuokaEngine

A STATIC LIBRARY for musical creativity with Central Pattern Generators

Top level class for most purposes is MatsuokaEngine
But may be useful to use other classes in isolation (e.g. MatsuNode may be easier to use if you just want a single oscillator)

Supports threadsafe operation - parameter changes are queued for execution using the doQueuedActions() method
So, for example, the GUI thread might call parameter update methods (marked QUEUED ACTION) as and when required, and these requests will be queued for actioning by the signal thread. They will only be actioned when doQueuedActions is called - ideally in the DSP / signal-rate thread. I would suggest calling doQueuedActions at the start of each signal vector calculation.

