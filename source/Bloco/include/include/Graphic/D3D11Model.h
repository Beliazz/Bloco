#ifndef CD3D11Model_h__
#define CD3D11Model_h__

#include "Bloco.h"


class CD3D11Model : public CD3D11Mesh
{
	public:
		CD3D11Model( cgl::PD3D11Effect pEffect, ClearModelSDK::CMeshNode* mesh );
		~CD3D11Model();

		virtual bool Create( );

		virtual char* getElement( cgl::CGL_INPUT_ELEMENT_SIGNATURE& inputElement, UINT elementIndex );
		virtual bool isCompatible( std::vector<cgl::CGL_INPUT_ELEMENT_SIGNATURE> pRequiredElements );

		void	SetContentDir( string filepath ) { m_sContentDirectory = filepath; }
		string	GetContentDir() { return m_sContentDirectory; }

		btTriangleMesh* GetBulletTriangleMesh();

	private:
		ClearModelSDK::CMeshNode* m_pMeshNode;
		string	m_sContentDirectory;
		btTriangleMesh* m_pBulletTriangleMesh;

		inline UINT GetVertexCount() { return m_pMeshNode->GetVertexBuffer()->Size(); }
};

#endif
