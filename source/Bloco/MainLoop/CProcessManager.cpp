#include "Bloco.h"

void CProcessManager::Attach( shared_ptr<CProcess> pProcess )
{
	m_ProcessList.push_back(pProcess);
	pProcess->SetAttached(true);
}
void CProcessManager::Detach(shared_ptr<CProcess> pProcess)
{
	m_ProcessList.remove(pProcess);
	pProcess->SetAttached(false);
}
bool CProcessManager::IsProcessActive( int nType )
{
	for (ProcessList::iterator i = m_ProcessList.begin() ; i!=m_ProcessList.end(); ++i)
	{
		if ( (*i)->GetType() == nType && 
			( (*i)->IsDead() == false || (*i)->GetNext()))
			return true;

	}
	return false;
}
bool CProcessManager::HasProcesses()
{
	return !m_ProcessList.empty();
}

void CProcessManager::UpdateProcesses(int deltaMilliseconds)
{
	ProcessList::iterator i = m_ProcessList.begin();
	ProcessList::iterator end = m_ProcessList.end();
	shared_ptr<CProcess> pNext;

	while( i != end)
	{
		shared_ptr<CProcess> p(*i);
		++i;

		if (p->IsDead())
		{
			pNext = p->GetNext();
			if(pNext)
			{
				p->SetNext(shared_ptr<CProcess>((CProcess*)NULL));
				Attach(pNext);
			}
			Detach(p);
		}
		else if (p->IsActive() && !p->IsPaused())
		{
			p->VOnUpdate(deltaMilliseconds);
		}
	}
}

CProcessManager::~CProcessManager()
{
	for (ProcessList::iterator i = m_ProcessList.begin(); i != m_ProcessList.end(); )
	{
		Detach(*(i++));
	}
}

