#include "Bloco.h"

class XACTAudio;
class XACTAudioEventListener : public IEventListener
{
private:
	XACTAudio* m_pAudio;

public:
	explicit XACTAudioEventListener(XACTAudio* pAudio) : m_pAudio(pAudio){};
	~XACTAudioEventListener(){ };

	char const * GetName(void) { return "XACTAudioEventListener"; }
	bool HandleEvent( IEventData const & event );
};

class XACTAudio
{	

friend class XACTSound;

private:
	IXACT3Engine*				m_pXACT;
	X3DAUDIO_HANDLE				m_hXACT3D;
	IXACT3SoundBank*			m_pSoundbank;
	IXACT3WaveBank*				m_pWavebank;
	EventListenerPtr			m_pEventListener;
	string						m_globalSettings;

	bool	GetCue(string soundName, IXACT3Cue** ppCue, X3DAUDIO_HANDLE* ppHandle);
	void	PlayCue(IXACT3Cue* pCue);

	void VDelete();

public:
	XACTAudio(string globalSettingsFile = "");
	~XACTAudio();

	bool VCreate();
	bool Load(string waveBankPath, string soundBankPath);
	void Update();
};

extern XACTAudio* g_pAudio;

