#ifndef CD3D11Model_h__
#define CD3D11Model_h__

#include "Bloco.h"


class CD3D11Model : public CD3D11Mesh
{
	public:
		CD3D11Model( cgl::PD3D11Effect pEffect, ClearModelSDK::CMeshNode* mesh );
		~CD3D11Model();

		virtual bool Create( );

		virtual bool getElement(char* pDest, UINT elementIndex, cgl::CGL_INPUT_ELEMENT_SIGNATURE* inputElementSignature, UINT inputElementCount  );
		virtual bool isCompatible( std::vector<cgl::CGL_INPUT_ELEMENT_SIGNATURE>& pRequiredElements );

		void	SetContentDir( string filepath ) { m_sContentDirectory = filepath; }
		string	GetContentDir() { return m_sContentDirectory; }

		btCollisionShape* GetCollisionShape();

	private:
		ClearModelSDK::CMeshNode* m_pMeshNode;
		string	m_sContentDirectory;
		btConvexShape* m_pConvexShape;

		inline UINT GetVertexCount() { return m_pMeshNode->GetVertexBuffer()->Size(); }
};

#endif
