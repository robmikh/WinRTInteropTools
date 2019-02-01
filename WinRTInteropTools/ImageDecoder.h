#pragma once

#include "ImageDecoder.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct ImageDecoder : ImageDecoderT<ImageDecoder>
    {
        ImageDecoder();
        ~ImageDecoder() { Close(); }

        Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface DecodeStreamIntoTexture(
            Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device, 
            Windows::Storage::Streams::IRandomAccessStream const& stream);
        void Close();

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

    private:
        com_ptr<IWICImagingFactory2> m_wicFactory;
        std::atomic<bool> m_closed = false;
    };
}

namespace winrt::WinRTInteropTools::factory_implementation
{
    struct ImageDecoder : ImageDecoderT<ImageDecoder, implementation::ImageDecoder>
    {
    };
}
