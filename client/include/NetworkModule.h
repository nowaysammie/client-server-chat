#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H
//клиентский NetworkModule
class NetworkModule {
private:
	uint32_t socket; //сокет клиента
	sockaddr_in serverAdress; //адрес сервера
public:
	NetworkModule(uint32_t server_ip, uint16_t port); //происходит инициализация, создаётся сокет, пытается соединиться с сервером
	pollfd* toPoll();
	uint8_t connectToServer();
	uint8_t getMessage(char* buffer); //получить сообщение
	uint8_t sendMessage(char* buffer); //отправить сообщение
	void closeSocket(); //закрыть сокет сервера
};

#endif