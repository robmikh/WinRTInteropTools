#pragma once
#include "Desktop.DispatcherQueueControllerInterop.g.h"

namespace winrt::WinRTInteropTools::Desktop::implementation
{
    struct DispatcherQueueControllerInterop : DispatcherQueueControllerInteropT<DispatcherQueueControllerInterop>
    {
        DispatcherQueueControllerInterop() = default;

        static Windows::System::DispatcherQueueController CreateDispatcherQueueControllerForCurrentThread();
        static int PumpMessagesIndefinitely();
    };
}
namespace winrt::WinRTInteropTools::Desktop::factory_implementation
{
    struct DispatcherQueueControllerInterop : DispatcherQueueControllerInteropT<DispatcherQueueControllerInterop, implementation::DispatcherQueueControllerInterop>
    {
    };
}
