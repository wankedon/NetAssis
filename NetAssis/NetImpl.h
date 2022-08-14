#pragma once
#include "NetTransport.h"
#include <functional>

using ReceiveDataHandler = std::function<void(const DataBlock& db, void* user)>;

class NetImpl
{
public:
	NetImpl(const ReceiveDataHandler& receiveDataHandler);
	~NetImpl();

public:
	void creatTransport(const WorkMode workMode);
	void setDestEndpoint(const char* destEndpoint);
	void setLocalPort(const unsigned short localPort);
	void setReceiveBufferSize(const size_t length);
	void start();
	void send(const DataBlock& db);
	void startTimer(const uint64_t timeInterval);
	void close();
	void setLoopSend(int state) { m_isLoopSend = (state == 2) ? true : false; }

private:
	static void receiveHandler(const DataBlock& db, void* user);
	static void timeOutHandler(void* user);

private:
	int m_fd;
	bool m_isLoopSend;
	DataBlock m_lastSendDb;
	ReceiveDataHandler m_receiveDataHandler;
};