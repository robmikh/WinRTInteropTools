#pragma once
#include "Desktop.CompositiorDesktopInterop.g.h"

namespace winrt::WinRTInteropTools::Desktop::implementation
{
    struct CompositiorDesktopInterop : CompositiorDesktopInteropT<CompositiorDesktopInterop>
    {
        CompositiorDesktopInterop() = default;

        static Windows::UI::Composition::Desktop::DesktopWindowTarget CreateWindowTarget(Windows::UI::Composition::Compositor const& compositor, uint64_t windowHandle, bool isTopMost);
    };
}
namespace winrt::WinRTInteropTools::Desktop::factory_implementation
{
    struct CompositiorDesktopInterop : CompositiorDesktopInteropT<CompositiorDesktopInterop, implementation::CompositiorDesktopInterop>
    {
    };
}
