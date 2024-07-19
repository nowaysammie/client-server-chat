#ifndef CLIENT_STORAGE_H
#define CLIENT_STORAGE_H

#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<string>
#include<cstring>
#include <map>



using namespace std;

struct friend_msg
	{
		uint32_t src_uid;
		char message[792];
	}

class ClientStorage {
private:
	vector<friend_msg> messages;
	map<array<char, 50>, uint32_t> users_list;

	bool isEmpty();

public:

	void updateList(union data, uint16_t users_count);// обновляет лист с пользователями
	uint32_t getClientUid(char* login, uint32_t* uid); // Возвращает Uid по логину
	uint8_t getClientLogin(string* login, uint32_t uid); //Возвращает логин по UID;
	uint8_t appendMsg(uint32_t friend_uid, const char* message);//Записывает сообщение и того кто его отправил из друзей
	vector<friend_msg> getMsg(uint32_t uid);//соритирует сообщения и возвращает сообщения от одного пользователя 
	//перезаписывает messages без сообщений и uid от одного выбранного пользователя 
	uint8_t countFriendMsg(uint32_t friend_uid);// считает кол-во сообщения от одного пользователя
	map<string, uint8_t> getList();//возвращает контейнер в котором лежит uid и количество сообщений от него
	bool isOnUserList(uint32_t friend_uid);//Проверяет онлаин ли данный пользователь по UID;

};

#endif
