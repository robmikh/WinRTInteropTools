﻿#include "pch.h"
#include "Direct3D11Texture2D.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Graphics;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;

namespace winrt::WinRTInteropTools::implementation
{
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