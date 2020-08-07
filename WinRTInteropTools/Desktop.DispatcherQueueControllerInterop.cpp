#include "pch.h"
#include "Desktop.DispatcherQueueControllerInterop.h"

namespace winrt::WinRTInteropTools::Desktop::implementation
{
    Windows::System::DispatcherQueueController DispatcherQueueControllerInterop::CreateDispatcherQueueControllerForCurrentThread()
    {
        namespace abi = ABI::Windows::System;

        DispatcherQueueOptions options
        {
            sizeof(DispatcherQueueOptions),
            DQTYPE_THREAD_CURRENT,
            DQTAT_COM_NONE
        };

        winrt::Windows::System::DispatcherQueueController controller{ nullptr };
        winrt::check_hresult(CreateDispatcherQueueController(options, reinterpret_cast<abi::IDispatcherQueueController**>(winrt::put_abi(controller))));
        return controller;
    }
}
