#include"Client.h"

void Client::disconnect()
{
	network_module.clientSocket();
}


uint8_t Client::eventHandler(pollfd fd)
{

}


void Client::printStartScreen(char* login)
{

	ui.printWelcome();
	ui.askLogin(); 
}


pollfd* Client::toPoll()
{
	network_module.toPoll();
}


uint8_t Client::init(uint32_t server_ip, uint16_t port)
{
	return network_module.init(server_ip, port);

}