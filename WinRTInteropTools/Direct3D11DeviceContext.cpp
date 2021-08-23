#include "pch.h"
#include "Direct3D11DeviceContext.h"

using namespace winrt;
using namespace Windows::Graphics;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;

namespace winrt::WinRTInteropTools::implementation
{
    Direct3D11DeviceContext::Direct3D11DeviceContext(
        com_ptr<ID3D11DeviceContext> deviceContext)
    {
        m_deviceContext = deviceContext;
    }

    void Direct3D11DeviceContext::CopyResource(
        IDirect3DSurface const& destination, 
        IDirect3DSurface const& source)
    {
        CheckClosed();

        auto destTexture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(destination);
        auto sourceTexture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(source);
        m_deviceContext->CopyResource(destTexture.get(), sourceTexture.get());
    }

    void Direct3D11DeviceContext::CopySubresourceRegion(
        IDirect3DSurface const& destination, 
        uint32_t destinationSubresource, 
        WinRTInteropTools::PositionUInt32 const& destinationPosition, 
        IDirect3DSurface const& source, 
        uint32_t sourceSubresource, 
        WinRTInteropTools::Direct3D11Box const& sourceBox)
    {
        CopySubResourceRegionInternal(
            destination, 
            destinationSubresource, 
            destinationPosition, 
            source, 
            sourceSubresource, 
            &sourceBox);
    }

    void Direct3D11DeviceContext::CopySubresourceRegion(
        IDirect3DSurface const& destination, 
        uint32_t destinationSubresource, 
        WinRTInteropTools::PositionUInt32 const& destinationPosition, 
        IDirect3DSurface const& source, 
        uint32_t sourceSubresource)
    {
        CopySubResourceRegionInternal(
            destination,
            destinationSubresource,
            destinationPosition,
            source,
            sourceSubresource,
            nullptr);
    }

    void Direct3D11DeviceContext::ClearRenderTargetView(
        WinRTInteropTools::Direct3D11RenderTargetView const& renderTargetView, 
        Windows::UI::Color const& color)
    {
        CheckClosed();

        std::array<float,4> colorf{ color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f  };

        auto d3dRTV = GetDXGIInterfaceFromObject<ID3D11RenderTargetView>(renderTargetView);
        m_deviceContext->ClearRenderTargetView(d3dRTV.get(), colorf.data());
    }

    void Direct3D11DeviceContext::CopySubResourceRegionInternal(
        IDirect3DSurface const& destination,
        uint32_t destinationSubresource,
        WinRTInteropTools::PositionUInt32 const& destinationPosition,
        IDirect3DSurface const& source,
        uint32_t sourceSubresource,
        const WinRTInteropTools::Direct3D11Box* pSourceBox)
    {
        CheckClosed();

        // Make sure the box is the right layout
        static_assert(sizeof(WinRTInteropTools::Direct3D11Box) == sizeof(D3D11_BOX), "WinRTInteropTools::Direct3D11Box layout must match D3D11_BOX layout");
        static_assert(offsetof(WinRTInteropTools::Direct3D11Box, Left) == offsetof(D3D11_BOX, left), "WinRTInteropTools::Direct3D11Box layout must match D3D11_BOX layout");
        static_assert(offsetof(WinRTInteropTools::Direct3D11Box, Top) == offsetof(D3D11_BOX, top), "WinRTInteropTools::Direct3D11Box layout must match D3D11_BOX layout");
        static_assert(offsetof(WinRTInteropTools::Direct3D11Box, Front) == offsetof(D3D11_BOX, front), "WinRTInteropTools::Direct3D11Box layout must match D3D11_BOX layout");
        static_assert(offsetof(WinRTInteropTools::Direct3D11Box, Right) == offsetof(D3D11_BOX, right), "WinRTInteropTools::Direct3D11Box layout must match D3D11_BOX layout");
        static_assert(offsetof(WinRTInteropTools::Direct3D11Box, Bottom) == offsetof(D3D11_BOX, bottom), "WinRTInteropTools::Direct3D11Box layout must match D3D11_BOX layout");
        static_assert(offsetof(WinRTInteropTools::Direct3D11Box, Back) == offsetof(D3D11_BOX, back), "WinRTInteropTools::Direct3D11Box layout must match D3D11_BOX layout");

        auto destTexture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(destination);
        auto sourceTexture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(source);
        m_deviceContext->CopySubresourceRegion(
            destTexture.get(), 
            destinationSubresource, 
            destinationPosition.X, destinationPosition.Y, destinationPosition.Z, 
            sourceTexture.get(), 
            sourceSubresource, 
            reinterpret_cast<const D3D11_BOX *>(pSourceBox));
    }

    void Direct3D11DeviceContext::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_deviceContext = nullptr;
        }
    }

    HRESULT __stdcall Direct3D11DeviceContext::GetInterface(GUID const & id, void ** object)
    {
        CheckClosed();

        return m_deviceContext->QueryInterface(id, object);
    }
}
