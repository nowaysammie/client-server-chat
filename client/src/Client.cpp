#include "../include/Client.h"

#include <limits>

void Client::clearInputBuffer()
{
	struct termios cooked, raw;
	char garbage[BUFFER_SIZE];
	network_module.unsetPollCin();
	tcgetattr(STDIN_FILENO, &cooked);
	raw = cooked;
	raw.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &raw);
	std::cin.getline(garbage, BUFFER_SIZE);
	std::cin.clear();
	tcsetattr(STDIN_FILENO, TCSANOW, &cooked);

	network_module.setPollCin();
}

// инициализация клиента
uint8_t Client::init(char *server_ip)
{
	return network_module.init(server_ip);
}
// опрос потока ввода и сокета
int8_t Client::toPoll()
{
	return network_module.toPoll();
}
// печать приветственного окна и запрос логина
void Client::printStartScreen()
{
	ui.printWelcome();
	ui.askLogin();
}

uint8_t Client::connectToServer()
{
	uint8_t state = network_module.connectToServer();
	if (state == SUCCESS)
	{
		return C_CONNECTED;
	}
	return C_DISCONNECTED;
}

void Client::disconnect()
{
	network_module.closeSocket();
}
// обработка пользовательского ввода
uint8_t Client::handleUserInput()
{
	cout << "userinput" << endl;
	char buffer[BUFFER_SIZE];
	// ввод логина
	if (ui.input_mode == 0)
	{
		std::cin.getline(buffer, 50);
		Package package;
		package_manager.createAuthRequestPackage(&package, buffer);
		package_manager.transferToBuffer(package, buffer);
		int status = network_module.sendMessage(buffer);
		if (status != SUCCESS)
		{
			ui.printState(status);
		}
		return C_OK;
	}
	if (ui.input_mode == 1)
	{
		std::cin.getline(buffer, BUFFER_SIZE);
		if (strstr(buffer, "/help") != NULL)
		{ // пользователь написал /help
			ui.displayHelp();
			ui.printInputMode();
			return C_OK;
		}
		else if (strstr(buffer, "/list"))
		{ // поьзователь написал команду /list
			Package package;
			package_manager.createUserListRequestPackage(&package, my_uid);
			package_manager.transferToBuffer(package, buffer);
			network_module.sendMessage(buffer);
			ui.input_mode = I_REQUEST;
			return C_OK;
		}
		else if (strstr(buffer, "/select") != NULL)
		{
			string str(buffer);
			size_t startPos = str.find(" ") + 1;
			string friend_login = str.substr(startPos, str.size() - 1);
			uint32_t friend_uid;
			uint8_t state = client_storage.getClientUid(friend_login, &friend_uid);
			if (state == SUCCESS)
			{
				ui.setFriend(friend_login.c_str(), friend_uid);
				ui.printSelectedUser();
				ui.printMissedMassege(client_storage.getMsg(ui.getFriendUid()));
				ui.input_mode = 2;
				ui.printInputMode();
				return C_OK;
			}
			return state; // поменть в сторедже
		}
		else if (strstr(buffer, "/exit") != NULL)
		{
			disconnect();
			return C_SHUTDOWN;
		}
	}

	if (ui.input_mode == 2) // это состояние обозначает что ты в чате с кем-то
	{
		if (strstr(buffer, "/leave"))
		{
			ui.removeFriend();
			ui.input_mode = 1;
			ui.printInputMode();
			return C_OK;
		}

		std::cin.getline(buffer, BUFFER_SIZE);
		Package package;
		package_manager.createMsgPackage(&package, my_uid, ui.getFriendUid(), buffer);
		package_manager.transferToBuffer(package, buffer);
		int state = network_module.sendMessage(buffer);
		if (state != SUCCESS)
		{
			ui.printState(state);
		}
		ui.printInputMode();
		return C_OK;
	}
	return E_WRONG_COMMAND;
}

uint8_t Client::eventHandler()
{
	pollfd *ready_fd = network_module.readyFd();
	if (ready_fd == NULL)
	{
		return C_HANDLE_END;
	}
	int state;
	char buffer[BUFFER_SIZE];

	ready_fd->revents = 0;
	// поток ввода
	if (ready_fd->fd == network_module.getFd(0)->fd)
	{
		state = handleUserInput();
		if (state == E_WRONG_COMMAND)
		{
			ui.printState(E_WRONG_COMMAND);
			ui.printInputMode();
		}
		else if (state == C_SHUTDOWN)
		{
			return state;
		}
		return C_OK;
	}
	// событие на клиентском сокете
	state = network_module.getMessage(buffer);
	Package package;
	state = package_manager.parseToPackage(&package, buffer);

	switch (package.header.cmd)
	{
	case USER_LIST:
		updateUserList(package);
		break;
	case MSG:
		handleMessage(package);
		break;
	case ERROR_MSG:
		errorHandler(package);
		break;
	case AUTH_CONFIRM:
		authConfirm(package);
		break;
	}
	return C_OK;
}

void Client::updateUserList(Package package)
{
	client_storage.updateList(package, (uint16_t)(package.header.payload / sizeof(package.data.s_user_list.user[0])));
	if (ui.input_mode == I_REQUEST)
	{
		ui.displayList(client_storage.getList());
		ui.printHint(H_SELECT);
		ui.input_mode = 1;
		ui.printInputMode();
	}
}
void Client::handleMessage(Package package)
{
	if (ui.getFriendUid() == package.data.s_msg.src_uid)
	{
		clearInputBuffer();
		ui.printMessage(package.data.s_msg.message);
		ui.printInputMode();
	}
	else
	{
		int state = client_storage.appendMsg(package.data.s_msg.src_uid, package.data.s_msg.message);
	}
}

void Client::errorHandler(Package package)
{
	ui.printState(package.data.s_error_msg.error_code);
}

void Client::authConfirm(Package package)
{
	my_uid = package.data.s_auth_confirm.client_uid;
	ui.input_mode = 1;
	ui.displayHelp();
	ui.printInputMode();
}