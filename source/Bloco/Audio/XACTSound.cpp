#include "Bloco.h"

bool XACTSound::VCreate(string fileName)
{
	m_fileName = fileName;
	if(!g_pAudio->GetCue(m_fileName, &m_pCue, &m_hX3D))
	{
		return false;
	}

	return true;
}

void XACTSound::Play()
{
	assert(m_pCue && "sound not initialized"); 
	
	DWORD state;
	m_pCue->GetState(&state);
	if (state == XACT_CUESTATE_PLAYING)
	{
		m_pCue->Pause(false);
	}
	else
	{
		m_pCue->Play();
	}
}

bool XACTSound::IsPlaying()
{
	assert(m_pCue && "sound not initialized"); 

	DWORD state;
	m_pCue->GetState(&state);
	if (state == XACT_CUESTATE_PLAYING)
	{
		return true;
	}

	return false;
}

void XACTSound::SetParams( Vec& pos, Vec& vel)
{
	m_emitter.Position.x = pos.GetX();
	m_emitter.Position.y = pos.GetY();
	m_emitter.Position.z = pos.GetZ();

	m_emitter.Velocity.x = vel.GetX();
	m_emitter.Velocity.y = vel.GetY();
	m_emitter.Velocity.z = vel.GetZ();
}

XACTSound::~XACTSound()
{
	if(m_pCue)
	{
		m_pCue->Destroy();
	}
}
