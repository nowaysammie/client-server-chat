#include "../include/Client.h"
// инициализация клиента
uint8_t Client::init(char *server_ip)
{
	return network_module.init(server_ip);
}
// опрос потока ввода и сокета
uint8_t Client::toPoll()
{
	return network_module.toPoll();
}
// печать приветственного окна и запрос логина
void Client::printStartScreen(char *login)
{
	ui.printWelcome();
	ui.askLogin(); // проверить ставится ли input_mode на LOGIN_IN
}

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
	//...
	// команда
	if (UI.input_mode == 1)
	{
		ui.printHint(0);
		ui.printInputMode();
		std::cin.getline(buffer, BUFFER_SIZE);
		if (strstr(buffer, "/help") != NULL)
		{ // пользователь написал /help
			UI.printHelp();
			return POLL_SUCCESS;
		}
		if (strstr(buffer, "/list"))
		{ // поьзователь написал команду /list
			Package package;
			package_manager.createUserListRequestPackage(&package, my_uid);
			package_manager.transferToBuffer(&package, buffer);
			network_module.sendMessage(buffer);
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
			uint8_t state = client_storage.getClientUid(friend_login, &friend_uid);
			if(state == SUCCESS)
			{
				ui.input_mode = 2;
				return POLL_SUCCESS;
			}
			return state; // поменть в сторедже
			
		}
		if (strstr(buffer, "/exit") != NULL)
		{
			this->disconnect();
		}
	}

	if (UI.input_mode == 2) // это состояние обозначает что ты в чате с кем-то
	{
		ui.setFriend(friend_login, friend_uid);
		ui.printSelectedUser(friend_login);
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
		updateUserList();
		break;
	case MSG:
		handleMessage();
		break;
	case ERROR_MSG:
		errorList();
		break;
	case AUTH_CONFIRM:
		authConfirm();
		break;
	}

	

}
void updateUserList()
{
	client_storage.updateList(package.Data, (package.header.payload/sizeof(package.data.s_user_list.user[0]));
}
void handleMessage()
{
	if(ui.getFriendUid() == package.data.s_msg.src_uid)
		{
			ui.printMessage(ui.friend_login, package.data.s_msg.message);
		}
		else
		{
			client_storage.appendMsg(package.data.s_msg.src_uid, package.data.s_msg.message);
		}
}
void errorList()
{
	ui.printState(package.data.s_error_msg.error_code);
}
void authConfirm()
{
	my_uid = package.data.s_auth_confirm.client_uid;
	ui.input_mode = 1;
}