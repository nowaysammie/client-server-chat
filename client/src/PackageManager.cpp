#include "../include/PackageManager.h"
#include <iostream>
// считывает header
uint8_t PackageManager::readHeaderFields(const char *buffer, uint16_t *cmd, uint16_t *payload)
{
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
// парсит присланный буфер в структуру
uint8_t PackageManager::parseToPackage(Package *package, const char *buffer)
{
    uint16_t cmd, payload;
    uint8_t status = readHeaderFields(buffer, &cmd, &payload);
    if (cmd > CMD_MAX || payload > PAYLOAD_MAX)
    {
        return E_DATA;
    }
    try
    {
        memcpy(package, buffer, sizeof(buffer));
    }
    catch (...)
    {
        return E_DATA;
    }
    return SUCCESS;
}
// формирование буффера, который отправляется по сети
uint8_t PackageManager::transferToBuffer(Package package, char *buffer)
{
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
// формирует пакет AUTH_REQUEST
void PackageManager::createAuthRequestPackage(Package *package, const char *login)
{
    package->header = {AUTH_REQUEST, 50};
    strncpy(package->data.s_auth_request.login, login, sizeof(package->data.s_auth_request.login));
}
// формирует пакет ERROR_MSG
void PackageManager::createErrorPackage(Package *package, uint8_t error_code)
{
    package->header = {ERROR_MSG, sizeof(package->data.s_error_msg)};
    package->data.s_error_msg.error_code = error_code;
}
// формирует пакет USER_LIST_REQUEST
void PackageManager::createUserListRequestPackage(Package *package, uint32_t client_uid)
{
    package->header = {USER_LIST_REQUEST, sizeof(package->data.s_user_list_request.client_uid)};
    package->data.s_user_list_request.client_uid = client_uid;
}

void PackageManager::createMsgPackage(Package *package, uint32_t src_uid, uint32_t dest_uid, char *message)
{
    package->header = {MSG, 792};
    package->data.s_msg.src_uid = src_uid;
    package->data.s_msg.dest_uid = dest_uid;
    strncpy(package->data.s_msg.message, message, 792);
}