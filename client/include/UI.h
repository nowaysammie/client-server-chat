#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string>

#define RESET_TEXT "\033[0m"
#define ERROR_TEXT "\033[0;30;41m"
#define SUCS_TEXT "\033[0;30;42m"
#define SERVER_TEXT "\033[1;33m"
#define GREEN_TEXT "\033[32m"
#define RED_TEXT "\033[31m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define FRIEND_TEXT "\033[1;32m"
#define CYAN    "\033[36m"
#define MY_TEXT "\033[1;36m" //поменять цвет
#define WHITE   "\033[37m"
#define OTHER_TEXT "\033[0;94m"


enum error_code {
	E_CONNECT = 1, 
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
	H_LIST,
	H_SELECT,
	H_LOGGED_OUT
};


using namespace std;

class UI
{
private:
	

	string friend_login;
	uint32_t friend_uid;


public:
	string read();
	void printWelcome();// Выводит Welcome
	void printError(uint8_t error_code);//
	void printHint(uint8_t hint_code);//
	void selectUser(string login);//
	void printMassege(string login, string massange);//
	string askLogin();
	void displayHelp();//
	void displayList(struct data);
	void setFriend(string friend_login, uint32_t friend_uid);
	void removeFriend();

};