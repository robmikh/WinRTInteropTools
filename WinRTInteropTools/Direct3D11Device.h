#pragma once

#include "Direct3D11Device.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct Direct3D11Device : Direct3D11DeviceT<Direct3D11Device, IDirect3DDxgiInterfaceAccess>
    {
        Direct3D11Device();
        ~Direct3D11Device() { Close(); }

        bool IsMultithreadProtected();
        void IsMultithreadProtected(bool value);
        WinRTInteropTools::Direct3D11Multithread TryGetMultithread();
        void Close();
        void Trim();

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
