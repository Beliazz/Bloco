#ifndef Events_h__
#define Events_h__

#include "Bloco.h"
//#include "Application.h"
//#include "LuaStateManager.h"

// Auxillary data decls...
//
// data that is passed per-event in the userData parameter
// 
// ( for some, but not all, events )

//-- DEBUG_CLIENTBLOCK object notification

// This event is sent out when an actor is
// *actually* created.

class LuaStateManager;

struct BLOCO_API EvtData_New_Actor : public BaseEventData
{

	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_New_Actor( ActorId id,
							  ActorParams *pCreateParams)
	{
		m_id = id;
		m_pActorParams = reinterpret_cast<ActorParams *>(DEBUG_CLIENTBLOCK char[pCreateParams->m_Size]);
		memcpy(m_pActorParams, pCreateParams, pCreateParams->m_Size);
		m_pActorParams->m_Id = id;
	}

	explicit EvtData_New_Actor( std::istrstream & in )
	{
		in >> m_id;
		m_pActorParams = ActorParams::CreateFromStream( in );
	}

	explicit EvtData_New_Actor( LuaObject srcData )
	{
		m_bHasLuaEventData = true;	//Our Lua event data got passed into us!
	}

	virtual IEventDataPtr VCopy() const;
	virtual ~EvtData_New_Actor() 
	{ 
		SAFE_DELETE(m_pActorParams); 
	}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	
	virtual void VSerialize(std::ostrstream &out) const
	{
		out << m_id << " ";
		m_pActorParams->VSerialize(out);
	}

	ActorId m_id;	//ID of actor created
	ActorParams *m_pActorParams;	//Parameters for actor

private:
	LuaObject	m_LuaEventData;
};


struct BLOCO_API EvtData_EneableMovementController : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_EneableMovementController( bool value )
		: m_enable(value)
	{
	}

	explicit EvtData_EneableMovementController( std::istream & in )
	{
		in >> m_enable;
	}

	explicit EvtData_EneableMovementController( LuaObject srcData )
	{
		m_bHasLuaEventData = true;	//Our Lua event data got passed into us!
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_EneableMovementController ( m_enable ) );
	}
	virtual ~EvtData_EneableMovementController() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);


	virtual void VSerialize(std::ostream &out) const
	{
		out << m_enable;
	}

	bool m_enable;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_SetMovementController : public BaseEventData
{

	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_SetMovementController( ActorId id) :
		m_id(id)
	{
	}

	explicit EvtData_SetMovementController( std::istream & in )
	{
		in >> m_id;
	}

	explicit EvtData_SetMovementController( LuaObject srcData )
	{
		m_bHasLuaEventData = true;	//Our Lua event data got passed into us!
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_SetMovementController ( m_id ) );
	}
	virtual ~EvtData_SetMovementController(){}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);

	virtual void VSerialize(std::ostrstream &out) const
	{
		out << m_id << " ";
	}

	ActorId m_id;	//ID of actor created

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_Wireframe : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Wireframe( bool value )
		: m_enable(value)
	{
	}

	explicit EvtData_Wireframe( std::istream & in )
	{
		in >> m_enable;
	}

	explicit EvtData_Wireframe( LuaObject srcData )
	{
		m_bHasLuaEventData = true;	//Our Lua event data got passed into us!
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_Wireframe ( m_enable ) );
	}
	virtual ~EvtData_Wireframe() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);


	virtual void VSerialize(std::ostream &out) const
	{
		out << m_enable;
	}

	bool m_enable;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_OnScriptExecution : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_OnScriptExecution( string value )
		: m_filename(value)
	{
	}

	explicit EvtData_OnScriptExecution( std::istream & in )
	{
		in >> m_filename;
	}

	explicit EvtData_OnScriptExecution( LuaObject srcData )
	{
		m_bHasLuaEventData = true;	//Our Lua event data got passed into us!
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_OnScriptExecution ( m_filename ) );
	}
	virtual ~EvtData_OnScriptExecution() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);


	virtual void VSerialize(std::ostream &out) const
	{
		out << m_filename;
	}

	string m_filename;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_FileModified : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_FileModified( string value )
		: m_filename(value)
	{
	}

	explicit EvtData_FileModified( std::istream & in )
	{
		in >> m_filename;
	}

	explicit EvtData_FileModified( LuaObject srcData )
	{
		m_bHasLuaEventData = true;	//Our Lua event data got passed into us!
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_FileModified ( m_filename ) );
	}
	virtual ~EvtData_FileModified() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);


	virtual void VSerialize(std::ostream &out) const
	{
		out << m_filename;
	}

	string m_filename;

private:
	LuaObject	m_LuaEventData;
};

// This event is sent by a client ASKING to
// create an actor.  It can be sent from script
// or via code.
struct BLOCO_API EvtData_Request_New_Actor : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Request_New_Actor( LuaObject srcData )
		: m_ActorDef( srcData )
		, m_pActorParams( NULL )
	{
		m_bHasLuaEventData = true;	//Our Lua event data got passed into us!
	}

	explicit EvtData_Request_New_Actor( const ActorParams *pCreateParams )
		: m_pActorParams( NULL )
	{
		m_bHasLuaEventData = false;	//We *don't* have Lua event data.
		m_pActorParams = reinterpret_cast<ActorParams *>(DEBUG_CLIENTBLOCK char[pCreateParams->m_Size]);
		memcpy(m_pActorParams, pCreateParams, pCreateParams->m_Size);
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_Request_New_Actor ( m_ActorDef ) );
	}

	virtual ~EvtData_Request_New_Actor()
	{
		SAFE_DELETE( m_pActorParams );
	}

	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_ActorDef;
	}

	virtual void VBuildLuaEventData(void)
	{
		assert( ( true == m_bHasLuaEventData ) && "Already built lua event data!" );
		return;	//Already "built" when the event got created.
	}

	ActorParams * m_pActorParams;

private:
	LuaObject m_ActorDef;
};

struct BLOCO_API EvtData_Destroy_Actor : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Destroy_Actor( ActorId id )
		: m_id( id )
	{
	}

	explicit EvtData_Destroy_Actor( std::istream & in )
	{
		in >> m_id;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_Destroy_Actor ( m_id ) );
	}
	virtual ~EvtData_Destroy_Actor() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{
		out << m_id;
	}

	ActorId m_id;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_Picked_Actor : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Picked_Actor( ActorId id, float dist, UINT faceindex, Vec hitPoint )
		: m_id( id ), m_dist(dist), m_faceindex(faceindex), m_hitPoint(hitPoint)
	{
	}

	explicit EvtData_Picked_Actor( std::istream & in )
	{
		in >> m_id;
		in >> m_dist;
		in >> m_faceindex;

		m_hitPoint = Vec(in);
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_Picked_Actor ( m_id, m_dist, m_faceindex, m_hitPoint ) );
	}
	virtual ~EvtData_Picked_Actor() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{		
		out << m_id;
		out << m_dist;
		out << m_faceindex;
		m_hitPoint.Serialize(out);
	}

	ActorId m_id;
	float	m_dist;
	UINT	m_faceindex;
	Vec		m_hitPoint;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_RayCast : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_RayCast( Vec rayStart,  Vec rayEnd )
		: m_rayStart(rayStart), m_rayEnd(rayEnd)
	{
	}

	explicit EvtData_RayCast( std::istream & in )
	{
		m_rayStart  = Vec(in);
		m_rayEnd = Vec(in);
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_RayCast ( m_rayStart, m_rayEnd ) );
	}
	virtual ~EvtData_RayCast() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{		
		m_rayStart.Serialize(out);
		m_rayEnd.Serialize(out);
	}

	Vec		m_rayStart;
	Vec		m_rayEnd;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_PerformePick : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_PerformePick( unsigned int x, unsigned int y )
		: m_posX(x), m_posY(y)
	{
	}

	explicit EvtData_PerformePick( std::istream & in )
	{
		in >> m_posX;
		in >> m_posY;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_PerformePick ( m_posX, m_posY ) );
	}
	virtual ~EvtData_PerformePick() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{		
		out << m_posX;
		out << m_posY;
	}

	unsigned int m_posX;
	unsigned int m_posY;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_Transform_Actor : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Transform_Actor( ActorId id, const Mat mat)
	{
		m_Id = id;
		m_Mat = mat;
	}

	explicit EvtData_Transform_Actor( std::istream & in )
	{
		in >> m_Id;

		m_Mat = Mat(in);
	}

	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}

	virtual void VBuildLuaEventData(void);

	virtual void VSerialize(std::ostream &out) const
	{
		out << m_Id << " ";

		m_Mat.Serialize(out); 
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_Transform_Actor(m_Id, m_Mat));
	}

	ActorId m_Id;
	Mat m_Mat;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_SetActorTransform : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_SetActorTransform( ActorId id, const Mat mat)
	{
		m_Id = id;
		m_Mat = mat;
	}

	explicit EvtData_SetActorTransform( std::istream & in )
	{
		in >> m_Id;

		m_Mat = Mat(in);
	}

	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}

	virtual void VBuildLuaEventData(void);

	virtual void VSerialize(std::ostream &out) const
	{
		out << m_Id << " ";

		m_Mat.Serialize(out); 
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_Transform_Actor(m_Id, m_Mat));
	}

	ActorId m_Id;
	Mat m_Mat;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_SelectActor : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_SelectActor( ActorId id )
		: m_id( id )
	{
	}

	explicit EvtData_SelectActor( std::istream & in )
	{
		in >> m_id;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_SelectActor ( m_id ) );
	}
	virtual ~EvtData_SelectActor() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{
		out << m_id;
	}

	ActorId m_id;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_SetTexture : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_SetTexture( ActorId id, unsigned int index, string filename)
	{
		m_Id = id;
		m_index = index;
		m_filename = filename;
	}

	explicit EvtData_SetTexture( std::istream & in )
	{
		in >> m_Id;
		in >> m_index;
		in >> m_filename;
	}

	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}

	virtual void VBuildLuaEventData(void);

	virtual void VSerialize(std::ostream &out) const
	{
		out << m_Id << " ";
		out << m_index << " ";
		out << m_filename << " ";
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_SetTexture(m_Id, m_index, m_filename));
	}

	ActorId m_Id;
	unsigned int m_index;
	string  m_filename;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_Debug_Draw : public BaseEventData
{
public:
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Debug_Draw( const BOOL bVal )
		: BaseEventData((float)0), m_bVal( bVal )
	{
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr (DEBUG_CLIENTBLOCK EvtData_Debug_Draw ( m_bVal ) );
	}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( 0 && "not available" );
		return LuaObject();
	}
	virtual void VBuildLuaEventData(void)
	{
		assert( 0 && "not available" );
	}
	virtual void VSerialize( std::ostream & out )
	{
		assert( 0 && "You should not be serializing debug draw!" );
	}
	virtual string VToString()
	{
		char buffer[1024];
		sprintf(buffer, "<Type>debug_draw<Type><Time>%f<Time><Data>%i<Data>", VGetTimeStamp(), m_bVal);
		return string(buffer);
	}

	BOOL m_bVal;

private:
	LuaObject m_LuaEventData;
};

struct BLOCO_API EvtData_Update_Tick : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Update_Tick( const int deltaMilliseconds )
		: BaseEventData((float)0), m_DeltaMilliseconds( deltaMilliseconds )
	{
	}

	explicit EvtData_Update_Tick( LuaObject srcData )
	{

	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr (DEBUG_CLIENTBLOCK EvtData_Update_Tick ( m_DeltaMilliseconds ) );
	}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize( std::ostream & out )
	{
		assert( 0 && "You should not be serializing update ticks!" );
	}
	virtual string VToString()
	{
		char buffer[1024];
		sprintf(buffer, "<Type>Update Tick<Type><Time>%f<Time><Data>%i<Data>", VGetTimeStamp(), m_DeltaMilliseconds);
		return string(buffer);
	}

	int m_DeltaMilliseconds;

private:
	LuaObject m_LuaEventData;
};

struct BLOCO_API EvtData_FileDrop : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_FileDrop( string filename, unsigned int posX, unsigned int posY  )
	{
		m_posX = posX;
		m_posY = posY;
		m_filename = filename;
	}

	explicit EvtData_FileDrop( std::istream & in )
	{
		in >> m_posX;
		in >> m_posY;
		in >> m_filename;
	}

	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}

	virtual void VBuildLuaEventData(void);

	virtual void VSerialize(std::ostream &out) const
	{
		out << m_posX << " ";
		out << m_posY << " ";
		out << m_filename << " ";
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_FileDrop(m_filename, m_posX, m_posY));
	}

	unsigned int m_posX;
	unsigned int m_posY;
	string  m_filename;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_EnableActor : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_EnableActor( ActorId id , bool m_enable)
		: m_id( id ), m_enable( m_enable )
	{
	}

	explicit EvtData_EnableActor( std::istream & in )
	{
		in >> m_id;
		in >> m_enable;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_EnableActor ( m_id , m_enable) );
	}
	virtual ~EvtData_EnableActor() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{
		out << m_id;
		out << m_enable;
	}

	ActorId m_id;
	bool m_enable;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_DragMove : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_DragMove( string filename, unsigned int posX, unsigned int posY  )
	{
		m_posX = posX;
		m_posY = posY;
		m_filename = filename;
	}

	explicit EvtData_DragMove( std::istream & in )
	{
		in >> m_posX;
		in >> m_posY;
		in >> m_filename;
	}

	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}

	virtual void VBuildLuaEventData(void);

	virtual void VSerialize(std::ostream &out) const
	{
		out << m_posX << " ";
		out << m_posY << " ";
		out << m_filename << " ";
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_DragMove(m_filename, m_posX, m_posY));
	}

	unsigned int m_posX;
	unsigned int m_posY;
	string  m_filename;

private:
	LuaObject	m_LuaEventData;
};


struct BLOCO_API EvtData_Editor_SetRemoveMode : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Editor_SetRemoveMode( bool value )
		: m_bValue(value)
	{
	}

	explicit EvtData_Editor_SetRemoveMode( std::istream & in )
	{
		in >> m_bValue;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_Editor_SetRemoveMode ( m_bValue ) );
	}
	virtual ~EvtData_Editor_SetRemoveMode() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{
		out << m_bValue;
	}
	bool m_bValue;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_Editor_SetSelectMode : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_Editor_SetSelectMode( bool value )
		: m_bValue(value)
	{
	}

	explicit EvtData_Editor_SetSelectMode( std::istream & in )
	{
		in >> m_bValue;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_Editor_SetSelectMode ( m_bValue ) );
	}
	virtual ~EvtData_Editor_SetSelectMode() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{
		out << m_bValue;
	}
	bool m_bValue;

private:
	LuaObject	m_LuaEventData;
};

struct BLOCO_API EvtData_SetStaticActor : public BaseEventData
{
	static const EventType sk_EventType;
	virtual const EventType & VGetEventType( void ) const
	{
		return sk_EventType;
	}

	explicit EvtData_SetStaticActor( ActorId id, bool value )
		: m_bValue(value),
		  m_id(id)
	{
	}

	explicit EvtData_SetStaticActor( std::istream & in )
	{
		in >> m_bValue;
		in >> m_id;
	}

	virtual IEventDataPtr VCopy() const
	{
		return IEventDataPtr ( DEBUG_CLIENTBLOCK EvtData_SetStaticActor ( m_id, m_bValue ) );
	}
	virtual ~EvtData_SetStaticActor() {}
	virtual LuaObject VGetLuaEventData(void) const
	{
		assert( ( true == m_bHasLuaEventData ) && "Can't get lua event data because it hasn't been built yet!  Call BulidLuaEventData() first!" );
		return m_LuaEventData;
	}
	virtual void VBuildLuaEventData(void);
	virtual void VSerialize(std::ostream &out) const
	{
		out << m_bValue;
		out << m_id;
	}
	bool	m_bValue;
	ActorId m_id;
private:
	LuaObject	m_LuaEventData;
};


#endif // Events_h__

