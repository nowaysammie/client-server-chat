#include "Client.h"
#include <termios.h>
#include <limits>
#include <unistd.h>
#include <regex>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

void Client::addToCfg(const char *text) // добавить вызов этого в main, сделать inet_ntop на сервере
{
	std::ofstream cfg;
	cfg.open("../data/client.cfg", std::ios::out | std::ios::app);
	cfg << text << std::endl;
	cfg.close();
}

uint8_t Client::countCfgLines()
{
	uint8_t line_count = 0;
	std::string line;
	std::ifstream cfg("../data/client.cfg");
	while (std::getline(cfg, line))
	{
		line_count++;
	}
	cfg.close();
	return line_count;
}

void Client::getLineFromCfg(int line, char *dest)
{
	std::ifstream file("../data/client.cfg");

	std::string temp;
	int currentLine = 0;

	while (std::getline(file, temp))
	{
		if (currentLine == line)
		{
			strncpy(dest, temp.c_str(), 20);
			break;
		}
		currentLine++;
	}
	file.close();
}

bool Client::testIp(std::string ip)
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
	uint8_t state = C_NO_VALID_IP;
	std::string temp = server_ip;
	ui.removeFriend();
	if (testIp(temp))
	{
		state = network_module.init(server_ip);
		if (state == E_CONNECT)
		{
			disconnect();
			state = C_SHUTDOWN;
		}
	}
	if (countCfgLines() == 0)
	{
		if (countCfgLines() == 1)
		{
			std::ofstream cfg("../data/client.cfg", std::ios::out | std::ios::trunc);
			cfg.close();
		}
		addToCfg(server_ip);
	}
	return state;
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
	if (ui.input_mode == I_LOGIN)
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
	else if (ui.input_mode == I_CMD)
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
			std::string str(buffer);
			size_t startPos = str.find(" ") + 1;
			std::string friend_login = str.substr(startPos, str.size() - 1);
			uint32_t friend_uid;
			state = client_storage.getClientUid(friend_login, &friend_uid);
			ui.setFriend(friend_login.c_str(), friend_uid);
			if (state == SUCCESS)
			{
				ui.printSelectedUser();
				ui.printMissedMassege(client_storage.getMsg(ui.getFriendUid()));
				ui.input_mode = I_MSG;
				ui.printInputMode();
				state = C_OK;
			}
			else
			{
				Package package;
				package_manager.createUserListRequestPackage(&package, my_uid);
				package_manager.transferToBuffer(package, buffer);
				network_module.sendMessage(buffer);
				ui.input_mode = I_SELECTED_NONAME;
			}
		}
	}
	else if (ui.input_mode == I_MSG) // это состояние обозначает что ты в чате с кем-то
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
		case EXIT_FRIEND:
			notifyExitFriend(package);
			break;
		}
	}
	return state;
}

void Client::notifyExitFriend(Package package)
{
	std::string friend_login;
	client_storage.getClientLogin(friend_login, package.data.s_exit_friend.friend_uid);
	clearInputBuffer();
	ui.notifyFriendExit(friend_login);
	if (ui.getFriendUid() == package.data.s_exit_friend.friend_uid && ui.input_mode == I_MSG)
	{
		ui.input_mode = I_CMD;
	}
	ui.printInputMode();
}

void Client::updateUserList(Package package)
{
	client_storage.updateList(package, (uint16_t)(package.header.payload / sizeof(package.data.s_user_list.user[0])));
	if (ui.input_mode == I_REQUEST)
	{
		ui.displayList(client_storage.getList());
		ui.printHint(H_SELECT);
		ui.input_mode = I_CMD;
		ui.printInputMode();
	}
	else if (ui.input_mode == I_SELECTED_NONAME)
	{
		char f_login[50];
		uint32_t f_uid;
		ui.getFriendLogin(f_login);
		if (client_storage.getClientUid(f_login, &f_uid) == SUCCESS)
		{
			ui.setFriend(f_login, f_uid);
			ui.printSelectedUser();
			ui.printMissedMassege(client_storage.getMsg(ui.getFriendUid()));
			ui.input_mode = I_MSG;
		}
		else
		{
			ui.printState(E_FRIEND_WRONG);
			ui.input_mode = I_CMD;
		}
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
	if (package.data.s_error_msg.error_code == E_LOGIN_SIZE || package.data.s_error_msg.error_code == E_LOGIN_BUSY || package.data.s_error_msg.error_code == E_LOGIN_WRONG)
	{
		ui.printState(package.data.s_error_msg.error_code);
		ui.askLogin();
	}
	else if (ui.input_mode == I_MSG && package.data.s_error_msg.error_code == E_FRIEND_OFFLINE)
	{
		clearInputBuffer();
		ui.printState(package.data.s_error_msg.error_code);
		ui.printInputMode();
	}
	else
	{
		ui.printState(package.data.s_error_msg.error_code);
	}
}

void Client::authConfirm(Package package)
{
	uint8_t state = SUCCESS;
	if (countCfgLines() == 2)
	{
		char ipFromCfg[20];
		getLineFromCfg(0, ipFromCfg);
		char enteredIp[20];
		inet_ntop(AF_INET, network_module.getSockaddrPtr(), enteredIp, sizeof(enteredIp));
		if (strcmp(ipFromCfg, enteredIp) == 0)
		{
			char uid_cfg_str[20];
			getLineFromCfg(1, uid_cfg_str);
			uint32_t uid_cfg = atoi(uid_cfg_str);
			if (package.data.s_auth_confirm.client_uid != uid_cfg)
			{
				state = E_LOGIN_WRONG;
				ui.printState(state);
				ui.askLogin();
				Package package;
				char buffer[BUFFER_SIZE];
				package_manager.createErrorPackage(&package, E_LOGIN_WRONG);
				package_manager.transferToBuffer(package, buffer);
				network_module.sendMessage(buffer);
			}
		}
		else
		{
			std::ofstream cfg("../data/client.cfg", std::ios::out | std::ios::trunc);
			cfg.close();
			addToCfg(enteredIp);
			state = SUCCESS;
		}
	}
	if (state == SUCCESS)
	{
		my_uid = package.data.s_auth_confirm.client_uid;
		if (countCfgLines() != 2)
		{
			addToCfg(std::to_string(my_uid).c_str());
		}
		ui.input_mode = I_CMD;
		ui.displayHelp();
		ui.printInputMode();
	}
}