#include "../include/UI.h"

void UI::printWelcome()
{
	cout << SERVER_TEXT << "Welcome to \"Chat\"!" << RESET_TEXT << endl;
}

void UI::displayHelp()
{
	cout << CYAN << "---------------------------------------------------------" << RESET_TEXT << endl;
	cout << CYAN << "COMAND LIST:" << RESET_TEXT << endl;
	cout << MAGENTA << "/help\t\t\t" << RESET_TEXT << "- display a list of commands" << endl;
	cout << MAGENTA << "/list\t\t\t" << RESET_TEXT << "- display a list of online users" << endl;
	cout << MAGENTA << "/select {login}\t\t" << RESET_TEXT << "- select a user to send a message" << endl;
	cout << MAGENTA << "/exit\t\t\t" << RESET_TEXT << "- exit the application" << endl;
	cout << CYAN << "---------------------------------------------------------" << RESET_TEXT << endl;
}

void UI::askLogin()
{
	input_mode = 0;
	cout << "Enter you login: " << std::flush;
}

void UI::displayList(map<string, uint8_t> myMap)
{
	cout << GREEN_TEXT << "ONLINE" << RESET_TEXT << " USERS:" << endl;
	map<string, uint8_t>::iterator iter;
	for (iter = myMap.begin(); iter != myMap.end(); iter++)
	{
		if (iter->second == 0)
		{
			cout << iter->first << endl;
		}
		else
		{
			cout << iter->first << " " << "[" << (int)iter->second << "*]" << endl
				 << flush;
		}
	}
}

void UI::printState(uint8_t error_code) // подключить ошибки из stashe
{
	cout << ERROR_TEXT << "Error! ";
	switch (error_code)
	{
	case E_CONNECT:
		cout << ERROR_TEXT << "No internet connection. Please, check your internet connection!";
		break;
	case E_DATA:
		cout << ERROR_TEXT << "Failde to transfer data!";
		break;
	case E_LOGIN_SIZE:
		cout << ERROR_TEXT << "The login is too long!";
		break;
	case E_LOGIN_BUSY:
		cout << ERROR_TEXT << "The login is already basy";
		break;
	case E_LOGIN_WRONG:
		cout << ERROR_TEXT << "Wrong username";
		break;
	case E_FRIEND_WRONG:
		cout << ERROR_TEXT << "Wrong username friend";
		break;
	case E_SELF_MSG:
		cout << ERROR_TEXT << "Trying to send a message to youself";
		break;
	}
	cout << RESET_TEXT << endl;
}

void UI::printHint(uint8_t hint_code)
{
	cout << SERVER_TEXT;
	switch (hint_code)
	{
	case H_LIST:
		cout << "Use \'/list\' to display a list of online users.";
		break;
	case H_SELECT:
		cout << "Use \'/select {login}\' to select a interlocutor.";
		break;
	case H_LOGGED_OUT:
		cout << "You have logged out of the chat. See you!";
		break;
	}
	cout << RESET_TEXT << endl;
}

// печатает про
void UI::printMissedMassege(vector<friend_msg> vec)
{
	vector<friend_msg>::iterator iter;

	for (iter = vec.begin(); iter != vec.end(); iter++)
	{
		cout << FRIEND_TEXT << friend_login << ": " << RESET_TEXT << *iter->message << endl
			 << flush;
	}
}

void UI::printSelectedUser()
{
	cout << SERVER_TEXT << "You have selected a interlocutor " << FRIEND_TEXT << friend_login << SERVER_TEXT << "." << RESET_TEXT << endl;
}

void UI::setFriend(const char *_friend_login, uint32_t _friend_uid)
{
	strncpy(friend_login, _friend_login, 50);
	friend_uid = _friend_uid;
}

void UI::removeFriend()
{
	strcpy(friend_login, "\0");
	friend_uid = 0;
}
void UI::printMessage(const char *message)
{
	cout << "\033[1K\r"
		 << FRIEND_TEXT << friend_login << ": " << RESET_TEXT << message << endl; // заменить возврат коретки на отчистку
}

uint32_t UI::getFriendUid()
{
	return friend_uid;
}

void UI::printInputMode()
{
	if (input_mode == 1)
	{
		cout << ">" << std::flush;
	}
	if (input_mode == 2)
	{
		cout << "You: " << flush;
	}
}