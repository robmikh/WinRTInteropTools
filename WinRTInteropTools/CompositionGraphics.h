#pragma once

#include "CompositionGraphics.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct CompositionGraphics : CompositionGraphicsT<CompositionGraphics>
    {
        CompositionGraphics() = delete;

        static Windows::UI::Composition::CompositionGraphicsDevice CreateCompositionGraphicsDevice(Windows::UI::Composition::Compositor const& compositor, Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device);
        static void CopyDirect3DSurfaceIntoCompositionSurface(Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const& device, Windows::Graphics::DirectX::Direct3D11::IDirect3DSurface const& d3dSurface, Windows::UI::Composition::CompositionDrawingSurface const& compositionSurface);
        static void ResizeSurface(Windows::UI::Composition::CompositionDrawingSurface const& compositionSurface, Windows::Foundation::Size const& size);
        static void ResizeSurface(Windows::UI::Composition::CompositionDrawingSurface const& compositionSurface, Windows::Graphics::SizeInt32 const& size);
    };
}

namespace winrt::WinRTInteropTools::factory_implementation
{
    struct CompositionGraphics : CompositionGraphicsT<CompositionGraphics, implementation::CompositionGraphics>
    {
    };
}
