#ifndef PROCESS_H
#define PROCESS_H

#include "Bloco.h"

class CProcessManager;

// Flags ------------------------------------------------
static const int PROCESS_FLAG_ATTACHED		= 0x00000001;

class CProcess
{
friend class CProcessManager;

protected:
	int						m_iType;
	bool					m_bKill;
	bool					m_bActive;
	bool					m_bPaused;
	bool					m_bInitialUpdate;
	shared_ptr<CProcess>	m_pNext;

private:
	unsigned int m_uProcessFlags;

public:
	CProcess(int nType, unsigned int uOrder = 0);
	virtual ~CProcess();

	bool IsDead()	const			{return m_bKill;}
	
	int GetType()	const			{return m_iType;}
	void SetType(const int t)		{m_iType = t;}

	bool IsActive()	const			{return m_bActive;}
	void SetActive(const bool b)	{m_bActive = b;}

	bool IsAttached() const;
	void SetAttached(const bool wantAttached);

	bool IsPaused()	const			{return m_bPaused;}
	bool IsInitialized() const		{return !m_bInitialUpdate;}

	shared_ptr<CProcess>	const GetNext()	const {return m_pNext;}
	void SetNext (shared_ptr<CProcess> pNext)	{m_pNext = pNext;}

	// Overloadables
	virtual void VOnUpdate(const int deltaMilliseconds);
	virtual void VOnInitialize(){};
	virtual void VKill()					{m_bKill = true;}				
	virtual void VTogglePause()				{m_bPaused = !m_bPaused;}
};


#endif