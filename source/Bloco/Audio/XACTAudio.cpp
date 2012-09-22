#include "Bloco.h"

XACTAudio* g_pAudio;

XACTAudio::XACTAudio(string globalSettingsFile /* = string */) :  m_globalSettings(globalSettingsFile)
{
	// Initialize COM
	CoInitialize(NULL);

	m_pXACT = NULL;
	m_pEventListener = shared_ptr<XACTAudioEventListener>(DEBUG_CLIENTBLOCK XACTAudioEventListener(this));
}
XACTAudio::~XACTAudio()
{
	VDelete();
}

void XACTAudio::VDelete()
{
	m_pXACT->ShutDown();
	m_pXACT = NULL;

	// Initialize COM
	CoUninitialize();
}

//************************************
// Method:    Init
// FullName:  XACTAudio::Init
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: string globalSettingsFile
//************************************
bool XACTAudio::VCreate()
{
	HRESULT hResult;
	XACT_RUNTIME_PARAMETERS	params;

	// Create XACT3 Engine
	hResult = XACT3CreateEngine(0, &m_pXACT);
	if (FAILED(hResult))
	{
		return false;
	}

	// Prepare Parameters
	ZeroMemory(&params, sizeof(params));
	params.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
	params.pXAudio2 = NULL;
	params.pMasteringVoice = NULL;

	// get global settings file
	Resource r = (m_globalSettings);
	ResHandlePtr handle = g_pApp->m_pResCache->GetHandle(&r);

	params.pGlobalSettingsBuffer = (void*)handle->Buffer();
	params.globalSettingsBufferSize = handle->Size();
	
	// Initialize XACT3 Engine
	hResult = m_pXACT->Initialize(&params);
	if (FAILED(hResult))
	{
		return false;
	}

	// Initialize XACT3D
	hResult = XACT3DInitialize(m_pXACT, m_hXACT3D);
	if (FAILED(hResult))
	{
		return false;
	}

	// listen to events
	safeAddListener(m_pEventListener, EvtData_Update_Tick::sk_EventType);

	return true;
}

//************************************
// Method:    Update
// FullName:  XACTAudio::Update
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void XACTAudio::Update()
{
	HRESULT hResult = m_pXACT->DoWork();
	if (FAILED(hResult))
	{
		assert( 0 && "XACT unable to update");
	}
}

//************************************
// Method:    Load
// FullName:  XACTAudio::Load
// Access:    public 
// Returns:   bool
// Qualifier:
//************************************
bool XACTAudio::Load( string wavebankPath, string soundbankPath )
{
	assert(m_pXACT && "XACT not initialized");

	HRESULT hResult;

	// Load audio resources
	// wave bank
	Resource r(wavebankPath);
	ResHandlePtr handle = g_pApp->m_pResCache->GetHandle(&r);
	hResult = m_pXACT->CreateInMemoryWaveBank(handle->Buffer(), handle->Size(), NULL, NULL, &m_pWavebank);
	if(FAILED(hResult))
	{
		return false;
	}

	// sound bank
	r = Resource(soundbankPath);
	handle = g_pApp->m_pResCache->GetHandle(&r);
	hResult = m_pXACT->CreateSoundBank(handle->Buffer(), handle->Size(), NULL, NULL, &m_pSoundbank);
	if(FAILED(hResult))
	{
		return false;
	}
	
	return true;
}

//************************************
// Method:    GetCue
// FullName:  XACTAudio::GetCue
// Access:    private 
// Returns:   bool
// Qualifier:
// Parameter: string soundName
// Parameter: IXACT3Cue * * pCue
//************************************
bool XACTAudio::GetCue( string soundName, IXACT3Cue** ppCue, X3DAUDIO_HANDLE* pHandle )
{
	XACTINDEX cueIndex = m_pSoundbank->GetCueIndex(soundName.c_str());
	XACTINDEX numCues;
	m_pSoundbank->GetNumCues(&numCues);
	if (cueIndex < numCues)
	{
		m_pSoundbank->Prepare(cueIndex, NULL, 0, ppCue);
		if (ppCue)
		{
			memcpy(pHandle, &m_hXACT3D, sizeof(X3DAUDIO_HANDLE));
			return true;
		}
	}

	return false;
}

//************************************
// Method:    HandleEvent
// FullName:  XACTAudioEventListener::HandleEvent
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: IEventData const & event
//************************************
bool XACTAudioEventListener::HandleEvent( IEventData const & event )
{
	m_pAudio->Update();
	return false;
}
