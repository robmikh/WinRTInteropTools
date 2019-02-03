#pragma once

#include "MediaGraphicsDevice.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct MediaGraphicsDevice : MediaGraphicsDeviceT<MediaGraphicsDevice>
    {
        MediaGraphicsDevice(
            winrt::com_ptr<IMFDXGIDeviceManager> deviceManager,
            UINT uiToken);

        Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice RenderingDevice();
        void RenderingDevice(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& value);
        void Close();

        static WinRTInteropTools::MediaGraphicsDevice CreateFromMediaStreamSource(Windows::Media::Core::MediaStreamSource const& mediaStreamSource);

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

    private:
        com_ptr<IMFDXGIDeviceManager> m_deviceManager;
        UINT m_uiToken;
        HANDLE m_deviceHandle;
        std::atomic<bool> m_closed = false;
    };
}

namespace winrt::WinRTInteropTools::factory_implementation
{
    struct MediaGraphicsDevice : MediaGraphicsDeviceT<MediaGraphicsDevice, implementation::MediaGraphicsDevice>
    {
    };
}
