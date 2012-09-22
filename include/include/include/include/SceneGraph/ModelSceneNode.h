#ifndef ModelSceneNode_h__
#define ModelSceneNode_h__

#include "Bloco.h"

class ModelSceneNode;
class BLOCO_API MeshSceneNode : public SceneNode
{
private:
	SceneNode*		m_pRootNode;
	CD3D11Model*    m_pModel;
	ClearModelSDK::CMeshNode* m_pMeshNode;


public:
	MeshSceneNode( ClearModelSDK::ModelElement* pNode, const optional<ActorId> actorId, SceneNode* rootNode, Mat* actorTransform );
	~MeshSceneNode();

 	HRESULT VRender(Scene *pScene);
 	HRESULT VOnUpdate(Scene *, DWORD const elapsedMs);
 	HRESULT VOnRestore(Scene *pScene);

	Mat m_matLocal;
	Mat m_matGlobal;

// 
// 	virtual bool	VRemoveChild(ActorId id);
// 	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);
};

class BLOCO_API EmptyMeshSceneNode : public SceneNode
{
private:
	ModelSceneNode* m_pRootNode;
	ClearModelSDK::ModelElement* m_pModelElement;

public:
	EmptyMeshSceneNode(  shared_ptr<ClearModelSDK::ModelElement> pNode, const optional<ActorId> actorId, ModelSceneNode* rootNode );
	~EmptyMeshSceneNode();

	 //	HRESULT VRender(Scene *pScene);
	// 	HRESULT VOnUpdate(Scene *, DWORD const elapsedMs);
	    HRESULT VOnRestore(Scene *pScene);
	// 
	// 	virtual bool	VRemoveChild(ActorId id);
	// 	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);
};

class BLOCO_API ModelSceneNode : public SceneNode
{
private:
	ClearModelSDK::CModel*	m_pModel;
	vector<MeshSceneNode*>	m_vecMeshSceneNodes;
	string					m_sFilename;


public:
	ModelSceneNode(const optional<ActorId> actorId, string name, string fileName);
	~ModelSceneNode();

	HRESULT VRender(Scene *pScene);
	HRESULT VOnUpdate(Scene *, DWORD const elapsedMs);
	HRESULT VOnRestore(Scene *pScene);
	string  GetFilename() { return m_sFilename; }
	virtual bool	VRemoveChild(ActorId id);
	virtual HRESULT VPick(Scene *pScene, RayCast *pRayCast);

	void AddCollisionData( btAlignedObjectArray<btVector3>* data);
	void AddMesh( MeshSceneNode* node );

	virtual void VSetTransform(Mat toWorld = MatIdentity() , Mat fromWorld = MatIdentity());

	btTriangleMesh*			m_pTriangleMesh;
	btCollisionShape*   	m_pCollisionShape;

	bool m_IsModified;
};

#endif