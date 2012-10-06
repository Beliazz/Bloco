#include "Bloco.h"
#include <process.h>
/************************************************************************/
/*CD3D11Bone                                                            */
/************************************************************************/
typedef struct 
{
  

  int iParam;
  TCHAR *szParam;
 
} THREADSTRUCT, *PTHREADSTRUCT;
 
DWORD WINAPI ThreadProc(LPVOID pVoid)
{
 
 /* 
  * Grab the argument passed to CreateThread(),
  * and cast it to the proper type.
  */
  PTHREADSTRUCT pts = (PTHREADSTRUCT)pVoid;
 

 /*
  * Clean up this thread's resources by returning.
  * Note that by returning from this function,
  * the system will automatically call ExitThread()
  * with the value returned.
  */
  return 0UL;
}










CD3D11Bone::CD3D11Bone( string name, string parentName, Mat bindPoseMatrix, Mat globalMatrix, Mat* global, IActor* pActor ) :
	m_sName(name), 
	m_sParent(parentName),
	m_matBindPose(bindPoseMatrix),
	m_matGlobal(globalMatrix),
	m_matParent(global),
	m_pActor(pActor)
{
}

CD3D11Bone::~CD3D11Bone()
{

}

bool CD3D11Bone::Init()
{
	return true;
}

bool CD3D11Bone::Restore()
{
	return true;
}

bool CD3D11Bone::Render( )
{
	return true;
}

bool CD3D11Bone::Update( DWORD keyindex, float interpol )
{
	m_matGlobal = m_pCurrentAnimTrack->GetKey(keyindex)->GetMatrix();

	return true;
}

void CD3D11Bone::SetCurrentAnimationsTrack( string name )
{
	m_pCurrentAnimTrack = m_AnimTrackMap[name];
}

void CD3D11Bone::AddAnimationsTrack( shared_ptr<AnimationsTrack> track )
{
	m_AnimTrackMap[track->GetName()] = track;
}

void CD3D11Bone::SetWorldMatrix( Mat world )
{
	m_matGlobal = world;
}

Mat CD3D11Bone::GetWorldMatrix()
{
	return m_matBindPose*m_matGlobal*(*m_matParent);
}



/************************************************************************/
/*CD3D11Skeleton                                                        */
/************************************************************************/

CD3D11Skeleton::CD3D11Skeleton( cgl::PD3D11Effect pEffect ) :
	m_CurrentTime(0.0f),
	m_lastKey(0),
	m_pEffect(pEffect)
{
	m_pevMatrixPalette = cgl::CD3D11EffectVariableFromName::Create( pEffect, "matBonePallet" );
}

CD3D11Skeleton::~CD3D11Skeleton()
{
	for (unsigned int i = 0; i < m_pBones.size() ; i++)
	{
		SAFE_DELETE(m_pBones[i]);
	}

	m_pBones.clear();
	m_AnimTrackMap.clear();
}
cgl::PCGLTimer t;
bool CD3D11Skeleton::Init()
{
	t = cgl::CGLCpuTimer::Create();


	m_CurrentTime = 0.0f;
	m_fTime = 1.0f;
	for (unsigned int i = 0; i < BoneCount() ; i++)
	{
		if(!GetBone(i)->Init())
			return false;
	}

	for (map<string,shared_ptr<AnimationsTrack>>::iterator it = m_AnimTrackMap.begin(); it !=m_AnimTrackMap.end() ; it++)
	{
		if(!it->second->Init())
			return false;
	}

	SetCurrentAnimationsTrack(  m_AnimTrackMap.begin()->first );

	m_BoneGlobals.resize(BoneCount());

	if (m_AnimTrackMap.empty())
		return false;

	if(!m_pevMatrixPalette->restore())
		return false;



	return true;
}

bool CD3D11Skeleton::Restore()
{
	m_CurrentTime = 0.0f;

	for (unsigned int i = 0; i < BoneCount() ; i++)
	{
		if(!GetBone(i)->Restore())
			return false;
	}

	for (map<string,shared_ptr<AnimationsTrack>>::iterator it = m_AnimTrackMap.begin(); it !=m_AnimTrackMap.end() ; it++)
	{
		if(!it->second->Restore())
			return false;
	}

	if (m_AnimTrackMap.empty())
		return false;


	SetCurrentAnimationsTrack(  m_AnimTrackMap.begin()->first );

	m_BoneGlobals.resize(BoneCount());

	if(!m_pevMatrixPalette->restore())
		return false;

	return true;
}

bool CD3D11Skeleton::VRender( )
{
	if(m_pevMatrixPalette->get()->AsMatrix()->SetMatrixArray( m_BoneGlobals[0].GetArray(), 0, m_BoneGlobals.size() ) != S_OK )
		return false;

	return true;
}

CD3D11Bone* CD3D11Skeleton::FindBone( string name )
{
	for (unsigned int i = 0; i < BoneCount() ; i++)
	{
		if (GetBone(i)->GetName() == name )
			return GetBone(i);
	}

	return NULL;
}

bool CD3D11Skeleton::SetupBoneHierarchy()
{
	for (unsigned int i = 0; i < BoneCount() ; i++)
	{
		if (GetBone(i)->GetParentName() == "Root")
		{
			GetBone(i)->SetParent(NULL);
			continue;
		}

		CD3D11Bone* parentBone= FindBone(GetBone(i)->GetParentName());

		if (!parentBone )
			return false;

		GetBone(i)->SetParent(parentBone);
	}

	return true;
}

void CD3D11Skeleton::SetProjectionsMatrix( Mat projection )
{
	for (unsigned int i = 0; i < BoneCount() ; i++)
	{
		//GetBone(i)->SetProjectionsMatrix(projection);
	}
}

void CD3D11Skeleton::SetViewMatrix( Mat view )
{
}

void CD3D11Skeleton::SetGlobalMatrix( Mat world )
{
}

void CD3D11Skeleton::SetWorldMatrix( Mat world )
{
}





bool CD3D11Skeleton::Update( DWORD const elapsedMs )
{
	m_fTime += (float)elapsedMs/1000;

	if (m_fTime > m_pCurrentAnimTrack->GetEndTime() )
	{
		//Set Next Track && Time = Start Time
		SetNextAnimationsTrack();
		m_lastKey = 0;
		m_fTime = m_pCurrentAnimTrack->GetStartTime();
	}

// 	THREADSTRUCT ts; 
// 
// 	/* Fill in the data structure */
// 	ts.iParam   = 69;
// 	ts.szParam  = _T("Hello, threaded world!");
// 
// 	m_thread = CreateThread(NULL,				/* Security attributes (NULL = default) */
// 		0,					/* Stack size (0 = default) */
// 		ThreadProc,		/* Address of thread callback */
// 		(LPVOID)&ts,		/* Argument casted to void pointer */
// 		0,					/* Creation flags (0 = not suspended) */
// 		NULL);				/* Pointer to thread ID value */
// 
// 	if (m_thread == NULL)
// 		printf("Failed to create thread! [%d]\n", GetLastError());
// 
// 
// 	t->Stop();

	t->Start();
	BuildAnimation();
	t->Stop();
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CD3D11Skeleton::BuildAnimation()
{
	DWORD firstKeyIndex = 0;
	DWORD nextKeyIndex  = 0;

	if (m_fTime < m_pCurrentAnimTrack->GetKey(m_lastKey)->GetTime())
		m_lastKey = 0;

	for (unsigned int i = m_lastKey; i < m_pCurrentAnimTrack->GetKeyCount() ; i+=m_pBones.size())
	{
		if (m_fTime < m_pCurrentAnimTrack->GetKey(i)->GetTime())
		{
			firstKeyIndex = i;
			m_lastKey = i;


			//Check if its the Last Key.
			if( i + BoneCount()  >= m_pCurrentAnimTrack->GetKeyCount()  )
			{
				nextKeyIndex = i;
			}
			else
			{
				nextKeyIndex  = i + BoneCount();
			}

			break;
		}
	}

	float time = 0.0f;
	for (unsigned int i = 0; i < BoneCount() ; i++)
	{
		Mat a = m_pCurrentAnimTrack->GetKey( firstKeyIndex + i )->GetMatrix();
		Mat b = m_pCurrentAnimTrack->GetKey( nextKeyIndex + i )->GetMatrix();

		float T1 = m_pCurrentAnimTrack->GetKey( firstKeyIndex + i )->GetTime();
		float T2 = m_pCurrentAnimTrack->GetKey( nextKeyIndex + i )->GetTime();

		//Last key
		if ( firstKeyIndex == nextKeyIndex || T1 == T2 )
		{
			m_BoneGlobals[i] = a;
			continue;
		}

		float s = (m_fTime - T1) / (T2 - T1);

		m_BoneGlobals[i] = a;

		GetBone(i)->SetWorldMatrix(a);

		//safeQueEvent(IEventDataPtr(shared_ptr<EvtData_SetActorTransform>(DEBUG_CLIENTBLOCK EvtData_SetActorTransform(GetBone(i)->GetID(),GetBone(i)->GetWorldMatrix()))));

		t->Start();		
		g_pApp->m_pGame->VGetGamePhysics()->VKinematicMove(GetBone(i)->GetWorldMatrix(),GetBone(i)->GetID());
		t->Stop();
		time += t->get();
	} 
}

void CD3D11Skeleton::AddAnimationsTrack( shared_ptr<AnimationsTrack> track )
{
	m_AnimTrackMap[track->GetName()] = track;
}

void CD3D11Skeleton::SetCurrentAnimationsTrack( string name )
{
	m_pCurrentAnimTrack = m_AnimTrackMap[name];

	for (unsigned int i = 0; i < BoneCount() ; i++)
		GetBone(i)->SetCurrentAnimationsTrack(name);
}

void CD3D11Skeleton::SetNextAnimationsTrack()
{
	m_pCurrentAnimTrack->GetName();

	map<string,shared_ptr<AnimationsTrack>>::iterator i = m_AnimTrackMap.find(m_pCurrentAnimTrack->GetName());

	if (i != m_AnimTrackMap.end())
	{
		i++;
		if (i != m_AnimTrackMap.end())
		{
			SetCurrentAnimationsTrack(i->first);
			return;
		}
	}

	SetCurrentAnimationsTrack(m_AnimTrackMap.begin()->first);
}

Mat CD3D11Skeleton::Lerp( D3DXMATRIX a, D3DXMATRIX b, float s )
{
	//Decompose a
	D3DXVECTOR3 t1, s1;
	D3DXQUATERNION q1;
	D3DXMatrixDecompose(&s1, &q1, &t1, &a);

	//Decompose b
	D3DXVECTOR3 t2, s2;
	D3DXQUATERNION q2;
	D3DXMatrixDecompose(&s2, &q2, &t2, &b);

	//Lerp each component
	D3DXVECTOR3 ti;
	D3DXVec3Lerp(&ti, &t1, &t2, s);

	D3DXVECTOR3 si;
	D3DXVec3Lerp(&si, &s1, &s2, s);

	D3DXQUATERNION qi;
	D3DXQuaternionSlerp(&qi, &q1, &q2, s);

	//Recompose each component
	D3DXMATRIX Tr, Sc, Rt;

	D3DXMatrixTranslation(&Tr, ti.x, ti.y, ti.z);
	D3DXMatrixScaling(&Sc, si.x, si.y, si.z);
	D3DXMatrixRotationQuaternion(&Rt, &qi);

	//Product
	D3DXMATRIX p = Sc * Rt * Tr;

	//Return
	return Mat((float*)p.m);
}

/************************************************************************/
/*Animations Tracks                                                     */
/************************************************************************/

AnimationsTrack::AnimationsTrack()
{

}

AnimationsTrack::~AnimationsTrack()
{

}

bool AnimationsTrack::Init()
{
	if (m_pAnimationsKey.empty())
		return false;

	m_fStartTime = m_pAnimationsKey.front()->GetTime();
	m_fEndTime   = m_pAnimationsKey.back()->GetTime();

	return true;
}

bool AnimationsTrack::Restore()
{
	if (m_pAnimationsKey.empty())
		return false;

	m_fStartTime = m_pAnimationsKey.front()->GetTime();
	m_fEndTime   = m_pAnimationsKey.back()->GetTime();

	return true;
}

void AnimationsTrack::AddAnimationsKey( shared_ptr<AnimationsKey> key )
{
	m_pAnimationsKey.push_back(key);
}

/************************************************************************/
/*Animations Keys                                                       */
/************************************************************************/

AnimationsKey::AnimationsKey( DWORD jointIndex, float timeStamp, Mat matrix ) :
	m_fTimeStamp(timeStamp),
	m_matKey(matrix),
	m_dBoneIndex(jointIndex)
{
}

AnimationsKey::~AnimationsKey( )
{

}

bool AnimationsKey::Init()
{
	return true;
}

bool AnimationsKey::Restore()
{

	return true;
}
