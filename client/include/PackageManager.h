<<<<<<< HEAD
#pragma once
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string>

using namespace std;


class PackageManager
{
private:

public:

	uint16_t readCommander(struct data);
	struct data parsePackage(char buf[], uint16_t);
	char* fromPackage(struct data);

};
=======
#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <map>
#include <string>
#include <cstring>
#include <stdint.h>
#include "Package.h"
#include "States.h"

class PackageManager
{
private:
	uint8_t readHeaderFields(const char *buffer, uint16_t *cmd, uint16_t *payload); // считывает код команды
public:
	uint8_t parseToPackage(Package *package, const char *buffer);				   // парсит присланный буфер в структуру
	uint8_t transferToBuffer(Package package, char *buffer);					   // формирует буфер из структуры
	void createErrorPackage(Package *package, uint8_t error_code);				   // формирует пакет ERROR_MSG
	void createAuthConfirmPackage(Package *package, uint32_t client_uid);		   // формирует пакет AUTH_CONFIRM
	void createUserListPackage(Package *package, std::map<std::string, uint32_t>); // формирует пакет USER_LIST
	void createExitFriendPackage(Package package, uint32_t friend_uid);			   // формирует пакет EXIT_FRIEND
};

#endif
>>>>>>> e94e262f96bfc744bf4c577195e2601f6521b53e
