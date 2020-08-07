#pragma once

// Windows SDK
#include <unknwn.h>
#include <inspectable.h>

// Must come before C++/WinRT
#include <wil/cppwinrt.h>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Core.h>

#include <Windows.Graphics.Capture.Interop.h>
#include <windows.ui.composition.interop.h>
#include <DispatcherQueue.h>

// STL
#include <atomic>
#include <memory>
#include <string>

// WIL
#include <wil/resource.h>

// DirectX
#include <d3d11_4.h>
#include <dxgi1_6.h>

// WIC
#include <wincodec.h>

// Media Foundation
#include <mfidl.h>
#include <mfapi.h>

// Helpers
#include "direct3d11.interop.h"
#include "d3dHelpers.h"
#include "composition.interop.h"
#include "streamHelpers.h"