#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H
//тут должны быть структуры
class PackageManager() {
private:

public:
	uint8_t readCommand(const char* buffer, uint16_t* cmd); //считывает код команды
	uint8_t parsePackage(uint16_t cmd, const char* buffer, struct Package* package); //парсит присланный буфер в структуру
	char* formPackage(struct Package);
};

#endif