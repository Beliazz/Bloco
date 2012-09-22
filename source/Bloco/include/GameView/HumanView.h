#ifndef HumanView_h__
#define HumanView_h__

#include "Bloco.h"

typedef std::list<shared_ptr<IScreenElement> > ScreenElementList;

#define  SCREEN_REFRESH_RATE 60

class BLOCO_API HumanView : public IGameView
{
protected:
	GameViewId		m_ViewId;
	optional<ActorId> m_ActorId;

	CProcessManager *m_pProcessManager;				// strictly for things like button animations, etc.

	DWORD m_currTick;		// time right now
	DWORD m_lastDraw;		// last time the game rendered
	bool m_runFullSpeed;	// set to true if you want to run full speed
	
	CD3D11Renderer*		m_pRenderer;
	IFW1Factory*		m_pFontFactory;
	CD3D11Font*			m_pFont;
	ScreenElementList	m_ScreenElements;				// a game screen entity

	virtual void VRenderText();

public:
	// Implement the IGameView interface
	virtual HRESULT VOnRestore();
	virtual void VOnRender(double fTime, float fElapsedTime );
	virtual void VOnLostDevice();
	virtual GameViewType VGetType() { return GameView_Human; }
	virtual GameViewId VGetId() const { return m_ViewId; }

	virtual void VOnAttach(GameViewId vid, optional<ActorId> aid)
	{
		m_ViewId = vid; 
		m_ActorId = aid;
	}
	virtual LRESULT CALLBACK VOnMsgProc( AppMsg msg );
	virtual void VOnUpdate( int deltaMilliseconds );

	virtual void VRenderDiagnostic();

	// Virtual methods to control the layering of interface elements
	virtual void VPushElement(shared_ptr<IScreenElement> pScreen);
	virtual void VPopElement(shared_ptr<IScreenElement> pElement);
	
	virtual ~HumanView();
	HumanView();

	// Audio
	bool InitAudio();

	//Camera adjustments.
	virtual void VSetCameraOffset( Vec camOffset ) { }

public:
	// This class offers a method for entering script commands at runtime.
	// Activate it by pressing the Tilde (~) key during runtime, and type
	// "exit" or "~" and pressing enter.
	class BLOCO_API Console : public cil::CILKeyboardState
	{
	public:
		Console(void);
		~Console(void);

		void AddDisplayText( const std::string  & newText );
		void AddDisplayText( const std::wstring & newText );
		void SetDisplayText( const std::string  & newText );

		void SetActive( const bool bIsActive )  { m_bActive = bIsActive; }
		bool IsActive(void) const  { return m_bActive; }

		void HandleKeyboardInput( const std::string  keyVal );
		void HandleKeyboardSysKey( const cil::CIL_KEY key, bool down );

		void Update( const int deltaMilliseconds );
		void Render( CD3D11Font* pFont, ID3D11DeviceContext* pContext );

		void SetFontColor(Col color);
		void SetFontSize(float size) {m_size = size;}
		void SetAlpha(float alpha)	 {m_alpha = alpha; m_InputColor.SetW(m_alpha); m_OutputColor.SetW(m_alpha);}

	private:
		bool m_bActive;

		std::queue<std::string> m_DisplayStrings;
		vector<string> m_history;

		RECT m_ConsoleOutputRect;	//Where results get shown
		RECT m_ConsoleInputRect;	//Where input is entered

		Col m_InputColor;
		Col m_OutputColor;
		float m_size;
		float m_alpha;

		std::string m_CurrentOutputString;	//What's the current output string?
		std::string m_CurrentInputString;	//What's the current input string?

		int m_CursorBlinkTimer;	//Countdown to toggle cursor blink state
		bool m_bCursorOn;	//Is the cursor currently displayed?

		UINT m_helperPos;

		//If this is true, we have a string to execute on our next update cycle.  
		//We have to do this because otherwise the user could interrupt in the midst
		//of an ::Update() call in another system.  This causes problems.
		bool m_bExecuteStringOnUpdate;

	protected:	
		void CustomOnKeyDown( const cil::CIL_KEY c);
		void CustomOnKeyUp( const cil::CIL_KEY c);
	};

	Console* m_pConsole;
};

#endif // HumanView_h__