#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <stdint.h>
#include <string>
#include <map>
#include "States.h"

#define RESET_TEXT "\033[0m"
#define ERROR_TEXT "\033[0;30;41m"
#define SUCS_TEXT "\033[0;30;42m"
#define SERVER_TEXT "\033[1;33m"
#define GREEN_TEXT "\033[32m"
#define RED_TEXT "\033[31m"
#define MAGENTA "\033[35m"
#define FRIEND_TEXT "\033[1;32m" //
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define OTHER_TEXT "\033[0;94m"
#define BLUE_TEXT "\033[34m"
#define MAGENTA_TEXT "\033[35m"
#define FRIEND_TEXT "\033[1;32m"
#define MY_TEXT "\033[1;36m"
#define WHITE "\033[37m"

enum hint_code
{
	H_LIST = 0,

	H_SELECT,
	H_LOGGED_OUT
};

#ifndef STR_FR_MSG
#define STR_FR_MSG
struct friend_msg
{
	uint32_t src_uid;
	char message[792];
};

#endif

enum
{
	I_LOGIN = 0,
	I_CMD,
	I_MSG,
	I_LIST,
	I_REQUEST
};

using namespace std;

class UI
{
private:
	char friend_login[50];
	uint32_t friend_uid;

public:
	void printInputMode();										   // меняет режим ввода
	uint8_t input_mode;											   // режим ввода
	void printWelcome();										   // выводит Welcome to Chat *
	void askLogin();											   // запрашивает у пользователя логин *
	void displayHelp();											   // печатает меню с командами *
	void displayList(std::map<std::string, uint8_t> myMap);		   // печатает список онлайн пользователей *
	void printHint(uint8_t hint_code);							   // выводит подсказку *
	void printState(uint8_t state_code);						   // выводит сообщение о состоянии выполнения операции *
	uint32_t getFriendUid();									   // возвращает uid друга *
	void setFriend(const char *friend_login, uint32_t friend_uid); // установить данные собседника в поля класса *
	void removeFriend();										   // убирает данные клиента из класса *
	void printSelectedUser();									   // выводит сообщение о выборе пользователя *
	void printMessage(const char *message);						   // выводит сообщение собеседника *
	void UselectUser(char *_friend_login, uint32_t _friend_uid);   // Ввод собщений
	void printMissedMassege(vector<friend_msg> vec);			   // выводит сообщения собеседника которые были написаны ранее *
};

#endif