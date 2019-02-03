#include "pch.h"
#include "Direct3D11Device.h"
#include "Direct3D11Multithread.h"
#include "Direct3D11DeviceContext.h"

namespace winrt::WinRTInteropTools::implementation
{
    Direct3D11Device::Direct3D11Device()
    {
        m_d3dDevice = CreateD3DDevice();
    }

    Direct3D11Device::Direct3D11Device(
        com_ptr<ID3D11Device> d3dDevice)
    {
        m_d3dDevice = d3dDevice;
    }

    WinRTInteropTools::Direct3D11Multithread Direct3D11Device::Multithread()
    {
        CheckClosed();

        auto multithread = m_d3dDevice.as<ID3D11Multithread>();
        auto result = make<Direct3D11Multithread>(multithread);
        return result;
    }

    WinRTInteropTools::Direct3D11DeviceContext Direct3D11Device::ImmediateContext()
    {
        CheckClosed();

        com_ptr<ID3D11DeviceContext> d3dContext;
        m_d3dDevice->GetImmediateContext(d3dContext.put());
        auto deviceContext = make<Direct3D11DeviceContext>(d3dContext);
        return deviceContext;
    }

    void Direct3D11Device::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_d3dDevice = nullptr;
        }
    }

    void Direct3D11Device::Trim()
    {
        CheckClosed();

        auto dxgiDevice = m_d3dDevice.as<IDXGIDevice3>();
        dxgiDevice->Trim();
    }

    HRESULT __stdcall Direct3D11Device::GetInterface(GUID const & id, void ** object)
    {
        CheckClosed();

        return m_d3dDevice->QueryInterface(id, object);
    }
}
