#ifndef PhysicEventListener_h__
#define PhysicEventListener_h__

#include "Bloco.h"


// Event listener used for watching and reacting to physics
// based events

class IGamePhysics;

class PhysicsEventListener : public IEventListener
{
public:
	
	explicit PhysicsEventListener( IGamePhysics *pPhysics );
	virtual ~PhysicsEventListener();
	virtual char const * GetName(void) { return "PhysicsEventListener"; }
	virtual bool HandleEvent( IEventData const & event );

private:

	IGamePhysics *m_pPhysics;
};


#endif