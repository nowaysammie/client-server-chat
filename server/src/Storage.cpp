#include "Storage.h"
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include "States.h"

uint8_t Storage::getClientUid(std::string login, uint32_t *_uid)
{
	std::set<userData>::iterator iter;
	uint8_t state = E_LOGIN_WRONG;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->login == login)
		{
			*_uid = iter->uid;
			state = SUCCESS;
			break;
		}
	}
	return state;
}
uint8_t Storage::getClientSocket(uint32_t client_uid, int32_t *client_socket)
{
	std::set<userData>::iterator iter;
	uint8_t state = E_FRIEND_WRONG;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->uid == client_uid)
		{
			*client_socket = iter->sock;
			state = SUCCESS;
			break;
		}
	}
	return state;
}
uint8_t Storage::appendClient(uint32_t client_uid, char *login, int32_t client_socket)
{
	std::string login_s = login;
	std::set<userData>::iterator iter;
	iter = clientData.begin();
	uint8_t state = E_LOGIN_BUSY;
	if (getClientUid(std::string(login), &client_uid) == E_LOGIN_WRONG)
	{
		std::set<uint32_t> empty_set;
		userData newClient = {std::string(login), client_uid, empty_set, client_socket};
		clientData.insert(newClient);
		state = SUCCESS;
	}
	return state;
}
uint8_t Storage::appendFriend(uint32_t client_uid, uint32_t friend_uid) // добавеляет в список друзей friend пользователя client
{
	std::set<userData>::iterator iter;
	uint8_t state = E_FRIEND_WRONG;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->uid == friend_uid)
		{
			userData updatedClient = *iter;
			updatedClient.friends.insert(client_uid);
			clientData.erase(iter);
			clientData.insert(updatedClient);
			state = SUCCESS;
			break;
		}
	}
	return state;
}

std::map<std::string, uint32_t> Storage::getUserList(uint32_t client_uid)
{
	std::map<std::string, uint32_t> u_list;
	std::set<userData>::iterator iter;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->uid != client_uid)
		{
			u_list[iter->login] = iter->uid;
		}
	}
	return u_list;
}
