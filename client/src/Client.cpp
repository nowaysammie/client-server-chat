#include "Client.h"

void Client::disconnect()
{
	network_module.closeSocket();
}

uint8_t Client::eventHandler(pollfd fd)
{
}

void Client::printStartScreen(char *login)
{

	ui.printWelcome();
	ui.askLogin();
}

pollfd *Client::toPoll()
{
	network_module.toPoll();
}

uint8_t Client::init(char *server_ip, uint16_t port)
{
	return network_module.init(server_ip, port);
}