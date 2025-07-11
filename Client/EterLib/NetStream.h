#pragma once

#include "../UserInterface/Locale_inc.h"

#ifdef _IMPROVED_PACKET_ENCRYPTION_
#include "../eterBase/cipher.h"
#endif
#include "../eterBase/tea.h"
#include "NetAddress.h"

class CNetworkStream
{
	public:
		CNetworkStream();
		virtual ~CNetworkStream();

		void SetRecvBufferSize(int recvBufSize);
		void SetSendBufferSize(int sendBufSize);

#ifndef _IMPROVED_PACKET_ENCRYPTION_
		void SetSecurityMode(bool isSecurityMode, const char* c_szTeaKey);
		void SetSecurityMode(bool isSecurityMode, const char* c_szTeaEncryptKey, const char* c_szTeaDecryptKey);
#endif
		bool IsSecurityMode();

		int	GetRecvBufferSize();

		void Clear();
		void ClearRecvBuffer();

		void Process();

		bool Connect(const CNetworkAddress& c_rkNetAddr, int limitSec = 3);
		bool Connect(const char* c_szAddr, int port, int limitSec = 3);
		bool Connect(DWORD dwAddr, int port, int limitSec = 3);
		void Disconnect();

		bool PeekNoFetch(int len);
		bool Peek(int len, char* pDestBuf);
		bool RecvNoFetch(int len);
		bool Recv(int len, char* pDestBuf);
		bool Send(int len, const char* pSrcBuf);

		bool Peek(int len, void* pDestBuf);
		bool Recv(int len, void* pDestBuf);

		bool Send(int len, const void* pSrcBuf);
		bool SendFlush(int len, const void* pSrcBuf);

		template<typename T, std::enable_if_t<utils::IsRawV<T>>* = nullptr>
		bool AutoPeek(T& c_pvData) {
			return Peek(sizeof(T), &c_pvData);
		}
		template<typename C, std::enable_if_t<utils::IsContiguousV<C>>* = nullptr>
		bool AutoPeek(C& v) {
			return Peek(v.size() * sizeof(typename C::value_type), v.data());
		}

		template<typename T, std::enable_if_t<utils::IsRawV<T>>* = nullptr>
		bool AutoRecv(T& c_pvData) {
			return Recv(sizeof(T), &c_pvData);
		}
		template<typename C, std::enable_if_t<utils::IsContiguousV<C>>* = nullptr>
		bool AutoRecv(C& v) {
			return Recv(v.size() * sizeof(typename C::value_type), v.data());
		}

		template<typename T, std::enable_if_t<utils::IsRawV<T>>* = nullptr>
		bool SendFlush(const T& c_pvData) {
			return SendFlush(sizeof(T), &c_pvData);
		}
		template<typename C, std::enable_if_t<utils::IsContiguousV<C>>* = nullptr>
		bool SendFlush(const C& v) {
			return SendFlush(v.size() * sizeof(typename C::value_type), v.data());
		}

		template<typename T, std::enable_if_t<utils::IsRawV<T>>* = nullptr>
		bool Send(const T& c_pvData) {
			return Send(sizeof(T), &c_pvData);
		}
		template<typename C, std::enable_if_t<utils::IsContiguousV<C>>* = nullptr>
		bool Send(const C& v) {
			return Send(v.size() * sizeof(typename C::value_type), v.data());
		}

		bool IsOnline();

		void SetPacketSequenceMode(bool isOn);
		bool SendSequence();

	protected:
		virtual void OnConnectSuccess();
		virtual void OnConnectFailure();
		virtual void OnRemoteDisconnect();
		virtual void OnDisconnect();
		virtual bool OnProcess();

		bool __SendInternalBuffer();
		bool __RecvInternalBuffer();

		void __PopSendBuffer();

		int __GetSendBufferSize();

#ifdef _IMPROVED_PACKET_ENCRYPTION_
		size_t Prepare(void* buffer, size_t* length);
		bool Activate(size_t agreed_length, const void* buffer, size_t length);
		void ActivateCipher();
#endif

	private:
		time_t	m_connectLimitTime;

		char*	m_recvTEABuf;
		int		m_recvTEABufInputPos;
		int		m_recvTEABufSize;

		char*	m_recvBuf;
		int		m_recvBufSize;
		int		m_recvBufInputPos;
		int		m_recvBufOutputPos;

		char*	m_sendBuf;
		int		m_sendBufSize;
		int		m_sendBufInputPos;
		int		m_sendBufOutputPos;

		char*	m_sendTEABuf;
		int		m_sendTEABufSize;
		int		m_sendTEABufInputPos;

		bool	m_isOnline;

#ifdef _IMPROVED_PACKET_ENCRYPTION_
		Cipher	m_cipher;
#else
		// Obsolete encryption stuff here
		bool	m_isSecurityMode;
		char	m_szEncryptKey[TEA_KEY_LENGTH];
		char	m_szDecryptKey[TEA_KEY_LENGTH];
#endif

		SOCKET	m_sock;

		CNetworkAddress m_addr;

#ifdef ENABLE_SEQUENCE_SYSTEM
		// Sequence
		DWORD					m_iSequence;
		bool					m_bUseSequence;
		std::vector<BYTE>		m_kVec_bSequenceTable;
#endif
};
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
