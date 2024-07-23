#include "Client.h"

#include <limits>
#include <unistd.h>

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

uint8_t Client::connectToServer() // ВОЗМОЖНО СДЕЛАТЬ VOID
{
	uint8_t status = network_module.connectToServer();
	if (status == SUCCESS)
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
	uint8_t state = E_WRONG_COMMAND;
	char buffer[BUFFER_SIZE];
	// ввод логина
	if (ui.input_mode == 0)
	{
		std::cin.getline(buffer, 50);
		Package package;
		package_manager.createAuthRequestPackage(&package, buffer);
		package_manager.transferToBuffer(package, buffer);
		state = network_module.sendMessage(buffer);

		if (state != SUCCESS)
		{
			ui.printState(state);
		}
		else
		{
			state = C_OK;
		}
	}
	else if (ui.input_mode == 1)
	{
		std::cin.getline(buffer, BUFFER_SIZE);

		if (strstr(buffer, "/help") != NULL)
		{
			// пользователь написал /help
			ui.displayHelp();
			ui.printInputMode();
			state = C_OK;
		}
		else if (strstr(buffer, "/list") != NULL)
		{
			Package package;
			package_manager.createUserListRequestPackage(&package, my_uid);
			package_manager.transferToBuffer(package, buffer);
			network_module.sendMessage(buffer);
			ui.input_mode = I_REQUEST;
			state = C_OK;
		}
		else if (strstr(buffer, "/leave"))
		{
			ui.printState(E_LEAVE_COMMAND);
			ui.printInputMode();
			state = C_OK;
		}
		else if (strstr(buffer, "/exit") != NULL)
		{
			state = C_SHUTDOWN;
			disconnect();
		}
		else if (strstr(buffer, "/select") != NULL)
		{
			string str(buffer);
			size_t startPos = str.find(" ") + 1;
			string friend_login = str.substr(startPos, str.size() - 1);
			uint32_t friend_uid;
			state = client_storage.getClientUid(friend_login, &friend_uid);
			ui.setFriend(friend_login.c_str(), friend_uid);
			if (state == SUCCESS)
			{
				ui.printSelectedUser();
				ui.printMissedMassege(client_storage.getMsg(ui.getFriendUid()));
				ui.input_mode = 2;
				ui.printInputMode();
				state = C_OK;
			}
			else
			{
				Package package;
				package_manager.createUserListRequestPackage(&package, my_uid);
				package_manager.transferToBuffer(package, buffer);
				network_module.sendMessage(buffer);
				ui.input_mode = I_SELECTED_NONAME; ///////
			}
		}
	}
	else if (ui.input_mode == 2) // это состояние обозначает что ты в чате с кем-то
	{
		std::cin.getline(buffer, BUFFER_SIZE);
		if (strstr(buffer, "/leave"))
		{
			ui.removeFriend();
			ui.input_mode = 1;
			ui.printInputMode();
			state = C_OK;
		}
		else
		{
			Package package;
			package_manager.createMsgPackage(&package, my_uid, ui.getFriendUid(), buffer);
			package_manager.transferToBuffer(package, buffer);
			state = network_module.sendMessage(buffer);
			if (state != SUCCESS)
			{
				ui.printState(state);
			}
			ui.printInputMode();
			state = C_OK;
		}
	}
	return state;
}

uint8_t Client::eventHandler()
{
	uint8_t state = C_OK;
	pollfd *ready_fd = network_module.readyFd();
	char buffer[BUFFER_SIZE];
	if (ready_fd == NULL)
	{
		state = C_HANDLE_END;
	}
	else if (ready_fd->fd == network_module.getFd(0)->fd)
	{
		ready_fd->revents = 0;
		// поток ввода
		state = handleUserInput();
		if (state == E_WRONG_COMMAND)
		{
			ui.printState(E_WRONG_COMMAND);
			ui.printInputMode();
		}
	}
	else
	{
		// событие на клиентском сокете
		ready_fd->revents = 0;
		Package package;
		state = network_module.getMessage(buffer);
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
	}
	return state;
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
	else if (ui.input_mode == I_SELECTED_NONAME)
	{
		char f_login[50];
		uint32_t f_uid;
		ui.getFriendLogin(f_login);
		client_storage.getClientUid(f_login, &f_uid);
		ui.setFriend(f_login, f_uid);
		ui.printSelectedUser();
		ui.printMissedMassege(client_storage.getMsg(ui.getFriendUid()));
		ui.input_mode = 2;
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