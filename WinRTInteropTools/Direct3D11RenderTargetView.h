#pragma once
#include "Direct3D11RenderTargetView.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct Direct3D11RenderTargetView : Direct3D11RenderTargetViewT<Direct3D11RenderTargetView, IDirect3DDxgiInterfaceAccess>
    {
        Direct3D11RenderTargetView(
            com_ptr<ID3D11RenderTargetView> renderTargetView,
            WinRTInteropTools::Direct3D11RenderTargetViewDescription const& description);

        WinRTInteropTools::Direct3D11RenderTargetViewDescription Description();
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
        com_ptr<ID3D11RenderTargetView> m_renderTargetView;
        WinRTInteropTools::Direct3D11RenderTargetViewDescription m_description;
        std::atomic<bool> m_closed = false;
    };
}
