#ifndef ScriptEventListener_h__
#define ScriptEventListener_h__

#include "Bloco.h"

//--
// class ScriptEventListener				- Chapter 11, page 334
//
// The ScriptEventListener holds a script callback function that responds
// to a particular event.
class ScriptEventListener : public IEventListener
{
public:
	ScriptEventListener( LuaObject explicitHandlerFunction );

	virtual ~ScriptEventListener()
	{
	}
	virtual char const * GetName( void )
	{
		return "Script Listener";
	}
	virtual bool HandleEvent( IEventData const & event );

	const LuaObject & GetHandlerFunction( void ) const
	{
		return m_HandlerFunction;
	}

protected:
	// This function is virtual as sub-classes may pass additional
	// parameters.
	virtual bool VCallLuaFunction( LuaObject & eventData );

	LuaObject m_HandlerFunction;
};

//--
// class ScriptActorEventListener					- Chapter 11, page 342
//
// The ScriptActorEventListener holds a script callback function tied
// to a specific actor, and when called, passes in the actor's script
// data.
class ScriptActorEventListener : public ScriptEventListener
{
public:
	ScriptActorEventListener( LuaObject explicitHandlerFunction, const ActorId actorID );

	virtual ~ScriptActorEventListener()
	{
	}
	virtual char const * GetName( void )
	{
		return "Script Actor Listener";
	}

	ActorId GetActorID( void ) const
	{
		return m_SrcActorID;
	}

private:

	// This will pass the event data object as well as
	// look up our actor's specific script data to pass.
	virtual bool VCallLuaFunction( LuaObject & eventData );

	// Our source actor.
	const ActorId m_SrcActorID;
};

#endif