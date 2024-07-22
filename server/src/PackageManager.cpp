#include "../include/PackageManager.h"
#include <iostream>
uint8_t PackageManager::readHeaderFields(const char *buffer, uint16_t *cmd, uint16_t *payload)
{ // считывает код команды
    try
    {
        memcpy(cmd, buffer, 2);
    }
    catch (...)
    {
        return E_DATA;
    }
    return SUCCESS;
}

uint8_t PackageManager::parseToPackage(Package *package, const char *buffer)
{ // парсит присланный буфер в структуру
    uint16_t cmd, payload;
    uint8_t status = readHeaderFields(buffer, &cmd, &payload);
    if (cmd > CMD_MAX || payload > PAYLOAD_MAX)
    {
        return E_DATA;
    }
    try
    {
        memcpy(package, buffer, 804);
    }
    catch (...)
    {
        return E_DATA;
    }
    return SUCCESS;
}

uint8_t PackageManager::transferToBuffer(Package package, char *buffer)
{ // формирование буффера, который отправляется по сети
    if (package.header.cmd > CMD_MAX || package.header.payload > PAYLOAD_MAX)
    { // проверка на ошибку
        return E_DATA;
    }
    try
    {
        memcpy(buffer, &package, sizeof(package));
    }
    catch (...)
    {
        return E_DATA;
    }
    return SUCCESS;
}

void PackageManager::createErrorPackage(Package *package, uint8_t error_code)
{ // формирует пакет ERROR_MSG
    package->header = {ERROR_MSG, sizeof(package->data.s_error_msg)};
    package->data.s_error_msg.error_code = error_code;
    return;
}

void PackageManager::createAuthConfirmPackage(Package *package, uint32_t client_uid)
{ // формирует пакет AUTH_CONFIRM
    package->header = {AUTH_CONFIRM, sizeof(package->data.s_auth_confirm)};
    package->data.s_auth_confirm.client_uid = client_uid;
    return;
}

void PackageManager::createUserListPackage(Package *package, std::map<std::string, uint32_t> u_list)
{ // формирует пакет USER_LIST
    package->header = {USER_LIST, (uint16_t)(u_list.size() * sizeof(package->data.s_user_list.user[0]))};
    std::map<std::string, uint32_t>::iterator it;
    uint8_t j = 0;
    unsigned int login_size = sizeof(package->data.s_user_list.user[0].login);
    for (it = u_list.begin(); it != u_list.end(); it++, j++)
    {
        strncpy(package->data.s_user_list.user[j].login, it->first.c_str(), sizeof(package->data.s_user_list.user[0].login));
        package->data.s_user_list.user[j].uid = it->second;
    }
}

void PackageManager::createExitFriendPackage(Package package, uint32_t friend_uid)
{ // формирует пакет EXIT_FRIEND
    package.header = {EXIT_FRIEND, sizeof(package.data.s_exit_friend)};
    package.data.s_exit_friend.friend_uid = friend_uid;
    return;
}