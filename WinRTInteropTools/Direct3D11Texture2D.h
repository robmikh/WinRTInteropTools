#pragma once

#include "Direct3D11Texture2D.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct Direct3D11Texture2D : Direct3D11Texture2DT<Direct3D11Texture2D, IDirect3DDxgiInterfaceAccess>
    {
        Direct3D11Texture2D(
            com_ptr<ID3D11Texture2D> texture,
            WinRTInteropTools::Direct3D11Texture2DDescription const& description);

        WinRTInteropTools::Direct3D11Texture2DDescription Description2D();
        void Close();
        Windows::Graphics::DirectX::Direct3D11::Direct3DSurfaceDescription Description();

        static WinRTInteropTools::Direct3D11Texture2D CreateFromDirect3DSurface(Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& surface);

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
        com_ptr<ID3D11Texture2D> m_texture;
        WinRTInteropTools::Direct3D11Texture2DDescription m_description;
        std::atomic<bool> m_closed = false;
    };
}
namespace winrt::WinRTInteropTools::factory_implementation
{
    struct Direct3D11Texture2D : Direct3D11Texture2DT<Direct3D11Texture2D, implementation::Direct3D11Texture2D>
    {
    };
}