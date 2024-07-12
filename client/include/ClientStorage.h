<<<<<<< HEAD
#pragma once
#include<iostream>
#include<string>
=======
#ifndef CLIENT_STORAGE_H
#define CLIENT_STORAGE_H

#include<iostream>
>>>>>>> e94e262f96bfc744bf4c577195e2601f6521b53e
#include<stdlib.h>
#include<stdio.h>

using namespace std;

<<<<<<< HEAD
class ClientStorage
{
private:
	struct data
	{

	};
public:

	void updateList(struct data);
	bool isEmpty();
	uint32_t getClientUid(string login);
	
};
=======
class ClientStorage {
private:
	map<array<char, 50>, uint32_t> users_list;
public:
	void updateList(struct data);
	bool isEmpty();
	uint32_t getClientUid(char* login);
};

#endif
>>>>>>> e94e262f96bfc744bf4c577195e2601f6521b53e
