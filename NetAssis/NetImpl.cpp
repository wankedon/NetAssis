#include "NetImpl.h"
#include <vector>

NetImpl::NetImpl(const ReceiveDataHandler& receiveDataHandler)
	:m_receiveDataHandler(receiveDataHandler),
	m_fd(0),
	m_isLoopSend(false),
	m_lastSendDb(DataBlock{nullptr,0})
{
	InitLib();
}

NetImpl::~NetImpl()
{
	DeInitLib();
}

void NetImpl::creatTransport(const WorkMode workMode)
{
	TransportCallBacks trcbs;
	trcbs.receive = receiveHandler;
	trcbs.timeout = timeOutHandler;
	trcbs.owner = this;
	CreateTransport(&m_fd, workMode, trcbs);
}

void NetImpl::receiveHandler(const DataBlock& db, void* user)
{
	NetImpl* self = (NetImpl*)user;
	if (self->m_receiveDataHandler)
		self->m_receiveDataHandler(db, user);
};

void NetImpl::timeOutHandler(void* user)
{
	NetImpl* self = (NetImpl*)user;
	if (self->m_isLoopSend)
		Send(self->m_fd, self->m_lastSendDb);
};

void NetImpl::setDestEndpoint(const char* destEndpoint)
{
	SetDestEndpoint(m_fd, destEndpoint);
}

void NetImpl::setLocalPort(const unsigned short localPort)
{
	SetLocalPort(m_fd, localPort);
}

void NetImpl::setReceiveBufferSize(const size_t length)
{
	SetReceiveBufferSize(m_fd, length);
}

void NetImpl::start()
{
	Start(m_fd);
}

void NetImpl::send(const DataBlock& db)
{
	Send(m_fd, db);
	m_lastSendDb.addr= db.addr;
	m_lastSendDb.length = db.length;
}

void NetImpl::startTimer(const uint64_t timeInterval)
{
	StartTimer(m_fd, timeInterval);
}

void NetImpl::close()
{
	CloseTransport(m_fd);
}