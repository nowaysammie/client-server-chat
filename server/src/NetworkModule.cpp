#include "NetworkModule.h"

//серверный NetworkModule

uint8_t NetworkModule::init(uint32_t server_ip, uint16_t port) {
	//создание массива файловых дескрипторов, добавление ФД потока ввода в массив опрашиваемых ФД 
	count_fds = 1;
	fds = new pollfd[1];
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
	//привязка сокета к порту
	uint8_t state = bind(socket, (sockaddr*) &serverAddress, sizeof(serverAddress));
	if (state < 0) {
		return E_CONNECT;
	}
	return SUCCESS;
}

void NetworkModule::toPoll() {
	//опрос сокетов неопределённое время
	int state = poll(fds, count_fds, -1); 
}

uint8_t NetworkModule::appendClient(int32_t client_socket) {
	//создание расширенного массива опрашиваемых ФД
	pollfd* temp = new pollfd[++count_fds];
	for (int i = 0; i < (count_fds - 1); i++) {
		temp[i] = fds[i];
	}
	//добавление нового элемента
	temp[count_fds - 1].fd = client_socket;
	temp[count_fds - 1].events = POLLIN || POLLOUT; //на счёт POLLOUT поразмышлять
	//смена указателя на обновленный массив
	delete[] fds; 
	fds = temp;
	return SUCCESS; //добавить ошибки?
}

uint8_t NetworkModule::removeClient(int32_t client_socket) {
	//создание уменьшенного массива опрашиваемых ФД 
	pollfd* temp = new pollfd[--count_fds];
	for (int i = 0; i < (count_fds + 1); i++) {
		if (temp[i].fd != client_socket) {
			temp[i] = fds[i];
		}
	}
	//смена указателя на обновленный массив
	delete[] fds; 
	fds = temp;
	return SUCCESS; //добавить ошибки?
}

uint8_t NetworkModule::getMessage(int32_t client_socket, char* buffer) {
	//получаем сообщение от клиента
	int state = recv(client_socket, buffer, BUFFER_SIZE, 0);
	if (state <= 0) {
		return E_CONNECT;
	}
	return SUCCESS;
}

uint8_t NetworkModule::sendMessage(int32_t client_socket, const char* buffer) {
	//отправляем сообщение от клиенту
	int state = send(client_socket, buffer, BUFFER_SIZE, 0);
	if (state <= 0) {
		return E_CONNECT;
	}
	return SUCCESS;
}

void NetworkModule::closeSocket() {
	//закрытие сокета сервера
	close(socket);
}
