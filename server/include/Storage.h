#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <map>
#include <set>

struct userData
{
	std::string login;
	uint32_t uid;
	std::set<uint32_t> friends;
	int32_t sock;

	bool operator==(const userData &other) const
	{
		return (login == other.login) && (uid == other.uid) && (friends == other.friends) && (sock == other.sock);
	}

	bool operator!=(const userData &other) const
	{
		return !(*this == other);
	}
	bool operator<(const userData &other) const
	{
		return uid < other.uid;
	}
};

class Storage
{
private:
	std::set<userData> clientData;

public:
	uint8_t getClientUid(std::string login, uint32_t *_uid); // получить uid клиента или ошибку
	uint32_t getClientUidToSocket(int32_t sock);
	uint8_t getClientSocket(uint32_t client_uid, int32_t *client_socket);
	uint8_t appendClient(uint32_t client_uid, char *login, int32_t client_socket); // добавить клиента в хранилище
	uint8_t appendFriend(uint32_t client_uid, uint32_t friend_uid);				   // добавление собеседника (которые ему пишут) в вектор
	uint8_t deleteClient(int32_t client_socket);
	std::map<std::string, uint32_t> getUserList(uint32_t client_uid); // вернуть список подключённых пользователей
	std::set<int32_t> getFriendList(uint32_t client_uid);			  // возвращает сокеты друзей
	std::set<uint32_t> offlineClient(int32_t client_socket);
};

#endif
