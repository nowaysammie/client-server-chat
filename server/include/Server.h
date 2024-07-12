#ifndef SERVER_H
#define SERVER_H

class Server {
private:
	NetworkModule network_module;
	Storage storage;
	PackageManager package_manager;
	void shutdown(); //закрывает сокет
public:
	Server(uint32_t server_ip, uint16_t port); //запуск сервера, бинд к порту
	pollfd* toPoll(); //запускает network_module.toPoll() и возвращает список готовых ивентов
	uint8_t eventHandler(pollfd fd); //делает reevent=0, обрабатывает событие, взаимодействует с PackageManager и Storage, возвращает статус
};

#endif