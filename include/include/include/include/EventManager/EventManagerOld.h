#ifndef EventManager_h__
#define EventManager_h__

#include "Bloco.h"

class DebugView;

typedef CILHashedString EventType;
class BLOCO_API ILuaable
{
public:
	virtual LuaObject VGetLuaEventData(void) const = 0;

	//Serializes the event data into the LuaObject.
	virtual void VBuildLuaEventData(void) = 0;

	//Called when testing whether or not the event has been serialized for script
	//(this allows us to only serialize ONCE per event trigger).
	virtual bool VHasLuaEventData( void ) const = 0;
};

class BLOCO_API IEventData;
typedef shared_ptr<IEventData> BLOCO_API IEventDataPtr;

class BLOCO_API IEventData : public ILuaable
{
public:
	virtual const EventType & VGetEventType( void ) const = 0;
	virtual float VGetTimeStamp() const = 0;
	virtual void VSerialize(std::ostringstream &out) const = 0;
	virtual string VToString()  = 0;
};

class BLOCO_API BaseEventData : public IEventData
{
public:
	explicit BaseEventData( const float timeStamp = 0.0f )
		: m_TimeStamp( timeStamp )
		, m_bHasLuaEventData( false )	//By default, the event does not have any Lua event data.
	{
	}

	virtual ~BaseEventData()	{ }

	//Called when sending the event data over to the script-side listener.
	virtual LuaObject VGetLuaEventData(void) const = 0;

	//Serializes the event data into the LuaObject.
	virtual void VBuildLuaEventData(void) = 0;

	//Called when testing whether or not the event has been serialized for script
	//(this allows us to only serialize ONCE per event trigger).
	virtual bool VHasLuaEventData( void ) const
	{
		return m_bHasLuaEventData;
	}

	//Returns the type of the event
	virtual const EventType & VGetEventType( void ) const = 0;

	float VGetTimeStamp( void ) const
	{
		return m_TimeStamp;
	}

	//Serializing for network out.
	virtual void VSerialize(std::ostringstream &out) const
	{
	};

	virtual string VToString() { return "Check this shit";};

protected:
	const float m_TimeStamp;
	bool m_bHasLuaEventData;	//We will build that *only if necessary* (i.e., there is a script-side listener).
};

// Helper for events that require no data (still need to derive for the EventType, tho).
class BLOCO_API EmptyEventData : public BaseEventData
{
public:
	explicit EmptyEventData( const float timeStamp = 0.0f )
		: BaseEventData( timeStamp )
	{
	}

	virtual ~EmptyEventData()	{ }

	//Called when sending the event data over to the script-side listener.
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}

	//Serializes the event data into the LuaObject.
	virtual void VBuildLuaEventData(void);

private:
	LuaObject m_LuaEventData;
};

//Note:  This event type will not serialize for Lua listeners.
template < typename T >
struct BLOCO_API EvtData : public BaseEventData
{
private:
	EvtData();					// disable default construction
	EvtData(const EvtData &);	// disable copy construction
	T m_Value;

	LuaObject	m_LuaEventData;
public:
	explicit EvtData<T>( T n )
	{
		m_Value = n;
	}

	const T GetValue() { return m_Value; }

	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void)
	{
		assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

		//Get the global state.
		LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
		//m_LuaEventData.AssignNewTable( pState );

		m_bHasLuaEventData = true;
	}
};

typedef EvtData<int> BLOCO_API EvtData_Int;

// struct EvtData_ScriptEvtData					- Chapter 11, page 328
//This type of event data is created by script-defined events.
struct BLOCO_API EvtData_ScriptEvtData : public BaseEventData
{
public:
	virtual const EventType & VGetEventType( void ) const
	{
		return m_EventType;
	}
	virtual string VToString()
	{
		return string("not implemented");
	}
	EvtData_ScriptEvtData( const EventType & eventType, const LuaObject & srcData )
		: m_EventType( eventType )
		, m_LuaEventData( srcData )
	{
		m_bHasLuaEventData = true;	//Our Lua event data got passed into us!
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr (DEBUG_CLIENTBLOCK EvtData_ScriptEvtData ( m_EventType, m_LuaEventData ) ); 
	}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void)
	{
		assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );
		return;	//Already "built" when the event got created.
	}

private:
	const EventType	m_EventType;	//Type of this event.
	LuaObject	m_LuaEventData;
};

// IEventListener Description
//
// Base type for all event handler objects, note tht this is a
// C++ interface in the classic sense in that it defines the call
// sequence but does not provide any functionality nor aggregate
// any data.
//
// Thus, this is suitable for both single-inheritance use and MAY
// be used safely in a mutiple-inheritance construct, neither is
// imposed or assumed.
class BLOCO_API IEventListener
{
public:

	explicit IEventListener()
	{}
	virtual ~IEventListener()
	{}

	// Returns ascii-text name for this listener, used mostly for
	// debugging

	virtual char const * GetName(void) = 0;

	// Return 'false' to indicate that this listener did NOT
	// consume the event, ( and it should continue to be
	// propogated )
	// 					
	// return 'true' to indicate that this listener consumed the
	//event, ( and it should NOT continue to be propgated )

	virtual bool HandleEvent( IEventData const & event ) = 0
	{		
		// Note: while HandleEvent() MUST be implemented in all
		// derivative classes, (as this function is pure-virtual
		// and thus the hook for IEventListener being an
		// interface definition) a base implementation is
		// provided here to make it easier to wire up do-nothing
		// stubs that can easily be wired to log the
		// unhandled-event (once logging is available)

		// HandleEvent() functioning should be kept as brief as
		// possible as multiple events will need to be evaluated
		// per-frame in many cases.
		return true;
	}
};


// IEventManager Description
//
// This is the object which maintains the list of registered
// events and their listeners
//
// This is a many-to-many relationship, as both one listener can
// be configured to process multiple event types and of course
// multiple listeners can be registered to each event type.
//
// The interface to this contruct uses smart pointer wrapped
// objects, the purpose being to ensure that no object that the
// registry is referring to is destoyed before it is removed from
// the registry AND to allow for the registry to be the only
// place where this list is kept ... the application code does
// not need to maintain a second list.
//
// Simply tearing down the registry (e.g.: destroying it) will
// automatically clean up all pointed-to objects ( so long as
// there are no other oustanding references, of course ).

typedef shared_ptr<IEventListener> BLOCO_API   EventListenerPtr;
//typedef concurrent_queue<IEventDataPtr> ThreadSafeEventQueue;

class BLOCO_API IEventManager
{
public:

	enum eConstants
	{
		kINFINITE = 0xffffffff
	};

	explicit IEventManager( char const * const pName,
		bool setAsGlobal );

	virtual ~IEventManager();

	// Register a handler for a specific event type, implicitly
	// the event type will be added to the known event types if
	// not already known.
	//
	// The function will return false on failure for any
	// reason. The only really anticipated failure reason is if
	// the input event type is bad ( e.g.: known-ident number
	// with different signature text, or signature text is empty
	// )

	virtual bool VAddListener ( EventListenerPtr const & inHandler,
		EventType const & inType ) = 0;

	// Remove a listener/type pairing from the internal tables
	//
	// Returns false if the pairing was not found.

	virtual bool VDelListener ( EventListenerPtr const & inHandler,
		EventType const & inType ) = 0;

	// Fire off event - synchronous - do it NOW kind of thing -
	// analogous to Win32 SendMessage() API.
	//
	// returns true if the event was consumed, false if not. Note
	// that it is acceptable for all event listeners to act on an
	// event and not consume it, this return signature exists to
	// allow complete propogation of that shred of information
	// from the internals of this system to outside uesrs.

	virtual bool VTrigger ( IEventData const & inEvent ) const = 0;

	// Fire off event - asynchronous - do it WHEN the event
	// system tick() method is called, normally at a judicious
	// time during game-loop processing.
	//
	// returns true if the message was added to the processing
	// queue, false otherwise.

	virtual bool VQueueEvent ( IEventDataPtr const & inEvent ) = 0;

	// Find the next-available instance of the named event type
	// and remove it from the processing queue.
	//
	// This may be done up to the point that it is actively being
	// processed ...  e.g.: is safe to happen during event
	// processing itself.
	//
	// if 'allOfType' is input true, then all events of that type
	// are cleared from the input queue.
	//
	// returns true if the event was found and removed, false
	// otherwise

	virtual bool VAbortEvent ( EventType const & inType,
		bool allOfType = false ) = 0;

	// Allow for processing of any queued messages, optionally
	// specify a processing time limit so that the event
	// processing does not take too long. Note the danger of
	// using this artificial limiter is that all messages may not
	// in fact get processed.
	//
	// returns true if all messages ready for processing were
	// completed, false otherwise (e.g. timeout )

	virtual bool VTick ( unsigned long maxMillis = kINFINITE ) = 0;

	// --- information lookup functions ---

	// Validate an event type, this does NOT add it to the
	// internal registry, only verifies that it is legal (
	// e.g. either the ident number is not yet assigned, or it is
	// assigned to matching signature text, and the signature
	// text is not empty ).

	virtual bool VValidateType( EventType const & inType ) const = 0;

	// hook debug view in
	void SetDebugView(DebugView* pDebugView)
	{
		m_pDebugListener = pDebugView;
	}
	// remove debug view
	void RemoveDebugView()
	{
		m_pDebugListener = NULL;
	}

	DebugView*		m_pDebugListener;		// for debugging
											// gets all events
private:

	// internal use only accessor for the static methods in the
	// helper to use to get the active global instance.

	static IEventManager * Get();

	// These methods are declared friends in order to get access to the
	// Get() method. Since there is no other private entity declared
	// in this class this does not break encapsulation, but does allow
	// us to do this without requiring macros or other older-style
	// mechanims.

	friend bool BLOCO_API safeAddListener( EventListenerPtr const & inHandler,
		EventType const & inType );

	friend bool BLOCO_API safeDelListener( EventListenerPtr const & inHandler,
		EventType const & inType );

	friend bool BLOCO_API safeTriggerEvent( IEventData const & inEvent );

	friend bool BLOCO_API safeQueEvent( IEventDataPtr const & inEvent );
	friend bool BLOCO_API threadSafeQueEvent( IEventDataPtr const & inEvent );

	friend bool BLOCO_API safeAbortEvent( EventType const & inType,
		bool allOfType = false );

	friend bool BLOCO_API safeTickEventManager( unsigned long maxMillis =
		IEventManager::kINFINITE );

	friend bool BLOCO_API safeValidateEventType( EventType const & inType );

	friend void BLOCO_API safeSetDebugView(DebugView* pDebugView);
	friend void BLOCO_API safeRemoveDebugView();
};

// Lastly, these methods are used for easy-access methods to carry
// out basic operations without needing to pass around a pointer-to
// ( or some other construct ) for sharing a global event manager.
bool BLOCO_API  safeAddListener( EventListenerPtr const & inHandler, EventType const & inType );
bool BLOCO_API  safeDelListener( EventListenerPtr const & inHandler, EventType const & inType );
bool BLOCO_API  safeTriggerEvent( IEventData const & inEvent );
bool BLOCO_API  safeQueEvent( IEventDataPtr const & inEvent );
bool BLOCO_API  threadSafeQueEvent( IEventDataPtr const & inEvent );
bool BLOCO_API  safeAbortEvent( EventType const & inType, bool allOfType /* = false */ );
bool BLOCO_API  safeTickEventManager( unsigned long maxMillis /* = IEventManager::kINFINITE */ );
bool BLOCO_API  safeValidateEventType( EventType const & inType );

#endif // EventManager_h__