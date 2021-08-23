#include "pch.h"
#include "Direct3D11RenderTargetView.h"

namespace winrt::WinRTInteropTools::implementation
{
    Direct3D11RenderTargetView::Direct3D11RenderTargetView(
        com_ptr<ID3D11RenderTargetView> renderTargetView, 
        WinRTInteropTools::Direct3D11RenderTargetViewDescription const& description)
    {
        m_renderTargetView = renderTargetView;
        m_description = description;
    }

    WinRTInteropTools::Direct3D11RenderTargetViewDescription Direct3D11RenderTargetView::Description()
    {
        CheckClosed();
        return m_description;
    }

    void Direct3D11RenderTargetView::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_renderTargetView = nullptr;
        }
    }

    HRESULT __stdcall Direct3D11RenderTargetView::GetInterface(GUID const& id, void** object)
    {
        CheckClosed();
        return m_renderTargetView->QueryInterface(id, object);
    }
}
