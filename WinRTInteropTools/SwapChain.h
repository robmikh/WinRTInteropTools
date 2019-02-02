#pragma once

#include "SwapChain.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct SwapChain : SwapChainT<SwapChain, IDirect3DDxgiInterfaceAccess>
    {
        SwapChain() = delete;
        SwapChain(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device, Windows::Graphics::DirectX::DirectXPixelFormat const& pixelFormat, int32_t numberOfBuffers, Windows::Graphics::SizeInt32 const& size);
        ~SwapChain() { Close(); }

        Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface GetBuffer(int32_t bufferIndex);
        void Present();
        Windows::UI::Composition::ICompositionSurface CreateSurface(Windows::UI::Composition::Compositor const& compositor);
        void Resize(Windows::Graphics::DirectX::DirectXPixelFormat const& pixelFormat, int32_t numberOfBuffers, Windows::Graphics::SizeInt32 const& size);
        void Close();

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
        com_ptr<IDXGISwapChain1> m_swapChain;
        std::atomic<bool> m_closed = false;
    };
}

namespace winrt::WinRTInteropTools::factory_implementation
{
    struct SwapChain : SwapChainT<SwapChain, implementation::SwapChain>
    {
    };
}
