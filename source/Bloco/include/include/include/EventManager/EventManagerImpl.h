#include "Bloco.h"

#include "Scripting\ScriptEventListener.h"

typedef std::vector<EventListenerPtr> EventListenerList;
typedef std::vector<EventType>      EventTypeList;
typedef CILHashedString EventType;


// Helper for events that require no data (still need to derive for the EventType, tho).
class EmptyEventData : public BaseEventData
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

class EventManager : public IEventManager
{
public:

	explicit EventManager( char const * const pName,
									   bool setAsGlobal );

	virtual ~EventManager();

	// --- primary use functions --- make it work ---
	
	// Register a listener for a specific event type, implicitly
	// the event type will be added to the known event types if
	// not already known.
	//
	// The function will return false on failure for any
	// reason. The only really anticipated failure reason is if
	// the input event type is bad ( e.g.: known-ident number
	// with different signature text, or signature text is empty
	// )
	virtual bool VAddListener ( EventListenerPtr const & inListener,
					   EventType const & inType );

	// Remove a listener/type pairing from the internal tables
	//
	// Returns false if the pairing was not found.
	
	virtual bool VDelListener ( EventListenerPtr const & inListener,
					   EventType const & inType );

	// Fire off event - synchronous - do it NOW kind of thing -
	// analogous to Win32 SendMessage() API.
	//
	// returns true if the event was consumed, false if not. Note
	// that it is acceptable for all event listeners to act on an
	// event and not consume it, this return signature exists to
	// allow complete propogation of that shred of information
	// from the internals of this system to outside uesrs.
	virtual bool VTrigger ( IEventData const & inEvent ) const;

	// Fire off event - asynchronous - do it WHEN the event
	// system tick() method is called, normally at a judicious
	// time during game-loop processing.
	//
	// returns true if the message was added to the processing
	// queue, false otherwise.

	virtual bool VQueueEvent ( IEventDataPtr const & inEvent );

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
					  bool allOfType );

	// Allow for processing of any queued messages, optionally
	// specify a processing time limit so that the event
	// processing does not take too long. Note the danger of
	// using this artificial limiter is that all messages may not
	// in fact get processed.
	//
	// returns true if all messages ready for processing were
	// completed, false otherwise (e.g. timeout )

	virtual bool VTick ( unsigned long maxMillis );

	// --- information lookup functions ---
	
	// Validate an event type, this does NOT add it to the
	// internal registry, only verifies that it is legal (
	// e.g. either the ident number is not yet assigned, or it is
	// assigned to matching signature text, and the signature
	// text is not empty ).

	virtual bool VValidateType( EventType const & inType ) const;

	// Get the list of listeners associated with a specific event
	// type

	EventListenerList GetListenerList ( EventType const & eventType ) const;

	// Get the list of known event types

	EventTypeList GetTypeList ( void ) const;

	// Registers an event type for the particular usage desired.
	// ...for an event defined in script:
	void RegisterScriptEvent( const EventType & eventType );
	// ...for an event defined by code, *NOT* callable by script.
	void RegisterCodeOnlyEvent( const EventType & eventType );
	// ...for an event defined by code, but callable by script.  REQUIRES the event type to have a constructor taking a LuaObject.
	template< class T> void RegisterEvent( const EventType & eventType );

private:
	
	// This class holds meta data for each event type, and allows 
	// (or disallows!) creation of code-defined events from script.
	
	// class IRegisteredEvent				- Chapter 11, page 324
	class IRegisteredEvent
	{
	public:
		//Meta data about the type and usage of this event.
		enum eRegisteredEventMetaData
		{
			// Event is defined in script.
			kREMD_ScriptDefined,
			
			//Event is defined by code, and is *NOT* callable from script.
			kREMD_CodeEventOnly,

			//Event is defined by code, but is callable from script.
			kREMD_CodeEventScriptCallable,
		};

		IRegisteredEvent( const eRegisteredEventMetaData metaData )
			: m_MetaData( metaData )
		{
		}

		virtual ~IRegisteredEvent()
		{
		}

		virtual bool VTriggerEventFromScript( LuaObject & srcData ) const = 0;
		virtual bool VQueueEventFromScript( LuaObject & srcData ) const = 0;
		eRegisteredEventMetaData GetEventMetaData( void ) const
		{
			return m_MetaData;
		}
	private:
		const eRegisteredEventMetaData m_MetaData;
	};

	// class ScriptDefinedEvent						- Chapter 11, page 327
	//   Script defined event type.
	class ScriptDefinedEvent : public IRegisteredEvent
	{
	public:
		ScriptDefinedEvent( const EventType & eventType )
			: IRegisteredEvent( IRegisteredEvent::kREMD_ScriptDefined )
			, m_EventType( eventType )
		{
		}

		virtual bool VTriggerEventFromScript( LuaObject & srcData ) const;
		virtual bool VQueueEventFromScript( LuaObject & srcData ) const;

	private:
		//We need to hold onto the event type for when it gets triggered.
		const EventType m_EventType;
	};

	// class CodeOnlyDefinedEvent					- Chapter 11, page 327
	//   Code defined, but script is NOT allowed to create this event type.
	class CodeOnlyDefinedEvent : public IRegisteredEvent
	{
	public:
		explicit CodeOnlyDefinedEvent( void )
			: IRegisteredEvent( IRegisteredEvent::kREMD_CodeEventOnly )
		{
		}

		virtual bool VTriggerEventFromScript( LuaObject & srcData ) const
		{
			assert( 0 && "Attempted to trigger a code-ONLY triggerable event!" );
			return false;
		}

		virtual bool VQueueEventFromScript( LuaObject & srcData ) const
		{
			assert( 0 && "Attempted to queue a code-ONLY queueable event!" );
			return false;
		}
	};

	// class ScriptCallableCodeEvent			- Chapter 11, page 330
	//   Code defined, but also creatable from script.
	template < class T >
	class ScriptCallableCodeEvent : public IRegisteredEvent
	{
	public:
		explicit ScriptCallableCodeEvent( void )
			: IRegisteredEvent( IRegisteredEvent::kREMD_CodeEventScriptCallable )
		{
		}

		virtual bool VTriggerEventFromScript( LuaObject & srcData ) const
		{
			const T eventData( srcData );	//Construct directly.
			return safeTriggerEvent( eventData );
		}

		virtual bool VQueueEventFromScript( LuaObject & srcData ) const
		{
			IEventDataPtr eventPtr( DEBUG_CLIENTBLOCK T( srcData ) );
			return safeQueEvent( eventPtr );
		}
	};		

	typedef shared_ptr< IRegisteredEvent > IRegisteredEventPtr;

	//Verifies that such an event does not already exist, then registers it.
	void AddRegisteredEventType( const EventType & eventType, IRegisteredEventPtr metaData );

	// one global instance
	typedef std::map< EventType, IRegisteredEventPtr >		EventTypeSet;

	// insert result into event type set
	typedef std::pair< EventTypeSet::iterator, bool >		EventTypeSetIRes;

	// one list per event type ( stored in the map )
	typedef std::list< EventListenerPtr >					EventListenerTable;

	// mapping of event ident to listener list
	typedef std::map< unsigned int, EventListenerTable >	EventListenerMap;

	// entry in the event listener map
	typedef std::pair< unsigned int, EventListenerTable >	EventListenerMapEnt;

	// insert result into listener map
	typedef std::pair< EventListenerMap::iterator, bool >	EventListenerMapIRes;

	// queue of pending- or processing-events
	typedef std::list< IEventDataPtr >						EventQueue;

	enum eConstants
	{
		kNumQueues = 2
	};
	
	EventTypeSet     m_typeList;           // list of registered
											// event types

	EventListenerMap m_registry;           // mapping of event types
											// to listeners

	EventQueue       m_queues[kNumQueues]; // event processing queue,
											// double buffered to
											// prevent infinite cycles

	int               m_activeQueue;        // valid denoting which
											// queue is actively
											// processing, en-queing
											// events goes to the
											// opposing queue

	// ALL SCRIPT-RELATED FUNCTIONS
private:
	// Registers a script-based event.
	void RegisterScriptEventType( char const * const pEventName );

	// Add/remove a script listener.  Note that we pass a Lua *function* as the
	// second parameter of each of these, and tie that function to a specific
	// event name.  We'll call the Lua function when the event type comes in.
	bool AddScriptListener( char const * const pEventName, LuaObject callbackFunction );
	bool RemoveScriptListener( char const * const pEventName, LuaObject callbackFunction );

	// Triggers an event from script.  The event data will be serialized for any
	// code listeners.  Script listeners will receive the data table passed in.
	bool TriggerEventFromScript( char const * const pEventName, LuaObject luaEventData );

	// Actor-specific event listener controls.  These will pass in the actor context
	// upon calling.
	bool AddScriptActorListener( char const * const pEventName, LuaObject callbackFunction, const int actorID );
	bool RemoveScriptActorListener( char const * const pEventName, LuaObject callbackFunction, const int actorID );

	// Allows access for script calls.
	LuaObject m_MetaTable;

	// Holds all allocated script listeners.
	// It maps an event ID to a set of listeners.
	typedef shared_ptr< ScriptEventListener > ScriptEventListenerPtr;
	typedef std::multimap< unsigned int, ScriptEventListenerPtr > ScriptEventListenerMap;
	ScriptEventListenerMap m_ScriptEventListenerMap;

	// Holds all allocated script actor listeners.
	typedef shared_ptr< ScriptActorEventListener > ScriptActorEventListenerPtr;
	typedef multimap< unsigned int, ScriptActorEventListenerPtr > ScriptActorEventListenerMap;
	ScriptActorEventListenerMap m_ScriptActorEventListenerMap;

	//Holds all registered script event types.
	typedef set< EventType > ScriptDefinedEventTypeSet;
	ScriptDefinedEventTypeSet m_ScriptDefinedEventTypeSet;
};

// EventManager::RegisterEvent						- Chapter 11, page 330
// Our templated registration function.
template<class T> void EventManager::RegisterEvent( const EventType & eventType )
{
	IRegisteredEventPtr metaData( DEBUG_CLIENTBLOCK ScriptCallableCodeEvent< T >() );
	AddRegisteredEventType( eventType, metaData );
}

// Event listener used for snoooping ... simply emits event stats
// for all processed & known events to runtime log ( actually, in
// this case, debugger output )

class EventSnooper : public IEventListener
{
public:
	
	explicit EventSnooper( char const * const kpLogFileName = NULL);
	virtual ~EventSnooper();
	char const * GetName(void) { return "Snoop"; }
	bool HandleEvent( IEventData const & event );

private:

	FILE *m_OutFile;
	char m_msgBuf[4090];

};

