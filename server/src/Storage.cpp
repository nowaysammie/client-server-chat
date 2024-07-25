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
			if (iter->sock != 0)
			{
				*_uid = iter->uid;
				state = SUCCESS;
				break;
			}
			else
			{
				state = E_FRIEND_OFFLINE;
			}
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
	if (getClientUid(std::string(login), &client_uid) != E_LOGIN_WRONG)
	{
		for (iter = clientData.begin(); iter != clientData.end(); iter++)
		{
			if (iter->uid == client_uid)
			{
				std::set<uint32_t> emptySet;
				userData updateUser = {iter->login, iter->uid, emptySet, client_socket};
				clientData.erase(iter);
				clientData.insert(updateUser);
				break;
				state = SUCCESS;
			}
		}
	}
	else
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

std::set<int32_t> Storage::getFriendList(uint32_t client_uid) ////////////////// обслуживание подключения доработать
{
	std::set<int32_t> friends;
	std::set<userData>::iterator iter;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->uid == client_uid)
		{
			std::set<uint32_t>::iterator iter_friend;
			for (iter_friend = iter->friends.begin(); iter_friend != iter->friends.end(); iter_friend++)
			{
				friends.insert(*iter_friend);
			}
		}
	}
	return friends;
}

std::set<uint32_t> Storage::offlineClient(int32_t client_socket)
{
	std::set<userData>::iterator iter;
	std::set<uint32_t> waitingFriends;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->sock == client_socket)
		{
			std::set<uint32_t> emptySet;
			userData updateUser = {iter->login, iter->uid, emptySet, 0};
			waitingFriends = iter->friends;
			clientData.erase(iter);
			clientData.insert(updateUser);
			break;
		}
	}
	return waitingFriends;
}

uint32_t Storage::getClientUidToSocket(int32_t sock)
{
	std::set<userData>::iterator iter;
	uint32_t finded_uid = 0;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->sock == sock)
		{
			finded_uid = iter->uid;
			break;
		}
	}
	return finded_uid;
}

void Storage::deleteClient(int32_t client_socket)
{
	std::set<userData>::iterator iter;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->sock == client_socket)
		{
			clientData.erase(iter);
		}
	}
}