#ifndef D3D11ModelGroups_h__
#define D3D11ModelGroups_h__

#include "Bloco.h"

struct sMaterialGroup
{
	string							Material;
	cgl::PD3D11IndexBuffer			pIndexBuffer;	
	cgl::PD3D11ShaderResourceView	pevShaderResourceView; 
};

class CD3D11Joint
{
public:
	CD3D11Joint(string name,Mat BindMatrix, Mat invBindMatrix, int ParentIndex) : m_sName(name), m_bindMatrix(BindMatrix), m_inv_BindMatrix(invBindMatrix), m_iParentIndex(ParentIndex) {}

	int				m_iParentIndex;
	string			m_sName;
	string			m_ParentName;
	Mat				m_bindMatrix;
	Mat				m_inv_BindMatrix;
};

//Basic Animation Key holder
class JointAnimationKey
{
public:	
	//Time this key is set
	float Time;

	//Matrix for this key
	Mat Matrix;

	//Bone this key affects
	int Bone;

};

#endif
