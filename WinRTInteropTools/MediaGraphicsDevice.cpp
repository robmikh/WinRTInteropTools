#include "pch.h"
#include "MediaGraphicsDevice.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Graphics;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Media::Core;

namespace winrt::WinRTInteropTools::implementation
{
    MediaGraphicsDevice::MediaGraphicsDevice(
        com_ptr<IMFDXGIDeviceManager> deviceManager,
        UINT uiToken)
    {
        m_deviceManager = deviceManager;
        m_uiToken = uiToken;
        m_deviceHandle = NULL;
    }

    IDirect3DDevice MediaGraphicsDevice::RenderingDevice()
    {
        CheckClosed();
        if (m_deviceHandle == NULL)
        {
            auto errorCode = m_deviceManager->OpenDeviceHandle(&m_deviceHandle);
            if (errorCode == MF_E_DXGI_DEVICE_NOT_INITIALIZED)
            {
                return nullptr;
            }
            else
            {
                check_hresult(errorCode);
            }
        }

        com_ptr<ID3D11Device> d3dDevice;
        HRESULT errorCode = S_OK;
        do
        {
            errorCode = m_deviceManager->GetVideoService(
                m_deviceHandle,
                guid_of<ID3D11Device>(),
                d3dDevice.put_void());

            if (errorCode == MF_E_DXGI_NEW_VIDEO_DEVICE)
            {
                // We don't care about the error
                m_deviceManager->CloseDeviceHandle(m_deviceHandle);
                check_hresult(m_deviceManager->OpenDeviceHandle(&m_deviceHandle));
            }
            else
            {
                check_hresult(errorCode);
            }
        } while (errorCode == MF_E_DXGI_NEW_VIDEO_DEVICE);
        
        auto dxgiDevice = d3dDevice.as<IDXGIDevice>();
        auto device = CreateDirect3DDevice(dxgiDevice.get());
        return device;
    }

    void MediaGraphicsDevice::RenderingDevice(
        IDirect3DDevice const& value)
    {
        CheckClosed();
        
        // We don't care about the error
        m_deviceManager->CloseDeviceHandle(m_deviceHandle);

        auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(value);
        check_hresult(m_deviceManager->ResetDevice(d3dDevice.get(), m_uiToken));

        check_hresult(m_deviceManager->OpenDeviceHandle(&m_deviceHandle));
    }

    void MediaGraphicsDevice::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_deviceManager = nullptr;
        }
    }

    WinRTInteropTools::MediaGraphicsDevice MediaGraphicsDevice::CreateFromMediaStreamSource(
        MediaStreamSource const& mediaStreamSource)
    {
        com_ptr<IMFDXGIDeviceManager> deviceManager;
        auto deviceManagerSource = mediaStreamSource.as<IMFDXGIDeviceManagerSource>();
        if (FAILED(deviceManagerSource->GetManager(deviceManager.put())))
        {
            UINT uiToken = 0;
            check_hresult(MFCreateDXGIDeviceManager(&uiToken, deviceManager.put()));
            return make<MediaGraphicsDevice>(deviceManager, uiToken);
        }
        else
        {
            throw hresult_error(E_UNEXPECTED);
        }
    }
}
