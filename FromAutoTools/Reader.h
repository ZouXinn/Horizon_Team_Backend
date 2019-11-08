#pragma once
#include <vector>
#include "Production.h"
#include "ProductionSym.h"
using namespace std;

class Reader
{
private:
	static char* buffer;
	static int length;
	static void readFile(const char* fileName);
	static void Error(const char* detail,int row);
public:
	static vector<Production*>* readProductions(const char* fileName);

};

