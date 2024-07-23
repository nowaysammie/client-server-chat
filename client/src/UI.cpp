#include "UI.h"
#include <iostream>
#include <cstring>
#include "States.h"
void UI::printWelcome()
{
	std::cout << SERVER_TEXT << "Welcome to \"Chat\"!" << RESET_TEXT << std::endl;
}

void UI::displayHelp()
{
	std::cout << CYAN << "---------------------------------------------------------" << RESET_TEXT << std::endl;
	std::cout << CYAN << "COMAND LIST:" << RESET_TEXT << std::endl;
	std::cout << MAGENTA << "/help\t\t\t" << RESET_TEXT << "- display a list of commands" << std::endl;
	std::cout << MAGENTA << "/list\t\t\t" << RESET_TEXT << "- display a list of online users" << std::endl;
	std::cout << MAGENTA << "/select {login}\t\t" << RESET_TEXT << "- select a user to send a message" << std::endl;
	std::cout << MAGENTA << "/leave\t\t\t" << RESET_TEXT << "- log out of the chat with the other person" << std::endl;
	std::cout << MAGENTA << "/exit\t\t\t" << RESET_TEXT << "- exit the application" << std::endl;
	std::cout << CYAN << "---------------------------------------------------------" << RESET_TEXT << std::endl;
}

void UI::askLogin()
{
	input_mode = 0;
	std::cout << "Enter you login: " << std::flush;
}

void UI::displayList(std::map<std::string, uint8_t> myMap)
{
	std::cout << GREEN_TEXT << "ONLINE" << RESET_TEXT << " USERS:" << std::endl;
	std::map<std::string, uint8_t>::iterator iter;
	for (iter = myMap.begin(); iter != myMap.end(); iter++)
	{
		if (iter->second == 0)
		{
			std::cout << iter->first << std::endl;
		}
		else
		{
			std::cout << iter->first << " " << "[" << (int)iter->second << "*]" << std::endl
					  << std::flush;
		}
	}
}

void UI::printState(uint8_t error_code) // подключить ошибки из stashe
{
	std::cout << ERROR_TEXT << "Error! ";
	switch (error_code)
	{
	case E_CONNECT:
		std::cout << ERROR_TEXT << "No internet connection. Please, check your internet connection!";
		break;
	case E_DATA:
		std::cout << ERROR_TEXT << "Failde to transfer data!";
		break;
	case E_LOGIN_SIZE:
		std::cout << ERROR_TEXT << "Invalid login format!";
		break;
	case E_LOGIN_BUSY:
		std::cout << ERROR_TEXT << "The entered username is busy";
		break;
	case E_LOGIN_WRONG:
		std::cout << ERROR_TEXT << "Wrong username";
		break;
	case E_FRIEND_WRONG:
		std::cout << ERROR_TEXT << "A non-existent username of the interlocutor has been entered";
		break;
	case E_SELF_MSG:
		std::cout << ERROR_TEXT << "Trying to send a message to yourself";
		break;
	case E_FRIEND_OFFLINE:
		std::cout << ERROR_TEXT << "Attempt to send a message to a disconnected user";
		break;
	case E_FRIEND_FULL:
		std::cout << ERROR_TEXT << "An attempt to send a message to a user with an overflowing inbox buffer";
		break;
	case E_WRONG_COMMAND:
		std::cout << ERROR_TEXT << "The user entered the wrong command";
		break;
	case E_LEAVE_COMMAND:
		std::cout << ERROR_TEXT << "This command can only be invoked while in the dialog";
		break;
	}
	std::cout << RESET_TEXT << std::endl;
}

void UI::printHint(uint8_t hint_code)
{
	std::cout << SERVER_TEXT;
	switch (hint_code)
	{
	case H_LIST:
		std::cout << "Use \'/list\' to display a list of online users.";
		break;
	case H_SELECT:
		std::cout << "Use \'/select {login}\' to select a interlocutor.";
		break;
	case H_LOGGED_OUT:
		std::cout << "You have logged out of the chat. See you!";
		break;
	}
	std::cout << RESET_TEXT << std::endl;
}

// печатает про
void UI::printMissedMassege(std::vector<friend_msg> vec) // убрать из структуры uid
{
	int len = vec.size();
	for (int i = 0; i < len; i++)
	{
		std::cout << FRIEND_TEXT << friend_login << ": " << RESET_TEXT << vec[i].message << std::endl
				  << std::flush;
	}
}

void UI::printSelectedUser()
{
	std::cout << SERVER_TEXT << "You have selected a interlocutor " << FRIEND_TEXT << friend_login << SERVER_TEXT << "." << RESET_TEXT << std::endl;
}

void UI::setFriend(const char *_friend_login, uint32_t _friend_uid)
{
	strncpy(friend_login, _friend_login, 50);
	friend_uid = _friend_uid;
}

void UI::removeFriend()
{
	friend_login[0] = '\0';
	friend_uid = 0;
}

void UI::printMessage(const char *message)
{
	std::cout << "\033[1K\r"
			  << FRIEND_TEXT << friend_login << ": " << RESET_TEXT << message << std::endl
			  << std::flush;
}

uint32_t UI::getFriendUid()
{
	return friend_uid;
}

void UI::printInputMode()
{
	if (input_mode == I_CMD)
	{
		std::cout << ">" << std::flush;
	}
	if (input_mode == I_MSG)
	{
		std::cout << "You: " << std::flush;
	}
}

uint8_t UI::getFriendLogin(char *f_login)
{
	uint8_t state = E_FRIEND_NOT_EXIST;
	if (friend_login[0] != '\0')
	{
		strncpy(f_login, friend_login, 50);
		state = SUCCESS;
	}
	return state;
}