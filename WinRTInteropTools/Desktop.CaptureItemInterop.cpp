#include "pch.h"
#include "Desktop.CaptureItemInterop.h"

namespace winrt::WinRTInteropTools::Desktop::implementation
{
    using namespace Windows::Graphics::Capture;

    inline auto GetItemInterop()
    {
        auto factory = try_get_activation_factory<GraphicsCaptureItem>();
        auto interop = factory.as<IGraphicsCaptureItemInterop>();
        return interop;
    }

    Windows::Graphics::Capture::GraphicsCaptureItem CaptureItemInterop::CreateForWindow(uint64_t windowHandle)
    {
        auto interop = GetItemInterop();
        GraphicsCaptureItem item{ nullptr };
        winrt::check_hresult(interop->CreateForWindow(reinterpret_cast<HWND>(windowHandle), guid_of<GraphicsCaptureItem>(), put_abi(item)));
        return item;
    }
    Windows::Graphics::Capture::GraphicsCaptureItem CaptureItemInterop::CreateForMonitor(uint64_t monitorHandle)
    {
        auto interop = GetItemInterop();
        GraphicsCaptureItem item{ nullptr };
        winrt::check_hresult(interop->CreateForMonitor(reinterpret_cast<HMONITOR>(monitorHandle), guid_of<GraphicsCaptureItem>(), put_abi(item)));
        return item;
    }
}
