#include "ClientStorage.h"
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

void ClientStorage::updateList(Package package, uint16_t users_count)
{
	users_list.clear();

	for (int i = 0; i < users_count; i++)
	{
		users_list[std::string(package.data.s_user_list.user[i].login)] = package.data.s_user_list.user[i].uid;
	}
}

bool ClientStorage::isEmpty()
{
	return users_list.empty();
}

uint8_t ClientStorage::getClientUid(std::string login, uint32_t *uid)
{
	uint8_t state = E_FRIEND_WRONG;
	std::map<std::string, uint32_t>::iterator iter;
	for (iter = users_list.begin(); iter != users_list.end(); iter++)
	{
		if (iter->first == std::string(login))
		{
			*uid = iter->second;
			state = SUCCESS;
		}
	}
	return state;
}

uint8_t ClientStorage::getClientLogin(std::string &login, uint32_t friend_uid)
{
	uint8_t state = E_FRIEND_NOT_EXIST;
	std::map<std::string, uint32_t>::iterator iter;
	for (iter = users_list.begin(); iter != users_list.end(); iter++)
	{
		if (iter->second == friend_uid)
		{
			login = iter->first;
			state = SUCCESS;
			break;
		}
	}
	return state;
}

uint8_t ClientStorage::appendMsg(uint32_t friend_uid, const char *message)
{
	uint8_t state = E_FRIEND_FULL;
	if (messages.size() < 50)
	{
		friend_msg fri;
		fri.src_uid = friend_uid;
		strncpy(fri.message, message, 792);
		messages.push_back(fri);
		state = SUCCESS;
	}
	return state;
}

std::vector<std::string> ClientStorage::getMsg(uint32_t uid)
{
	std::vector<std::string> friend_messages;
	std::vector<friend_msg> other_msg;
	std::vector<friend_msg>::iterator iter;
	friend_msg fri;

	for (iter = messages.begin(); iter != messages.end(); iter++)
	{
		if (iter->src_uid == uid)
		{
			friend_messages.push_back(iter->message);
		}
		else
		{
			fri.src_uid = iter->src_uid;
			strncpy(fri.message, iter->message, 792);
			other_msg.push_back(fri);
		}
	}
	messages.clear();
	messages = other_msg;

	return friend_messages;
}

uint8_t ClientStorage::countFriendMsg(uint32_t friend_uid) // Подсчитывает сообщения от одного пользователя
{
	unsigned int count = 0;
	std::vector<friend_msg>::iterator iter;
	for (iter = messages.begin(); iter != messages.end(); iter++)
	{
		if (iter->src_uid == friend_uid)
		{
			count++;
		}
	}
	return count;
}

std::map<std::string, uint8_t> ClientStorage::getList()
{
	std::map<std::string, uint8_t> myMap;
	std::map<std::string, uint32_t>::iterator iter;

	for (iter = users_list.begin(); iter != users_list.end(); iter++)
	{
		myMap[iter->first] = countFriendMsg(iter->second);
	}

	return myMap;
}

bool ClientStorage::isOnUserList(uint32_t friend_uid)
{
	std::map<std::string, uint32_t>::iterator iter;

	for (iter = users_list.begin(); iter != users_list.end(); iter++)
	{
		if (iter->second == friend_uid)
		{
			return true;
		}
	}
	return false;
}