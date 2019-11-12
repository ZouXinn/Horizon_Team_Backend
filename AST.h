#pragma once
#include <vector>
#include <string>
#include "ClassProject.h"
#include "../src/FromLexer/MyLexer.h"
using namespace std;



class AST
{
public:
	int row = -1;
	string content;
	void setRow(int row);
	virtual ~AST();
	virtual Value* codegen();// virtual Value* codegen() throw (Exception);
	virtual ArrayRef<Value*> codegenArr();
	
};

