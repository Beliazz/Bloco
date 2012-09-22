#include "Bloco.h"

class XACTSound
{
public:
	XACTSound(): m_pCue(NULL) {}
	~XACTSound();

	bool VCreate(string fileName);

	void Play();
	void Pause()	{assert(m_pCue && "sound not initialized"); m_pCue->Pause(true);}
	void Stop()		{assert(m_pCue && "sound not initialized"); m_pCue->Stop(NULL);}
	bool IsPlaying();

	void SetParams(Vec& pos, Vec& vel);

private:
	string					m_fileName;
	IXACT3Cue*				m_pCue;
	X3DAUDIO_EMITTER		m_emitter;
	X3DAUDIO_LISTENER		m_listener;
	X3DAUDIO_DSP_SETTINGS	m_dsp;
	X3DAUDIO_HANDLE			m_hX3D;
};