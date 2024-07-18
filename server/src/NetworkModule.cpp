#include "NetworkModule.h"
// серверный NetworkModule

// инициализация сетевого модуля
uint8_t NetworkModule::init(uint32_t server_ip, uint16_t port)
{
	count_fds = 1;
	fds = new pollfd[2];
	// добавляем в отслеживамые поток ввода
	fds[0].fd = fileno(stdin);
	fds[0].events = POLLIN;
	// настройка протокола, ip-адреса и порта
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr;		  // server_ip внести
	serverAddress.sin_port = htons(port); // тестить
	// создание сокета
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		return E_CONNECT;
	}
	// добавляем в отслеживаемые слушающий сокет
	fds[1].fd = serverSocket;
	fds[1].events = POLLIN;
	// разблокировка сокета
	fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	// привязка сокета к порту
	uint8_t state = bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress));
	if (state < 0)
	{
		return E_CONNECT;
	}
	return SUCCESS;
}
// опросить дескрипторы
void NetworkModule::toPoll()
{
	// опрос сокетов неопределённое время
	int state = poll(fds, count_fds, -1);
	// ОБРАБОТАТЬ state!
}
// добавить клиента в отслеживаемые
uint8_t NetworkModule::appendClient()
{
	int32_t client_socket = accept(serverSocket, NULL, NULL);
	if (client_socket < 0)
	{
		return E_CONNECT;
	}
	if (count_fds >= CONNECTED_USERS_MAX)
	{
		close(client_socket);
		return E_SRV_FULL;
	}
	// создание расширенного массива опрашиваемых ФД
	pollfd *temp = new pollfd[++count_fds];
	for (int i = 0; i < (count_fds - 1); i++)
	{
		temp[i] = fds[i];
	}
	// добавление нового элемента
	temp[count_fds - 1].fd = client_socket;
	temp[count_fds - 1].events = POLLIN || POLLOUT; // на счёт POLLOUT поразмышлять
	// смена указателя на обновленный массив
	delete[] fds;
	fds = temp;
	return SUCCESS; // добавить ошибки?
}
// убрать клиента из отслеживаемых и закрыть его сокет
uint8_t NetworkModule::removeClient(int32_t client_socket)
{
	// создание уменьшенного массива опрашиваемых ФД
	pollfd *temp = new pollfd[--count_fds];
	for (int i = 0; i < (count_fds + 1); i++)
	{
		if (temp[i].fd != client_socket)
		{
			temp[i] = fds[i];
		}
	}
	// смена указателя на обновленный массив
	delete[] fds;
	fds = temp;
	close(client_socket);
	return SUCCESS; // добавить ошибки?
}
// получить буфер
uint8_t NetworkModule::getMessage(int32_t client_socket, char *buffer)
{
	// получаем сообщение от клиента
	int state = recv(client_socket, buffer, BUFFER_SIZE, 0);
	if (state <= 0)
	{
		return E_CONNECT;
	}
	return SUCCESS;
}
// отправить буфер
uint8_t NetworkModule::sendMessage(int32_t client_socket, const char *buffer)
{
	// отправляем сообщение клиенту
	int state = send(client_socket, buffer, BUFFER_SIZE, 0);
	if (state <= 0)
	{
		return E_CONNECT;
	}
	return SUCCESS;
}
// закрытие сокета сервера
void NetworkModule::closeSocket()
{
	// закрытие сокета сервера
	close(serverSocket);
}
// вернуть указатель на pollfd, на котором случился event
pollfd *NetworkModule::readyFd()
{
	for (int i = 0; i < count_fds; i++)
	{
		if (fds[i].revents & POLLIN)
		{
			return (fds + i);
		}
	}
	return NULL;
}
// получение указателя на fds[index]
pollfd *NetworkModule::getFd(unsigned int index)
{
	return (fds + index);
}
// получение сокета клиента по его uid
uint8_t NetworkModule::getClientSocket(uint32_t client_uid, int32_t *dest_socket)
{
}
