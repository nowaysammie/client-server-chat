#pragma once
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string>

using namespace std;


class PackageManager
{
private:

public:

	uint16_t readCommander(struct data);
	struct data parsePackage(char buf[], uint16_t);
	char* fromPackage(struct data);

};