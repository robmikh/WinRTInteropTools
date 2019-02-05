#include "pch.h"
#include "Direct3D11Device.h"
#include "Direct3D11Multithread.h"
#include "Direct3D11DeviceContext.h"
#include "Direct3D11Texture2D.h"

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

    WinRTInteropTools::Direct3D11Texture2D Direct3D11Device::CreateTexture2D(
        WinRTInteropTools::Direct3D11Texture2DDescription const& description)
    {
        CheckClosed();

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = description.Base.Width;
        desc.Height = description.Base.Height;
        desc.MipLevels = description.MipLevels;
        desc.ArraySize = description.ArraySize;
        desc.Format = static_cast<DXGI_FORMAT>(description.Base.Format);
        desc.SampleDesc.Count = description.Base.MultisampleDescription.Count;
        desc.SampleDesc.Quality = description.Base.MultisampleDescription.Quality;
        desc.Usage = static_cast<D3D11_USAGE>(description.Usage);
        desc.BindFlags = static_cast<D3D11_BIND_FLAG>(description.BindFlags);
        desc.CPUAccessFlags = static_cast<D3D11_CPU_ACCESS_FLAG>(description.CpuAccessFlags);
        desc.MiscFlags = static_cast<D3D11_RESOURCE_MISC_FLAG>(description.MiscFlags);

        com_ptr<ID3D11Texture2D> texture;
        check_hresult(m_d3dDevice->CreateTexture2D(&desc, nullptr, texture.put()));
        auto surface = make<Direct3D11Texture2D>(texture, description);

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

    WinRTInteropTools::Direct3D11Device Direct3D11Device::CreateFromDirect3D11Device(
        IDirect3DDevice const& device)
    {
        auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(device);
        auto result = make<Direct3D11Device>(d3dDevice);
        return result;
    }

    HRESULT __stdcall Direct3D11Device::GetInterface(GUID const & id, void ** object)
    {
        CheckClosed();

        return m_d3dDevice->QueryInterface(id, object);
    }
}
