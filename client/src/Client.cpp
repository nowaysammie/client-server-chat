#include "../include/Client.h"

void Client::disconnect()
{
	network_module.closeSocket();
}
// обработка пользовательского ввода
uint8_t Client::handleUserInput()
{
	char buffer[BUFFER_SIZE];
	// ввод логина
	if (UI.input_mode == 0)
	{
		std::cin.getline(buffer, 50);
	}
	Package package;
	package_manager.createAuthRequest(&package, buffer);
	package_manager.transferToBuffer(package, buffer);
	int status = network_module.sendMessage(buffer);
	return POLL_SUCCESS;
	ui.input_mode = 1;
	//...
	// команда
	if (UI.input_mode == 1)
	{
		ui.printHint(0);
		ui, printInputMode();
		std::cin.getline(buffer, BUFFER_SIZE);
		if (strstr(buffer, "/help") != NULL)
		{ // пользователь написал /help
			UI.printHelp();
			return POLL_SUCCESS;
		}
		if (strstr(buffer, "/list"))
		{ // поьзователь написал команду /list
			client_storage.updateList();
			UI.displayList(client_storage.getList());
			ui.printHint(1);
			return POLL_SUCCESS;
		}
		if (strstr(buffer, "/select") != NULL)
		{
			string str = buffer;
			size_t startPos = str.find(" ") + 1;
			string friend_login = str.substr(startPos, str.size() - 1;)

									  uint32_t friend_uid;
			client_storage.getClientUid(friend_login, &friend_uid);
			ui.setFriend(friend_login, friend_uid);
			ui.printSelectedUser(friend_login);
			ui.input_mode = 2;
		}
		if (strstr(buffer, "/exit") != NULL)
		{
			this->disconnect();
		}
	}

	if (UI.input_mode == 2) // это состояние обозначает что ты в чате с кем-то
	{
		ui.printMissedMassege(client_storage.getMsg(), ui.friend_login);
		std::cin.getline(buffer, BUFFER_SIZE);
		ui.printMessage();
	}
}

uint8_t Client::eventHandler(pollfd fd)
{
	pollfd *ready_fd = network_module.readyFd();
	if (ready_fd == NULL)
	{
		return POLL_END;
	}
	int state;
	char *buffer[BUFFER_SIZE];

	ready_fd->revents = 0;
	// поток ввода
	if (ready_fd->fd == network_module.getFd(0)->fd)
	{
		state = handleUserInput();
	}
	state = network_module.getMessage(buffer);

	Package package;
	state = package_manager.parseToPackage(&package, buffer);
	switch (package.header.cmd)
	{
	case USER_LIST:
		client_storage.updateList(package.Data, (package.header.payload/sizeof(package.data.s_user_list.user[0]));
		break;
	case S_ERROR_MSG:

		break;
	}
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