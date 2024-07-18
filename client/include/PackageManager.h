#ifndef PACKAGE_MANAGER_CLIENT_H
#define PACKAGE_MANAGER_CLIENT_H

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
	uint8_t parseToPackage(Package *package, const char *buffer);			  // парсит присланный буфер в структуру
	uint8_t transferToBuffer(Package package, char *buffer);				  // формирует буфер из структуры
	void createAuthRequestPackage(Package *package, const char *login);		  // формирует пакет AUTH_REQUEST
	void createErrorPackage(Package *package, uint8_t error_code);			  // формирует пакет ERROR_MSG
	void createUserListRequestPackage(Package *package, uint32_t client_uid); // формирует пакет USER_LIST_REQUEST
};

#endif