#include "Renderer/InstancedRenderable.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::InstancedRenderable
      Summary:  Constructor
      Args:     const XMFLOAT4& outputColor
                  Default color of the renderable
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    InstancedRenderable::InstancedRenderable(_In_ const XMFLOAT4& outputColor) :
        Renderable(outputColor),
        m_aInstanceData(),
        m_instanceBuffer(),
        m_padding()
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::InstancedRenderable
      Summary:  Constructor
      Args:     std::vector<InstanceData>&& aInstanceData
                  An instance data
                const XMFLOAT4& outputColor
                  Default color of the renderable
      Modifies: [m_instanceBuffer, m_aInstanceData].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    InstancedRenderable::InstancedRenderable(_In_ std::vector<InstanceData>&& aInstanceData, _In_ const XMFLOAT4& outputColor) :
        Renderable(outputColor),
        m_aInstanceData(aInstanceData),
        m_instanceBuffer(),
        m_padding()
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::SetInstanceData
      Summary:  Sets the instance data
      Args:     std::vector<InstanceData>&& aInstanceData
                  Instance data
      Modifies: [m_aInstanceData].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void InstancedRenderable::SetInstanceData(_In_ std::vector<InstanceData>&& aInstanceData) {
        m_aInstanceData = std::move(aInstanceData);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::GetInstanceBuffer
      Summary:  Returns the instance buffer
      Returns:  ComPtr<ID3D11Buffer>&
                  Instance buffer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11Buffer>& InstancedRenderable::GetInstanceBuffer() {
        return m_instanceBuffer;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::GetNumInstances
      Summary:  Returns the number of instances
      Returns:  UINT
                  Number of instances
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    UINT InstancedRenderable::GetNumInstances() const {
        return static_cast<UINT>(m_aInstanceData.size());
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   InstancedRenderable::initializeInstance
      Summary:  Creates an instance buffer
      Args:     ID3D11Device* pDevice
                  Pointer to a Direct3D 11 device
      Modifies: [m_instanceBuffer].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT InstancedRenderable::initializeInstance(_In_ ID3D11Device* pDevice) {

        HRESULT hr = S_OK;

        XMFLOAT3 aInstancedData[] = {
            XMFLOAT3(1.0f, 0.0f, 0.0f),
            XMFLOAT3(0.0f, 1.0f, 0.0f),
            XMFLOAT3(0.0f, 0.0f, 1.0f)
        };

        D3D11_BUFFER_DESC bd = {
            .ByteWidth = sizeof(XMFLOAT3) * ARRAYSIZE(aInstancedData),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = 0
        };

        D3D11_SUBRESOURCE_DATA initData = {
            .pSysMem = aInstancedData
        };

        hr = pDevice->CreateBuffer(&bd, &initData, m_instanceBuffer.GetAddressOf());

        if (FAILED(hr)) {
            return E_FAIL;
        }

        return hr;
    }
}