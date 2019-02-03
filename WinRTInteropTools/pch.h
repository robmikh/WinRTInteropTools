//
// pch.h
// Header for platform projection include files
//

#pragma once

// Interop support
#include <unknwn.h>
#include <inspectable.h>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Core.h>

// STL
#include <atomic>
#include <memory>

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