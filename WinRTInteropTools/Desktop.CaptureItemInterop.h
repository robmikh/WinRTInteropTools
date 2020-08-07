#pragma once
#include "Desktop.CaptureItemInterop.g.h"

namespace winrt::WinRTInteropTools::Desktop::implementation
{
    struct CaptureItemInterop : CaptureItemInteropT<CaptureItemInterop>
    {
        CaptureItemInterop() = default;

        static Windows::Graphics::Capture::GraphicsCaptureItem CreateForWindow(uint64_t windowHandle);
        static Windows::Graphics::Capture::GraphicsCaptureItem CreateForMonitor(uint64_t monitorHandle);
    };
}
namespace winrt::WinRTInteropTools::Desktop::factory_implementation
{
    struct CaptureItemInterop : CaptureItemInteropT<CaptureItemInterop, implementation::CaptureItemInterop>
    {
    };
}
