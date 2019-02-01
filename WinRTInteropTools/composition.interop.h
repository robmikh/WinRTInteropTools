#pragma once
#include <winrt/Windows.UI.Composition.h>
#include <windows.ui.composition.interop.h>

inline auto CreateCompositionGraphicsDevice(
    winrt::Windows::UI::Composition::Compositor const& compositor,
    ::IUnknown* device)
{
    winrt::Windows::UI::Composition::CompositionGraphicsDevice graphicsDevice{ nullptr };
    auto compositorInterop = compositor.as<ABI::Windows::UI::Composition::ICompositorInterop>();
    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionGraphicsDevice> graphicsInterop;
    winrt::check_hresult(compositorInterop->CreateGraphicsDevice(device, graphicsInterop.put()));
    winrt::check_hresult(graphicsInterop->QueryInterface(winrt::guid_of<winrt::Windows::UI::Composition::CompositionGraphicsDevice>(),
        reinterpret_cast<void**>(winrt::put_abi(graphicsDevice))));
    return graphicsDevice;
}

inline void ResizeSurface(
    winrt::Windows::UI::Composition::CompositionDrawingSurface const& surface,
    winrt::Windows::Foundation::Size const& size)
{
    auto surfaceInterop = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
    SIZE newSize = {};
    newSize.cx = static_cast<LONG>(std::round(size.Width));
    newSize.cy = static_cast<LONG>(std::round(size.Height));
    winrt::check_hresult(surfaceInterop->Resize(newSize));
}

inline void ResizeSurface(
    winrt::Windows::UI::Composition::CompositionDrawingSurface const& surface,
    winrt::Windows::Graphics::SizeInt32 const& size)
{
    auto surfaceInterop = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
    SIZE newSize = {};
    newSize.cx = static_cast<LONG>(size.Width);
    newSize.cy = static_cast<LONG>(size.Height);
    winrt::check_hresult(surfaceInterop->Resize(newSize));
}

inline auto SurfaceBeginDraw(
    winrt::Windows::UI::Composition::CompositionDrawingSurface const& surface,
    POINT* offset)
{
    auto surfaceInterop = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
    winrt::com_ptr<IDXGISurface1> dxgiSurface;
    winrt::check_hresult(surfaceInterop->BeginDraw(nullptr, __uuidof(IDXGISurface1), dxgiSurface.put_void(), offset));
    return dxgiSurface;
}

inline void SurfaceEndDraw(
    winrt::Windows::UI::Composition::CompositionDrawingSurface const& surface)
{
    auto surfaceInterop = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
    winrt::check_hresult(surfaceInterop->EndDraw());
}

inline auto CreateCompositionSurfaceForSwapChain(
    winrt::Windows::UI::Composition::Compositor const& compositor,
    ::IUnknown* swapChain)
{
    winrt::Windows::UI::Composition::ICompositionSurface surface{ nullptr };
    auto compositorInterop = compositor.as<ABI::Windows::UI::Composition::ICompositorInterop>();
    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionSurface> surfaceInterop;
    winrt::check_hresult(compositorInterop->CreateCompositionSurfaceForSwapChain(swapChain, surfaceInterop.put()));
    winrt::check_hresult(surfaceInterop->QueryInterface(winrt::guid_of<winrt::Windows::UI::Composition::ICompositionSurface>(),
        reinterpret_cast<void**>(winrt::put_abi(surface))));
    return surface;
}