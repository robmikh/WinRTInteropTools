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
