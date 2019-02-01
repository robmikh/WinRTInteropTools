#pragma once

#include "Direct3D11Multithread.g.h"

namespace winrt::WinRTInteropTools::implementation
{
    struct Direct3D11Multithread : Direct3D11MultithreadT<Direct3D11Multithread>
    {
        Direct3D11Multithread(com_ptr<ID3D11Multithread> multithread)
        {
            m_multithread = multithread;
        }
        ~Direct3D11Multithread() { Close(); }

        Windows::Foundation::IClosable Lock();
        void Close();

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

    private:
        com_ptr<ID3D11Multithread> m_multithread;
        std::atomic<bool> m_closed = false;
    };

    struct Direct3D11MultithreadLockSession : implements<Direct3D11MultithreadLockSession, Windows::Foundation::IClosable>
    {
        Direct3D11MultithreadLockSession(com_ptr<ID3D11Multithread> multithread)
        {
            m_multithread = multithread;
            m_multithread->Enter();
        }
        ~Direct3D11MultithreadLockSession() { Close(); }

        void Close()
        {
            auto expected = false;
            if (m_closed.compare_exchange_strong(expected, true))
            {
                m_multithread->Leave();
                m_multithread = nullptr;
            }
        }

    private:
        void CheckClosed()
        {
            if (m_closed.load() == true)
            {
                throw hresult_error(RO_E_CLOSED);
            }
        }

    private:
        com_ptr<ID3D11Multithread> m_multithread;
        std::atomic<bool> m_closed = false;
    };
}
