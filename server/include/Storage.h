#ifndef STORAGE_H
#define STORAGE_H

#include <iostream>
#include <map>

using namespace std;

class Storage {
private:
	map<array<char, 50>, pair<uint32_t, uint32_t>> clientData; //3 поля: логин, свой uid, uid собеседника 
public:
	uint8_t getClientUid(string login, uint32_t* _uid); //получить uid клиента или ошибку
	uint8_t appendClient(uint32_t UID, string login); //добавить клиента в хранилище
	uint8_t updateFriendUid(uint32_t friend_uid); //изменить uid последнего собеседника
};

#endif