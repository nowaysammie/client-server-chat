#ifndef STATES_H
#define STATES_H

enum
{
    SUCCESS = 0,        // Успех
    E_CONNECT,          // Отсутствует связь
    E_DATA,             // Ошибка передачи или обработки данных (неправильный код команды, длина полезной нагрузки более 800, безуспешное формировании пакета)
    E_LOGIN_SIZE,       // Неверный формат логина
    E_LOGIN_BUSY,       // Введённый логин занят
    E_LOGIN_WRONG,      // Неверный логин при повторной авторизации
    E_UID_WRONG,        // Запрос списка пользователей от незарегистрированного пользователя
    E_FRIEND_WRONG,     // Введён несуществующий логин собеседника
    E_FRIEND_NOT_EXIST, // сообщение от неизвестный пользователь
    E_SELF_MSG,         // Попытка отправить сообщение самому себе
    E_SRV_FULL,         // Ошибка при подключении на сервер с максимальным количеством подключённых пользователей
    E_FRIEND_OFFLINE,   // Попытка отправки сообщения отключившемуся пользователю
    E_FRIEND_FULL,      // Попытка отправки сообщения пользователю с переполненным буффером входящих сообщений
    C_DISCONNECTED,     // клиент отключён
    C_CONNECTED,        // клиент подключился
    C_SHUTDOWN,         // клиент завершил работу
    C_OK,               // обработка выполнена
    C_HANDLE_END,       // завершена обработка всех событий
    E_WRONG_COMMAND,    // пользователь ввёл неверную команду
    E_LEAVE_COMMAND,    // эта команда можно вызвать только находясь в диалоге
    C_NO_VALID_IP       // Проверяет ip на правильность ввода
};

#endif