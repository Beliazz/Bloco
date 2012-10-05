//========================================================================
//
// BaseGameLogic implementation
//
//========================================================================

#include "Bloco.h"

 BLOCO_API BaseGameLogic::BaseGameLogic(struct GameOptions const &options)
{
	m_LastActorId = 0;
	m_pProcessManager = DEBUG_CLIENTBLOCK CProcessManager;
	m_State = BGS_Initializing;
	m_bProxy = false;
	m_RenderDiagnostics = true;
	m_ExpectedPlayers = 0;
	m_ExpectedRemotePlayers = 0;
	m_ExpectedAI = 0;
}


 BLOCO_API BaseGameLogic::~BaseGameLogic()
{
	// MrMike: 12-Apr-2009 
	//   Added this to explicity remove views from the game logic list.

	while (!m_gameViews.empty())
		m_gameViews.pop_front();

	SAFE_DELETE(m_pProcessManager);

	assert (m_ActorList.empty() && _T("You should destroy the actor list in the inherited class!") );
}

//
// BaseGameLogic::VAddActor				- Chapter 19, page 750
//
void BLOCO_API BaseGameLogic::VAddActor(shared_ptr<IActor> actor, ActorParams *p)
{
	optional< ActorId > destActorID = p->m_Id;
	assert( destActorID.valid() && "Attempted to add an actor with no actor ID!" );
	m_ActorList[ *destActorID ] = actor;
	actor->VSetID( *destActorID );

	//Ensure script knows about this actor, too.
	LuaState * pState = g_pApp->m_pLuaStateManager->GetGlobalState().Get();
	LuaObject globalActorTable = g_pApp->m_pLuaStateManager->GetGlobalActorTable();
	assert( globalActorTable.IsTable() && "Global actor table is NOT a table!" );
	LuaObject addedActorData = globalActorTable.CreateTable( *p->m_Id );	//The actor ID is the key.
	addedActorData.SetInteger( "ActorID", *p->m_Id );

	if ( 0 != p->m_OnCreateLuaFunctionName[0] ) 
	{
		addedActorData.SetString( "OnCreateFunc", p->m_OnCreateLuaFunctionName );
	}

	if ( 0 != p->m_OnDestroyLuaFunctionName[0] ) 
	{
		addedActorData.SetString( "OnDestroyFunc", p->m_OnDestroyLuaFunctionName );
	}

	//If this actor has any script-specific functions to call, do so now.
	if ( 0 != strlen( p->m_OnCreateLuaFunctionName ) )
	{
		//First attempt to FIND the function specified.
		LuaObject foundObj = g_pApp->m_pLuaStateManager->GetGlobalState()->GetGlobal( p->m_OnCreateLuaFunctionName );
		if ( foundObj.IsNil() )
		{
			assert( 0 && "Unable to find specified OnCreateFunc function!" );
		}
		else
		{
			//Make sure it actually *IS* a function.
			if ( false == foundObj.IsFunction() )
			{
				assert( 0 && "Specified OnCreateFunc doesn't exist!" );
			}
			else
			{
				//Attempt to call the function.
				LuaFunction< void > onCreateFunc( foundObj );
				onCreateFunc( *p->m_Id, addedActorData );	//Pass in the actor ID and this actor's user-owned data table.
			}
		}
	}
}

//
// BaseGameLogic::VRemoveActor				- Chapter 19, page 751
//
void BLOCO_API BaseGameLogic::VRemoveActor(ActorId aid)
{
	if (m_bProxy)
		return;

	if( NULL!=VGetActor(aid).get() )
	{
		m_pPhysics->VRemoveActor(aid);
		m_ActorList.erase(aid);
	}
	else
	{
		assert(0 && _T("Unknown actor!"));
	}
}

void BLOCO_API BaseGameLogic::VTransformActor(const ActorId id, Mat const &mat)
{
	shared_ptr<IActor> pActor = VGetActor(id);
	if (pActor)
	{
		pActor->VTransform(mat);
	}
}

void BLOCO_API BaseGameLogic::VSetActorTransform( const ActorId id, Mat const &mat )
{
	shared_ptr<IActor> pActor = VGetActor(id);
	if (pActor)
	{
		pActor->VSetMat(mat);
	}
}

shared_ptr<IActor> BLOCO_API BaseGameLogic::VGetActor(const ActorId id)
{
	ActorMap::iterator i = m_ActorList.find(id);
	if (i==m_ActorList.end())
	{
		shared_ptr<IActor> null;
		return null;
	}

	return (*i).second;
}

void BLOCO_API BaseGameLogic::VOnUpdate(float time, float elapsedTime)
{
	int deltaMilliseconds = int(elapsedTime * 1000.0f);

	switch(m_State)
	{
	case BGS_Initializing:
		// If we get to here we're ready to attach players
		VChangeState(BGS_MainMenu);
		break;

	case BGS_LoadingGameEnvironment:
		if (g_pApp->VLoadGame())
		{	
			VChangeState(BGS_SpawnAI);
		}
		else
		{
			assert(0 && _T("The game failed to load."));
			g_pApp->AbortGame();
		}
		break;

	case BGS_MainMenu:
		// do nothing
		break;

	case BGS_WaitingForPlayers:
		// do nothing - the game class should handle this one.
		break;

	case BGS_SpawnAI:
		if (this->m_ExpectedAI == 0)
		{
			// the base game logic doesn't spawn AI - your child class will do that.
			// in the case no AI are coming, the base game logic will go ahead and move to 
			// the next state.
			VChangeState(BGS_Running);
		}
		break;

	case BGS_Running:
		m_pProcessManager->UpdateProcesses(deltaMilliseconds);
		break;

	default:
		assert(0 && _T("Unrecognized state."));
		// Not a bad idea to throw an exception here to 
		// catch this in a release build...
	}

	GameViewList::iterator i=m_gameViews.begin();
	GameViewList::iterator end=m_gameViews.end();
	while (i != end)
	{
		(*i)->VOnUpdate( deltaMilliseconds );
		++i;
	}

	if(m_pPhysics)
	{
		m_pPhysics->VOnUpdate(elapsedTime);
		m_pPhysics->VSyncVisibleScene();
	}
}

void BaseGameLogic::VChangeState(BaseGameState newState)
{
// 	if (newState==BGS_WaitingForPlayers)
// 	{
// 		// Get rid of the Main Menu...
// 		m_gameViews.pop_front();
// 
// 		// Note: Split screen support would require this to change!
// 		m_ExpectedPlayers = 1;
// 		m_ExpectedRemotePlayers = g_pApp->m_pOptions->m_expectedPlayers - 1;
// 		m_ExpectedAI = g_pApp->m_pOptions->m_numAIs;
// 
// 		if (!g_pApp->m_pOptions->m_gameHost.empty())
// 		{
// 			VSetProxy();
// 			m_ExpectedAI = 0;			// the server will create these
// 			m_ExpectedRemotePlayers = 0;	// the server will create these
// 			ClientSocketManager *pClient = GCC_NEW ClientSocketManager(g_pApp->m_pOptions->m_gameHost, g_pApp->m_pOptions->m_listenPort);
// 			if (!pClient->Connect())
// 			{
// 				// Throw up a main menu
// 				VChangeState(BGS_MainMenu);
// 				return;
// 			}
// 			g_pApp->m_pBaseSocketManager = pClient;
// 		}
// 		else if (m_ExpectedRemotePlayers > 0)
// 		{
// 			BaseSocketManager *pServer = GCC_NEW BaseSocketManager();
// 			if (!pServer->Init())
// 			{
// 				// Throw up a main menu
// 				VChangeState(BGS_MainMenu);	
// 				return;
// 			}
// 
// 			pServer->AddSocket(DEBUG_CLIENTBLOCK GameServerListenSocket(g_pApp->m_pOptions->m_listenPort));
// 			g_pApp->m_pBaseSocketManager = pServer;
// 		}
// 	}

	m_State = newState;
	if (!m_bProxy)
	{
		/*safeQueEvent( IEventDataPtr(EvtData_Game_State(m_State)) );*/
	}
}

void BLOCO_API BaseGameLogic::VBuildInitialScene()
{
	//Execute our startup script file first.
	const bool bStartupScriptSuccess = g_pApp->m_pLuaStateManager->DoFile( SCRIPT_PATH("startup") );
	if ( false == bStartupScriptSuccess )
	{
		assert( 0 && "Unable to execute startup.lua!" );
	}
}

void BLOCO_API BaseGameLogic::VRenderDiagnostics() 
{ 
}

void BLOCO_API BaseGameLogic::TogglePause(bool active)
{
	// TODO This whole body of code belongs in the view, not the game!
	// AND it should fire off a pause event to all listeners.

// 	// Pause or resume audio	
// 	if ( active )
// 	{
// 		//ResetTimer();
// 		if (g_pAudio) 
// 			g_pAudio->VPauseAllSounds();
// 	}
// 	else
// 	{
// 		if (g_pAudio)
// 			g_pAudio->VResumeAllSounds();
// 		//SaveBackground();
// 	}
}

void BLOCO_API BaseGameLogic::VAddView(shared_ptr<IGameView> pView, optional<ActorId> actorId)
{
	// This makes sure that all views have a non-zero view id.
	int viewId = static_cast<int>(m_gameViews.size());
	m_gameViews.push_back(pView);
	pView->VOnAttach(viewId, actorId);
	pView->VOnRestore();
}

void BLOCO_API BaseGameLogic::VRemoveView(shared_ptr<IGameView> pView)
{
	m_gameViews.remove(pView);
}

bool BLOCO_API BaseGameLogic::VLoadGame( std::string gameName )
{
	VBuildInitialScene();

	return true;
}

shared_ptr<IGameView> BLOCO_API BaseGameLogic::GetView( unsigned int index )
{
	GameViewList::iterator end = m_gameViews.end();

	for ( GameViewList::iterator iter = m_gameViews.begin(); iter != end; ++iter )
	{
		shared_ptr<IGameView> pGameView = *iter;
		const GameViewId testID = pGameView->VGetId();
		if ( testID == index )
		{
			return (*iter);
		}
	}
	return NULL;
}

void BLOCO_API BaseGameLogic::VMoveActor( const ActorId id, Mat mat )
{
	shared_ptr<IActor> pActor = VGetActor(id);
	if (pActor)
	{
		pActor->VSetMat(mat);
	}
}


