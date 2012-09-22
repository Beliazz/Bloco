#include "Bloco.h"

#define RENDER_STATE_STD_RASTERIZER		(CD3D11RasterizerState*)	NULL
#define RENDER_STATE_STD_BLEND			(CD3D11BlendState*)			NULL
#define RENDER_STATE_STD_DEPTH_STENCIL	(CD3D11DepthStencilState*)	NULL

enum PASS
{
	PASS_SCENE,
	PASS_POST_PROCESSING,
	PASS_OVERLAY
};

class BLOCO_API CD3D11StateSaver;
class BLOCO_API CD3D11Renderer
{
private:
	// device and context
	cgl::PD3D11Device			m_pDevice;

	// blend States
	cgl::PD3D11BlendState	m_pCurrentBlendState;
	cgl::PD3D11BlendState	m_pStdBlendState;
	float					m_blendColor[4];

	// rasterizer States
	cgl::PD3D11RasterizerState	m_pCurrentRasterizerState;
	cgl::PD3D11RasterizerState	m_pSavedRasterizerState;
	cgl::PD3D11RasterizerState	m_pStdRasterizerState;
	cgl::PD3D11RasterizerState	m_pWireframeRasterizerState;

	// depth-stencil State
	cgl::PD3D11DepthStencilState m_pCurrentDepthStencilState;
	cgl::PD3D11DepthStencilState m_pStdDepthStencilState;
	UINT						 m_stencilRef;

	cgl::PD3D11Resource	m_pDepthStencilBuffer;
	cgl::PD3D11DepthStencilView	m_pDepthStencilView;

	// back buffer
	cgl::PD3D11Resource			m_pBackBuffer;
	cgl::PD3D11RenderTargetView		m_pBackBufferTarget;
	
	// post buffer
	cgl::PD3D11Resource		m_pPostBuffer;
	cgl::PD3D11RenderTargetView		m_pPostBufferTarget;
	cgl::PD3D11ShaderResourceView	m_pPostBufferView;

	// post processing
	bool						m_postProcessing;

	// clear
	float						m_clearColor[4];
	float						m_clearDepth;
	UINT8						m_clearStencil;

	// state saver
	CD3D11StateSaver*			m_pStateSaver;

	// PROFILING ======================================================
	// queries
	cgl::PD3D11Query	m_pDisjointQuery;
	cgl::PD3D11Query	m_pTimeQuery;
	bool				m_queryDataValid;

	// whole draw time
	UINT64					m_drawTicks;
	float					m_drawTime;

	// scene draw time 
	float					m_sceneTime;
	UINT64					m_sceneTicks;

	// post processing time
	float					m_postProcessingTime;
	UINT64					m_postProcessingTicks;

	// overlay draw time
	float					m_overlayTime;
	UINT64					m_overlayTicks;
	// =================================================================

public:
	CD3D11Renderer();
	~CD3D11Renderer() {VDelete();}

	void Begin();
	void End();

	void BeginPass(UINT pass);
	void EndPass(UINT pass);

	inline ID3D11DeviceContext* GetContext()	{return m_pDevice->get()->pContext;}
	bool						GetRenderStats( float* out);

	bool VCreate();
	void VDelete()		{VOnLostDevice();}
	bool VOnRestore();
	void VOnLostDevice();

	bool SaveState();
	bool LoadState();

	void SetState(cgl::PD3D11RasterizerState pRasterizerState);
	void SetState(cgl::PD3D11BlendState pBlendState);
	void SetState(cgl::PD3D11DepthStencilState pDepthStencilState);
	void SetBlendColor(Col blendColor)	{blendColor.GetArray(m_blendColor);}
	void SetClearColor(Col clearColor, float depth = 1.0f, UINT8 stencil = 0)	{clearColor.GetArray(m_clearColor); m_clearDepth = depth; m_clearStencil = stencil;}
	void SetStencilRef(UINT stencilRef)	{m_stencilRef = stencilRef;}
	void SetWireframe(BOOL val);

};

class BLOCO_API CD3D11StateSaver {
	// Public functions
public:
	CD3D11StateSaver();
	~CD3D11StateSaver();

	HRESULT SaveCurrentState(ID3D11DeviceContext *pContext);
	HRESULT RestoreSavedState();
	void ReleaseSavedState();

protected:
	CD3D11StateSaver(const CD3D11StateSaver&);
	CD3D11StateSaver& operator=(const CD3D11StateSaver&);

	bool						m_savedState;
	D3D_FEATURE_LEVEL			m_featureLevel;
	ID3D11DeviceContext*		m_pContext;
	D3D11_PRIMITIVE_TOPOLOGY	m_primitiveTopology;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11BlendState*			m_pBlendState;
	FLOAT						m_blendFactor[4];
	UINT						m_sampleMask;
	ID3D11DepthStencilState*	m_pDepthStencilState;
	UINT						m_stencilRef;
	ID3D11RasterizerState*		m_pRasterizerState;
	ID3D11ShaderResourceView*	m_pPSSRV;
	ID3D11SamplerState*			m_pSamplerState;
	ID3D11VertexShader*			m_pVS;
	ID3D11ClassInstance*		m_pVSClassInstances[256];
	UINT						m_numVSClassInstances;
	ID3D11Buffer*				m_pVSConstantBuffer;
	ID3D11GeometryShader*		m_pGS;
	ID3D11ClassInstance*		m_pGSClassInstances[256];
	UINT						m_numGSClassInstances;
	ID3D11Buffer*				m_pGSConstantBuffer;
	ID3D11ShaderResourceView*	m_pGSSRV;
	ID3D11PixelShader*			m_pPS;
	ID3D11ClassInstance*		m_pPSClassInstances[256];
	UINT						m_numPSClassInstances;
	ID3D11HullShader*			m_pHS;
	ID3D11ClassInstance*		m_pHSClassInstances[256];
	UINT						m_numHSClassInstances;
	ID3D11DomainShader*			m_pDS;
	ID3D11ClassInstance*		m_pDSClassInstances[256];
	UINT						m_numDSClassInstances;
	ID3D11Buffer*				m_pVB;
	UINT						m_vertexStride;
	UINT						m_vertexOffset;
	ID3D11Buffer*				m_pIndexBuffer;
	DXGI_FORMAT					m_indexFormat;
	UINT						m_indexOffset;
};



