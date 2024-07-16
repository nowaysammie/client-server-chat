#include "NetworkModule.h"

//клиентский NetworkModule

uint8_t NetworkModule::init(uint32_t server_ip, uint16_t port) {
	//создание массива файловых дескрипторов, добавление ФД потока ввода в массив опрашиваемых ФД 
	count_fds = 2;
	fds = new pollfd[2];
	fds[0].fd = fileno(stdin);
	fds[0].events = POLLIN;
	//настройка протокола, ip-адреса и порта
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = server_ip; //inet_pton и тд.
	serverAddress.sin_port = htons(port); //работает?
	//создание сокета
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		return E_CONNECT;
	}
	//разблокировка сокета
	fcntl(socket, F_SETFL, O_NONBLOCK);
	return SUCCESS;
}

uint8_t NetworkModule::connectToServer() {
	//попытка присоединиться к серверу
	int state = connect(serverAddress, (sockaddr*) &serverAddress, sizeof(serverAddress));
	if (state < 0) {
		return E_CONNECT;
	}
	fds[1].fd = socket;
	fds[2].events = POLLIN;
	return SUCCESS;
}

void NetworkModule::toPoll() {
	//опрос сокетов неопределённое время
	int state = poll(fds, count_fds, -1); 
}

uint8_t NetworkModule::getMessage(char* buffer) {
	//получаем сообщение от клиента
	int state = recv(socket, buffer, BUFFER_SIZE, 0);
	if (state <= 0) {
		return E_CONNECT;
	}
	return SUCCESS;
}

uint8_t NetworkModule::sendMessage(const char* buffer) {
	//отправляем сообщение от клиенту
	int state = send(socket, buffer, BUFFER_SIZE, 0);
	if (state <= 0) {
		return E_CONNECT;
	}
	return SUCCESS;
}

void NetworkModule::closeSocket() {
	//закрытие сокета клиента
	close(socket);
}
