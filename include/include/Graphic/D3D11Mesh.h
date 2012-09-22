#ifndef CD3D11Mesh_h__
#define CD3D11Mesh_h__

#include "Bloco.h"

class CD3D11Effect;
class Material;
class CD3D11Bone;
class CD3D11Skeleton;
class AnimationsTrack;

class CD3D11MaterialGroup
{
	public:
		CD3D11MaterialGroup( cgl::PD3D11Effect pEffect );
		~CD3D11MaterialGroup();

		void AddIndex(DWORD index);
		void AddIndexData(vector<DWORD> index);

		void SetTextureName( string filename );

		bool Init();
		bool Restore();
		void Render(cgl::PD3D11EffectPass pPass);
		
		CD3D11Material	m_Material;
		
	private:
		bool SetTexture();
		
		string m_sTextureName;

		cgl::PD3D11IndexBuffer			m_pIndexBuffer;
		cgl::PD3D11EffectVariable		m_pevShaderResourceView;
		cgl::PD3D11ShaderResourceView	m_pShaderResourceView;
		cgl::PD3D11Resource				m_pShaderResource_Texture;
		ID3D11SamplerState*			    m_pSamplerLinear;
		cgl::PD3D11Effect				m_pEffect;
};

struct VertexElement
{
	UINT size;
	char* pData;
};

class CD3D11Mesh : public cgl::ICGLInputProcessorDataProvider
{
	friend class CD3D11Model;

private:
	cgl::PD3D11Effect m_pEffect;
	cgl::PCGLInputDataProcessor m_pProcessor;
	cgl::PD3D11InputLayout m_pInputLayout;
	cgl::PD3D11InputBuffer m_pInputBuffer;
	cgl::PD3D11IndexBuffer m_pIndexBuffer;

	cgl::PD3D11EffectVariable  m_pevMatWorld;
	cgl::PD3D11EffectVariable  m_pevMatView;
	cgl::PD3D11EffectVariable  m_pevMatProj;

	cgl::PD3D11EffectVariable  m_pevCameraPosition;
	cgl::PD3D11EffectVariable  m_pevLightDirection;
	shared_ptr<CD3D11Skeleton> m_pSkeleton;

	vector<shared_ptr<CD3D11MaterialGroup>> m_MaterialGroups;

	bool m_bHasAnimation;

	// for testing only single pass
	cgl::PD3D11EffectPass m_pPass;

	virtual UINT GetVertexCount()=0;

public:
	CD3D11Mesh(cgl::PD3D11Effect pEffect);
	~CD3D11Mesh();

	void AddMaterialGroup( shared_ptr<CD3D11MaterialGroup> group );
	void AddBone( CD3D11Bone* bone );
	void AddAnimationsTrack(  shared_ptr<AnimationsTrack> track  );

	bool SetWorldMatrix( Mat worldMatrix );
	bool SetViewMatrix( Mat viewMatrix );
	bool SetProjectionMatrix( Mat projMatrix );

	bool SetLightDirection( Vec dir );
	bool SetCameraPosition( Vec pos );

	bool IsAnimated() { return m_bHasAnimation; }

	void Update( DWORD const elapsedMs );

	bool Init();
	bool Restore();
	void Render();
};


// class CD3D11Mesh
// {
// 	public: 
// 		CD3D11Mesh( string effect, string technique, bool animated );
// 		~CD3D11Mesh();
// 
// 
// 		virtual bool Create();
// 		bool Update( DWORD const elapsedMs );
// 
// 		void Render();
// 		void RenderVertices();
// 		void RenderSkeleton();
// 		void RenderModel();
// 		void SetRenderFunc(void (CD3D11Mesh::*pRenderFunc)());
// 
// 		void AddVertex( void* vertex );
// 		void AddMaterialGroup( MaterialGroup* group );
// 		void AddBone(CD3D11Bone* bone);
// 		void AddAnimationsTrack( shared_ptr<AnimationsTrack> track );
// 
// 		void SetVertexData( void* vertices, UINT count );
// 		void SetProjectionsMatrix( Mat projection );
// 		void SetViewMatrix( Mat view );
// 		void SetGlobalMatrix( Mat world );
// 		void SetWorldMatrix( Mat world );
// 		void SetEffect( string filename, string technique );
// 
// 		void Delete();
// 
// 		inline Mat GetGlobalMatrix() { return m_matGlobal; }
// 		inline WORD GetMaterialGroupCount() { return m_materialGroups.size();}
// 		inline vector<char*>* GetIndices(WORD index) { return m_materialGroups[index]->GetIndices();}
// 		inline bool IsAnimated() { return m_bAnimated; }
// 		inline vector<char*>* GetVertices()  { return m_pVertexBuffer->Data();}
// 
// 		// vertices
// 		cgl::PD3D11VertexBuffer  m_pVertexBuffer;
// 		size_t m_vertexType;
// 
// 		vector<MaterialGroup*> m_materialGroups;
// 
// 		bool m_bAnimated;
// 		bool m_bRenderSkeleton;
// 		bool m_bRenderModel;
// 
// // 		//Skeleton
// // 		CD3D11Skeleton* m_pSkeleton;
// 
// 		//InputLayout
// 		cgl::PD3D11InputLayout m_pInputLayout;
// 
// 		//Effect
// 		cgl::PD3D11Effect m_pEffect;
// 		ResHandlePtr m_effectHandle;
// 		string m_sEffect;
// 		string m_sTechnique;
// 
// 		cgl::PD3D11EffectVariable m_evProjMatrix;
// 		cgl::PD3D11EffectVariable m_evViewMatrix;
// 		cgl::PD3D11EffectVariable m_evWorldMatrix;
// 		cgl::PD3D11EffectVariable m_evTransformMatrixArray;
// 		cgl::PD3D11EffectVariable m_evInverseBindShape;
// 
// 		//Global Matrix
// 		Mat m_matGlobal;
// 
// 		// render func
// 		void (CD3D11Mesh::*m_pRenderFunc)();
// };

#endif
