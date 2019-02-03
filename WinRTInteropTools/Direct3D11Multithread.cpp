#include "pch.h"
#include "Direct3D11Multithread.h"

namespace winrt::WinRTInteropTools::implementation
{
    bool Direct3D11Multithread::IsMultithreadProtected()
    {
        CheckClosed();
        return m_multithread->GetMultithreadProtected();
    }

    void Direct3D11Multithread::IsMultithreadProtected(bool value)
    {
        CheckClosed();
        m_multithread->SetMultithreadProtected(value);
    }

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
