#include "pch.h"
#include "Desktop.DispatcherQueueControllerInterop.h"

template <typename F>
F* LoadFunction(wil::unique_hmodule const& hmodule, std::string const& name)
{
    return reinterpret_cast<F*>(winrt::check_pointer(WINRT_IMPL_GetProcAddress(hmodule.get(), name.c_str())));
}

namespace winrt::WinRTInteropTools::Desktop::implementation
{
    using GetMessageW_f = int32_t __stdcall(LPMSG lpmsg, HWND hwnd, uint32_t wmsgfiltermin, uint32_t wmsgfiltermax) noexcept;
    using TranslateMessage_f = int32_t __stdcall(const MSG* lpmsg) noexcept;
    using DispatchMessageW_f = int32_t __stdcall(const MSG* lpmsg) noexcept;

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

    int DispatcherQueueControllerInterop::PumpMessagesIndefinitely()
    {
        wil::unique_hmodule user32(winrt::check_pointer(reinterpret_cast<HMODULE>(WINRT_IMPL_LoadLibraryW(L"user32.dll"))));
        auto GetMessageW = LoadFunction<GetMessageW_f>(user32, "GetMessageW");
        auto TranslateMessage = LoadFunction<TranslateMessage_f>(user32, "TranslateMessage");
        auto DispatchMessageW = LoadFunction<DispatchMessageW_f>(user32, "DispatchMessageW");

        MSG msg = {};
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        return static_cast<int>(msg.wParam);
    }
}
