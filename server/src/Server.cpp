#include "Server.h"
#include "../include/States.h"
#include <regex>
#include <iostream>

bool Server::testIp(std::string ip)
{
	std::regex ipv4("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])");
	bool state = false;
	// Checking if it is a valid IPv4 addresses
	if (std::regex_match(ip, ipv4))
	{
		state = true;
	}
	return state;
}

uint8_t Server::init(char *server_ip)
{ // запуск сервера, бинд к порту
	uint8_t state = SRV_NO_VALID_IP;
	std::string temp = server_ip;
	if (testIp(temp))
	{
		state = network_module.init(server_ip);
		if (state == E_CONNECT)
		{
			shutdown();
			state = SRV_SHUTDOWN;
		}
	}
	return state;
}

uint8_t Server::toPoll()
{ // запускает network_module.toPoll()
	return network_module.toPoll();
}

void Server::authorization(Package package, int32_t client_socket)
{
	if (package.header.payload == LOGIN_SIZE)
	{
		char login[LOGIN_SIZE];
		std::regex login_regex("^[a-zA-Z0-9_]{5,50}$");
		strncpy(login, package.data.s_auth_request.login, LOGIN_SIZE);
		if (std::regex_match(login, login_regex) != true)
		{
			sendErrorPackage(client_socket, E_LOGIN_SIZE);
		}
		else
		{
			uint32_t client_uid = network_module.getClientUid(std::string(package.data.s_auth_request.login));
			uint8_t state = storage.appendClient(client_uid, login, client_socket);

			Package reply_package;
			char buffer[BUFFER_SIZE];
			package_manager.createAuthConfirmPackage(&reply_package, client_uid);
			package_manager.transferToBuffer(reply_package, buffer);
			state = network_module.sendMessage(client_socket, buffer);
			if (state != SUCCESS)
			{
				// пользователь отключился, убираем его из списка отслеживаемых
				network_module.removeClient(client_socket);
			}
		}
	}
	else
	{
		sendErrorPackage(client_socket, E_LOGIN_SIZE);
	}
}

void Server::sendErrorPackage(int32_t client_socket, uint8_t error_code)
{
	Package package;
	char buffer[BUFFER_SIZE];
	package_manager.createErrorPackage(&package, error_code);
	package_manager.transferToBuffer(package, buffer);
	uint8_t state = network_module.sendMessage(client_socket, buffer);
	// если не удалось отправить пакет
	if (state != SUCCESS)
	{
		// пользователь отключился, убираем его из списка отслеживаемых
		network_module.removeClient(client_socket);
	}
}

uint8_t Server::instruction()
{ // запускает выход c ввода админа
	char buffer[BUFFER_SIZE];
	std::cin.getline(buffer, BUFFER_SIZE);
	uint8_t state = SRV_OK;
	if (strstr(buffer, "/shutdown") != NULL)
	{
		state = SRV_SHUTDOWN;
	}
	return state;
}

void Server::sendUserList(Package package, int32_t client_socket)
{
	if (package.header.payload <= PAYLOAD_MAX)
	{
		Package p_user_list;
		std::map<std::string, uint32_t> u_list = storage.getUserList(package.data.s_user_list_request.client_uid);
		char buffer[BUFFER_SIZE];
		package_manager.createUserListPackage(&p_user_list, u_list);
		package_manager.transferToBuffer(p_user_list, buffer);
		uint8_t state = network_module.sendMessage(client_socket, buffer);
		if (state != SUCCESS)
		{
			// пользователь отключился, убираем его из списка отслеживаемых
			network_module.removeClient(client_socket);
		}
	}
	else
	{
		sendErrorPackage(client_socket, E_DATA);
	}
}

void Server::forwardMsg(Package package, uint32_t client_socket, char *buffer)
{
	if (package.data.s_msg.src_uid != package.data.s_msg.dest_uid)
	{
		if (package.header.payload == MSG_SIZE)
		{
			uint8_t state = storage.appendFriend(package.data.s_msg.src_uid, package.data.s_msg.dest_uid);
			if (state != E_FRIEND_WRONG)
			{
				int32_t dest_socket;
				state = storage.getClientSocket(package.data.s_msg.dest_uid, &dest_socket);
				if (state != E_FRIEND_WRONG)
				{
					state = network_module.sendMessage(dest_socket, buffer);
					if (state == E_CONNECT)
					{
						// network_module.removeClient(dest_socket);
					}
				}
				else
				{
					sendErrorPackage(client_socket, E_FRIEND_WRONG);
				}
			}
			else
			{
				sendErrorPackage(client_socket, E_FRIEND_WRONG);
			}
		}
		else
		{
			sendErrorPackage(client_socket, E_DATA);
		}
	}
	else
	{
		sendErrorPackage(client_socket, E_SELF_MSG);
	}
}

uint8_t Server::eventHandler()
{ // делает reevent=0, обрабатывает событие, взаимодействует с PackageManager и Storage, возвращает статус
	// получаем указатель на готовый pollfd
	pollfd *ready_fd = network_module.readyFd();
	uint8_t state;
	if (ready_fd != NULL)
	{
		// необходимые переменные состояния и буфера
		char buffer[BUFFER_SIZE];
		// помечаем event как обработанный
		ready_fd->revents = 0;
		// если сработал event потока ввода
		if (ready_fd->fd == network_module.getFd(0)->fd)
		{
			// запускаем метод обработки введённой на сервере инструкции
			state = instruction();
			if (state == SRV_SHUTDOWN)
			{
				shutdown();
			}
			else
			{
				std::cout << "Wrong instruction!" << std::endl;
				state = SRV_OK;
			}
		}
		// если сработал слушающий сокет
		else if (ready_fd->fd == network_module.getFd(1)->fd)
		{
			// подключаем клиента и добавляем в список отслеживаемых
			state = network_module.appendClient();
			if (state != SUCCESS)
			{
				state = SRV_POLL_E_CONNECT;
			}
			else
			{
				state = SRV_OK;
			}
		}
		else
		{
			// если это сообщение от клиента, то принимаем его
			state = network_module.getMessage(ready_fd->fd, buffer);
			if (state == E_DATA)
			{
				sendErrorPackage(ready_fd->events, E_DATA);
			}
			else if (state == E_CONNECT) // здесь будет отключение
			{
				network_module.removeClient(ready_fd->fd);
			}
			else
			{
				// создаём пакет, в который будем записывать присланный буфер
				Package package;
				// парсим буфер в пакет
				state = package_manager.parseToPackage(&package, buffer);
				// если не удалость спарсить
				if (state != SUCCESS)
				{
					// отправляем пакет с ошибкой клиенту
					sendErrorPackage(ready_fd->fd, E_DATA);
				}
				else
				{
					// если спарсить удалось - проверяем код команды
					switch (package.header.cmd)
					{
					case AUTH_REQUEST:
						authorization(package, ready_fd->fd);
						break;
					case USER_LIST_REQUEST:
						sendUserList(package, ready_fd->fd);
						break;
					case MSG:
						forwardMsg(package, ready_fd->fd, buffer);
						break;
					default:
						sendErrorPackage(ready_fd->fd, E_DATA);
					}
				}
				state = SRV_OK;
			}
		}
	}
	else
	{
		state = SRV_HANDLE_END;
	}
	return state;
}

void Server::shutdown()
{
	network_module.closeSocket();
}
