#pragma once
#include<iostream>
#include<map>
#include<string>

using namespace std;

class Storage
{
private:
	map <string, uint32_t> data;

public:
	uint8_t getClientUID(string login, uint32_t* _uid);
	uint8_t appendClient(uint32_t UID, string login);
};