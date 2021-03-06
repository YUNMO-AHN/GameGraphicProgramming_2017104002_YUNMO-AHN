#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer
      Summary:  Constructor
      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView, m_vertexShader,
                  m_pixelShader, m_vertexLayout, m_vertexBuffer].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    Renderer::Renderer() :
        m_driverType(D3D_DRIVER_TYPE_NULL), m_featureLevel(D3D_FEATURE_LEVEL_11_0), 
        m_d3dDevice(),
        m_d3dDevice1(),
        m_immediateContext(),
        m_immediateContext1(),
        m_swapChain(),
        m_swapChain1(),
        m_renderTargetView(),
        m_depthStencil(),
        m_depthStencilView(),
        m_camera(XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f)),
        m_projection(XMMatrixIdentity()),
        m_renderables(std::unordered_map<std::wstring, std::shared_ptr<Renderable>>()),
        m_vertexShaders(std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>()),
        m_pixelShaders(std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>()),
        m_cbLights(),
        m_aPointLights()
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize
      Summary:  Creates Direct3D device and swap chain
      Args:     HWND hWnd
                  Handle to the window
      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                  m_d3dDevice1, m_immediateContext1, m_swapChain1,
                  m_swapChain, m_renderTargetView, m_vertexShader,
                  m_vertexLayout, m_pixelShader, m_vertexBuffer].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    HRESULT Renderer::Initialize(_In_ HWND hWnd) {

        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        UINT createDeviceFlags = 0;

#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] = {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };

        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };

        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
            m_driverType = driverTypes[driverTypeIndex];

            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG) {
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1],
                    numFeatureLevels - 1, D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel,
                    m_immediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr)) {
                break;
            }
        }

        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<IDXGIFactory1> DxgiFactory; {
            ComPtr<IDXGIDevice> DxgiDevice;
            hr = m_d3dDevice.As(&DxgiDevice);

            if (SUCCEEDED(hr)) {
                ComPtr<IDXGIAdapter> Adapter;

                if (SUCCEEDED(DxgiDevice->GetAdapter(Adapter.GetAddressOf()))) {
                    Adapter->GetParent(IID_PPV_ARGS(DxgiFactory.GetAddressOf()));
                }
            }
        }

        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<IDXGIFactory2> DxgiFactory2;
        hr = DxgiFactory.As(&DxgiFactory2);

        if (DxgiFactory2) {
            if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1))) {
                m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd = {};

            sd.Width = width;
            sd.Height = height;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            hr = DxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr,
                m_swapChain1.GetAddressOf());

            if (SUCCEEDED(hr)) {
                hr = m_swapChain1.As(&m_swapChain);
            }
        }

        else {
            DXGI_SWAP_CHAIN_DESC sd = {};

            sd.BufferCount = 1;
            sd.BufferDesc.Width = width;
            sd.BufferDesc.Height = height;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hWnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;

            hr = DxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        DxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

        if (FAILED(hr)) {
            return hr;
        }

        ComPtr<ID3D11Texture2D> p_BackBuffer;
        D3D11_TEXTURE2D_DESC bbDesc;

        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &p_BackBuffer);

        if (FAILED(hr)) {
            return hr;
        }

        hr = m_d3dDevice->CreateRenderTargetView(p_BackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());

        if (FAILED(hr)) {
            return hr;
        }

        p_BackBuffer->GetDesc(&bbDesc);

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

        D3D11_TEXTURE2D_DESC descDepth = {
            .Width = width,
            .Height = height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
            .SampleDesc{.Count = 1, .Quality = 0},
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_DEPTH_STENCIL,
            .CPUAccessFlags = 0,
            .MiscFlags = 0
        };

        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, m_depthStencil.GetAddressOf());

        if (FAILED(hr)) {
            return hr;
        }

        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {
            .Format = descDepth.Format,
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
            .Texture2D{.MipSlice = 0}
        };

        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, m_depthStencilView.GetAddressOf());

        if (FAILED(hr)) {
            return hr;
        }

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        D3D11_VIEWPORT vp = {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = (FLOAT)width,
            .Height = (FLOAT)height,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f
        };
        
        m_immediateContext->RSSetViewports(1, &vp);
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        float fovAngleY = XM_PIDIV2;
        float nearZ = 0.01f;
        float farZ = 100.0f;
        m_projection = XMMatrixPerspectiveFovLH(fovAngleY, (float)bbDesc.Width / (float)bbDesc.Height, nearZ, farZ);

        D3D11_BUFFER_DESC cBufferDesc = {
            .ByteWidth = sizeof(CBChangeOnResize),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = 0
        };

        CBChangeOnResize cb = {
            .Projection = XMMatrixTranspose(m_projection)
        };

        D3D11_SUBRESOURCE_DATA cData = {
            .pSysMem = &cb,
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0
        };

        hr = m_d3dDevice->CreateBuffer(&cBufferDesc, &cData, &m_cbChangeOnResize);
        if (FAILED(hr)) return hr;

        m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());

        D3D11_BUFFER_DESC cbLightsDesc = {
            .ByteWidth = sizeof(CBLights),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
            .StructureByteStride = 0
        };

        CBLights cbLights = {};

        D3D11_SUBRESOURCE_DATA cbLightsData = {
            .pSysMem = &cbLights,
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0
        };

        hr = m_d3dDevice->CreateBuffer(&cbLightsDesc, &cbLightsData, &m_cbLights);

        if (FAILED(hr)) {
            return hr;
        }

        m_immediateContext->VSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());
        m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());

        for (auto iVShader = m_vertexShaders.begin(); iVShader != m_vertexShaders.end(); iVShader++)
        {
            hr = iVShader->second->Initialize(m_d3dDevice.Get());
            if (FAILED(hr))
            {
                return hr;
            }
        }

        for (auto iPShader = m_pixelShaders.begin(); iPShader != m_pixelShaders.end(); iPShader++)
        {
            hr = iPShader->second->Initialize(m_d3dDevice.Get());
            if (FAILED(hr))
            {
                return hr;
            }
        }

        hr = m_camera.Initialize(m_d3dDevice.Get());

        if (FAILED(hr))
        {
            return hr;
        }

        D3D11_BUFFER_DESC bd = {
            .ByteWidth = sizeof(CBChangeOnResize),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0u
        };

        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbChangeOnResize.GetAddressOf());

        if (FAILED(hr)) {
            return hr;
        }

        for (auto iRenderable = m_renderables.begin(); iRenderable != m_renderables.end(); iRenderable++)
        {
            hr = iRenderable->second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
            if (FAILED(hr))
            {
                return hr;
            }
        }

        XMFLOAT4 camPos;
        XMStoreFloat4(&camPos, m_camera.GetEye());
        CBChangeOnCameraMovement cbCamera = {
            .View = XMMatrixTranspose(m_camera.GetView()),
            .CameraPosition = camPos
        };
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cbCamera, 0, 0);

        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / (FLOAT)height, 0.01f, 100.0f);
        CBChangeOnResize cbChangesOnResize;
        cbChangesOnResize.Projection = XMMatrixTranspose(m_projection);
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cbChangesOnResize, 0, 0);

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddRenderable
      Summary:  Add a renderable object and initialize the object
      Args:     PCWSTR pszRenderableName
                  Key of the renderable object
                const std::shared_ptr<Renderable>& renderable
                  Unique pointer to the renderable object
      Modifies: [m_renderables].
      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddRenderable(_In_ PCWSTR pszRenderableName, _In_ const std::shared_ptr<Renderable>& renderable) {

        std::unordered_map<std::wstring, std::shared_ptr<Renderable>>::iterator it = m_renderables.begin();

        for (it = m_renderables.begin(); it != m_renderables.end(); it++) {
            if (it->first == pszRenderableName) {
                return E_FAIL;
            }
        }

        m_renderables.insert(std::pair<std::wstring, std::shared_ptr<Renderable>>(pszRenderableName, renderable));

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPointLight
      Summary:  Add a point light
      Args:     size_t index
                  Index of the point light
                const std::shared_ptr<PointLight>& pointLight
                  Shared pointer to the point light object
      Modifies: [m_aPointLights].
      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddPointLight(_In_ size_t index, _In_ const std::shared_ptr<PointLight>& pPointLight) {
        if (index >= NUM_LIGHTS || !pPointLight)
        {
            return E_FAIL;
        }

        m_aPointLights[index] = pPointLight;
        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddVertexShader
      Summary:  Add the vertex shader into the renderer
      Args:     PCWSTR pszVertexShaderName
                  Key of the vertex shader
                const std::shared_ptr<VertexShader>&
                  Vertex shader to add
      Modifies: [m_vertexShaders].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddVertexShader(_In_ PCWSTR pszVertexShaderName, _In_ const std::shared_ptr<VertexShader>& vertexShader) {

        std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>::iterator it;

        for (it = m_vertexShaders.begin(); it != m_vertexShaders.end(); it++) {
            if (it->first == pszVertexShaderName) {
                return E_FAIL;
            }
        }

        m_vertexShaders.insert(std::pair<std::wstring, std::shared_ptr<VertexShader>>(pszVertexShaderName, vertexShader));

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPixelShader
      Summary:  Add the pixel shader into the renderer
      Args:     PCWSTR pszPixelShaderName
                  Key of the pixel shader
                const std::shared_ptr<PixelShader>&
                  Pixel shader to add
      Modifies: [m_pixelShaders].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddPixelShader(_In_ PCWSTR pszPixelShaderName, _In_ const std::shared_ptr<PixelShader>& pixelShader) {

        std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>::iterator it = m_pixelShaders.begin();

        for (it = m_pixelShaders.begin(); it != m_pixelShaders.end(); it++) {
            if (it->first == pszPixelShaderName) {
                return E_FAIL;
            }
        }

        m_pixelShaders.insert(std::pair<std::wstring, std::shared_ptr<PixelShader>>(pszPixelShaderName, pixelShader));
        
        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddScene
      Summary:  Add a scene
      Args:     PCWSTR pszSceneName
                  Key of a scene
                const std::filesystem::path& sceneFilePath
                  File path to initialize a scene
      Modifies: [m_scenes].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddScene(_In_ PCWSTR pszSceneName, const std::filesystem::path& sceneFilePath) {
        for (auto it = m_scenes.begin(); it != m_scenes.end(); it++) {
            if (it->first == pszSceneName) {
                return E_FAIL;
            }
        }

        std::shared_ptr<Scene> scene = std::make_shared<Scene>(sceneFilePath);
        m_scenes.insert(std::pair<std::wstring, std::shared_ptr<Scene>>(pszSceneName, scene));

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetMainScene
      Summary:  Set the main scene
      Args:     PCWSTR pszSceneName
                  Name of the scene to set as the main scene
      Modifies: [m_pszMainSceneName].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetMainScene(_In_ PCWSTR pszSceneName) {

        m_pszMainSceneName = pszSceneName;

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::HandleInput
      Summary:  Add the pixel shader into the renderer and initialize it
      Args:     const DirectionsInput& directions
                  Data structure containing keyboard input data
                const MouseRelativeMovement& mouseRelativeMovement
                  Data structure containing mouse relative input data
      Modifies: [m_camera].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::HandleInput(_In_ const DirectionsInput& directions, 
        _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime) {
        m_camera.HandleInput(directions, mouseRelativeMovement, deltaTime);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Update
      Summary:  Update the renderables each frame
      Args:     FLOAT deltaTime
                  Time difference of a frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Update(_In_ FLOAT deltaTime) {

        std::unordered_map<std::wstring, std::shared_ptr<Renderable>>::iterator it;

        for (it = m_renderables.begin(); it != m_renderables.end(); it++) {
            it->second->Update(deltaTime);
        }

        for (auto& light : m_aPointLights) {
            light->Update(deltaTime);
        }


        m_camera.Update(deltaTime);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render
      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/

    void Renderer::Render() {

        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        XMFLOAT4 camPos;
        XMStoreFloat4(&camPos, m_camera.GetEye());
        CBChangeOnCameraMovement cbCamera = {
            .View = XMMatrixTranspose(m_camera.GetView()),
            .CameraPosition = camPos
        };
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cbCamera, 0, 0);
        m_immediateContext->VSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
        m_immediateContext->PSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());

        CBLights cbLights = {};

        for (int i = 0; i < NUM_LIGHTS; i++)
        {
            if (!m_aPointLights[i]) continue;
            cbLights.LightPositions[i] = m_aPointLights[i]->GetPosition();
            cbLights.LightColors[i] = m_aPointLights[i]->GetColor();
        }

        m_immediateContext->UpdateSubresource( m_cbLights.Get(), 0u, nullptr, &cbLights, 0u, 0u);
        m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());

        for (auto it = m_renderables.begin(); it != m_renderables.end(); it++) {

            UINT uStride = sizeof(SimpleVertex);
            UINT uOffset = 0;
            m_immediateContext->IASetVertexBuffers(0u, 1u, it->second->GetVertexBuffer().GetAddressOf(),
                &uStride, &uOffset);
            m_immediateContext->IASetIndexBuffer(it->second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
            m_immediateContext->IASetInputLayout(it->second->GetVertexLayout().Get());

            CBChangesEveryFrame cb = {
                .World = XMMatrixTranspose(it->second->GetWorldMatrix()),
                .OutputColor = it->second->GetOutputColor()
            };

            m_immediateContext->UpdateSubresource(it->second->GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);

            m_immediateContext->VSSetShader(it->second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->VSSetConstantBuffers(2, 1, it->second->GetConstantBuffer().GetAddressOf());
            m_immediateContext->PSSetShader(it->second->GetPixelShader().Get(), nullptr, 0);
            m_immediateContext->PSSetConstantBuffers(2, 1, it->second->GetConstantBuffer().GetAddressOf());

            if(it->second->HasTexture()){
                for (UINT i = 0; i < it->second->GetNumMeshes(); ++i) {
                    UINT materialIndex = it->second->GetMesh(i).uMaterialIndex;
                    m_immediateContext->PSSetShaderResources(0, 1, 
                        it->second->GetMaterial(materialIndex).pDiffuse->GetTextureResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(0, 1, 
                        it->second->GetMaterial(materialIndex).pDiffuse->GetSamplerState().GetAddressOf());
                    m_immediateContext->DrawIndexed(it->second->GetMesh(i).uNumIndices,
                        it->second->GetMesh(i).uBaseIndex, it->second->GetMesh(i).uBaseVertex);
                }
            }

            else {
                m_immediateContext->DrawIndexed(it->second->GetNumIndices(), 0, 0);
            }
        }

        m_swapChain->Present(0, 0);
        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfRenderable
      Summary:  Sets the vertex shader for a renderable
      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszVertexShaderName
                  Key of the vertex shader
      Modifies: [m_renderables].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetVertexShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszVertexShaderName) {

        HRESULT hr = S_OK;

        std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>::iterator itVertex;

        for (auto it = m_renderables.begin(); it != m_renderables.end(); it++) {
            if (it->first == pszRenderableName) {
                for (itVertex = m_vertexShaders.begin(); itVertex != m_vertexShaders.end(); itVertex++) {
                    if (itVertex->first == pszVertexShaderName) {
                        it->second->SetVertexShader(itVertex->second);
                    }
                }
            }
        }

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfRenderable
      Summary:  Sets the pixel shader for a renderable
      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader
      Modifies: [m_renderables].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetPixelShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszPixelShaderName) {

        HRESULT hr = S_OK;

        std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>::iterator itPixel = m_pixelShaders.begin();

        for (auto it = m_renderables.begin(); it != m_renderables.end(); it++) {
            if (it->first == pszRenderableName) {
                for (itPixel = m_pixelShaders.begin(); itPixel != m_pixelShaders.end(); itPixel++) {
                    if (itPixel->first == pszPixelShaderName) {
                        it->second->SetPixelShader(itPixel->second);
                    }
                }
            }
        }

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfScene
      Summary:  Sets the vertex shader for the voxels in a scene
      Args:     PCWSTR pszSceneName
                  Key of the scene
                PCWSTR pszVertexShaderName
                  Key of the vertex shader
      Modifies: [m_scenes].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetVertexShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszVertexShaderName) {

        std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>::iterator itVertex;

        for (auto it = m_scenes.begin(); it != m_scenes.end(); it++) {
            if (it->first == pszSceneName) {
                for (itVertex = m_vertexShaders.begin(); itVertex != m_vertexShaders.end(); itVertex++) {
                    if (itVertex->first == pszVertexShaderName) {
                        for (std::vector<Voxel>::size_type i = 0; i < it->second->GetVoxels().size(); i++) {
                            it->second->GetVoxels()[i]->SetVertexShader(itVertex->second);
                        }
                    }
                }
            }
        }

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfScene
      Summary:  Sets the pixel shader for the voxels in a scene
      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader
      Modifies: [m_renderables].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetPixelShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszPixelShaderName) {

        std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>::iterator itPixel = m_pixelShaders.begin();

        for (auto it = m_scenes.begin(); it != m_scenes.end(); it++) {
            if (it->first == pszSceneName) {
                for (itPixel = m_pixelShaders.begin(); itPixel != m_pixelShaders.end(); itPixel++) {
                    if (itPixel->first == pszPixelShaderName) {
                        for (std::vector<Voxel>::size_type i = 0; i < it->second->GetVoxels().size(); i++) {
                            it->second->GetVoxels()[i]->SetPixelShader(itPixel->second);
                        }
                    }
                }
            }
        }

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetDriverType
      Summary:  Returns the Direct3D driver type
      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const {
        return m_driverType;
    }
}