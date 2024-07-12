#include<stdio.h>
#include<stdlib.h>
#include<iostream>

#define RESET_TEXT "\033[0m"
#define ERROR_TEXT "\033[0;30;41m"
#define SUCS_TEXT "\033[0;30;42m"
#define SERVER_TEXT "\033[1;33m"
#define GREEN_TEXT "\033[32m"
#define RED_TEXT "\033[31m"
#define BLUE_TEXT    "\033[34m"
#define MAGENTA_TEXT "\033[35m"
#define FRIEND_TEXT "\033[1;32m"
#define MY_TEXT "\033[1;36m"
#define WHITE   "\033[37m"
#define FRIEND_TEXT "\033[0;94m"

enum state_code {
	SUCCESS = 0,
	E_CONNECT, 
	E_DATA, 
	E_LOGIN_SIZE, 
	E_LOGIN_BUSY, 
	E_LOGIN_WRONG,
	E_FRIEND_WRONG, 
	E_SELF_MSG, 
	E_MSG_TOO_BIG, 
	E_FRIEND_OFFLINE
};

enum hint_code {
	H_LIST = 0,
	H_SELECT,
	H_LOGGED_OUT
};


using namespace std;

class UI {
private:
	char input_mode;
	char friend_login[50];
	uint32_t friend_uid;
	void printInputMode();
public:
	UI();
	void printWelcome(); //выводит Welcome to Chat
	void askLogin(char* login); //запрашивает у пользователя логин
	void printHelp(); //печатает меню с командами
	void printList(); //печатает список онлайн пользователей 
	void printHint(uint8_t hint_code); //выводит подсказку
	void printState(uint8_t state_code); //выводит сообщение о состоянии выполнения операции
	void getFriend(char* friend_login, uint32_t* friend_uid);
	void setFriend(const char* friend_login, uint32_t friend_uid); //установить данные собседника в поля класса
	void removeFriend(); //убирает данные клиента из класса
	void printSelectedUser(const char* login); //выводит сообщение о выборе пользователя
	void printMessage(const char* login, const char* message); //выводит сообщение собеседника
};