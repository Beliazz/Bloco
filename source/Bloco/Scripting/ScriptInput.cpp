#include "Bloco.h"

BLOCO_API CScriptInput::CScriptInput(LuaState* globalState) : 
	CILKeyboardState("script input handler"),
	m_globalState(globalState)
{
	m_keyTable = globalState->GetGlobal("keys");

	for (int i = 1; i < 257 ; i++)
	{
		m_keyTable.SetBoolean(i,false);
	}

	memset(m_keys, false, 0xff);

	g_pInput->AddListener((ICILMouseHandler*)this);
	g_pInput->AddListener((ICILKeyboardHandler*)this);
}

void BLOCO_API CScriptInput::OnUpdate( const int deltaMilliseconds )
{
	for (int i = 1; i < 257 ; i++)
	{
		m_keyTable.SetBoolean(i,(bool)m_keys[i-1]);
	}
}

void BLOCO_API CScriptInput::OnMouseMove( const POINT &mousePos, const POINT& delta )
{

}

void BLOCO_API CScriptInput::OnLButtonDown( const POINT &mousePos )
{

}

void BLOCO_API CScriptInput::OnLButtonUp( const POINT &mousePos )
{

}

void BLOCO_API CScriptInput::OnRButtonDown( const POINT &mousePos )
{

}

void BLOCO_API CScriptInput::OnRButtonUp( const POINT &mousePos )
{

}

void BLOCO_API CScriptInput::OnMButtonDown( const POINT &mousePos )
{

}

void BLOCO_API CScriptInput::OnMButtonUp( const POINT &mousePos )
{

}

void BLOCO_API CScriptInput::OnWheelMove( const POINT &mousePos, const short delta )
{

}
