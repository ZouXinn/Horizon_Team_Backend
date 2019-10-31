#pragma once
#include "AST.h"
class StmtAST:public AST
{
public:
	int son = -1;//0: <assign_stmt>   1: <func_call_stmt>  2: <if_stmt>(°üÀ¨ifelse)  3: <while_stmt>  4: <return_stmt>  5: <break_stmt>	6:<continue_stmt>	7: <var_dec>
	//0: <assign_stmt>   1: <func_call_stmt>  2: <if_stmt>(°üÀ¨ifelse)  3: <while_stmt>  4: <return_stmt>
	//5: <break_stmt>	6:<continue_stmt>	7: <var_dec>

	virtual Value* codegen()
	{
		return nullptr;
	}
};

