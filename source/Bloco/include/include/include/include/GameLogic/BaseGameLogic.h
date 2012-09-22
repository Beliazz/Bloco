#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Bloco.h"

//
// enum BaseGameState				- Chapter 19, page 710
//
enum BLOCO_API BaseGameState
{
	BGS_Initializing,
	BGS_LoadingGameEnvironment,
	BGS_MainMenu,
	BGS_WaitingForPlayers,
	BGS_SpawnAI,
	BGS_Running
};

typedef std::map<ActorId, shared_ptr<IActor> > BLOCO_API ActorMap;

class BLOCO_API BaseGameLogic : public IGameLogic
{
	friend class Application;						// This is only to gain access to the view list

protected:
	CProcessManager *m_pProcessManager;				// a game logic entity
	//CRandom m_random;								// our RNG
	ActorMap m_ActorList;
	ActorId  m_LastActorId;
	BaseGameState m_State;							// game state: loading, running, etc.
	int m_ExpectedPlayers;							// how many local human players
	int m_ExpectedRemotePlayers;					// expected remote human players
	int m_ExpectedAI;								// how man AI players
	GameViewList m_gameViews;						// views that are attached to our game
	//shared_ptr<PathingGraph> m_pPathingGraph;		// the pathing graph
	// EventListenerPtr m_pAiEventListener;			// AI event listener


	bool m_bProxy;									// set if this is a proxy game logic, not a real one
	int m_remotePlayerId;							// if we are a remote player - what is out socket number on the server

	// Refactor Chapter 19/20 work.
	bool m_RenderDiagnostics;						// Are we rendering diagnostics?
	shared_ptr<IGamePhysics> m_pPhysics;

public:

	BaseGameLogic(struct GameOptions const &optionss);
	virtual ~BaseGameLogic();

	// Pausing
	void TogglePause(bool active);
	void SetProxy(bool isProxy) { m_bProxy = isProxy; }

	ActorId GetNewActorID( void )
	{
		return ++m_LastActorId;
	}

	ActorId GetRandomActor(optional<ActorId> ignoreMe);

	shared_ptr<IGameView> GetView( unsigned int index);

	//shared_ptr<PathingGraph> GetPathingGraph(void) { return m_pPathingGraph; }
	//CRandom& GetRNG(void) { return m_random; }

	virtual void VBuildInitialScene();

	virtual void VAddView(shared_ptr<IGameView> pView, optional<ActorId> actorId=optional_empty());
	virtual void VRemoveView(shared_ptr<IGameView> pView);

	virtual void VMoveActor( const ActorId id, Mat mat );

	virtual void VAddActor(shared_ptr<IActor> actor, ActorParams *p);
	virtual shared_ptr<IActor> VGetActor(const ActorId id);				// TODO: Make Protected...
	virtual ActorMap VGetActorList() { return m_ActorList; }
	virtual void VRemoveActor(ActorId id);

	virtual void VSetActorTransform(const ActorId id, Mat const &mat);
	virtual void VTransformActor(const ActorId id, Mat const &mat);

	virtual bool VLoadGame(std::string gameName);
	virtual void VSetProxy() { m_bProxy = true; }

	// Logic Update
	virtual void VOnUpdate(float time, float elapsedTime);

	// Changing Game Logic State
	virtual void VChangeState(BaseGameState newState);

	// Render Diagnostics
	virtual void VRenderDiagnostics();
	virtual shared_ptr<IGamePhysics> VGetGamePhysics(void) { return m_pPhysics; }

	void AttachProcess(shared_ptr<CProcess> pProcess) { if (m_pProcessManager) {m_pProcessManager->Attach(pProcess);} }
};

#endif