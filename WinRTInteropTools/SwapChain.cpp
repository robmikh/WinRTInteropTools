#include "pch.h"
#include "SwapChain.h"

using namespace winrt;
using namespace Windows::Graphics;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::UI::Composition;

namespace winrt::WinRTInteropTools::implementation
{
    SwapChain::SwapChain(
        IDirect3DDevice const& device, 
        DirectXPixelFormat const& pixelFormat, 
        int32_t numberOfBuffers, 
        SizeInt32 const& size)
    {
        auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(device);
        m_swapChain = CreateDXGISwapChain(
            d3dDevice, 
            size.Width, 
            size.Height, 
            static_cast<DXGI_FORMAT>(pixelFormat), 
            (uint32_t)numberOfBuffers);
    }

    IDirect3DSurface SwapChain::GetBuffer(int32_t bufferIndex)
    {
        CheckClosed();

        com_ptr<IDXGISurface> dxgiSurface;
        check_hresult(m_swapChain->GetBuffer((UINT)bufferIndex, guid_of<IDXGISurface>(), dxgiSurface.put_void()));
        auto result = CreateDirect3DSurface(dxgiSurface.get());
        return result;
    }

    void SwapChain::Present()
    {
        CheckClosed();

        check_hresult(m_swapChain->Present(0, 0));
    }

    ICompositionSurface SwapChain::CreateSurface(
        Compositor const& compositor)
    {
        CheckClosed();

        auto surface = CreateCompositionSurfaceForSwapChain(compositor, m_swapChain.get());
        return surface;
    }

    void SwapChain::Resize(
        DirectXPixelFormat const& pixelFormat, 
        int32_t numberOfBuffers, 
        SizeInt32 const& size)
    {
        CheckClosed();

        check_hresult(m_swapChain->ResizeBuffers(
            (UINT)numberOfBuffers, 
            size.Width, 
            size.Height, 
            static_cast<DXGI_FORMAT>(pixelFormat), 
            0));
    }

    void SwapChain::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_swapChain = nullptr;
        }
    }

    HRESULT __stdcall SwapChain::GetInterface(GUID const & id, void ** object)
    {
        CheckClosed();

        return m_swapChain->QueryInterface(id, object);
    }
}
