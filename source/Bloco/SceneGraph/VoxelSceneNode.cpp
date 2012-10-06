 #include "Bloco.h"


VoxelSceneNode::VoxelSceneNode( int width, int height, int depth, int chunkSize, string name, const optional<ActorId> actorId, Mat* actorTransform ) :
	SceneNode(actorId, name, RenderPass_Actor,actorTransform),
	m_width(width),
	m_height(height),
	m_depth(depth),
	m_chunkSize(chunkSize)
{
}

VoxelSceneNode::~VoxelSceneNode()
{
}

HRESULT VoxelSceneNode::VRender( Scene *pScene )
{
	return S_OK;
}

HRESULT VoxelSceneNode::VOnUpdate( Scene *, DWORD const elapsedMs )
{
	return S_OK;
}

HRESULT VoxelSceneNode::VOnRestore( Scene *pScene )
{
	return S_OK;
}
