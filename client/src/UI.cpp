#include"Ui.h"


void UI::printWelcome()
{
	cout << SERVER_TEXT << "Welcome to \"Chat\"!" << endl;
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


void UI::askLogin()//точно должна ли возвращать 
{
	cout << SERVER_TEXT << "Enter your login: " << RESET_TEXT << endl;
}


void UI::displayList(map<string, uint8_t> myMap)
{
	cout << GREEN_TEXT << "ONLINE" << RESET_TEXT << "USERS:" << endl;
	map<string, uint32_t> :: iterator iter;
	for(iter = myMap.begin(); iter != myMap.end(); iter++)
	{
		if(myMap.second == 0)
		{
			cout << iter.first << endl;
		}
		else
		{
			printf("%s [%d*]\n", iter.first, iter.second);
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
  cout << SERVER_TEXT
	switch (hint_code)
	{
	case H_LIST:
		cout << SERVER_TEXT << "Use \'/list\' to display a list of online users.";
		break;
	case H_SELECT:
		cout << SERVER_TEXT << "Use \'/select {login}\' to select a interlocutor.";
		break;
	case H_LOGGED_OUT:
		cout << SERVER_TEXT << "You have logged out of the chat. See you!";
		break;
	}

	cout << RESET_TEXT << endl;
}


void UI::printMassege(string login, string massege)
{
	cout << GREEN_TEXT << login << ": " << massege << RESET_TEXT << endl;
}

void UI::selectUser(char* _friend_login, uint32_t _friend_uid)
{
	strncpy(friend_login, _friend_login, 50);
	friend_uid = _friend_uid;
	cout << "y" << endl;
	input_mode = MSG_IN;
	printInputMode();
}