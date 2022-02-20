#include "pch.h"
#include "Direct3D11Texture2D.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Graphics;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;

namespace util
{
    using namespace robmikh::common::uwp;
}

namespace winrt::WinRTInteropTools::implementation
{
    auto PrepareStagingTexture(com_ptr<ID3D11Texture2D> const& texture)
    {
        // If our texture is already set up for staging, then use it.
        // Otherwise, create a staging texture.
        D3D11_TEXTURE2D_DESC desc = {};
        texture->GetDesc(&desc);
        if (desc.Usage == D3D11_USAGE_STAGING && desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ)
        {
            return texture;
        }

        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;

        // Get the device and context
        com_ptr<ID3D11Device> d3dDevice;
        texture->GetDevice(d3dDevice.put());
        com_ptr<ID3D11DeviceContext> d3dContext;
        d3dDevice->GetImmediateContext(d3dContext.put());

        // Create our staging texture and copy to it
        com_ptr<ID3D11Texture2D> stagingTexture;
        check_hresult(d3dDevice->CreateTexture2D(&desc, nullptr, stagingTexture.put()));
        d3dContext->CopyResource(stagingTexture.get(), texture.get());

        return stagingTexture;
    }

    Direct3D11Texture2D::Direct3D11Texture2D(
        com_ptr<ID3D11Texture2D> texture,
        WinRTInteropTools::Direct3D11Texture2DDescription const& description)
    {
        m_texture = texture;
        m_description = description;
    }

    WinRTInteropTools::Direct3D11Texture2DDescription Direct3D11Texture2D::Description2D()
    {
        CheckClosed();
        return m_description;
    }

    com_array<uint8_t> Direct3D11Texture2D::GetBytes()
    {
        CheckClosed();
        auto bytes = util::CopyBytesFromTexture(m_texture);
        return com_array<uint8_t>(bytes);
    }

    void Direct3D11Texture2D::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_texture = nullptr;
        }
    }

    Direct3DSurfaceDescription Direct3D11Texture2D::Description()
    {
        CheckClosed();
        return m_description.Base;
    }

    WinRTInteropTools::Direct3D11Texture2D Direct3D11Texture2D::CreateFromDirect3DSurface(
        IDirect3DSurface const& surface)
    {
        auto texture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(surface);

        D3D11_TEXTURE2D_DESC desc = {};
        texture->GetDesc(&desc);

        Direct3D11Texture2DDescription description = {};
        description.Base.Width = desc.Width;
        description.Base.Height = desc.Height;
        description.Base.Format = static_cast<DirectXPixelFormat>(desc.Format);
        description.Base.MultisampleDescription.Count = desc.SampleDesc.Count;
        description.Base.MultisampleDescription.Quality = desc.SampleDesc.Quality;
        description.MipLevels = desc.MipLevels;
        description.ArraySize = desc.ArraySize;
        description.Usage = static_cast<Direct3DUsage>(desc.Usage);
        description.BindFlags = static_cast<Direct3DBindings>(desc.BindFlags);
        description.CpuAccessFlags = static_cast<Direct3D11CpuAccessFlag>(desc.CPUAccessFlags);
        description.MiscFlags = static_cast<Direct3D11ResourceMiscFlag>(desc.MiscFlags);

        auto result = make<Direct3D11Texture2D>(texture, description);
        return result;
    }

    HRESULT __stdcall Direct3D11Texture2D::GetInterface(GUID const & id, void ** object)
    {
        CheckClosed();
        return m_texture->QueryInterface(id, object);
    }
}
