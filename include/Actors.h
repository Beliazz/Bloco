#ifndef Actors_h__
#define Actors_h__

#include "bloco.h"

class BaseGameLogic;
class SceneNode;

#define VIEWID_NO_VIEW_ATTACHED	(0)

enum ActorType
{
	AT_Unknown,
	AT_StaticMesh,
	AT_SkinnedMesh,
	AT_SphereMesh,
	AT_CubeMesh,
	AT_GridMesh,
	AT_Map,
	AT_Model,
	AT_Bone,
	AT_ModelChild_Mesh,
	AT_ModelChild_Bone,
	AT_Camera,
	AT_Plane,
};

class BLOCO_API BaseActor : public IActor
{
	friend class BaseGameLogic;
protected:
	ActorId		m_id;

	Mat			m_Mat;
	int			m_Type;
	bool		m_IsSelected;
	shared_ptr<ActorParams> m_Params;

	virtual void VSetID(ActorId id)		 { m_id = id; } 
	virtual void VSetMat( Mat newMat )	 { m_Mat = newMat; }
	virtual void VTransform( Mat value ) { m_Mat *= value; }

public:
	BaseActor(Mat  mat, int type, shared_ptr<ActorParams> params)
	{ 
		m_Mat = mat;
		m_Type	 = type; 
		m_Params = params; 
	}
	~BaseActor();
	shared_ptr<ActorParams> VGetParams()			  	 { return m_Params; }
	virtual Mat*		VGetMat()						 { return &m_Mat; }		// virtual dumb here?
	virtual const int	VGetType()						 { return m_Type; }		// virtual dumb here?
	virtual ActorId		VGetID()						 { return m_id; }			// virtual dumb here?
	virtual bool		VIsSelected()					 { return m_IsSelected; }
	virtual void		VSetSelected( bool value )		 { m_IsSelected = value; }
	virtual bool		VIsPhysical()					 { return true; }
	virtual bool		VIsGeometrical()				 { return true; }
	virtual void		VOnUpdate(int deltaMilliseconds) {}
	virtual void		VRotateY(float angleRadians)	 {}
};

class BLOCO_API ActorParams
{
public:
	int					m_Size;
	optional<ActorId>	m_Id;
	Vec					m_Pos;
	ActorType			m_Type;
	Col					m_Col;
	Vec					m_AABB;
	static const int	sk_MaxFuncName = 256;
	bool				m_IsModified;
	Mat					m_Mat;
	bool				m_bDynamic;
	bool				m_bPhysicActor;
	bool				m_bHasScript;
	char				m_sScriptFilename[ sk_MaxFuncName ];
	char				m_sName[ sk_MaxFuncName ];
	shared_ptr<IActor>  m_pActor;

	char m_OnCreateLuaFunctionName[ sk_MaxFuncName ];	//Lua function name to call for this actor, upon creation.
	char m_OnDestroyLuaFunctionName[ sk_MaxFuncName ];	//As above, but on destruction.

	ActorParams();

	virtual ~ActorParams();

	virtual bool VInit(std::istrstream &in);
	virtual void VSerialize(std::ostrstream &out) const;

	typedef std::deque< std::string > TErrorMessageList;
	virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );

	static ActorParams *CreateFromStream(std::istrstream &in);
	static ActorParams *CreateFromLuaObj( LuaObject srcData );

	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic) { shared_ptr<IActor> p; return p; }
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene) { shared_ptr<SceneNode> p; return p; }
};

class BLOCO_API ModelObjectParams : public ActorParams
{
public:
	char m_FXFileName[_MAX_PATH];
	char m_FXTechniqueName[_MAX_PATH];
	char m_ModellFilename[_MAX_PATH];

	ModelObjectParams();
	ModelObjectParams( const char* modelFilename, const char* fxFilename, const char* fxTechinqueName );

	virtual bool VInit(std::istrstream &in);
	virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};

class BLOCO_API BoneObjectParams : public ActorParams
{
public:
	BoneObjectParams(string name, string parentName, Mat global, Mat bindPose);

	virtual shared_ptr<IActor>	  VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);

	string m_parentName;

	Mat m_globalMatrix;
	Mat m_bindPoseMatrix;
};

class BLOCO_API ModelChildObjectParams : public ModelObjectParams
{
public:
	ModelChildObjectParams( ActorType type );
	ModelChildObjectParams( int actorID );
	ModelChildObjectParams( const char* modelFilename, const char* fxFilename, const char* fxTechinqueName, int actorID );

	virtual shared_ptr<IActor>	  VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
	
	ClearModelSDK::ModelElement*			m_pModelElement;
	SceneNode*								m_pRootNode;
};

class BLOCO_API PhysicObjectParams : public ActorParams
{
public:
	PhysicObjectParams();
	PhysicObjectParams( int actorID );

	virtual bool VInit(std::istrstream &in);
	virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};

class CameraObjectParams : public ActorParams
{
public:
	Mat m_Mat;

	CameraObjectParams();
	CameraObjectParams( int actorID );

	virtual bool VInit(std::istrstream &in);
	virtual bool VInit( LuaObject srcData, TErrorMessageList & errorMessages );
	virtual void VSerialize(std::ostrstream &out) const;
	virtual shared_ptr<IActor> VCreate(BaseGameLogic *logic);
	virtual shared_ptr<SceneNode> VCreateSceneNode(shared_ptr<Scene> pScene);
};

#endif // Actors_h__
