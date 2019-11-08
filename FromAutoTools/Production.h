#pragma once
#include <vector>
#include "ProductionSym.h"
//class ProductionSym;
using namespace std;

class Production
{
private:
	ProductionSym* left;
	vector<ProductionSym*>* right;
	int dotIndex = 0;

public:
	bool setLeft(ProductionSym* left);
	bool setRight(vector<ProductionSym*>* right);

	Production(ProductionSym* left, vector<ProductionSym*>* right);
	Production();
	Production(const Production & pro);
	virtual ~Production();
	ProductionSym* getLeft();
	vector<ProductionSym*>* getRight();
	bool theSameAs(Production* pro);
	int getDotIndex();
	void addDotIndex();
};

