#include "ValListAST.h"
#define STATIC 0
#include "../src/FromLexer/MyLexer.h"

ValListAST::ValListAST(ValAST* val)
{
	this->vals = new vector<ValAST*>();
	addVal(val);
}

void ValListAST::addVal(ValAST* val)
{
	if (this->vals != nullptr)
	{
#ifdef STATIC
		if (vals->size() == 0)
		{
			this->vals->push_back(val);
			if (val->numberAST != nullptr)
			{
				if (val->numberAST->integer != nullptr)
				{
					this->isInt = true;
				}
				else if (val->numberAST->real_number != nullptr)
				{
					this->isReal = true;
				}
			}
			return;
		}
		else
		{
			if (vals->at(0)->charAST == nullptr && vals->at(0)->numberAST == nullptr)//string
			{
				if (val->charAST == nullptr && val->numberAST == nullptr)
				{
					this->vals->push_back(val);
					return;
				}
			}
			else if (vals->at(0)->charAST == nullptr && vals->at(0)->stringAST == nullptr)//number
			{
				if (val->charAST == nullptr && val->stringAST == nullptr)
				{
					if (this->isReal)
					{
						if (val->numberAST->real_number != nullptr || val->numberAST->integer != nullptr)
						{
							this->vals->push_back(val);
							return;
						}
					}
					else if (this->isInt)
					{
						if (val->numberAST->real_number != nullptr)
						{
							this->vals->push_back(val);
							this->isReal = true;
							return;
						}
						else if (val->numberAST->integer != nullptr)
						{
							this->vals->push_back(val);
							return;
						}
					}
				}
			}
			else if (vals->at(0)->numberAST == nullptr && vals->at(0)->stringAST == nullptr)//char
			{
				if (val->numberAST == nullptr && val->stringAST == nullptr)
				{
					this->vals->push_back(val);
					return;
				}
			}
		}
		throw Exception(StaticSemaEx, val->row, "数据列表类型不一致!");
#else
		this->vals->push_back(val);
#endif // STATIC
	}
}

ValListAST::~ValListAST()
{
	if (this->vals != nullptr)
	{
		delete[] this->vals;
	}
}