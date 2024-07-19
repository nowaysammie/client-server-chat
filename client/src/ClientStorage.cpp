#include "ClientStorage.h"

void ClientStorage::updateList(Package package, uint16_t users_count)
{
	users_list.clear();

	for (int i = 0; i < users_count; i++)
	{
		users_list[string(package.data.s_user_list.user[i].login)] = package.data.s_user_list.user[i].uid;
	}
}

bool ClientStorage::isEmpty()
{

	return users_list.empty();
}

uint8_t ClientStorage::getClientUid(char *login, uint32_t *uid)
{
	map<string, uint32_t>::iterator iter;

	for (iter = users_list.begin(); iter != users_list.end(); iter++)
	{
		if (iter->first == string(login))
		{
			*uid = iter->second;
			return SUCCESS;
		}
	}
	return E_FRIEND_WRONG;
}

uint8_t ClientStorage::getClientLogin(string *login, uint32_t uid)
{
	map<string, uint32_t>::iterator iter;

	for (iter = users_list.begin(); iter != users_list.end(); iter++)
	{
		if (iter->second == uid)
		{
			*login = iter->first;
			return SUCCESS;
		}
	}
	return E_FRIEND_NOT_EXIST;
}

uint8_t ClientStorage::appendMsg(uint32_t friend_uid, const char *message)
{

	if (messages.size() >= 50)
	{
		return E_FRIEND_FULL;
	}
	if (!isOnUserList(friend_uid))
	{
		return E_FRIEND_NOT_EXIST;
	}
	friend_msg fri = {friend_uid, *message};

	messages.push_back(fri);

	return SUCCESS;
}

vector<friend_msg> ClientStorage::getMsg(uint32_t uid)
{
	vector<friend_msg> friend_messages;
	vector<friend_msg> other_msg;
	vector<friend_msg>::iterator iter;
	friend_msg fri;

	for (iter = messages.begin(); iter != messages.end(); iter++)
	{
		if (iter->src_uid == uid)
		{
			fri.src_uid = uid;
			strncpy(fri.message, iter->message, 792);
			friend_messages.push_back(fri);
		}
		else
		{
			fri.src_uid = uid;
			strncpy(fri.message, iter->message, 792);
			other_msg.push_back(fri);
		}
	}
	messages.clear();
	messages == other_msg;

	return friend_messages;
}

uint8_t ClientStorage::countFriendMsg(uint32_t friend_uid) // Подсчитывает сообщения от одного пользователя
{
	unsigned int count = 0;
	vector<friend_msg>::iterator iter;
	for (iter = messages.begin(); iter != messages.end(); iter++)
	{
		if (iter->src_uid == friend_uid)
		{
			count++;
		}
	}
	return count;
}

map<string, uint8_t> ClientStorage::getList();
{
	map<string, uint8_t> myMap;
	map<string, uint32_t>::iterator iter;

	for (iter = users_list.begin(); iter != users_list.end(); iter++)
	{
		myMap[iter->first] = countFriendMsg(iter->second);
	}

	return myMap;
}

bool ClientStorage::isOnUserList(uint32_t friend_uid)
{
	map<string, uint32_t>::iterator iter;

	for (iter = users_list.begin(); iter != users_list.end(); iter++)
	{
		if (iter->second == friend_uid)
		{
			return true;
		}
	}
	return false;
}