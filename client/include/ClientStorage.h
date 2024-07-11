#ifndef CLIENT_STORAGE_H
#define CLIENT_STORAGE_H

#include<iostream>
#include<stdlib.h>
#include<stdio.h>

using namespace std;

class ClientStorage {
private:
	map<array<char, 50>, uint32_t> users_list;
public:
	void updateList(struct data);
	bool isEmpty();
	uint32_t getClientUid(char* login);
};

#endif