#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H
//серверный NetworkModule
class NetworkModule {
private:
	uint32_t socket; //сокет сервера
	sockaddr_in serverAdress; //адрес сервера
	nfds_t count_clients; //количество элементов в fds
	pollfd* clients; //массив структур
public:
	NetworkModule(uint32_t server_ip, uint16_t port); //происходит инициализация, создаётся сокет, сокет биндится к порту
	pollfd* toPoll(); //возвращает массив готовых файловых дескрипторов
	uint8_t appendClient(uint32_t client_socket, uint16_t event); //добавляем клиента в отслеживаемый массив !узнать тип event
	uint8_t removeClient(); //убрать клиента из списка отслеживаемых
	uint8_t getMessage(uint32_t client_socket, char* buffer); //получить сообщение
	uint8_t sendMessage(uint32_t client_socket, char* buffer); //отправить сообщение
	void closeSocket(); //закрыть сокет сервера
};

#endif