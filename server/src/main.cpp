#include "Server.h"
#include <iostream>
#include "States.h"

int main(int argc, char *argv[])
{ // аргументами функции указываются IPv4-адресс
	if (argc < 2)
	{
		std::cout << "Enter IP address!" << std::endl;
	}
	else
	{
		Server server;
		uint8_t state = server.init(argv[1]);
		if (state != SRV_SHUTDOWN)
		{
			if (state != SRV_NO_VALID_IP)
			{
				std::cout << "Server starts..." << std::endl;
				uint8_t state = SRV_OK;
				while (state != SRV_SHUTDOWN)
				{
					state = server.toPoll();
					if (state == 0)
					{
						continue;
					}
					state = SRV_OK;
					while (state != SRV_HANDLE_END && state != SRV_SHUTDOWN)
					{
						state = server.eventHandler();
					}
				}
			}
			else
			{
				std::cout << "No valid IP-address!" << std::endl;
			}
		}
		else
		{
			std::cout << "Server shuts down!" << std::endl;
		}
	}
	return 0;
}