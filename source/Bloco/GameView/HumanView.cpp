#include "Bloco.h"

class Mesh;

/*const unsigned int SCREEN_REFRESH_RATE(1000/60);*/

 BLOCO_API HumanView::HumanView()
{
	InitAudio(); 

	m_pProcessManager = DEBUG_CLIENTBLOCK CProcessManager();
	m_pFont = NULL;
	m_pFontFactory = NULL;
	m_pRenderer = NULL;
	
	m_pConsole = DEBUG_CLIENTBLOCK Console();
}

 BLOCO_API HumanView::~HumanView()
{
	while (!m_ScreenElements.empty())
	{
		m_ScreenElements.pop_front();		
	}

	SAFE_DELETE( m_pProcessManager );

	VOnLostDevice();

	SAFE_DELETE(m_pRenderer);
	SAFE_DELETE(m_pFont);
	SAFE_DELETE(m_pConsole);
	
	// post press - I changed the g_pAudio pointer to a shared_ptr
	// since there can be multiple human views
}

void BLOCO_API HumanView::VOnRender(double fTime, float fElapsedTime )
{
	m_pRenderer->Begin();

	ScreenElementList::iterator i = m_ScreenElements.begin();
	while ( i!=m_ScreenElements.end() && (*i)->VGetZOrder() <= PASS_SCENE)
	{
		if ( (*i)->VIsVisible() )
		{
			(*i)->VOnRender(fTime, fElapsedTime);
		}

		++i;
	}

	VRenderDiagnostic();

	// render diagnostics
	VRenderText();

	m_pRenderer->End();


	return;




	m_currTick = timeGetTime();
	if (m_currTick == m_lastDraw)
		return;

	// It is time to draw ?
	if( m_runFullSpeed || ( (m_currTick - m_lastDraw) > SCREEN_REFRESH_RATE) )
	{
		// sort list (z-order)
		//m_ScreenElements.sort(SortBy_SharedPtr_Content<IScreenElement>());

		// begin issuing draw calls
		m_pRenderer->Begin();

		// SCENE =================================================================================
		// begin pass
		m_pRenderer->BeginPass(PASS_SCENE);

		ScreenElementList::iterator i = m_ScreenElements.begin();
		while ( i!=m_ScreenElements.end() && (*i)->VGetZOrder() <= PASS_SCENE)
		{
			if ( (*i)->VIsVisible() )
			{
				(*i)->VOnRender(fTime, fElapsedTime);
			}
			
			++i;
		}

		VRenderDiagnostic();


		// end pass
		m_pRenderer->EndPass(PASS_SCENE);
		// =========================================================================================

		// POST PROCESSING =========================================================================
		// start pass
		m_pRenderer->BeginPass(PASS_POST_PROCESSING);

		while ( i!=m_ScreenElements.end() && (*i)->VGetZOrder() <= PASS_POST_PROCESSING)
		{
			if ( (*i)->VIsVisible() )
			{
				(*i)->VOnRender(fTime, fElapsedTime);
			}

			++i;
		}

		// end pass
		m_pRenderer->EndPass(PASS_POST_PROCESSING);
		// ========================================================================================

		// OVERLAY ================================================================================
		// start pass
		m_pRenderer->BeginPass(PASS_OVERLAY);

		while ( i!=m_ScreenElements.end() && (*i)->VGetZOrder() <= PASS_OVERLAY)
		{
			if ( (*i)->VIsVisible() )
			{
				(*i)->VOnRender(fTime, fElapsedTime);
			}

			++i;
		}

		// Let the console render.
		m_pConsole->Render(m_pFont, m_pRenderer->GetContext());	

		// end pass
		m_pRenderer->EndPass(PASS_OVERLAY);
		// ========================================================================================

		// end drawing
		m_pRenderer->End();

		// record the last successful paint
		m_lastDraw = m_currTick;
	}
}

HRESULT BLOCO_API HumanView::VOnRestore()
{
	HRESULT hr = S_OK;

	// Initialize the font
	m_pFontFactory = NULL;
	FW1CreateFactory(FW1_VERSION, &m_pFontFactory);
	if(!m_pFontFactory)
	{
		return S_FALSE;
	}

	if(!m_pFont)
	{
		m_pFont = DEBUG_CLIENTBLOCK CD3D11Font(L"Arial");
	}
	if(!m_pFont->VCreate(m_pFontFactory))
	{
		return S_FALSE;
	}
	
	if( !m_pRenderer )
	{
		m_pRenderer = DEBUG_CLIENTBLOCK CD3D11Renderer();
	}
	if (!m_pRenderer->VOnRestore())
	{
		return S_FALSE;
	}

	for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
	{
		( (*i)->VOnRestore() );
	}

	return hr;
}

void BLOCO_API HumanView::VOnLostDevice() 
{
	SAFE_RELEASE( m_pFontFactory );
	
	m_pFont->VOnLostDevice();
	m_pRenderer->VOnLostDevice();
}

bool BLOCO_API HumanView::InitAudio()
{
	if (!g_pAudio)
	{
		g_pAudio = DEBUG_CLIENTBLOCK XACTAudio();			// use this line for DirectSound
	}

	if (!g_pAudio)
		return false;

	if (!g_pAudio->VCreate())
		return false;

	return true;
}

LRESULT BLOCO_API CALLBACK HumanView::VOnMsgProc( AppMsg msg )
{
	// Iterate through the screen layers first
	// In reverse order since we'll send input messages to the 
	// screen on top
	for(ScreenElementList::reverse_iterator i=m_ScreenElements.rbegin(); i!=m_ScreenElements.rend(); ++i)
	{
		if ( (*i)->VIsVisible() )
		{
			if ( (*i)->VOnMsgProc( msg ) )
			{
				return 1;
			}
		}
	}

	return 0;
}

void BLOCO_API HumanView::VOnUpdate( int deltaMilliseconds )
{
	m_pConsole->Update(deltaMilliseconds);

	// This section of code was added post-press. It runs through the screenlist
	// and calls VOnUpdate. Some screen elements need to update every frame, one 
	// example of this is a 3D scene attached to the human view.
	//
	for(ScreenElementList::iterator i=m_ScreenElements.begin(); i!=m_ScreenElements.end(); ++i)
	{
		(*i)->VOnUpdate(deltaMilliseconds);
	}
}

void BLOCO_API HumanView::VPushElement(shared_ptr<IScreenElement> pElement)
{
	m_ScreenElements.push_front(pElement);
}

void BLOCO_API HumanView::VPopElement(shared_ptr<IScreenElement> pElement)
{
	//m_ScreenElements.pop_front();
	m_ScreenElements.remove(pElement);
}

void BLOCO_API HumanView::VRenderText()
{
	stringstream ss;
	float times[5] = {0, 0, 0, 0, 0};
	if (m_pRenderer->GetRenderStats(times))
	{
		ss	<< "draw: " << times[0] * 1000.0f << " ms" << endl
			<< " scene: " << times[1] * 1000.0f << " ms" << endl
			<< " post: " << times[2] * 1000.0f << " ms" << endl
			<< " overlay: " << times[3] * 1000.0f << " ms" << endl
			<< " other: " << times[4] * 1000.0f << " ms" << '\0';																																		
	}
	else
	{
		ss << "FPS: " << (int) g_pApp->GetFPS();
	}

	m_pFont->VRender(g_pApp->GetDeviceContext(), ss.str(), 14, Vec(g_pApp->GetScreenWidth() - 200, 15), Col(1, 1, 1, 1));
}

void BLOCO_API HumanView::VRenderDiagnostic()
{

}

// Begin definition of the HumanView::Console class.
const int kCursorBlinkTimeMS = 500;

char const * const kExitString = "exit";
char const * const kClearString = "clear";
char const * const kFrameStatsString = "frame";

BLOCO_API HumanView::Console::Console()
	: CILKeyboardState("console keyboard handler"), m_bActive( false ), m_bExecuteStringOnUpdate( false )
{
	m_CurrentInputString = std::string("");

	SetFontColor(Col( 1.0f, 1.0f, 1.0f, 1.0f ));
	SetFontSize(20.0f);

	m_CursorBlinkTimer = kCursorBlinkTimeMS;
	m_bCursorOn = true;
	
	m_helperPos = 0;

	g_pInput->AddListener((ICILKeyboardHandler*)this);
}

BLOCO_API HumanView::Console::~Console()
{
}

void BLOCO_API HumanView::Console::AddDisplayText( const std::string & newText )
{
	string temp = m_CurrentOutputString;
	m_CurrentOutputString.clear();
	m_CurrentOutputString += newText;
	m_CurrentOutputString += '\n';
	m_CurrentOutputString += temp;
}

void BLOCO_API HumanView::Console::AddDisplayText( const std::wstring & newText )
{
	stringstream ss;
	ss << newText.c_str();
	AddDisplayText(ss.str());
}

void BLOCO_API HumanView::Console::SetDisplayText( const std::string & newText )
{
	m_CurrentOutputString = newText;
}

void BLOCO_API HumanView::Console::Update( const int deltaMilliseconds )
{
	//Don't do anything if not active.
	if ( !m_bActive )
	{
		return;	//Bail!
	}

	//Do we have a string to execute?
	if ( true == m_bExecuteStringOnUpdate )
	{
		const std::string renderedInputString = std::string("> ") + m_CurrentInputString;
		if ( 0 == m_CurrentInputString.compare( kExitString ) )
		{
			SetActive( false );
			m_CurrentInputString.clear();
			m_helperPos = -1;
		}
		else if ( 0 == m_CurrentInputString.compare( kClearString ) )
		{
			m_CurrentOutputString.clear();	//clear
			m_CurrentInputString.clear();
			m_helperPos = -1;
		}
		else
		{
			//Attempt to execute the current input string...
			if (m_CurrentInputString.size() > 1)
			{
				const int retVal = g_pApp->m_pLuaStateManager->ExecuteString( m_CurrentInputString.c_str() );
				
				//Clear the input string
				if ( 0 == retVal )
				{	
					vector<string> temp;
					temp.push_back(m_CurrentInputString);
					for (UINT i = 0; i < m_history.size(); i++)
					{
						temp.push_back(m_history[i]);
					}

					m_history = temp;
					
					//Put the input string into the output window.
					AddDisplayText( renderedInputString );
				}
			}

			m_CurrentInputString.clear();		
		}

		//We're accepting input again.
		m_bExecuteStringOnUpdate = false;
	}

	//Update the cursor blink timer...
	m_CursorBlinkTimer -= deltaMilliseconds;

	if ( m_CursorBlinkTimer < 0 )
	{
		m_CursorBlinkTimer = 0;

		m_bCursorOn = !m_bCursorOn;

		m_CursorBlinkTimer = kCursorBlinkTimeMS;
	}
}

void BLOCO_API HumanView::Console::Render( CD3D11Font* pFont, ID3D11DeviceContext* pContext )
{
	//Don't do anything if not active.
	if ( !m_bActive )
	{
		return;	//Bail!
	}

	POINTF inputTextPos, outputTextPos, shadowPos;

	//Display the console text at screen top, below the other text displayed.
	const string finalInputString = string( "> " ) + m_CurrentInputString + ( m_bCursorOn ? '\xa0' : '_' );
	inputTextPos.x= 5;
	inputTextPos.y = 15;

	//Draw with shadow first.
	shadowPos = inputTextPos;
	++shadowPos.x;
	++shadowPos.y;

	pFont->VRender(pContext, finalInputString.c_str(), m_size,  Vec(shadowPos.x, shadowPos.y), Col(0.0f, 0.0f, 0.0f, m_alpha));

	//Now bright text.
	pFont->VRender(pContext, finalInputString.c_str(), m_size,  Vec(inputTextPos.x, inputTextPos.y), m_InputColor);

	//Now display the output text just below the input text.
	outputTextPos.x = inputTextPos.x + 10;
	outputTextPos.y = inputTextPos.y + m_size + 5;

	//Draw with shadow first.
	shadowPos = outputTextPos;
	++shadowPos.x;
	++shadowPos.y;

	pFont->VRender(pContext, m_CurrentOutputString.c_str(), m_size - m_size / 10 * 2,  Vec(shadowPos.x, shadowPos.y), Col(0.0f, 0.0f, 0.0f, m_alpha));

	//Now bright text.
	pFont->VRender(pContext, m_CurrentOutputString.c_str(), m_size - m_size / 10 * 2,  Vec(outputTextPos.x, outputTextPos.y), m_OutputColor);
}

void BLOCO_API HumanView::Console::HandleKeyboardInput( const std::string keyVal )
{
	if ( true == m_bExecuteStringOnUpdate )
	{
		//We've already got a string to execute; ignore.
		return;
	}

	m_CurrentInputString += keyVal;
	m_helperPos = -1;
}

void BLOCO_API HumanView::Console::HandleKeyboardSysKey( const cil::CIL_KEY key, bool down )
{
	switch( key )
	{
	case VK_BACK:
		{
			if (down)
			{
				const size_t strSize = m_CurrentInputString.size();
				if ( strSize > 0 )
				{
					m_CurrentInputString.erase( ( strSize - 1 ), 1 );
				}

				m_helperPos = -1;
			}

		}
		break;

	case VK_RETURN:
		{
			if (down)
			{
				m_bExecuteStringOnUpdate = true;	//Execute this string.
				m_helperPos = -1;
			}

		}
		break;

	case VK_UP:
		{	
			if (!down)
			{
				m_helperPos--;
				m_helperPos = clamp<int>(m_helperPos, -1, m_history.size() - 1);
				
				if (m_helperPos == -1)
					m_CurrentInputString.clear();

				if (m_helperPos > -1 && m_helperPos < m_history.size())
				{
					m_CurrentInputString = m_history[m_helperPos];
				}	
			}

		} break;

	case VK_DOWN:
		{			
			if (!down)
			{
				m_helperPos++;
				m_helperPos = clamp<int>(m_helperPos, -1, m_history.size() - 1);

				if (m_helperPos > -1 && m_helperPos < m_history.size())
				{
					m_CurrentInputString = m_history[m_helperPos];
				}	
			}
		} break;
	}
}

void BLOCO_API HumanView::Console::SetFontColor( Col color )
{
	m_InputColor = Col(color.GetX(), color.GetY(), color.GetZ(), 1.0f); 
	m_OutputColor = m_InputColor * 0.85f;
}

void BLOCO_API HumanView::Console::CustomOnKeyUp( const cil::CIL_KEY c )
{
	if(IsActive())
	{
		
		if(c == VK_PAUSE)
		{
			SetActive(false);
		}
		else
		{
			HandleKeyboardSysKey(c, false);
		}

// 		if (c != VK_BACK && c != VK_RETURN && c != VK_OEM_5)
// 		{
// 			std::string s = ToAscii(c);
// 			if(!s.empty())
// 			{
// 				HandleKeyboardInput( s );
// 			}
// // 			USHORT buff[3];
// // 			int scan;
// // 			HKL layout = GetKeyboardLayout(0);
// // 			BYTE keys[256];
// // 			GetKeyboardState(keys);
// // 			if(layout)
// // 			{
// // 				if (c == 17)
// // 				{
// // 					bool gotcha = true;
// // 				}
// // 
// // 				scan = MapVirtualKeyEx(c, 0, layout);
// // 				if (ToAsciiEx(c, scan, keys, buff, 0, layout) > 0)
// // 				{
// // 					std::string s;
// // 					s += buff[0];
// // 					HandleKeyboardInput( s );	
// // 				}
// // 			}	
// 		}

	}
	else if(c == VK_PAUSE)
	{
		SetActive(true);
	}
}

void BLOCO_API HumanView::Console::CustomOnKeyDown( const cil::CIL_KEY c )
{
	if(IsActive())
	{
		if( c != VK_OEM_2 )
		{
			HandleKeyboardSysKey(c, true);

			if (c != VK_BACK && c != VK_RETURN && c != VK_OEM_5)
			{
				std::string s = ToAscii(c);
				if(!s.empty())
				{
					HandleKeyboardInput( s );
				}
			}
		}
	}
}

