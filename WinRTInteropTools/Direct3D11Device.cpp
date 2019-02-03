#include "pch.h"
#include "Direct3D11Device.h"
#include "Direct3D11Multithread.h"
#include "Direct3D11DeviceContext.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Graphics;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;

namespace winrt::WinRTInteropTools::implementation
{
    Direct3D11Device::Direct3D11Device()
    {
        m_d3dDevice = CreateD3DDevice();
    }

    Direct3D11Device::Direct3D11Device(
        com_ptr<ID3D11Device> d3dDevice)
    {
        m_d3dDevice = d3dDevice;
    }

    WinRTInteropTools::Direct3D11Multithread Direct3D11Device::Multithread()
    {
        CheckClosed();

        auto multithread = m_d3dDevice.as<ID3D11Multithread>();
        auto result = make<Direct3D11Multithread>(multithread);
        return result;
    }

    WinRTInteropTools::Direct3D11DeviceContext Direct3D11Device::ImmediateContext()
    {
        CheckClosed();

        com_ptr<ID3D11DeviceContext> d3dContext;
        m_d3dDevice->GetImmediateContext(d3dContext.put());
        auto deviceContext = make<Direct3D11DeviceContext>(d3dContext);
        return deviceContext;
    }

    IDirect3DSurface Direct3D11Device::CreateTexture2D(
        Direct3DSurfaceDescription const& description)
    {
        CheckClosed();

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = description.Width;
        desc.Height = description.Height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = static_cast<DXGI_FORMAT>(description.Format);
        desc.SampleDesc.Count = description.MultisampleDescription.Count;
        desc.SampleDesc.Quality = description.MultisampleDescription.Quality;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        com_ptr<ID3D11Texture2D> texture;
        check_hresult(m_d3dDevice->CreateTexture2D(&desc, nullptr, texture.put()));
        
        auto dxgiSurface = texture.as<IDXGISurface>();
        auto surface = CreateDirect3DSurface(dxgiSurface.get());

        return surface;
    }

    void Direct3D11Device::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_d3dDevice = nullptr;
        }
    }

    void Direct3D11Device::Trim()
    {
        CheckClosed();

        auto dxgiDevice = m_d3dDevice.as<IDXGIDevice3>();
        dxgiDevice->Trim();
    }

    HRESULT __stdcall Direct3D11Device::GetInterface(GUID const & id, void ** object)
    {
        CheckClosed();

        return m_d3dDevice->QueryInterface(id, object);
    }
}
