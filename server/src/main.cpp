#include "Server.h"

int main(int argc, char *argv[])
{ // аргументами функции указываются IPv4-адресс
	if (argc < 2)
	{
		std::cout << "Enter IP address!" << std::endl;
		return 0;
	}
	Server server;
	server.init(argv[1]);
	uint8_t state = POLL_SUCCESS;
	while (state != SHUTDOWN)
	{
		state = server.toPoll();
		if (state == 0)
		{
			continue;
		}
		state = POLL_SUCCESS;
		while (state != POLL_END || state != SHUTDOWN)
		{
			state = server.eventHandler();
		}
	}
	return 0;
}