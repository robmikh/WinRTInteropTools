#include "pch.h"
#include "Direct3D11Multithread.h"

namespace winrt::WinRTInteropTools::implementation
{
    Windows::Foundation::IClosable Direct3D11Multithread::Lock()
    {
        CheckClosed();

        auto session = make<Direct3D11MultithreadLockSession>(m_multithread);
        return session;
    }

    void Direct3D11Multithread::Close()
    {
        auto expected = false;
        if (m_closed.compare_exchange_strong(expected, true))
        {
            m_multithread = nullptr;
        }
    }
}
