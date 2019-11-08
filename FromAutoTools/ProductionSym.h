#pragma once
#include <iostream>
#include <string>
using namespace std;
class ProductionSym
{
private:
	string val;
	bool Vn;//Vn = true表示是非终结符

public:
	ProductionSym(string val, bool Vn);
	ProductionSym(ProductionSym* sym);
	string getVal();
	bool isVn();

	~ProductionSym();
};

