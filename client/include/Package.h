#ifndef PACKAGE_H
#define PACKAGE_H

#define CMD_MAX 6
#define PAYLOAD_MAX 800
#define LOGIN_SIZE_MAX 50
#define BUFFER_SIZE_MAX 804
#define MSG_SIZE 792

enum
{ // коды команд
    AUTH_REQUEST = 0,
    AUTH_CONFIRM,
    USER_LIST_REQUEST,
    USER_LIST,
    EXIT_FRIEND,
    MSG,
    ERROR_MSG
};
// структуры пакета в зависимости от кода команды
struct S_AUTH_REQUEST
{
    char login[50];
};

struct S_AUTH_CONFIRM
{
    uint32_t client_uid;
};

struct S_USER_LIST_REQUEST
{
    uint32_t client_uid;
};

struct S_USER_LIST
{
    struct
    {
        char login[50];
        uint32_t uid;
    } user[14];
};

struct S_EXIT_FRIEND
{
    uint32_t friend_uid;
};

struct S_MSG
{
    uint32_t src_uid;
    uint32_t dest_uid;
    char message[792];
};

struct S_ERROR_MSG
{
    uint8_t error_code;
};

struct Header
{ // структура заголовка
    uint16_t cmd;
    uint16_t payload;
};

union Data
{ // поле данных
    S_AUTH_REQUEST s_auth_request;
    S_AUTH_CONFIRM s_auth_confirm;
    S_USER_LIST_REQUEST s_user_list_request;
    S_USER_LIST s_user_list;
    S_EXIT_FRIEND s_exit_friend;
    S_MSG s_msg;
    S_ERROR_MSG s_error_msg;
};

struct Package
{ // структура пакета
    Header header;
    Data data;
};

#endif