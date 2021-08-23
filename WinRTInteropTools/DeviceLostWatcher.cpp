#include "pch.h"
#include "DeviceLostWatcher.h"

namespace winrt::WinRTInteropTools::implementation
{
    void DeviceLostWatcher::WatchDevice(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device)
    {
        // Stop listening for device lost if we currently are
        StopWatchingCurrentDevice();

        // Set the current device to the new device
        m_device = device;

        // Get the D3D Device
        auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device4>(device);

        // Create a wait struct
        OnDeviceLostHandler = {};
        OnDeviceLostHandler = CreateThreadpoolWait(DeviceLostWatcher::OnDeviceLost, (PVOID)this, NULL);

        // Create a handle and a cookie
        m_eventHandle = winrt::check_pointer(CreateEvent(NULL, FALSE, FALSE, NULL));
        m_cookie = NULL;

        // Register for device lost
        SetThreadpoolWait(OnDeviceLostHandler, m_eventHandle, NULL);
        check_hresult(d3dDevice->RegisterDeviceRemovedEvent(m_eventHandle, &m_cookie));
    }
    void DeviceLostWatcher::StopWatchingCurrentDevice()
    {
        if (m_device)
        {
            // Get the D3D Device
            auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device4>(m_device);

            // Unregister from device lost
            CloseThreadpoolWait(OnDeviceLostHandler);
            d3dDevice->UnregisterDeviceRemoved(m_cookie);

            // Clear member variables
            OnDeviceLostHandler = NULL;
            CloseHandle(m_eventHandle);
            m_eventHandle = NULL;
            m_cookie = NULL;
            m_device = nullptr;
        }
    }
    void DeviceLostWatcher::RaiseDeviceLostEvent(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& oldDevice)
    {
        if (m_deviceLostEvent)
        {
            m_deviceLostEvent(*this, oldDevice);
        }
    }
    void DeviceLostWatcher::OnDeviceLost(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WAIT wait, TP_WAIT_RESULT waitResult)
    {
        UNREFERENCED_PARAMETER(instance);
        UNREFERENCED_PARAMETER(wait);
        UNREFERENCED_PARAMETER(waitResult);

        auto deviceLostHelper = reinterpret_cast<DeviceLostWatcher*>(context);
        auto oldDevice = deviceLostHelper->m_device;
        deviceLostHelper->StopWatchingCurrentDevice();

        deviceLostHelper->RaiseDeviceLostEvent(oldDevice);
    }
}
