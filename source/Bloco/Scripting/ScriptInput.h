#ifndef ScriptInput_h__
#define ScriptInput_h__

#include "Bloco.h"

class BLOCO_API CScriptInput : public cil::ICILMouseHandler, public cil::CILKeyboardState
{
protected:
	LuaObject m_keyTable;
	LuaState* m_globalState;
public:
	CScriptInput(LuaState* globalState);

	string GetName()  {return "CScriptInput input handler";}
	void OnUpdate( const int deltaMilliseconds );

	virtual void OnMouseMove(const POINT &mousePos, const POINT& delta);
	virtual void OnLButtonDown(const POINT &mousePos);
	virtual void OnLButtonUp(const POINT &mousePos);
	virtual void OnRButtonDown(const POINT &mousePos);
	virtual void OnRButtonUp(const POINT &mousePos);
	virtual void OnMButtonDown(const POINT &mousePos);
	virtual void OnMButtonUp(const POINT &mousePos);
	virtual void OnWheelMove(const POINT &mousePos, const short delta);
};



#endif