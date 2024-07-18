#pragma once
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

class ClientStorage
{
private:
	map<array<char, 50>, uint32_t> users_list;

public:
	void updateList(struct data);
	bool isEmpty();
	uint32_t getClientUid(char *login);
};
