#include "pch.h"
#include "ImageDecoder.h"

using namespace winrt;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;

namespace util
{
    using namespace robmikh::common::uwp;
}

namespace winrt::WinRTInteropTools::implementation
{
    ImageDecoder::ImageDecoder()
    {
        m_wicFactory = util::CreateWICFactory();
    }

    IDirect3DSurface ImageDecoder::DecodeStreamIntoTexture(
        Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device, 
        IRandomAccessStream const& randomAccessStream)
    {
        CheckClosed();

        auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(device);

        auto stream = util::CreateStreamFromRandomAccessStream(randomAccessStream);
        com_ptr<IWICBitmapDecoder> wicBitmapDecoder;
        check_hresult(m_wicFactory->CreateDecoderFromStream(
            stream.get(),
            nullptr,
            WICDecodeMetadataCacheOnDemand,
            wicBitmapDecoder.put()));

        com_ptr<IWICBitmapFrameDecode> wicFrameBitmapDecode;
        check_hresult(wicBitmapDecoder->GetFrame(0, wicFrameBitmapDecode.put()));

        com_ptr<IWICFormatConverter> wicFormatConverter;
        check_hresult(m_wicFactory->CreateFormatConverter(wicFormatConverter.put()));

        auto targetPixelFormat = GUID_WICPixelFormat32bppPBGRA;
        check_hresult(wicFormatConverter->Initialize(
            wicFrameBitmapDecode.get(),
            targetPixelFormat,
            WICBitmapDitherTypeNone,
            NULL,
            0,
            WICBitmapPaletteTypeMedianCut));

        uint32_t width, height;
        check_hresult(wicFormatConverter->GetSize(&width, &height));

        uint32_t stride, bufferSize;
        stride = 4 * width;
        bufferSize = stride * height;
        std::unique_ptr<uint8_t> temp(new uint8_t[bufferSize]);
        check_hresult(wicFormatConverter->CopyPixels(NULL, stride, bufferSize, temp.get()));

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = temp.get();
        initData.SysMemPitch = static_cast<UINT>(stride);
        initData.SysMemSlicePitch = static_cast<UINT>(bufferSize);

        com_ptr<ID3D11Texture2D> texture;
        check_hresult(d3dDevice->CreateTexture2D(&desc, &initData, texture.put()));
        
        auto dxgiSurface = texture.as<IDXGISurface>();
        auto result = CreateDirect3DSurface(dxgiSurface.get());

        return result;
    }

    void ImageDecoder::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_wicFactory = nullptr;
        }
    }
}
