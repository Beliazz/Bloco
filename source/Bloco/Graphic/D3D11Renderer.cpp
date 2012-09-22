#include "Bloco.h"

 BLOCO_API CD3D11Renderer::CD3D11Renderer()
{
	// device and context
	m_pDevice = NULL;

	// queries
	m_pDisjointQuery = NULL;
	m_pTimeQuery = NULL;

	// blend States
	m_pCurrentBlendState = NULL;
	m_pStdBlendState = NULL;

	// rasterizer States
	m_pCurrentRasterizerState = NULL;
	m_pSavedRasterizerState = NULL;
	m_pStdRasterizerState = NULL;
	m_pWireframeRasterizerState = NULL;
	
	// depth-stencil State
	m_pCurrentDepthStencilState = NULL;
	m_pStdDepthStencilState = NULL;

	m_pDepthStencilBuffer  = NULL;
	m_pDepthStencilView = NULL;

	// back buffer
	m_pBackBuffer = NULL;
	m_pBackBufferTarget = NULL;

	// post buffer
	m_pPostBuffer = NULL;
	m_pPostBufferTarget = NULL;
	m_pPostBufferView = NULL;

	// post processing
	m_postProcessing = false;

	// state saver
	m_pStateSaver = NULL;
}

void BLOCO_API CD3D11Renderer::Begin()
{
// 	// begin issuing commands
// 	m_pDevice->GetContext()->Begin(m_pDisjointQuery->get());
// 
// 	// get start ticks
// 	m_pDevice->GetContext()->End(m_pTimeQuery->get());
// 	m_pTimeQuery->GetData(&m_drawTicks);

	// clear surface and the depth stencil buffer
	m_pDevice->GetContext()->ClearRenderTargetView(m_pBackBufferTarget->get(), m_clearColor);
	m_pDevice->GetContext()->ClearDepthStencilView(m_pDepthStencilView->get(), D3D11_CLEAR_DEPTH  | D3D11_CLEAR_STENCIL, m_clearDepth, m_clearStencil);

	// set render targets and clear post buffer if needed
	if (m_postProcessing)
	{
		m_pDevice->GetContext()->ClearRenderTargetView(m_pPostBufferTarget->get(), m_clearColor);
		m_pDevice->GetContext()->OMSetRenderTargets(1, m_pPostBufferTarget->ptr(), m_pDepthStencilView->get());
	}
	else
	{
		m_pDevice->GetContext()->OMSetRenderTargets(1, m_pBackBufferTarget->ptr(), m_pDepthStencilView->get());
	}

	// set render states
	/*m_pImmediateContext->OMSetBlendState(m_pCurrentBlendState->Get(), m_blendColor, NULL);*/
	m_pDevice->GetContext()->OMSetDepthStencilState(m_pCurrentDepthStencilState->get(), m_stencilRef);
	m_pDevice->GetContext()->RSSetState(m_pCurrentRasterizerState->get());
}
void BLOCO_API CD3D11Renderer::End()
{	
// 	m_pDevice->GetContext()->End(m_pTimeQuery->get());
// 
// 	// get end ticks
// 	UINT64 temp = 0;
// 	m_pTimeQuery->GetData(&temp);
// 	if ( temp != 0)
// 	{
// 		m_drawTicks = temp - m_drawTicks;
// 	}
// 
// 	// end issuing commands
// 	m_pDevice->GetContext()->End(m_pDisjointQuery->get());
// 
// 	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
// 	ZeroMemory(&disjointData, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT));
// 	m_pDisjointQuery->GetData(&disjointData);
// 
// 	m_queryDataValid = !disjointData.Disjoint;
// 	if (!disjointData.Disjoint)
// 	{
// 		// query data is valid
// 		// calculate time in seconds
// 		m_drawTime = m_drawTicks / (float)disjointData.Frequency;
// 		m_sceneTime = m_sceneTicks / (float)disjointData.Frequency;
// 		m_postProcessingTime = m_postProcessingTicks / (float)disjointData.Frequency;
// 		m_overlayTime = m_overlayTicks / (float)disjointData.Frequency;
// 	}
}

void BLOCO_API CD3D11Renderer::BeginPass( UINT pass )
{
	switch (pass)
	{
		case PASS_SCENE:
			{			
				// get scene start ticks
// 				m_sceneTicks = 0;
// 				m_pDevice->GetContext()->End(m_pTimeQuery->get());
// 				m_pTimeQuery->GetData(&m_sceneTicks);

			} break;

		case PASS_POST_PROCESSING:
			{			
				m_postProcessingTicks = 0;
				if (m_postProcessing)
				{	
					// get post processing start ticks
// 					m_pDevice->GetContext()->End(m_pTimeQuery->get());
// 					m_pTimeQuery->GetData(&m_postProcessingTicks);
// 					m_pDevice->GetContext()->PSSetShaderResources(0, 1, m_pPostBufferView->ptr());
// 					m_pDevice->GetContext()->OMSetRenderTargets(1, m_pBackBufferTarget->ptr(), m_pDepthStencilView->get());
				}

			} break;

		case PASS_OVERLAY:
			{
// 				// get overlay start ticks
// 				m_overlayTicks = 0;
// 				m_pDevice->GetContext()->End(m_pTimeQuery->get());
// 				m_pTimeQuery->GetData(&m_overlayTicks);

			} break;
	}
}
void BLOCO_API CD3D11Renderer::EndPass( UINT pass )
{
	switch (pass)
	{
	case PASS_SCENE:
		{			
// 			// get scene ticks
// 			UINT64 temp = 0;
// 			m_pDevice->GetContext()->End(m_pTimeQuery->get());
// 			m_pTimeQuery->GetData(&temp);
// 			if (temp != 0)
// 			{
// 				m_sceneTicks = temp - m_sceneTicks;
// 			}

		} break;

	case PASS_POST_PROCESSING:
		{					
// 			if (m_postProcessing)
// 			{
// 				// get post processing ticks
// 				UINT64 temp = 0;
// 				m_pDevice->GetContext()->End(m_pTimeQuery->get());
// 				m_pTimeQuery->GetData(&temp);
// 				if (temp != 0)
// 				{
// 					m_postProcessingTicks = temp - m_postProcessingTicks;
// 				}
// 			}

		} break;

	case PASS_OVERLAY:
		{
// 			// get overlay ticks
// 			UINT64 temp = 0;
// 			m_pDevice->GetContext()->End(m_pTimeQuery->get());
// 			m_pTimeQuery->GetData(&temp);
// 			if (temp != 0)
// 			{
// 				m_overlayTicks = temp - m_overlayTicks;
// 			}

		} break;
	}
}

bool BLOCO_API CD3D11Renderer::VCreate()
{
	// get d3d11 core
	m_pDevice = g_pCGLMgr->GetDevice();

	// set up back buffer access
	m_pBackBuffer = cgl::CD3D11BackBuffer::Create();
	m_pBackBuffer->restore();
	m_pBackBufferTarget = cgl::CD3D11RenderTargetView::Create(m_pBackBuffer);

	//////////////////////////////////////////////////////////////////////////
	// create queries
	D3D11_QUERY_DESC queryDesc;
	ZeroMemory(&queryDesc, sizeof(D3D11_QUERY_DESC));
	
	// disjoint
	queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	m_pDisjointQuery = cgl::CD3D11Query::Create(queryDesc);

	// time stamp
	queryDesc.Query = D3D11_QUERY_TIMESTAMP;
	m_pTimeQuery = cgl::CD3D11Query::Create(queryDesc);

	//////////////////////////////////////////////////////////////////////////
	// create post buffer
	D3D11_TEXTURE2D_DESC backBufferDesc;
	((ID3D11Texture2D*)m_pBackBuffer->get())->GetDesc(&backBufferDesc);
	backBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	m_pPostBuffer = cgl::CD3D11Texture2DBlank::Create(backBufferDesc);
	m_pPostBufferView = cgl::CD3D11ShaderResourceView::Create(m_pPostBuffer);
	m_pPostBufferTarget = cgl::CD3D11RenderTargetView::Create(m_pPostBuffer);

	//////////////////////////////////////////////////////////////////////////
	// clear values
	m_clearColor[0] = 0.0f;
	m_clearColor[1] = 0.25f;
	m_clearColor[2] = 0.25f;
	m_clearColor[3] = 0.0f;
	m_clearDepth = 1.0f;
	m_clearStencil = 0;

	// blend state
	m_blendColor[0] = 1.0f;
	m_blendColor[1] = 1.0f;
	m_blendColor[2] = 1.0f;
	m_blendColor[3] = 1.0f;

	//////////////////////////////////////////////////////////////////////////
	// blend state
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

	//m_pStdBlendState = cgl::PD3D11BlendState(DEBUG_CLIENTBLOCK cgl::CD3D11BlendState(blendDesc, ));

	//////////////////////////////////////////////////////////////////////////
	// rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;

	m_pStdRasterizerState = cgl::CD3D11RasterizerState::Create(rasterizerDesc);

	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

	m_pWireframeRasterizerState = cgl::CD3D11RasterizerState::Create(rasterizerDesc);

	//////////////////////////////////////////////////////////////////////////
	// depth stencil state
	m_stencilRef = 0;
 
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xff;
	dsDesc.StencilWriteMask = 0xff;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	m_pStdDepthStencilState = cgl::CD3D11DepthStencilState::Create(dsDesc, 0);

	//////////////////////////////////////////////////////////////////////////
	// create depth stencil resources
	CD3D11_TEXTURE2D_DESC dsTexDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, g_pApp->GetScreenWidth(), g_pApp->GetScreenHeight(), 1, 0, D3D11_BIND_DEPTH_STENCIL);

	m_pDepthStencilBuffer = cgl::CD3D11Texture2DBlank::Create(dsTexDesc);
	m_pDepthStencilView = cgl::CD3D11DepthStencilView::Create(m_pDepthStencilBuffer);

	//////////////////////////////////////////////////////////////////////////
	// restore all objects
	CGL_RESTORE_ALL(g_pCGLMgr);
	CGL_RESTORE(m_pBackBufferTarget);
	CGL_RESTORE(m_pPostBufferTarget);
	CGL_RESTORE(m_pDepthStencilView);


	//////////////////////////////////////////////////////////////////////////
	// set render targets
	ID3D11RenderTargetView* views[] = { m_pBackBufferTarget->get(), m_pPostBufferTarget->get() };
	m_pDevice->GetContext()->OMSetRenderTargets(2, views, m_pDepthStencilView->get());

	//////////////////////////////////////////////////////////////////////////
	// set render states
	SetState(m_pStdDepthStencilState);
	SetState(m_pStdRasterizerState);

	return true;
}
bool BLOCO_API CD3D11Renderer::VOnRestore()
{
	return VCreate();
}
void BLOCO_API CD3D11Renderer::VOnLostDevice()
{

}

void BLOCO_API CD3D11Renderer::SetState( cgl::PD3D11RasterizerState pRasterizerState )
{		
	if (!pRasterizerState)
	{
		pRasterizerState = m_pStdRasterizerState;
	}

	if(m_pCurrentRasterizerState != pRasterizerState)
	{
		m_pCurrentRasterizerState = pRasterizerState;
	}
}
void BLOCO_API CD3D11Renderer::SetState( cgl::PD3D11DepthStencilState pDepthStencilState )
{
	if (!pDepthStencilState)
	{
		pDepthStencilState = m_pStdDepthStencilState;
	}

	if(m_pCurrentDepthStencilState != pDepthStencilState)
	{
		m_pCurrentDepthStencilState = pDepthStencilState;
	}
}
void BLOCO_API CD3D11Renderer::SetState( cgl::PD3D11BlendState pBlendState )
{
	if (!pBlendState)
	{
		pBlendState = m_pStdBlendState;
	}

	if(m_pCurrentBlendState != pBlendState)
	{
		m_pCurrentBlendState = pBlendState;
	}
}
void BLOCO_API CD3D11Renderer::SetWireframe( BOOL val )
{
// 	if (val)
// 	{
// 		if (!m_pSavedRasterizerState)
// 		{
// 			m_pSavedRasterizerState = m_pCurrentRasterizerState;
// 			SetState(m_pWireframeRasterizerState);
// 		}
// 	}
// 	else 
// 	{
// 		if (m_pSavedRasterizerState)
// 		{
// 			SetState(m_pSavedRasterizerState);
// 			m_pSavedRasterizerState = NULL;
// 		}
// 	}
}

bool BLOCO_API CD3D11Renderer::SaveState()
{
	if (FAILED(m_pStateSaver->SaveCurrentState(m_pDevice->get()->pContext)))
	{
		m_pStateSaver->ReleaseSavedState();
		return false;
	}

 	return true;
}
bool BLOCO_API CD3D11Renderer::LoadState()
{
	bool result = SUCCEEDED(m_pStateSaver->RestoreSavedState());
	m_pStateSaver->ReleaseSavedState();

	return result;
}

bool BLOCO_API CD3D11Renderer::GetRenderStats( float* out )
{
// 	if (m_queryDataValid)
// 	{
// 		out[0] = m_drawTime;
// 		out[1] = m_sceneTime;
// 		out[2] = m_postProcessingTime;
// 		out[3] = m_overlayTime;
// 		out[4] = m_drawTime - m_sceneTime - m_postProcessingTime - m_overlayTime;
// 
// 		return true;
// 	}
// 
 	return true;
}
//
////// State Saver Definition //////////////////////////////////////////////////////////
//
 BLOCO_API CD3D11StateSaver::CD3D11StateSaver() :
m_savedState(false),
	m_featureLevel(D3D_FEATURE_LEVEL_11_0),
	m_pContext(NULL),
	m_primitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED),
	m_pInputLayout(NULL),
	m_pBlendState(NULL),
	m_sampleMask(0xffffffff),
	m_pDepthStencilState(NULL),
	m_stencilRef(0),
	m_pRasterizerState(NULL),
	m_pPSSRV(NULL),
	m_pSamplerState(NULL),
	m_pVS(NULL),
	m_numVSClassInstances(0),
	m_pVSConstantBuffer(NULL),
	m_pGS(NULL),
	m_numGSClassInstances(0),
	m_pGSConstantBuffer(NULL),
	m_pGSSRV(NULL),
	m_pPS(NULL),
	m_numPSClassInstances(0),
	m_pHS(NULL),
	m_numHSClassInstances(0),
	m_pDS(NULL),
	m_numDSClassInstances(0),
	m_pVB(NULL),
	m_vertexStride(0),
	m_vertexOffset(0),
	m_pIndexBuffer(NULL),
	m_indexFormat(DXGI_FORMAT_UNKNOWN),
	m_indexOffset(0)
{
	for(int i=0; i < 4; ++i)
		m_blendFactor[i] = 0.0f;
	for(int i=0; i < 256; ++i) {
		m_pVSClassInstances[i] = NULL;
		m_pGSClassInstances[i] = NULL;
		m_pPSClassInstances[i] = NULL;
		m_pHSClassInstances[i] = NULL;
		m_pDSClassInstances[i] = NULL;
	}
}
 BLOCO_API CD3D11StateSaver::~CD3D11StateSaver() 
{
	ReleaseSavedState();
}
HRESULT BLOCO_API CD3D11StateSaver::SaveCurrentState(ID3D11DeviceContext *pContext)
{
	if(m_savedState)
		ReleaseSavedState();
	if(pContext == NULL)
		return E_INVALIDARG;

	ID3D11Device *pDevice;
	pContext->GetDevice(&pDevice);
	if(pDevice != NULL) {
		m_featureLevel = pDevice->GetFeatureLevel();
		pDevice->Release();
	}

	pContext->AddRef();
	m_pContext = pContext;

	m_pContext->IAGetPrimitiveTopology(&m_primitiveTopology);
	m_pContext->IAGetInputLayout(&m_pInputLayout);

	m_pContext->OMGetBlendState(&m_pBlendState, m_blendFactor, &m_sampleMask);
	m_pContext->OMGetDepthStencilState(&m_pDepthStencilState, &m_stencilRef);

	m_pContext->RSGetState(&m_pRasterizerState);

	m_numVSClassInstances = 256;
	m_pContext->VSGetShader(&m_pVS, m_pVSClassInstances, &m_numVSClassInstances);
	m_pContext->VSGetConstantBuffers(0, 1, &m_pVSConstantBuffer);

	m_numPSClassInstances = 256;
	m_pContext->PSGetShader(&m_pPS, m_pPSClassInstances, &m_numPSClassInstances);
	m_pContext->PSGetShaderResources(0, 1, &m_pPSSRV);
	pContext->PSGetSamplers(0, 1, &m_pSamplerState);

	if(m_featureLevel >= D3D_FEATURE_LEVEL_10_0) {
		m_numGSClassInstances = 256;
		m_pContext->GSGetShader(&m_pGS, m_pGSClassInstances, &m_numGSClassInstances);
		m_pContext->GSGetConstantBuffers(0, 1, &m_pGSConstantBuffer);

		m_pContext->GSGetShaderResources(0, 1, &m_pGSSRV);

		if(m_featureLevel >= D3D_FEATURE_LEVEL_11_0) {
			m_numHSClassInstances = 256;
			m_pContext->HSGetShader(&m_pHS, m_pHSClassInstances, &m_numHSClassInstances);

			m_numDSClassInstances = 256;
			m_pContext->DSGetShader(&m_pDS, m_pDSClassInstances, &m_numDSClassInstances);
		}
	}

	m_pContext->IAGetVertexBuffers(0, 1, &m_pVB, &m_vertexStride, &m_vertexOffset);

	m_pContext->IAGetIndexBuffer(&m_pIndexBuffer, &m_indexFormat, &m_indexOffset);

	m_savedState = true;

	return S_OK;
}
HRESULT BLOCO_API CD3D11StateSaver::RestoreSavedState()
{
	if(!m_savedState)
		return E_FAIL;

	m_pContext->IASetPrimitiveTopology(m_primitiveTopology);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pContext->OMSetBlendState(m_pBlendState, m_blendFactor, m_sampleMask);
	m_pContext->OMSetDepthStencilState(m_pDepthStencilState, m_stencilRef);

	m_pContext->RSSetState(m_pRasterizerState);

	m_pContext->VSSetShader(m_pVS, m_pVSClassInstances, m_numVSClassInstances);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pVSConstantBuffer);

	m_pContext->PSSetShader(m_pPS, m_pPSClassInstances, m_numPSClassInstances);
	m_pContext->PSSetShaderResources(0, 1, &m_pPSSRV);
	m_pContext->PSSetSamplers(0, 1, &m_pSamplerState);

	if(m_featureLevel >= D3D_FEATURE_LEVEL_10_0) {
		m_pContext->GSSetShader(m_pGS, m_pGSClassInstances, m_numGSClassInstances);
		m_pContext->GSSetConstantBuffers(0, 1, &m_pGSConstantBuffer);

		m_pContext->GSSetShaderResources(0, 1, &m_pGSSRV);

		if(m_featureLevel >= D3D_FEATURE_LEVEL_11_0) {
			m_pContext->HSSetShader(m_pHS, m_pHSClassInstances, m_numHSClassInstances);

			m_pContext->DSSetShader(m_pDS, m_pDSClassInstances, m_numDSClassInstances);
		}
	}

	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &m_vertexStride, &m_vertexOffset);

	m_pContext->IASetIndexBuffer(m_pIndexBuffer, m_indexFormat, m_indexOffset);

	return S_OK;
}
void BLOCO_API CD3D11StateSaver::ReleaseSavedState()
{
	m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pBlendState);
	for(int i=0; i < 4; ++i)
		m_blendFactor[i] = 0.0f;
	m_sampleMask = 0xffffffff;
	SAFE_RELEASE(m_pDepthStencilState);
	m_stencilRef = 0;
	SAFE_RELEASE(m_pRasterizerState);
	SAFE_RELEASE(m_pPSSRV);
	SAFE_RELEASE(m_pSamplerState);
	SAFE_RELEASE(m_pVS);
	for(UINT i=0; i < m_numVSClassInstances; ++i)
		SAFE_RELEASE(m_pVSClassInstances[i]);
	m_numVSClassInstances = 0;
	SAFE_RELEASE(m_pVSConstantBuffer);
	SAFE_RELEASE(m_pGS);
	for(UINT i=0; i < m_numGSClassInstances; ++i)
		SAFE_RELEASE(m_pGSClassInstances[i]);
	m_numGSClassInstances = 0;
	SAFE_RELEASE(m_pGSConstantBuffer);
	SAFE_RELEASE(m_pGSSRV);
	SAFE_RELEASE(m_pPS);
	for(UINT i=0; i < m_numPSClassInstances; ++i)
		SAFE_RELEASE(m_pPSClassInstances[i]);
	m_numPSClassInstances = 0;
	SAFE_RELEASE(m_pHS);
	for(UINT i=0; i < m_numHSClassInstances; ++i)
		SAFE_RELEASE(m_pHSClassInstances[i]);
	m_numHSClassInstances = 0;
	SAFE_RELEASE(m_pDS);
	for(UINT i=0; i < m_numDSClassInstances; ++i)
		SAFE_RELEASE(m_pDSClassInstances[i]);
	m_numDSClassInstances = 0;
	SAFE_RELEASE(m_pVB);
	m_vertexStride = 0;
	m_vertexOffset = 0;
	SAFE_RELEASE(m_pIndexBuffer);
	m_indexFormat = DXGI_FORMAT_UNKNOWN;
	m_indexOffset = 0;

	SAFE_RELEASE(m_pContext);
	m_featureLevel = D3D_FEATURE_LEVEL_11_0;

	m_savedState = false;
}
