#include "Tools.h"
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

std::string getLocalIp()
{
	char ip[16];
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret == 0)
	{
		char hostname[256];
		ret = gethostname(hostname, sizeof(hostname));
		if (ret != SOCKET_ERROR)
		{
			HOSTENT* host = gethostbyname(hostname);
			if (host != NULL)
			{
				strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
			}
		}
	}
	WSACleanup();
	return std::string(ip);
}