#include "DecVarNameAST.h"
#include "FromLexer/MyLexer.h"

DecVarNameAST::DecVarNameAST(IdentifierAST* identifier)
{
	this->identifier = identifier;
}

DecVarNameAST::DecVarNameAST(IdentifierAST* identifier, bool LSRS):DecVarNameAST(identifier)
{
	this->LSRS = LSRS;
}

DecVarNameAST::DecVarNameAST(IdentifierAST* identifier, bool LSRS, IntAST* intAST) : DecVarNameAST(identifier, LSRS)
{
	if (intAST == nullptr)
	{
		throw Exception(ParserEx, 0, "");
	}
	else
	{
		this->intAST = intAST;
	}

}

DecVarNameAST::~DecVarNameAST()
{
	if (this->identifier != nullptr)
	{
		delete this->identifier;
	}
	if (this->intAST != nullptr)
	{
		delete this->intAST;
	}
}

string DecVarNameAST::codegenStr()
{
	return identifier->codegenStr();
}