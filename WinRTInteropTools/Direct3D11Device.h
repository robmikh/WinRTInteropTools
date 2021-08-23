#pragma once

#include "Direct3D11Device.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct Direct3D11Device : Direct3D11DeviceT<Direct3D11Device, IDirect3DDxgiInterfaceAccess>
    {
        Direct3D11Device();
        Direct3D11Device(winrt::com_ptr<ID3D11Device> d3dDevice);
        ~Direct3D11Device() { Close(); }

        WinRTInteropTools::Direct3D11DeviceContext ImmediateContext();
        WinRTInteropTools::Direct3D11Multithread Multithread();
        WinRTInteropTools::Direct3D11Texture2D CreateTexture2D(WinRTInteropTools::Direct3D11Texture2DDescription const& description);
        WinRTInteropTools::Direct3D11Texture2D CreateTexture2D(WinRTInteropTools::Direct3D11Texture2DDescription const& description, array_view<uint8_t const> data);
        WinRTInteropTools::Direct3D11Texture2D CreateTexture2D(WinRTInteropTools::Direct3D11Texture2DDescription const& description, array_view<uint8_t const> data, uint32_t pitch);
        WinRTInteropTools::Direct3D11RenderTargetView CreateRenderTargetView(Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& resource);
        void Close();
        void Trim();

        static WinRTInteropTools::Direct3D11Device CreateFromDirect3D11Device(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device);

        virtual HRESULT __stdcall GetInterface(GUID const& id, void** object);

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

    private:
        com_ptr<ID3D11Device> m_d3dDevice;
        std::atomic<bool> m_closed = false;
    };
}

namespace winrt::WinRTInteropTools::factory_implementation
{
    struct Direct3D11Device : Direct3D11DeviceT<Direct3D11Device, implementation::Direct3D11Device>
    {
    };
}
