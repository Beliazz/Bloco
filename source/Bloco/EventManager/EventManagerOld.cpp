#include "Bloco.h"

#include <cstring>

#include "EventManager/EventManager.h"
#include "Application.h"

//Serializes the event data into the LuaObject.
void BLOCO_API EmptyEventData::VBuildLuaEventData(void)
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();

	//We just set a nil object.
	m_LuaEventData.AssignNil( pState );

	//Now we've "got data".
	m_bHasLuaEventData = true;
}

static IEventManager * g_pEventMgr = NULL;
IEventManager BLOCO_API * IEventManager::Get()
{
	return g_pEventMgr;
}

 BLOCO_API IEventManager::IEventManager(
	char const * const pName,
	bool setAsGlobal )
{
	if ( setAsGlobal )
		g_pEventMgr = this;
}

 BLOCO_API IEventManager::~IEventManager()
{
	if ( g_pEventMgr == this )
		g_pEventMgr = NULL;
}



//--

// lastly, some macros that make it less difficult to safely use
// a possibly not-ready system ...

bool BLOCO_API  safeAddListener( EventListenerPtr const & inHandler, EventType const & inType )
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->VAddListener( inHandler, inType );
}
bool BLOCO_API  safeDelListener( EventListenerPtr const & inHandler, EventType const & inType )
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->VDelListener( inHandler, inType );
}
bool BLOCO_API  safeTriggerEvent( IEventData const & inEvent )
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->VTrigger( inEvent );
}
bool BLOCO_API  safeQueEvent( IEventDataPtr const & inEvent )
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->VQueueEvent( inEvent );
}
bool BLOCO_API  safeAbortEvent( EventType const & inType, bool allOfType /*= false*/ )
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->VAbortEvent( inType, allOfType );
}
bool BLOCO_API  safeTickEventManager( unsigned long maxMillis /*= kINFINITE*/ )
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->VTick( maxMillis );
}
bool BLOCO_API  safeValidateEventType( EventType const & inType )
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->VValidateType( inType );
}
void BLOCO_API  safeSetDebugView(DebugView* pDebugView)
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->SetDebugView( pDebugView );
}
void BLOCO_API  safeRemoveDebugView()
{
	assert(IEventManager::Get() && _T("No event manager!"));
	return IEventManager::Get()->RemoveDebugView();
}






