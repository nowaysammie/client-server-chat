#include "../include/Storage.h"

uint8_t Storage::getClientUid(std::string login, uint32_t *_uid)
{
	std::map<std::string, std::pair<uint32_t, std::set<uint32_t>>>::iterator iter;
	iter = clientData.begin();
	iter = clientData.find(login);
	if (iter == clientData.end())
	{
		return E_LOGIN_WRONG;
	}
	*_uid = iter->second.first;
	return SUCCESS;
}
uint8_t Storage::appendClient(uint32_t client_uid, std::string login)
{
	std::string login_s = login;
	std::map<std::string, std::pair<uint32_t, std::set<uint32_t>>>::iterator iter;
	iter = clientData.begin();
	iter = clientData.find(login);
	if (iter != clientData.end())
	{
		if (client_uid == iter->second.first)
		{
			return SUCCESS;
		}
		return E_LOGIN_BUSY;
	}
	std::pair<uint32_t, std::set<uint32_t>> value;
	value.first = client_uid;
	clientData[login_s] = value;
	return SUCCESS;
}
uint8_t Storage::appendFriend(uint32_t client_uid, uint32_t friend_uid)
{
	std::map<std::string, std::pair<uint32_t, std::set<uint32_t>>>::iterator iter;

	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		if (iter->second.first == friend_uid)
		{
			break;
		}
	}
	if (iter == clientData.end())
	{
		return E_FRIEND_WRONG; // вернет ошибку: друга такого нет
	}
	iter->second.second.insert(client_uid);
	return SUCCESS;
}

std::map<std::string, uint32_t> Storage::getUserList(uint32_t client_uid)
{
	// map<string, pair<uint32_t, vector<uint32_t>>> clientData
	std::map<std::string, uint32_t> u_list;
	std::map<std::string, std::pair<uint32_t, std::set<uint32_t>>>::iterator iter;
	for (iter = clientData.begin(); iter != clientData.end(); iter++)
	{
		u_list[iter->first] = iter->second.first;
	}
	return u_list;
}