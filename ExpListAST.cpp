#include "ExpListAST.h"
//#include "..//src/Parser/FromLexer/MyLexer.h"
#include "../src/FromLexer/MyLexer.h"
#define STATIC 0
ExpListAST::ExpListAST(ExpAST* exp)
{
	this->exps = new vector<ExpAST*>();
	addExp(exp);
}

void ExpListAST::setTypes(ExpAST* exp)
{
	this->expType = exp->expType;
	this->pointerNum = exp->pointerNum;
	this->structName = exp->structName;
	this->finalToType = exp->finalToType;
}

void ExpListAST::addExp(ExpAST* exp)
{
#ifdef STATIC
	if (this->exps != nullptr)
	{
		if (check(exp))
		{
			if (this->exps->size() == 0)
			{
				setTypes(exp);
			}
			if (this->expType == zx::Type::INT && exp->expType == zx::Type::REAL)
			{
				this->expType = zx::Type::REAL;
			}
			this->exps->push_back(exp);
		}
		else
		{
			throw Exception(StaticSemaEx, exp->row, "列表元素数据类型不一致");
		}
	}
#else
	if (this->exps != nullptr)
	{
		this->exps->push_back(exp);
	}
#endif
}

bool ExpListAST::check(ExpAST* exp)
{
	if (this->exps->size() == 0)//还未赋值
	{
		return true;
	}
	else
	{
		if (this->expType == exp->expType)
		{
			if (this->expType == zx::Type::STRUCT)
			{
				if (this->structName == exp->structName)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else if (this->expType == zx::Type::POINTER)
			{
				if (this->pointerNum == exp->pointerNum && this->finalToType == exp->finalToType)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return true;
			}
		}
		else 
		{
			if (this->expType == zx::Type::REAL && exp->expType == zx::Type::INT || this->expType == zx::Type::INT && exp->expType == zx::Type::REAL)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

ExpListAST::~ExpListAST()
{
	if (exps != nullptr)
	{
		for (int i = 0; i < exps->size(); i++)
		{
			delete exps->at(i);
		}
		delete exps;
	}
}