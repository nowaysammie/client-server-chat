#include "../include/Server.h"

int main(int argc, char *argv[])
{ // аргументами функции указываются IPv4-адресс
	if (argc < 2)
	{
		std::cout << "Enter IP address!" << std::endl;
		return 0;
	}
	Server server;
	std::cout << (int)server.init(argv[1]) << std::endl;
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
			std::cout << "event" << std::endl;
			state = server.eventHandler();
		}
	}
	return 0;
}