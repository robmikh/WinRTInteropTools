#include "pch.h"
#include "CompositionGraphics.h"
#include "Direct3D11Device.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Composition;
using namespace Windows::Graphics;
using namespace Windows::Graphics::DirectX;
using namespace Windows::Graphics::DirectX::Direct3D11;

namespace util
{
    using namespace robmikh::common::uwp;
}

namespace winrt::WinRTInteropTools::implementation
{
    CompositionGraphicsDevice CompositionGraphics::CreateCompositionGraphicsDevice(
        Compositor const& compositor, 
        IDirect3DDevice const& device)
    {
        auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(device);
        auto graphicsDevice = util::CreateCompositionGraphicsDevice(compositor, d3dDevice.get());
        return graphicsDevice;
    }

    void CompositionGraphics::CopyDirect3DSurfaceIntoCompositionSurface(
        IDirect3DDevice const& device, 
        IDirect3DSurface const& d3dSurface, 
        CompositionDrawingSurface const& compositionSurface)
    {
        auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(device);
        auto d3dSourceTexture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(d3dSurface);
        auto toolsDevice = make<Direct3D11Device>(d3dDevice);
        auto multithread = toolsDevice.Multithread();

        com_ptr<ID3D11DeviceContext> d3dContext;
        d3dDevice->GetImmediateContext(d3dContext.put());

        auto width = d3dSurface.Description().Width;
        auto height = d3dSurface.Description().Height;
        compositionSurface.Resize(SizeInt32{ width, height });

        POINT point = {};
        auto dxgiSurface = util::SurfaceBeginDraw(compositionSurface, &point);
        auto destination = dxgiSurface.as<ID3D11Texture2D>();

        {
            auto lockSession = multithread.Lock();
            d3dContext->CopySubresourceRegion(destination.get(), 0, point.x, point.y, 0, d3dSourceTexture.get(), 0, NULL);
        }

        util::SurfaceEndDraw(compositionSurface);
    }

    void CompositionGraphics::ResizeSurface(
        CompositionDrawingSurface const& compositionSurface, 
        Size const& size)
    {
        util::ResizeSurface(compositionSurface, size);
    }

    void CompositionGraphics::ResizeSurface(
        CompositionDrawingSurface const& compositionSurface, 
        SizeInt32 const& size)
    {
        compositionSurface.Resize(size);
    }

    IDirect3DDevice CompositionGraphics::GetRenderingDevice(
        CompositionGraphicsDevice const& compGraphics)
    {
        auto graphicsDeviceInterop = compGraphics.as<ABI::Windows::UI::Composition::ICompositionGraphicsDeviceInterop>();
        com_ptr<::IUnknown> unknown;
        check_hresult(graphicsDeviceInterop->GetRenderingDevice(unknown.put()));
        auto d3dDevice = unknown.as<ID3D11Device>();
        return make<Direct3D11Device>(d3dDevice);
    }

    void CompositionGraphics::SetRenderingDevice(
        CompositionGraphicsDevice const& compGraphics, 
        IDirect3DDevice const& device)
    {
        auto graphicsDeviceInterop = compGraphics.as<ABI::Windows::UI::Composition::ICompositionGraphicsDeviceInterop>();
        auto d3dDevice = GetDXGIInterfaceFromObject<ID3D11Device>(device);
        check_hresult(graphicsDeviceInterop->SetRenderingDevice(d3dDevice.get()));
    }
}
