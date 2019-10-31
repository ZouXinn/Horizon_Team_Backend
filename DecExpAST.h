#pragma once
#include "AST.h"
class DecExpAST:public AST
{
public:
	int son = -1;//0 表示<var_dec>    1表示  <struct_dec>

	/*c++规定，当一个成员函数被声明为虚函数后，其派生类中的同名函数都自动成为虚函数。
	因此，在子类从新声明该虚函数时，可以加，也可以不加，
	但习惯上每一层声明函数时都加virtual, 使程序更加清晰。*/
	virtual Value* codegen();
};

