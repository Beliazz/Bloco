#include "bloco.h"

cil::CILRawInput BLOCO_API* g_pInput;
cgl::PCGLManager  g_pCGLMgr;

CResourceManager* g_pResourceManager = NULL;

using namespace std;
using namespace std::tr1;
using namespace cgl;

using namespace cgl;

BLOCO_API Application::Application(Application** ppApp)
{
	(*ppApp) = this;
	//m_pSystem = NULL;
	m_pResCache = NULL;
	m_pEventManager = NULL;
	m_pLuaStateManager = g_pAppLuaStateManager;
	m_pGame = NULL;
	g_pInput = NULL;
	m_pFrameSmoother = NULL;
	m_updateInterval = 1.0f / 60.0f;
}
BLOCO_API Application::~Application()
{
	//Exit();
}

void BLOCO_API Application::Run()
{
	m_pGameLoop = new cgl::CGLGameLoop(this,m_pWindow->get());

	m_pGameLoop->EnableFullSpeed(true);

	m_pGameLoop->Run();


// 	UINT64 start;
// 	UINT64 end;
// 	UINT64 frequency;
// 
// 	bool updated = false;
// 	bool drawn = false;
// 	bool occluded = false;
// 	bool minimized = false;
// 
// 	MSG msg;
// 
// 	// get timer frequency
// 	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
// 
// 	// set running
// 	m_bIsRunning = true;
// 	m_bQuitting = false;
// 
// 	// reset time
// 	m_time = 0.0;
// 
// 	while (!m_bQuitting)
// 	{
// 		// measure start ticks
// 		QueryPerformanceCounter((LARGE_INTEGER*)&start);
// 
// 		// if the game was updated
// 		// and the window is visible
// 		// render
// 		if(updated && !occluded)
// 		{
// 			OnRender( m_time, m_updateInterval);
// 
// 			drawn = true;
// 			updated = false;
// 		}
// 
// 		// if the elapsed time is greater than
// 		// the screen refresh rate update it
// 		if (m_time >= m_updateInterval)
// 		{
// 			while (m_time >= m_updateInterval)
// 			{
// 				OnUpdate(m_time, m_updateInterval);
// 				m_time -= m_updateInterval;
// 			}
// 
// 			// if the window was not visible previously
// 			// perform a check if its still occluded
// 			if (occluded)
// 			{
// 				if (S_OK == m_pCore->GetSwapChain()->Present(0, DXGI_PRESENT_TEST))
// 				{
// 					// the window is visible again 
// 					// -> resume rendering
// 					occluded = false;
// 				}
// 			}
// 
// 			updated = true;
// 		}
// 
// 		// if the game was drawn
// 		// present it
// 		if (drawn)
// 		{
// 			HRESULT hResult = m_pCore->GetSwapChain()->Present(0, 0);
// 			switch ( hResult )
// 			{
// 				// the device has been removed
// 				// recreation of d3d11 objects needed
// 			case DXGI_ERROR_DEVICE_REMOVED:
// 			case DXGI_ERROR_DEVICE_RESET:
// 				{
// 
// 				} break;
// 
// 				// the window is occluded, do not render the game
// 				// until it is visible again
// 			case DXGI_STATUS_OCCLUDED:
// 				{
// 					occluded = true;
// 				} break;
// 
// 				// presenting successful
// 			case S_OK:
// 				{
// 
// 				} break;
// 			}
// 
// 
// 			drawn = false;
// 		}
// 
// 		if(PeekMessage(&msg, m_pWindow->get(), 0, 0, PM_REMOVE))
// 		{
// 			TranslateMessage(&msg);
// 			DispatchMessage(&msg);
// 		}
// 
// 		// process window messages
// 		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
// 		{
// 			TranslateMessage(&msg);
// 			DispatchMessage(&msg);
// 		}
// 
// 		// measure end ticks
// 		QueryPerformanceCounter((LARGE_INTEGER*)&end);
// 
// 		// calculate elapsed time in seconds and
// 		// add it to the frame smoother
// 		m_pFrameSmoother->Add( (float)(end - start) / frequency );
// 
// 		// update absolute time
// 		// with smoothed elapsed time
// 		m_time += m_pFrameSmoother->GetSmoothed();
// 	}
}
void BLOCO_API Application::Update()
{
	// measure start ticks
	QueryPerformanceCounter((LARGE_INTEGER*)&m_start);

	// if the game was updated
	// and the window is visible
	// render
	if(m_updated && !m_occluded)
	{
		OnRender( m_time, m_updateInterval);

		m_drawn = true;
		m_updated = false;
	}

	// if the elapsed time is greater than
	// the screen refresh rate update it
	if (m_time >= m_updateInterval)
	{
		while (m_time >= m_updateInterval)
		{
			OnUpdate(m_time, m_updateInterval);
			m_time -= m_updateInterval;
		}

		// if the window was not visible previously
		// perform a check if its still occluded
		if (m_occluded)
		{
			if (S_OK == m_pCore->GetSwapChain()->Present(0, DXGI_PRESENT_TEST))
			{
				// the window is visible again 
				// -> resume rendering
				m_occluded = false;
			}
		}

		m_updated = true;
	}

	// if the game was drawn
	// present it
	if (m_drawn)
	{
		HRESULT hResult = m_pCore->GetSwapChain()->Present(0, 0);
		switch ( hResult )
		{
			// the device has been removed
			// recreation of d3d11 objects needed
		case DXGI_ERROR_DEVICE_REMOVED:
		case DXGI_ERROR_DEVICE_RESET:
			{

			} break;

			// the window is occluded, do not render the game
			// until it is visible again
		case DXGI_STATUS_OCCLUDED:
			{
				m_occluded = true;
			} break;

			// presenting successful
		case S_OK:
			{

			} break;
		}


		m_drawn = false;
	}

	// process window messages
	if(PeekMessage(&m_msg, m_pWindow->get(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
	}

	// measure end ticks
	QueryPerformanceCounter((LARGE_INTEGER*)&m_end);

	// calculate elapsed time in seconds and
	// add it to the frame smoother
	m_pFrameSmoother->Add( (float)(m_end - m_start) / m_frequency );

	// update absolute time
	// with smoothed elapsed time
	m_time += m_pFrameSmoother->GetSmoothed();
}
void BLOCO_API Application::OnRender(double dTime, float fElapsed)
{
	for(GameViewList::iterator i=m_pGame->m_gameViews.begin(),
		end=m_pGame->m_gameViews.end(); i!=end; ++i)
	{

		(*i)->VOnRender((FLOAT)dTime, fElapsed);


	}
}
bool BLOCO_API Application::OnUpdate(double dTime, float fElapsed)
{
	if(m_pGame)
	{
		safeTickEventManager(10);

		g_pInput->Update();
		m_pGame->VOnUpdate((float)dTime, fElapsed);
	}

	return true;
}
void Application::OnIdle()
{
	MSG msg;

	if(PeekMessage(&msg, m_pWindow->get(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// process window messages
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


LRESULT BLOCO_API	CALLBACK Application::WindowProc( HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
	case WM_KEYDOWN:
		{
// 			g_pCGLMgr->Reset();
// 			g_pCGLMgr->Restore();
		}

	case WM_INPUT:
		{
			g_pInput->ProcessInput(lparam);
		} break;
	case WM_DESTROY:
		{
			g_pApp->SetQuitting(true);
			PostQuitMessage(0);
		} break;
	}

	if (g_pApp->m_pGame)
	{	
		AppMsg app_msg;
		app_msg.m_hWnd = hWnd;
		app_msg.m_lParam = lparam;
		app_msg.m_wParam = wparam;
		app_msg.m_uMsg = msg;

		BaseGameLogic *pGame = g_pApp->m_pGame;

		for(GameViewList::iterator i=pGame->m_gameViews.begin(),
			end=pGame->m_gameViews.end(); i!=end; ++i)
		{
			(*i)->VOnMsgProc(app_msg);
		}
	}

	return DefWindowProc(hWnd, msg, wparam, lparam);
}
bool BLOCO_API CALLBACK Application::CGLNotificationProc( UINT type, cgl::CGLObject* pObject, HRESULT result, std::string error, std::string errordesc )
{
	return FAILED(result);
}

LPWSTR BLOCO_API Application::GetSaveGameDirectory(LPWSTR gameAppDirectory)
{
	HRESULT hResult;
	static WCHAR m_SaveGameDirectory[MAX_PATH];
	WCHAR userDataPath[MAX_PATH];

	// app data path ( %USERPROFILE%\AppData\ )
	hResult = SHGetSpecialFolderPath(NULL/*m_pWindow->get()*/, userDataPath, CSIDL_APPDATA, true );

	wcscpy(m_SaveGameDirectory, userDataPath);
	wcscat(m_SaveGameDirectory, L"\\");
	wcscat(m_SaveGameDirectory, gameAppDirectory);

	// create directories if they don't exist
	if (0xffffffff == GetFileAttributes(m_SaveGameDirectory))
		SHCreateDirectoryEx(m_pWindow->get(), m_SaveGameDirectory, NULL);

	return m_SaveGameDirectory;
}
LPSTR  BLOCO_API Application::GetExePathA()
{
	CHAR temp[MAX_PATH];
	static CHAR exePath[MAX_PATH];
	static bool bDone = false;

	if (bDone)
		return exePath;

	GetModuleFileNameA(NULL, temp, MAX_PATH);
	int i;
	for (i = 1; i < MAX_PATH; i++)
	{
		CHAR snoop = temp[MAX_PATH-i];
		if (temp[MAX_PATH - i] == '\\')
		{
			memcpy(&exePath[0], &temp[0], (MAX_PATH-i)*sizeof(CHAR));
			break;
		}
	}

	bDone = true;
	return exePath;
}
LPWSTR BLOCO_API Application::GetExePathW()
{
	WCHAR temp[MAX_PATH];
	static WCHAR exePath[MAX_PATH];
	static bool bDone = false;

	if (bDone)
		return exePath;

	GetModuleFileNameW(NULL, temp, MAX_PATH);

	int i;
	for (i = 1; i < MAX_PATH; i++)
	{
		WCHAR snoop = temp[MAX_PATH-i];
		if (temp[MAX_PATH - i] == '\\')
		{
			memcpy(&exePath[0], &temp[0], (MAX_PATH-i)*sizeof(WCHAR));
			break;
		}
	}

	bDone = true;
	return exePath;
}

bool BLOCO_API Application::Init(wstring title, UINT width, UINT height, bool bWindowed, HWND hWindow /*= (HWND)NULL*/ )
{
	/************************************************************************/
	/* Schindige Debug Console                                              */
	/************************************************************************/
	if(true)
	{
		AllocConsole();
		SetConsoleTitleW(L"Bloco-Engine Console");

		HANDLE stdOutputHandle = GetStdHandle( STD_OUTPUT_HANDLE ); 

		int hConsole = _open_osfhandle( (intptr_t)stdOutputHandle, 0x4000);

		FILE *pFile = _fdopen( hConsole, "w" );
		*stdout = *pFile;
		setvbuf( stdout, NULL, _IONBF, 0 );

		ios::sync_with_stdio();
	}

	// time
	m_pFrameSmoother = DEBUG_CLIENTBLOCK cgl::Smoother<float>(5, m_updateInterval);
	
	// Set up checks for memory leaks.
	// always perform a leak check just before app exits.
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;  
	_CrtSetDbgFlag(tmpDbgFlag);

	// Create lua state manager
	m_pLuaStateManager = DEBUG_CLIENTBLOCK LuaStateManager();
	if (!m_pLuaStateManager)
		return false;

	// Create event manager
	m_pEventManager = DEBUG_CLIENTBLOCK EventManager("Bloco Event Manager", true);
	if(!m_pEventManager)
		return false;

	// Register Base Game Events
	RegisterBaseEvents();

	// Check system requirements
	// 	m_pSystem = DEBUG_CLIENTBLOCK GameSystem();
	// 	if(!m_pSystem->Analyze())
	// 	{
	// 		return true;
	// 	}

	if(!InitGraphics(title, width, height, bWindowed, hWindow))
	{
		return false;
	}

	// Create resource cache
	m_pResCache = DEBUG_CLIENTBLOCK ResCache(100, DEBUG_CLIENTBLOCK ResourceZipFile(FULLPATH_W(_T("\\data\\data.zip"))));
	if(!m_pResCache->Init())
		return false;

	// init input
	g_pInput = DEBUG_CLIENTBLOCK cil::CILRawInput();
	if(!g_pInput->Init())
	{
		return false;
	}

	//  init lua state manager (run init file)
	if (!m_pLuaStateManager->Init(FULLPATH("\\data\\script\\init.lua")))
		return false;
	
	// register script functions
	RegisterBaseScriptFunctions();

	VCreateGameAndView();
	m_pGame->VChangeState(BGS_LoadingGameEnvironment);

	GetSaveGameDirectory(VGetGameAppDirectory());

	return true;
}

bool BLOCO_API Application::Init( string title, UINT width, UINT height, bool bWindowed, HWND hWindow /*= (HWND)NULL*/ )
{
	/************************************************************************/
	/* Schindige Debug Console                                              */
	/************************************************************************/
	if(true)
	{
		AllocConsole();
		SetConsoleTitleW(L"Bloco-Engine Console");

		HANDLE stdOutputHandle = GetStdHandle( STD_OUTPUT_HANDLE ); 

		int hConsole = _open_osfhandle( (intptr_t)stdOutputHandle, 0x4000);

		FILE *pFile = _fdopen( hConsole, "w" );
		*stdout = *pFile;
		setvbuf( stdout, NULL, _IONBF, 0 );

		ios::sync_with_stdio();
	}

	// time
	m_pFrameSmoother = DEBUG_CLIENTBLOCK cgl::Smoother<float>(5, m_updateInterval);

	// Set up checks for memory leaks.
	// always perform a leak check just before app exits.
	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;  
	_CrtSetDbgFlag(tmpDbgFlag);

	// Create lua state manager
	m_pLuaStateManager = DEBUG_CLIENTBLOCK LuaStateManager();
	if (!m_pLuaStateManager)
	{
		printf("[SYSTEM] Application::m_pLuaStateManager is invalid\n");
		return false;
	}
	// Create event manager
	m_pEventManager = DEBUG_CLIENTBLOCK EventManager("Bloco Event Manager", true);
	if(!m_pEventManager)
	{
		printf("[SYSTEM] Application::m_pEventManager is invalid\n");
		return false;
	}
	// Register Base Game Events
	RegisterBaseEvents();

	// Check system requirements
	// 	m_pSystem = DEBUG_CLIENTBLOCK GameSystem();
	// 	if(!m_pSystem->Analyze())
	// 	{
	// 		return true;
	// 	}

	WCHAR wTitle[256];
	AnsiToWideCch(wTitle,title.c_str(),title.length()+1);

	if(!InitGraphics(wTitle, width, height, bWindowed, hWindow))
	{
		printf("[SYSTEM] Application::InitGraphics() \t\t Failed\n");
		return false;
	}

	// Create resource cache
	m_pResCache = DEBUG_CLIENTBLOCK ResCache(100, DEBUG_CLIENTBLOCK ResourceZipFile(FULLPATH_W(_T("\\data\\data.zip"))));
	if(!m_pResCache->Init())
	{
		printf("[SYSTEM] Application::m_pResCache->Init() \t\t Failed\n");
		return false;
	}
	// init input
	g_pInput = DEBUG_CLIENTBLOCK cil::CILRawInput();
	if(!g_pInput->Init())
	{
		printf("[SYSTEM] Application::g_pInput->Init() \t\t Failed\n");
		return false;
	}

	//  init lua state manager (run init file)
	if (!m_pLuaStateManager->Init(FULLPATH("\\data\\script\\init.lua")))
	{
		printf("[SYSTEM] Application::m_pLuaStateManager->Init() \t\t Failed\n");
		return false;
	}

	// register script functions
	RegisterBaseScriptFunctions();

	VCreateGameAndView();
	m_pGame->VChangeState(BGS_LoadingGameEnvironment);

	GetSaveGameDirectory(VGetGameAppDirectory());

	return true;
}

bool BLOCO_API Application::InitGraphics(wstring title, UINT width, UINT height, bool bWindowed, HWND hWindow /*= (HWND)NULL*/ )
{	
	// get engine settings
	UINT iniDevice		=	GetPrivateProfileIntA(GAME_INI_GRAPHIC_SECTION, GAME_INI_GRAPHIC_DEVICE,        0, FULLPATH("\\engine.ini"));
	UINT iniOutput		=	GetPrivateProfileIntA(GAME_INI_GRAPHIC_SECTION, GAME_INI_GRAPHIC_OUTPUT,        0, FULLPATH("\\engine.ini"));
	UINT iniWidth		=	GetPrivateProfileIntA(GAME_INI_GRAPHIC_SECTION, GAME_INI_GRAPHIC_WIDTH,	     1600, FULLPATH("\\engine.ini"));
	UINT iniHeight		=	GetPrivateProfileIntA(GAME_INI_GRAPHIC_SECTION, GAME_INI_GRAPHIC_HEIGHT,      900, FULLPATH("\\engine.ini"));
	UINT iniBpp			=	GetPrivateProfileIntA(GAME_INI_GRAPHIC_SECTION, GAME_INI_GRAPHIC_BPP,		   32, FULLPATH("\\engine.ini"));
	UINT iniWindowed	=	GetPrivateProfileIntA(GAME_INI_GRAPHIC_SECTION, GAME_INI_GRAPHIC_WINDOWED,		1, FULLPATH("\\engine.ini"));
	UINT iniRefreshRate	=	GetPrivateProfileIntA(GAME_INI_GRAPHIC_SECTION, GAME_INI_GRAPHIC_REFRESHRATE,  60, FULLPATH("\\engine.ini"));

	// initialize COM
	CoInitialize(NULL);

	// create cgl manager
	g_pCGLMgr = cgl::CGLManager::Create();

	// set notification callback to
	// control debug output
	g_pCGLMgr->SetNotificationCallback(Application::CGLNotificationProc);


	if (!hWindow)
	{
		m_pWindow = cgl::CGLWindowFromConfig::Create(title.c_str(), width, height, Application::WindowProc);
	}
	else
	{
		m_pWindow = cgl::CGLWindowFromExisting::Create(hWindow);
	}


	if (!m_pWindow->restore())
	{
		printf("[SYSTEM] Application::m_pWindow->restore() \t\t Failed\n");
		return false;
	}
	// create dxgi factory
	m_pFactory = cgl::CDXGIFactory::Create();
	if (!m_pFactory->restore())
	{
		printf("[SYSTEM] Application::m_pFactory->restore() \t\t Failed\n");
		return false;
	}

	// get installed adapters
	if (iniDevice < 0 || iniDevice > m_pFactory->GetAdapterCount())
		iniDevice = 0;

	cgl::PDXGIAdapter pAdapter = cgl::CDXGIAdapter::Create(m_pFactory, iniDevice);
	if (!pAdapter->restore())
	{
		printf("[SYSTEM] Application::pAdapter->restore() \t\t Failed\n");
		return 1;
	}

	// get adapter outputs
	if (iniOutput < 0 || iniOutput > pAdapter->GetOutputCount())
		iniOutput = 0;

	cgl::PDXGIOutput pOutput = cgl::CDXGIOutput::Create(pAdapter, iniOutput);
	if (!pOutput->restore())
	{
		printf("[SYSTEM] Application::pOutput->restore() \t\t Failed\n");
		return 1;
	}
	// get format
	if (iniBpp != 16 && iniBpp != 32)
		iniBpp = 32;

	DXGI_FORMAT format;
	if (iniBpp == 32)		{ format = DXGI_FORMAT_B8G8R8A8_UNORM; }
	else if (iniBpp == 16)  { format = DXGI_FORMAT_B5G6R5_UNORM;   }


	DXGI_MODE_DESC modeToMatch;
	modeToMatch.Format = format;
	modeToMatch.Width = iniWidth;
	modeToMatch.Height = iniHeight;
	modeToMatch.RefreshRate.Denominator = 1;
	modeToMatch.RefreshRate.Numerator = iniRefreshRate;
	modeToMatch.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	modeToMatch.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_MODE_DESC foundDesc;
	if (FAILED(pOutput->get()->FindClosestMatchingMode(&modeToMatch, &foundDesc, NULL)))
	{
		printf("[SYSTEM] Application::pOutput->get()->FindClosestMatchingMode() \t\t Failed\n");
		return false;
	}

	cgl::CGL_D3D11_DEVICE_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.driverType = D3D_DRIVER_TYPE_HARDWARE;
	desc.featureLevel = D3D_FEATURE_LEVEL_11_0;
	desc.flags = D3D11_CREATE_DEVICE_DEBUG;
	desc.swapDesc.BufferCount = 1;
	desc.swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.swapDesc.OutputWindow = m_pWindow->get();
	desc.swapDesc.SampleDesc.Count = 1;
	desc.swapDesc.SampleDesc.Quality = 0;
	desc.swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.swapDesc.Windowed = iniWindowed;
	desc.swapDesc.BufferDesc = foundDesc;

	// D3D device couldn't be created
	int tryCount = 0;

	while (!m_pCore)
	{		
		tryCount++;

		switch(tryCount)
		{
		case 1: desc.featureLevel = D3D_FEATURE_LEVEL_11_0; break;
		case 2: desc.featureLevel = D3D_FEATURE_LEVEL_10_1; break;
		case 3: desc.featureLevel = D3D_FEATURE_LEVEL_10_0; break;
		case 4: desc.featureLevel = D3D_FEATURE_LEVEL_9_3;  break;
		case 5: desc.featureLevel = D3D_FEATURE_LEVEL_9_2;  break;
		case 6: desc.featureLevel = D3D_FEATURE_LEVEL_9_1;  break;
		default: 
			{
				MessageBoxA(m_pWindow->get(), "D3D Device could not be created! Maybe your graphics card is not supported", "Error", MB_OK | MB_ICONEXCLAMATION);
				return false;
			}
		}

		m_pCore = cgl::CD3D11Device::Create(desc);
	}

	// create view port
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.Width = (FLOAT)foundDesc.Width;
	viewPort.Height = (FLOAT)foundDesc.Height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	m_pCore->GetContext()->RSSetViewports(1, &viewPort);

	g_pResourceManager = DEBUG_CLIENTBLOCK CResourceManager();


	return true;
}
void BLOCO_API Application::Exit()
{
	SAFE_DELETE(m_pGame);
	SAFE_DELETE(g_pResourceManager);
	SAFE_DELETE(m_pEventManager);
	SAFE_DELETE(m_pLuaStateManager);
	SAFE_DELETE(m_pResCache);
	SAFE_DELETE(m_pFrameSmoother);


	m_pWindow.reset();
	m_pFactory.reset();
	m_pCore.reset();
	g_pCGLMgr.reset();

	SAFE_DELETE(g_pInput);
	SAFE_DELETE(g_pAudio);

	// close console window
	ShowWindow(GetConsoleWindow(), 0);

	// uninitialize COM
	CoUninitialize();
}

void BLOCO_API Application::RegisterBaseEvents()
{
	// Events
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_PhysCollision::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_PhysSeparation::sk_EventType);

	m_pEventManager->RegisterCodeOnlyEvent(EvtData_Update_Tick::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_Debug_Draw::sk_EventType);

	m_pEventManager->RegisterCodeOnlyEvent(EvtData_Move_Actor::sk_EventType);

	m_pEventManager->RegisterEvent<EvtData_Wireframe>(EvtData_Wireframe::sk_EventType);
	m_pEventManager->RegisterEvent<EvtData_Phys_RenderDiagnostic>(EvtData_Phys_RenderDiagnostic::sk_EventType);

	m_pEventManager->RegisterCodeOnlyEvent(EvtData_SelectActor::sk_EventType);

	m_pEventManager->RegisterCodeOnlyEvent(EvtData_FileDrop::sk_EventType);

	m_pEventManager->RegisterEvent<EvtData_New_Actor>(EvtData_New_Actor::sk_EventType);
	m_pEventManager->RegisterEvent<EvtData_Request_New_Actor>(EvtData_Request_New_Actor::sk_EventType);
	m_pEventManager->RegisterEvent<EvtData_Phys_TogglePause>(EvtData_Phys_TogglePause::sk_EventType);

	m_pEventManager->RegisterCodeOnlyEvent(EvtData_Transform_Actor::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_SetActorTransform::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_Destroy_Actor::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_SetTexture::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_RunScript::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_FileModified::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_Picked_Actor::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_PerformePick::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_EnableActor::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_DragMove::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_Editor_SetRemoveMode::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_Editor_SetSelectMode::sk_EventType);

	m_pEventManager->RegisterCodeOnlyEvent(EvtData_SetStaticActor::sk_EventType);

	m_pEventManager->RegisterCodeOnlyEvent(EvtData_RayCast::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_RayCast_Result::sk_EventType);


	m_pEventManager->RegisterCodeOnlyEvent(EvtData_PhysTrigger_Enter::sk_EventType);
	m_pEventManager->RegisterCodeOnlyEvent(EvtData_PhysTrigger_Leave::sk_EventType);


	m_pEventManager->RegisterEvent<EvtData_EneableMovementController>(EvtData_EneableMovementController::sk_EventType);
 	m_pEventManager->RegisterEvent<EvtData_SetMovementController>(EvtData_SetMovementController::sk_EventType);


	VRegisterEvents();
}
void BLOCO_API Application::RegisterBaseScriptFunctions()
{
	//Physic
	m_pLuaStateManager->RegisterFunction("phyDebugMode", scriptPhysicsDebugMode);

	//Debug
	m_pLuaStateManager->RegisterFunction("print", scriptPrint);
	m_pLuaStateManager->RegisterFunction("doScript", scriptExecuteScript);

	//Scene
	m_pLuaStateManager->RegisterFunction("enableMovementController", scriptEneableMovmentController);
	m_pLuaStateManager->RegisterFunction("setMovementController", scriptSetMovmentController);

	m_pLuaStateManager->RegisterFunction("translateActor",	scriptTranslateActor);
	m_pLuaStateManager->RegisterFunction("rotateActor",		scriptRotateActor);
	m_pLuaStateManager->RegisterFunction("scaleActor",		scriptScaleActor);


	m_pLuaStateManager->RegisterFunction("setActorTranslation",	scriptSetActorTranslation);
	m_pLuaStateManager->RegisterFunction("setActorRotation",	scriptSetActorRotation);
	m_pLuaStateManager->RegisterFunction("setActorScaling",		scriptSetActorScale);

	m_pLuaStateManager->RegisterFunction("setTexture",	scriptSetTexture);
	m_pLuaStateManager->RegisterFunction("selectActor", scriptSelectActor);
	m_pLuaStateManager->RegisterFunction("enableActor", scriptEnableActor);

	m_pLuaStateManager->RegisterFunction("removeActor", scriptRemoveActor);

	VRegisterScriptFunctions();
};

UINT Application::GetWindowWidth()
{
	RECT rct; 
	GetWindowRect(m_pWindow->get(),&rct); 
	return rct.right-rct.left;	
}

UINT Application::GetWindowHeight()
{
	RECT rct; 
	GetWindowRect(m_pWindow->get(),&rct); 
	return rct.bottom-rct.top;
}

void Application::RunScript(string filename)
{
	safeTriggerEvent(EvtData_RunScript(filename));
}

float Application::GetFPS()
{
	return 1.0f/m_pGameLoop->ElapsedTimeExact();
}

float Application::GetDrawTime()
{
	return m_pGameLoop->DrawTime();
}

float Application::GetUpdateTime()
{
	return m_pGameLoop->OnUpdateTime();
}

float Application::GetIdleTime()
{
	return m_pGameLoop->OnIdleTime();
}

void Application::AbortGame()
{
	m_pGameLoop->Quit();
}
