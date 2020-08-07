#include "pch.h"
#include "Desktop.CompositiorDesktopInterop.h"

namespace winrt::WinRTInteropTools::Desktop::implementation
{
    using namespace Windows::UI::Composition;
    using namespace Windows::UI::Composition::Desktop;

    namespace abi
    {
        using namespace ::ABI::Windows::UI::Composition::Desktop;
    }

    Windows::UI::Composition::Desktop::DesktopWindowTarget CompositiorDesktopInterop::CreateWindowTarget(Compositor const& compositor, uint64_t windowHandle, bool isTopMost)
    {
        auto interop = compositor.as<abi::ICompositorDesktopInterop>();
        DesktopWindowTarget target{ nullptr };
        winrt::check_hresult(interop->CreateDesktopWindowTarget(reinterpret_cast<HWND>(windowHandle), isTopMost, reinterpret_cast<abi::IDesktopWindowTarget**>(put_abi(target))));
        return target;
    }
}
