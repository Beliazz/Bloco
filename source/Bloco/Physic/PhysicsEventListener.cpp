#include "Bloco.h"
#include "PhysicsEventListener.h"

PhysicsEventListener::PhysicsEventListener( IGamePhysics *pPhysics )
	: m_pPhysics(pPhysics)
{
}

PhysicsEventListener::~PhysicsEventListener()
{
}

bool PhysicsEventListener::HandleEvent( IEventData const & event )
{
	const EventType & eventType = event.VGetEventType();

	if ( EvtData_PhysTrigger_Enter::sk_EventType == eventType )
	{
		printf("EvtData_PhysTrigger_Enter\n");
	}
	else if ( EvtData_PhysTrigger_Leave::sk_EventType == eventType )
	{
		printf("EvtData_PhysTrigger_Leave\n");
	}
	else if ( EvtData_PhysCollision::sk_EventType == eventType )
	{
		printf("EvtData_PhysCollision\n");		
	}
	else if ( EvtData_PhysSeparation::sk_EventType == eventType )
	{
		printf("EvtData_PhysSeparation\n");
	}
	else if ( EvtData_RayCast::sk_EventType == eventType )
	{
		const EvtData_RayCast & ed = static_cast< const EvtData_RayCast & >( event );

		m_pPhysics->VRayCast(ed.m_rayStart,ed.m_rayEnd);
	}

	return false;
}

