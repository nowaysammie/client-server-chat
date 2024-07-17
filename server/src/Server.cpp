#include "Server.h"

uint8_t Server::init(uint32_t server_ip, uint16_t port)
{ // запуск сервера, бинд к порту
	return network_module.init(server_ip, port);
}

void Server::toPoll()
{ // запускает network_module.toPoll()
	network_module.toPoll();
}

uint8_t Server::authorization(Package package, int32_t client_socket)
{
	if (package.header.payload == LOGIN_SIZE_MAX)
	{
		char login[50];
		strncpy(login, package.data.s_auth_request.login, LOGIN_SIZE_MAX);
		uint32_t client_uid = getClientUid();
		state = storage.appendClient(client_uid, login);
		if (state == E_LOGIN_BUSY) {
			sendErrorPackage(client_socket, E_LOGIN_BUSY);
			return POLL_SUCCESS;
		}
		Package reply_package;
		package_manager.createAuthConfirmPackage(&reply_package, client_uid);
		char buffer[BUFFER_SIZE];
		package_manager.transferToBuffer(reply_package, buffer);
		state = send(client_socket, buffer, BUFFER_SIZE, 0);
		if (state <= 0)
		{
			// пользователь отключился, убираем его из списка отслеживаемых
			network_module.removeClient(client_socket);
		}
	}
	else
	{
		sendErrorPackage(client_socket, E_LOGIN_SIZE);
	}
	return POLL_SUCCESS;
}

void Server::sendErrorPackage(int32_t client_socket, uint8_t error_code)
{
	Package package;
	char buffer[BUFFER_SIZE];
	package_manager.createErrorPackage(&package, error_code);
	int state = network_module.sendMessage(client_socket, buffer);
	// если не удалось отправить пакет с ошибкой
	if (state <= 0)
	{
		// пользователь отключился, убираем его из списка отслеживаемых
		network_module.removeClient(client_socket);
	}
}

uint8_t Server::instruction()
{ // запускает выход и может ещё что-то
	// считываем ввод
	char buffer[BUFFER_SIZE];
	std::cin.getline(buffer, BUFFER_SIZE);
	if (strstr(buffer, "/shutdown") != NULL)
	{
		return SHUTDOWN;
	}
	return WRONG_INSTRUCTION;
}

void Server::sendUserList(Package package, int32_t client_socket)
{
	if (package.header.payload <= PAYLOAD_MAX)
	{
		Package p_user_list;
		std::map<std::string, uint32_t> u_list = storage.getUserList(uint32_t client_socket); // getUserList возращает список пользователей без указанного
		package_manager.createUserListPackage(&p_user_list, u_list);
		char buffer[BUFFER_SIZE];
		package_manager.transferToBuffer(p_user_list, buffer);
		int state = send(client_socket, buffer, BUFFER_SIZE, 0);
		if (state <= 0)
		{
			// пользователь отключился, убираем его из списка отслеживаемых
			network_module.removeClient(client_socket);
		}
	}
	else
	{
		sendErrorPackage(client_socket, E_DATA);
	}
	return;
}

uint8_t Server::forwardMsg(Package package, uint32_t client_socket, char* buffer) {
	if (package.data.s_msg.src_uid == package.data.s_msg.dest_uid) {
		sendErrorPackage(client_socket, E_SELF_MSG);
		return POLL_SUCCESS;
	}
	if (package.header.payload != PAYLOAD_MAX) {
		sendErrorPackage(client_socket, E_DATA);
		return POLL_SUCCESS;
	}
	int status = storage.appendFriend(package.data.s_msg.src_uid, package.data.s_msg.dest_uid);
	if (status == E_FRIEND_WRONG) {
		sendErrorPackage(client_socket, E_FRIEND_WRONG);
		return POLL_SUCCESS;
	}
	int32_t dest_socket;
	int status = network_module.getClientSocket(package.data.s_msg.dest_uid, &dest_socket);
	if (status == E_FRIEND_WRONG) {
		sendErrorPackage(client_socket, E_FRIEND_WRONG);
		return POLL_SUCCESS;
	}
	status = sendMessage(dest_socket, buffer);
	if (status == E_CONNECT) {
		network_module.removeClient(dest_socket);
		return POLL_SUCCESS;
	}
	return POLL_SUCCESS;
}

uint32_t getClientUid() {
	//воспользоваться getsockopt	
}

uint8_t Server::eventHandler()
{ // делает reevent=0, обрабатывает событие, взаимодействует с PackageManager и Storage, возвращает статус
	// получаем указатель на готовый pollfd
	pollfd *ready_fd = network_module.readyFd();
	if (ready_fd == NULL)
	{
	// если их нет, то возвращаем POLL_END
		return POLL_END;
	}
	// необходимые переменные состояния и буфера
	int state;
	char buffer[BUFFER_SIZE];
	// помечаем event как обработанный
	ready_fd->revents = 0;
	// если сработал event потока ввода
	if (ready_fd->fd == network_module.getFd(0)->fd)
	{
		// запускаем метод обработки введённой на сервере инструкции
		state = instruction();
		if (state == SHUTDOWN)
		{
			shutdown();
			return state;
		}
		std::cout << "Wrong instruction!" << std::endl;
		return POLL_SUCCESS;
	}
	// если сработал слушающий сокет
	if (ready_fd->fd == network_module.getFd(1)->fd)
	{
		// подключаем клиента и добавляем в список отслеживаемых
		state = network_module.appendClient();
		if (state != SUCCESS)
		{
			return POLL_E_CONNECT;//??
		}
		return POLL_SUCCESS;
	}
	// если это сообщение от клиента, то принимаем его
	state = recv(ready_fd->fd, buffer, sizeof(buffer), 0);
	if (state < 0)
	{
		sendErrorPackage(ready_fd->events, E_DATA);
		return POLL_SUCCESS;
	}
	if else (state == 0) { //здесь будет отключение
		network_module.removeClient(ready_fd->fd);
	}
	// создаём пакет, в который будем записывать присланный буфер
	Package package;
	// парсим буфер в пакет
	state = package_manager.parseToPackage(&package, buffer);
	// если не удалость спарсить
	if (state != SUCCESS)
	{
		// отправляем пакет с ошибкой клиенту
		sendErrorPackage(ready_fd->fd, E_DATA);
		return POLL_SUCCESS;
	}

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
		forwardMsg(package, ready_fd->fd, buffer);//добавить отправителя в отслеживаемые(Storage.appendFriend), проверить на ошибки, отправить получателю
		break;
	default:
		sendErrorPackage(ready_fd->fd, E_DATA);
	}
	return POLL_SUCCESS;
	//отключение клиента

	return SUCCESS;
}
