#pragma once
#include <vector>
#include <string>
#include "ClassProject.h"
using namespace std;



class AST
{
public:
	int row = -1;
	string content;
	void setRow(int row);
	virtual ~AST();
	virtual Value* codegen();
	virtual ArrayRef<Value*> codegenArr();
	
};

