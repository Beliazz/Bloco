#include "Bloco.h"



struct VertexColor
{
	Vec pos;
	Vec color;
};

struct VertexColorTexture
{
	Vec pos;
	Vec color;
	Vec texCoord;
};

struct VertexColorTextureNormal
{
	Vec pos;
	Vec color;
	Vec texCoord;
	Vec normal;
};

struct StaticVertex
{
	Vec pos;
	Vec normal;
	Vec texCoord;
	Vec tangents;
	Vec biNormals;
};


struct VertexTexture
{
	Vec pos;
	Vec texCoord;
};
struct VertexTextureNormal
{
	Vec pos;
	Vec texCoord;
	Vec normal;
};

struct VertexNormalTexture
{
	Vec pos;
	Vec normal;
	Vec texCoord;
};



struct VertexNormalTextureWeightBoneIndices
{
	Vec		pos;
	Vec		normal;
	Vec		texCoord;
	float	weights[4];
	UINT	boneindices[4];
};

struct SkinnedVertex
{
	Vec  pos;
	Vec  normal;
	Vec  texCoord;
	Vec  tangents;
	Vec  biNormals;
	Vec  weights;
	UINT boneindices[4];
};