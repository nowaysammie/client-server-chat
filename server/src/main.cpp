#include "Server.h"

int main(int argc, char* argv[]) { //аргументами функции указываются IPv4-адресс и порт
	//проверку на корректность ip и порта
	Server server;
	server.init(argv[1], argv[2]);
	uint8_t state = 0;
	while (state != SHUTDOWN) {
		server.toPoll();
		while (state != POLL_END || state != SHUTDOWN) {
			state = server.eventHandler();
		}
	}
	return 0;
}