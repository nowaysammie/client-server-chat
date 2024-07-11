#pragma once
#include<iostream>
#include<string>
#include<stdlib.h>
#include<stdio.h>

using namespace std;

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