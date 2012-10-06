#ifndef VoxelSceneNode_h__
#define VoxelSceneNode_h__

#include "Bloco.h"

class BLOCO_API VoxelSceneNode : public SceneNode
{
private:
	int m_width;
	int m_height;
	int m_depth;
	int m_chunkSize;

public:
	VoxelSceneNode( int width, int height, int depth, int chunkSize, string name, const optional<ActorId> actorId, Mat* actorTransform );
	~VoxelSceneNode();

 	HRESULT VRender(Scene *pScene);
 	HRESULT VOnUpdate(Scene *, DWORD const elapsedMs);
 	HRESULT VOnRestore(Scene *pScene);
};

#endif