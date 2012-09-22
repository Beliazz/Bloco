#ifndef Application_h__
#define Application_h__

#include "bloco.h"

#define FULLPATH(path)			(LPSTR)(string(g_pApp->GetExePathA()) + string(path)).data()
#define FULLPATH_W(path)		(LPWSTR)(wstring(g_pApp->GetExePathW()) + wstring(path)).data()
#define SHADER_PATH(path)		(LPSTR)(string("shader\\") + string(path) + string(".fxc")).data()
#define TEXTURE_PATH(path)		(LPSTR)(string("texture\\") + string(path)).data()
#define MODEL_PATH(path)		(LPSTR)(string(g_pApp->GetExePathA()) + string("\\data\\model\\") + string(path)).data()
#define SCRIPT_PATH(path)		(LPSTR)(string(g_pApp->GetExePathA()) + string("\\data\\script\\") + string(path) + string(".lua")).data()

class BLOCO_API Application
{
private:
	bool		m_bWindowed;
	bool		m_bIsRunning;
	bool		m_bQuitRequested;
	bool		m_bQuitting;
	HINSTANCE	m_hInstance;

	UINT64	m_start;
	UINT64	m_end;
	UINT64	m_frequency;

	bool m_updated;
	bool m_drawn;
	bool m_occluded;
	bool m_minimized;

	MSG  m_msg;

	// time
	float m_updateInterval;
	double m_time;
	Smoother<float>* m_pFrameSmoother;

	cgl::PCGLWindow		m_pWindow;
	cgl::PDXGIFactory	m_pFactory;
	cgl::PD3D11Device	m_pCore;

	void	GetGameAdapter();
	void	CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
	void	CheckHardDisk(const DWORDLONG diskSpaceNeeded);	
	LPWSTR	GetSaveGameDirectory(LPWSTR gameAppDirectory);
	void	RegisterBaseEvents();
	void	RegisterBaseScriptFunctions();

public:
	LuaStateManager*	m_pLuaStateManager;
	EventManager*		m_pEventManager;
	BaseGameLogic*		m_pGame; 
	ResCache*			m_pResCache;
	CProcessManager*	m_pProcessManager;
	//GameSystem*			m_pSystem;

	Application(Application** ppApp);
	~Application();

	virtual LPWSTR			VGetGameTitle() = 0;
	virtual HICON			VGetIcon() = 0;
	virtual LPWSTR			VGetGameAppDirectory() = 0;
	virtual void			VCreateGameAndView() = 0;
	virtual bool			VLoadGame()=0;
	virtual void			VRegisterEvents() = 0;
	virtual void			VRegisterScriptFunctions() = 0;

	// WIN32 specific
	inline HWND GetHWND()				{ return m_pWindow->get(); }
	inline HINSTANCE GetInstance()		{ return m_hInstance; }
	inline ID3D11Device* GetDevice()	{ return m_pCore->GetDevice(); }
	inline ID3D11DeviceContext* GetDeviceContext()	{ return m_pCore->GetContext(); }
	inline UINT GetScreenWidth()		{ return 1600;	/*DXUTGetDXGIBackBufferSurfaceDesc()->Width*/;}
	inline UINT GetScreenHeight()		{ return 900;	/*DXUTGetDXGIBackBufferSurfaceDesc()->Height*/;}
	inline float GetFPS()				{ return 1.0f / m_pFrameSmoother->GetSmoothed();}

	bool	Init(wstring title, UINT width, UINT height, bool bWindowed, HWND hWindow = (HWND)NULL);
	bool	Init(string title, UINT width, UINT height, bool bWindowed, HWND hWindow = (HWND)NULL);
	bool	InitGraphics(wstring title, UINT width, UINT height, bool bWindowed, HWND hWindow = (HWND)NULL);
	LPSTR	GetExePathA();
	LPWSTR	GetExePathW();

	//inline GameSystem&  GetGameSystem()		{ return *m_pSystem;}

	void Run();		//Blocking func
	void Update();  //none blocking
	void OnRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, double dTime, float fElapsed);
	void OnUpdate(double dTime, float fElapsed);

	static bool CALLBACK CGLNotificationProc( UINT type, cgl::CGLObject* pObject, HRESULT result, std::string error, std::string errordesc);
	static LRESULT CALLBACK WindowProc( HWND hwnd, unsigned int message, WPARAM wParam, LPARAM lParam );

	// main loop processing
	void AbortGame()				{ m_bQuitting = true; }
	//int GetExitCode()				{ return DXUTGetExitCode(); }
	bool IsRunning()				{ return m_bIsRunning; }
	void SetQuitting(bool quitting) { m_bQuitting = quitting; }
	void Exit();
};

extern Application BLOCO_API *	g_pApp;
extern cil::CILRawInput* 	g_pInput;
extern cgl::CGLManager*	 	g_pCGLMgr;
extern CResourceManager* 	g_pResourceManager;

#endif // Application_h__
