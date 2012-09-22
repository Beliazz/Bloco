#include "Bloco.h"

/*const EventType EvtData_New_Game::sk_EventType( "new_game" );*/
const EventType EvtData_EneableMovementController::sk_EventType( "enableMovementController" );
const EventType EvtData_SetMovementController::sk_EventType( "setMovementController" );
const EventType EvtData_Wireframe::sk_EventType( "wireframe" );
const EventType EvtData_New_Actor::sk_EventType( "new_actor" );
const EventType EvtData_Destroy_Actor::sk_EventType( "destroy_actor" );
const EventType EvtData_Update_Tick::sk_EventType("update_Tick");
const EventType EvtData_Debug_Draw::sk_EventType("debug_draw");
const EventType EvtData_Transform_Actor::sk_EventType( "transform_actor" );
const EventType EvtData_Request_New_Actor::sk_EventType( "request_new_actor" );
const EventType EvtData_SetActorTransform::sk_EventType( "setActorTransform" );
const EventType EvtData_SetTexture::sk_EventType( "setTexture" );
const EventType EvtData_FileModified::sk_EventType( "FileModified" );
const EventType EvtData_Picked_Actor::sk_EventType( "PickedActor" );
const EventType EvtData_PerformePick::sk_EventType( "performePick" );
const EventType EvtData_SelectActor::sk_EventType( "selectActor" );
const EventType EvtData_FileDrop::sk_EventType( "fileDroped" );
const EventType EvtData_DragMove::sk_EventType( "dragMove" );
const EventType EvtData_EnableActor::sk_EventType( "enableActor" );
const EventType EvtData_Editor_SetRemoveMode::sk_EventType( "editorSetRemoveMode" );
const EventType EvtData_Editor_SetSelectMode::sk_EventType( "editorSetSelectMode" );
const EventType EvtData_SetStaticActor::sk_EventType( "setStaticActor" );
const EventType EvtData_RayCast::sk_EventType( "raycast" );
const EventType EvtData_RunScript::sk_EventType( "runScript" );


void BLOCO_API EvtData_New_Actor::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );


	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "actorID", m_id );
	m_LuaEventData.SetBoolean( "hasScript", m_pActorParams->m_bHasScript );
	m_LuaEventData.SetString(  "scriptFilename", m_pActorParams->m_sScriptFilename );
	m_LuaEventData.SetString(  "name", m_pActorParams->m_sName );

	if (m_pActorParams->m_Type == AT_ModelChild_Mesh)
	{
		//Pass the parent ID, so the script can add this actor as a child
		m_LuaEventData.SetInteger( "parentID", *((ModelChildObjectParams*)m_pActorParams)->m_pRootNode->VGet()->ActorId() );
	}


	if ( NULL != m_pActorParams )
	{
		//TODO JWC!   James what the hell is supposed to go here???
	}

	m_bHasLuaEventData = true;
}

IEventDataPtr BLOCO_API EvtData_New_Actor::VCopy() const
{
	return IEventDataPtr( DEBUG_CLIENTBLOCK EvtData_New_Actor( m_id, m_pActorParams ) );
}

void BLOCO_API EvtData_EneableMovementController::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetBoolean( "Enable", m_enable );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_SetMovementController::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );


	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "ActorId", m_id );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_Wireframe::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetBoolean( "Enable", m_enable );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_Destroy_Actor::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "ActorId", m_id );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_Transform_Actor::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Now assign the data.
	m_LuaEventData.SetInteger( "ActorId", m_Id );

	//We don't want a whole 4x4 matrix, so just give us the position.
	Vec srcPos = MatGetPosition(m_Mat); 
	LuaObject posTable = m_LuaEventData.CreateTable( "Pos", 3 );
	posTable.SetNumber( 1, srcPos.GetX() );
	posTable.SetNumber( 2, srcPos.GetY() );
	posTable.SetNumber( 3, srcPos.GetZ() );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_Update_Tick::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "DeltaMS", m_DeltaMilliseconds );
	m_LuaEventData.SetNumber( "Seconds", ( (float)m_DeltaMilliseconds / 1000.0f ) );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_SetActorTransform::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Now assign the data.
	m_LuaEventData.SetInteger( "ActorId", m_Id );

	//We don't want a whole 4x4 matrix, so just give us the position.
	Vec srcPos = MatGetPosition(m_Mat); 
	LuaObject posTable = m_LuaEventData.CreateTable( "Pos", 3 );
	posTable.SetNumber( 1, srcPos.GetX() );
	posTable.SetNumber( 2, srcPos.GetY() );
	posTable.SetNumber( 3, srcPos.GetZ() );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_SetTexture::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Now assign the data.
	m_LuaEventData.SetInteger( "ActorId", m_Id );

	//Now assign the data.
	m_LuaEventData.SetInteger( "Index", m_index );

	//Now assign the data.
	m_LuaEventData.SetString( "Filename", m_filename.c_str() );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_FileModified::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetString( "Filename", m_filename.c_str() );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_Picked_Actor::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "ActorId", m_id );

	m_LuaEventData.SetInteger( "FaceIndex", m_faceindex );

	m_LuaEventData.SetNumber( "Distance", m_dist );

	m_LuaEventData.SetNumber( "HitX", m_hitPoint.GetX() );
	m_LuaEventData.SetNumber( "HitY", m_hitPoint.GetY() );
	m_LuaEventData.SetNumber( "HitZ", m_hitPoint.GetZ() );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_PerformePick::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "PosX", m_posX );
	m_LuaEventData.SetInteger( "PosY", m_posY );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_SelectActor::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "ActorId", m_id );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_FileDrop::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Now assign the data.
	m_LuaEventData.SetString( "Filename", m_filename.c_str() );

	//Now assign the data.
	m_LuaEventData.SetInteger( "PosX", m_posX );

	//Now assign the data.
	m_LuaEventData.SetInteger( "PosY", m_posY );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_EnableActor::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "ActorId", m_id );

	//Add aditional data here
	m_LuaEventData.SetBoolean( "Enable", m_enable );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_DragMove::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Now assign the data.
	m_LuaEventData.SetString( "Filename", m_filename.c_str() );

	//Now assign the data.
	m_LuaEventData.SetInteger( "PosX", m_posX );

	//Now assign the data.
	m_LuaEventData.SetInteger( "PosY", m_posY );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_Editor_SetRemoveMode::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Add aditional data here
	m_LuaEventData.SetBoolean( "Value", m_bValue );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_Editor_SetSelectMode::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Add aditional data here
	m_LuaEventData.SetBoolean( "Value", m_bValue );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_SetStaticActor::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetInteger( "ActorId", m_id );
	
	m_LuaEventData.SetBoolean( "Value", m_bValue );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_RayCast::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	m_LuaEventData.SetNumber( "RayStartX", m_rayStart.GetX() );
	m_LuaEventData.SetNumber( "RayStartY", m_rayStart.GetY() );
	m_LuaEventData.SetNumber( "RayStartZ", m_rayStart.GetZ() );

	m_LuaEventData.SetNumber( "RayEndX", m_rayEnd.GetX() );
	m_LuaEventData.SetNumber( "RayEndY", m_rayEnd.GetY() );
	m_LuaEventData.SetNumber( "RayEndZ", m_rayEnd.GetZ() );

	m_bHasLuaEventData = true;
}

void BLOCO_API EvtData_RunScript::VBuildLuaEventData( void )
{
	assert( ( false == m_bHasLuaEventData ) && "Already built lua event data!" );

	//Get the global state.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	m_LuaEventData.AssignNewTable( pState );

	//Serialize the data necessary.
	m_LuaEventData.SetString( "filename", m_filename.c_str() );

	m_bHasLuaEventData = true;
}
