#include "Bloco.h"


int scriptPrint( LuaState* state )
{
	LuaStack args(state);

	printf(args[1].GetString());

	return 0;
}
int scriptEneableMovmentController( LuaState* state )
{
	LuaStack args(state);

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_EneableMovementController(args[1].GetBoolean()));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptSetMovmentController( LuaState* state )
{
	LuaStack args(state);

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_SetMovementController( args[1].GetInteger() ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptWireframe( LuaState* state )
{
	LuaStack args(state);

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_Wireframe( args[1].GetBoolean() ));
	safeQueEvent(pEvtData);

	return 0;
}


int scriptTranslateActor( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=4)
		return 1;

	Mat translate = MatTranslation(Vec( args[2].GetFloat(), args[3].GetFloat(), args[4].GetFloat() ));


	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_Transform_Actor( args[1].GetInteger(), translate ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptRotateActor( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=4)
		return 1;

	Mat rotation = MatRotation(args[2].GetFloat(), args[3].GetFloat(), args[4].GetFloat() );

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_Transform_Actor( args[1].GetInteger(), rotation ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptScaleActor( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=4)
		return 1;

	Mat scale =  MatScaling( Vec(args[2].GetFloat(), args[3].GetFloat(), args[4].GetFloat()) );

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_Transform_Actor( args[1].GetInteger(), scale ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptSetActorTranslation( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=4)
		return 1;

	Mat translate = MatTranslation( Vec(args[2].GetFloat(), args[3].GetFloat(), args[4].GetFloat()) );

	const EvtData_SetActorTransform evtData( args[1].GetInteger(), translate );
	safeTriggerEvent(evtData);

	return 0;
}

int scriptSetActorRotation( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=4)
		return 1;

	Mat rotation = MatRotation(args[2].GetFloat(), args[3].GetFloat(), args[4].GetFloat() );

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_SetActorTransform( args[1].GetInteger(), rotation ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptSetActorScale( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=4)
		return 1;

	Mat scale = MatScaling( Vec(args[2].GetFloat(), args[3].GetFloat(), args[4].GetFloat()) );

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_SetActorTransform( args[1].GetInteger(), scale ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptSetTexture( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=3)
		return 1;

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_SetTexture( args[1].GetInteger(), args[2].GetInteger() ,args[3].GetString() ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptExecuteScript( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=1)
		return 1;

	if(!g_pApp->m_pLuaStateManager->DoFile(SCRIPT_PATH(args[1].GetString())))
		return 1;

	return 0;
}

int scriptSelectActor( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=1)
		return 1;

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_SelectActor( args[1].GetInteger() ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptEnableActor( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=2)
		return 1;


	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_EnableActor( args[1].GetInteger(), args[2].GetBoolean() ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptRemoveActor( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=1)
		return 1;

	IEventDataPtr pEvtData = IEventDataPtr(DEBUG_CLIENTBLOCK EvtData_Destroy_Actor( args[1].GetInteger() ));
	safeQueEvent(pEvtData);

	return 0;
}

int scriptModelSetPosition( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=4)
		return 1;



	g_pApp->m_pGame->VGetGamePhysics()->VStaticActor( args[1].GetInteger() );


	return 0;
}

int scriptPhysicsDebugMode( LuaState* state )
{
	LuaStack args(state);

	if (args.Count()!=1)
		return 1;


	g_pApp->m_pGame->VGetGamePhysics()->VSetDebugMode( args[1].GetInteger() );

	return 0;
}
