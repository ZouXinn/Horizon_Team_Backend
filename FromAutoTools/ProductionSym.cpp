#include "ProductionSym.h"



ProductionSym::ProductionSym(string val, bool Vn)
{
	this->val = val;
	this->Vn = Vn;
}

ProductionSym::ProductionSym(ProductionSym* sym)
{
	this->Vn = sym->Vn;
	this->val = sym->val;
}

string ProductionSym::getVal()
{
	return this->val;
}

bool ProductionSym::isVn()
{
	return this->Vn;
}

ProductionSym::~ProductionSym()
{
	// nothing need to do
}
