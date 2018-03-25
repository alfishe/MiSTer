#include "systemmanager.h"

#include "../common/logger/logger.h"

#include <vector>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include "../3rdparty/tinyformat/tinyformat.h"

SystemManager& SystemManager::instance()
{
	static SystemManager instance;

	return instance;
}

uint32_t SystemManager::getBoardIPAddr()
{
	uint32_t result = 0x00000000;

	struct ifaddrs* ifaddr;
	struct ifaddrs* ifa;

	if (getifaddrs(&ifaddr) != -1)
	{
		for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
		{
			if (ifa->ifa_addr != nullptr)
			{
				string ifName(ifa->ifa_name);

				if (ifName == "eth0" && ifa->ifa_addr->sa_family == AF_INET)
				{
					result = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;

					// MSB to LSB conversion
					asm("rev %1,%0" : "=r" (result) : "r" (result));
				}
			}
		}

		freeifaddrs(ifaddr);
	}
	else
	{

	}

	return result;
}

string SystemManager::getBoardIP()
{
	string result;

	struct ifaddrs* ifaddr;
	struct ifaddrs* ifa;
	vector<char> host(NI_MAXHOST);

	if (getifaddrs(&ifaddr) != -1)
	{
		for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
		{
			if (ifa->ifa_addr != nullptr)
			{
				string ifName(ifa->ifa_name);

				if (ifName == "eth0" && ifa->ifa_addr->sa_family == AF_INET)
				{
					int res = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr), &host[0], host.size(), NULL, 0, NI_NUMERICHOST);

					if (res == 0)
					{
						result.assign(host.begin(), host.end());
						break;
					}
					else
					{
						string decoded;
						switch (res)
						{
							case EAI_AGAIN:
								decoded = "EAI_AGAIN";
								break;
							case EAI_BADFLAGS:
								decoded = "EAI_BADFLAGS";
								break;
							case EAI_FAIL:
								decoded = "EAI_FAIL";
								break;
							case EAI_FAMILY:
								decoded = "EAI_FAMILY";
								break;
							case EAI_MEMORY:
								decoded = "EAI_MEMORY";
								break;
							case EAI_NONAME:
								decoded = "EAI_NONAME";
								break;
							case EAI_OVERFLOW:
								decoded = "EAI_OVERFLOW0";
								break;
							case EAI_SYSTEM:
								decoded = tfm::format("EAI_SYSTEM %s", logger::geterror());
								break;
						}

						LOGERROR("%s: error occured while getting interface name. Code: 0x%x (%s)", __PRETTY_FUNCTION__, res, decoded.c_str());
					}
				}
			}
		}

		freeifaddrs(ifaddr);
	}
	else
	{
		LOGERROR("%s: Unable to enumerate network interfaces. %s", __PRETTY_FUNCTION__, logger::geterror());
	}

	return result;
}
