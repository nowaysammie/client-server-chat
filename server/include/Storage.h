#ifndef STORAGE_H
#define STORAGE_H

#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "States.h"

class Storage
{
private:
	std::map<std::string, std::pair<uint32_t, std::set<uint32_t>>> clientData; // 3 поля: логин, свой uid, uid собеседников
public:
	uint8_t getClientUid(std::string login, uint32_t *_uid);		  // получить uid клиента или ошибку
	uint8_t appendClient(uint32_t client_uid, std::string login);	  // добавить клиента в хранилище
	uint8_t appendFriend(uint32_t client_uid, uint32_t friend_uid);	  // добавление собеседника (которые ему пишут) в вектор
	std::map<std::string, uint32_t> getUserList(uint32_t client_uid); // вернуть список подключённых пользователей
};

#endif