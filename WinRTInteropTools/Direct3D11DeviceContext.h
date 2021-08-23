#pragma once

#include "Direct3D11DeviceContext.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct Direct3D11DeviceContext : Direct3D11DeviceContextT<Direct3D11DeviceContext, IDirect3DDxgiInterfaceAccess>
    {
        Direct3D11DeviceContext(winrt::com_ptr<ID3D11DeviceContext> deviceContext);

        void CopyResource(Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& destination, Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& source);
        void CopySubresourceRegion(
            Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& destination, 
            uint32_t destinationSubresource, 
            WinRTInteropTools::PositionUInt32 const& destinationPosition, 
            Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& source, 
            uint32_t sourceSubresource, 
            WinRTInteropTools::Direct3D11Box const& sourceBox);
        void CopySubresourceRegion(
            Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& destination, 
            uint32_t destinationSubresource, 
            WinRTInteropTools::PositionUInt32 const& destinationPosition, 
            Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& source, 
            uint32_t sourceSubresource);
        void ClearRenderTargetView(WinRTInteropTools::Direct3D11RenderTargetView const& renderTargetView, array_view<float const> colorRGBA);
        void Close();

        virtual HRESULT __stdcall GetInterface(GUID const& id, void** object);

    private:
        void CopySubResourceRegionInternal(
            Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& destination,
            uint32_t destinationSubresource,
            WinRTInteropTools::PositionUInt32 const& destinationPosition,
            Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& source,
            uint32_t sourceSubresource,
            const WinRTInteropTools::Direct3D11Box* pSourceBox);

        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

    private:
        com_ptr<ID3D11DeviceContext> m_deviceContext;;
        std::atomic<bool> m_closed = false;
    };
}
