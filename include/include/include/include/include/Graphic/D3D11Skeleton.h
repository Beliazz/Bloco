#ifndef CD3D11Skeleton_h__
#define CD3D11Skeleton_h__

#include "Bloco.h"

class CD3D11Bone;
class AnimationsTrack;
class AnimationsKey;

class CD3D11Skeleton
{
public:
	CD3D11Skeleton( cgl::PD3D11Effect pEffect );
	~CD3D11Skeleton();
	bool Init();
	bool Restore();

	void AddBone( CD3D11Bone* bone )	{ m_pBones.push_back(bone); }
	DWORD BoneCount()					{ return m_pBones.size();}
	CD3D11Bone* GetBone( DWORD index )	{ return m_pBones[index]; }
	CD3D11Bone* FindBone( string name );

	void  AddAnimationsTrack( shared_ptr<AnimationsTrack> track );
	shared_ptr<AnimationsTrack> GetAnimationsTrack( string name )  { return m_AnimTrackMap[name]; }
	shared_ptr<AnimationsTrack> GetCurrentAnimationsTrack()		{ return m_pCurrentAnimTrack; }
	void SetNextAnimationsTrack();

	DWORD AnimationsTrackCount() { return m_AnimTrackMap.size(); }

	void SetCurrentAnimationsTrack( string name );

	Mat Lerp( D3DXMATRIX a, D3DXMATRIX b, float s );

	bool SetupBoneHierarchy();
	bool VCreate();
	bool VRender();
	bool Update( DWORD const elapsedMs );

	void BuildAnimation();

	void LinearAnimate();

	void SetProjectionsMatrix( Mat projection );
	void SetViewMatrix( Mat view );
	void SetGlobalMatrix( Mat world );
	void SetWorldMatrix( Mat world );

	vector<Mat> GetTransformArray() { return m_BoneGlobals;}
	Mat			GetInverseBindShape() { return m_matInverseBindShape; }

private:
	cgl::PD3D11Effect			m_pEffect;
	cgl::PD3D11EffectVariable	m_pevMatrixPalette;


	vector<CD3D11Bone*> m_pBones;
	vector<Mat> m_BoneGlobals;
	map<string,shared_ptr<AnimationsTrack>> m_AnimTrackMap;
	shared_ptr<AnimationsTrack> m_pCurrentAnimTrack;
	Mat m_matInverseBindShape;
	float m_CurrentTime;
	float m_fTime;
	DWORD m_lastKey;
};

class AnimationsTrack
{
public:
	AnimationsTrack();
	~AnimationsTrack();

	bool Init();
	bool Restore();

	void   AddAnimationsKey( shared_ptr<AnimationsKey> key ); 
	void   SetName( string name )		{ strcpy(m_sName,name.c_str()); }
	void   SetStartTime( float start )	{ m_fStartTime = start; }
	void   SetEndTime( float end )		{ m_fEndTime = end; }

	char*  GetName()			  { return m_sName; }
	float  GetStartTime()		  { return m_fStartTime; }
	float  GetEndTime()			  { return m_fEndTime; }
	DWORD  GetKeyCount()		  { return m_pAnimationsKey.size(); }	
	shared_ptr<AnimationsKey> GetKey( DWORD index ) { return m_pAnimationsKey[index]; }

	vector<shared_ptr<AnimationsKey>> m_pAnimationsKey;
private:

	char   m_sName[256];
	float  m_fStartTime;
	float  m_fEndTime;
};

class AnimationsKey
{
public:
	AnimationsKey( DWORD jointIndex, float timeStamp, Mat matrix );
	~AnimationsKey( );
	bool Init();
	bool Restore();

	float   GetTime()		{ return m_fTimeStamp; }
	DWORD   GetBoneIndex()	{ return m_dBoneIndex; }
	char*   GetName()		{ return m_sJointName; }
	Mat		GetMatrix()		{ return m_matKey; }

private:
	char   m_sJointName[256];
	DWORD  m_dBoneIndex;
	float  m_fTimeStamp;
	Mat    m_matKey;
};

class CD3D11Bone
{
public:
	CD3D11Bone( string name, string parentName, Mat bindPoseMatrix, Mat globalMatrix );
	~CD3D11Bone();

	bool Init();
	bool Restore();
	bool Render();

	void		AddAnimationsTrack( shared_ptr<AnimationsTrack> track );
	void		SetCurrentAnimationsTrack( string name );
	string		GetName()	{return m_sName;}
	CD3D11Bone* GetParent() { return m_pParent; }
	string		GetParentName() { return m_sParent; }
	Mat			GetGlobal() { return m_matGlobal; }
	Mat			GetBindShape() { return m_matBindPose; }
	void		SetParent( CD3D11Bone* parent ) { m_pParent = parent; }
	DWORD		AnimationsTrackCount() { return m_AnimTrackMap.size(); }
	bool		Update( DWORD keyindex, float interpol );
	void		SetWorldMatrix( Mat world );

private:
	string				m_sName;
	string				m_sParent;
	CD3D11Bone*			m_pParent;

	Mat m_matBindPose;
	Mat m_matGlobal;

	map<string,shared_ptr<AnimationsTrack>> m_AnimTrackMap;
	shared_ptr<AnimationsTrack>			 m_pCurrentAnimTrack;
};

#endif
