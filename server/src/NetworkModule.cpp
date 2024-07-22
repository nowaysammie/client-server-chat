#include "../include/NetworkModule.h"
// серверный NetworkModule

// инициализация сетевого модуля
uint8_t NetworkModule::init(char *server_ip)
{
	count_fds = 2;
	fds = new pollfd[2];
	// добавляем в отслеживамые поток ввода
	fds[0].fd = fileno(stdin);
	fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	// настройка протокола, ip-адреса и порта
	serverAddress.sin_family = AF_INET;
	int state = inet_pton(AF_INET, server_ip, &serverAddress.sin_addr.s_addr);
	if (state <= 0)
	{
		return E_CONNECT;
	}
	serverAddress.sin_port = htons(22280);
	// создание сокета
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		return E_CONNECT;
	}
	// разблокировка сокета
	fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	// добавляем в отслеживаемые слушающий сокет
	fds[1].fd = serverSocket;
	fds[1].events = POLLIN;
	fds[1].revents = 0;
	// привязка сокета к порту
	state = bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress));
	if (state < 0)
	{
		return E_CONNECT;
	}
	listen(serverSocket, 50);
	return SUCCESS;
}
// опросить дескрипторы
uint8_t NetworkModule::toPoll()
{
	// опрос сокетов неопределённое время
	int state = poll(fds, count_fds, -1);
	return state;
}
// добавить клиента в отслеживаемые
uint8_t NetworkModule::appendClient()
{
	int32_t client_socket = accept(serverSocket, NULL, NULL);
	if (client_socket < 0)
	{
		std::cout << "err append accept" << std::endl;
		close(client_socket);
		return E_CONNECT;
	}
	if (count_fds >= CONNECTED_USERS_MAX)
	{
		std::cout << "err append max users" << std::endl;
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
	temp[count_fds - 1].events = POLLIN;
	temp[count_fds - 1].revents = 0;
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
int NetworkModule::sendMessage(int32_t client_socket, const char *buffer)
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
	if (index < count_fds)
	{
		return (fds + index);
	}
	return NULL;
}
// получение UID пользователя по сокету
uint32_t NetworkModule::getClientUid(int32_t client_socket)
{
	ucred _ucred;
	unsigned int len = sizeof(_ucred);
	getsockopt(client_socket, SOL_SOCKET, SO_PEERCRED, &_ucred, &len);
	return _ucred.uid;
}

// получение сокета клиента по его uid
uint8_t NetworkModule::getClientSocket(uint32_t client_uid, int32_t *dest_socket)
{
	uint32_t temp_uid;
	for (int i = 0; i < count_fds; i++)
	{
		temp_uid = getClientUid(fds[i].fd);
		if (temp_uid == client_uid)
		{
			*dest_socket = temp_uid;
			return SUCCESS;
		}
	}
	return E_UID_WRONG;
}