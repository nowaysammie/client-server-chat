#ifndef CLIENT_STORAGE_H
#define CLIENT_STORAGE_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include "States.h"
#include "Package.h"

#ifndef STR_FR_MSG
#define STR_FR_MSG
struct friend_msg
{
	uint32_t src_uid;
	char message[792];
};
#endif

class ClientStorage
{
private:
	std::vector<friend_msg> messages;
	std::map<std::string, uint32_t> users_list;

	bool isEmpty();

public:
	void updateList(Package package, uint16_t users_count);		 // обновляет лист с пользователями
	uint8_t getClientUid(std::string login, uint32_t *uid);		 // Возвращает Uid по логину
	uint8_t getClientLogin(std::string *login, uint32_t uid);	 // Возвращает логин по UID;
	uint8_t appendMsg(uint32_t friend_uid, const char *message); // Записывает сообщение и того кто его отправил из друзей
	std::vector<std::string> getMsg(uint32_t uid);				 // соритирует сообщения и возвращает сообщения от одного пользователя
	// перезаписывает messages без сообщений и uid от одного выбранного пользователя
	uint8_t countFriendMsg(uint32_t friend_uid); // считает кол-во сообщения от одного пользователя
	std::map<std::string, uint8_t> getList();	 // возвращает контейнер в котором лежит uid и количество сообщений от него
	bool isOnUserList(uint32_t friend_uid);		 // Проверяет онлаин ли данный пользователь по UID;
};

#endif
