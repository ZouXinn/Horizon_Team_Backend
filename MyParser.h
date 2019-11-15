#pragma once
#include <map>
#include <string>
#include "AssignStmtAST.h"
#include "BreakStmtAST.h"
#include "ContinueStmtAST.h"
#include "DecExpAST.h"
#include "DecVarNameAST.h"
#include "DirectTypeSpecifyAST.h"
#include "EndAST.h"
#include "ExpAST.h"
#include "ValListAST.h"
#include "FormalParaItemAST.h"
#include "FormalParaListAST.h"
#include "FuncCallStmtAST.h"
#include "FuncDefineAST.h"
#include "IdItemAST.h"
#include "IdListAST.h"
#include "IfStmtAST.h"
#include "NumberAST.h"
#include "PointerAST.h"
#include "PointerTypeSpecifyAST.h"
#include "ProgramAST.h"
#include "RealParaItemAST.h"
#include "RealParaListAST.h"
#include "ReturnStmtAST.h"
#include "SingleVarDecAST.h"
#include "StmtAST.h"
#include "StmtsAST.h"
#include "StructDecAST.h"
#include "TypeSpecifyAST.h"
#include "ValAST.h"
#include "ExpListAST.h"
#include "VarDecAST.h"
#include "VarListAST.h"
#include "VarNameAST.h"
#include "WhileStmtAST.h"
#include "OtherSymAST.h"
#include "IntAST.h"
#include "CharAST.h"
#include "StringAST.h"
#include "RealAST.h"
#include "OperatorAST.h"
#include "TypeAST.h"
#include "ReturnStmtAST.h"
#include "AST.h"
#include "Variable.h"
/*#include "..//AutoTools/DFATools.h"
#include "..//AutoTools/DFATools.cpp"
#include "..//AutoTools/Reader.h"
#include "..//AutoTools/Reader.cpp"

#include "..//Lexer/MyLexer.h"
#include "..//Lexer/MyLexer.cpp"
#include "..//AutoTools/Production.h"
#include "..//AutoTools/Production.cpp"*/

#include "../src/FromAutoTools/Production.h"
#include "../src/FromAutoTools/Reader.h"
#include "../src/FromAutoTools/ProductionSym.h"
//#include "..//Parser/FromLexer/header.h"
#include "../src/FromLexer/MyLexer.h"
//#include "..//Parser/FromLexer/unitTest.h"

enum Behavior{ S, r, GOTO, acc };
struct Action
{
	Behavior behavior;
	int aim = -1;//若behavior是S或者goto,则表示目标状态；若behavior是r，则表示产生式序号
};
enum Reduce{

};
struct VarIndex
{
	string name;
	int level;
	string funcName;
	FormalParaListAST* formalParaList;
	friend bool operator < (const struct VarIndex& v1, const struct VarIndex& v2);
};
//from AutoTools start
/*enum Token {
	// 终结符
	Plus,// +
	Minus,// -
	Star,// *
	Divide,// /
	Mode,// %
	PlusPlus,// ++
	MinusMinus,// --
	//Star,// *
	More,// >
	Less,// <
	MoreOrEq,// >=
	LessOrEq,// <=
	Eq,// ==
	Neq,// <>
	Or,//  ||
	And,// &&
	Void,// void
	Int,// int
	Real,// real
	Char,// char
	LB,// {
	RB,// }
	LS,// [
	RS,// ]
	LP,// (
	RP,// )
	Comma,// ,
	Semi,// ;
	Dot,// .
	Assign,// =
	Struct,// struct
	If,// if
	Else,// else
	While,// while
	Return,// return
	Continue,//continue
	Break,// break
	Tag,// 标识符
	Const_int,// 常量，如1  1.1  a
	Const_real,
	Const_char,
	Const_string,
	Pointer_Sign,//  ->
	End,//$
	Not// !
};*/
struct DicIndex//用于map使用 : (state,sym) -> string
{
	int state;
	std::string sym;
	friend bool operator < (const struct DicIndex& d1, const struct DicIndex& d2);
};
inline bool operator <(const struct DicIndex& d1, const struct DicIndex& d2) {
	if (d1.state != d2.state) {
		return d1.state < d2.state;
	}
	else {
		if (d1.sym.length() != d2.sym.length()) {
			return d1.sym.length() < d2.sym.length();
		}
		else {
			for (int i = 0; i < d1.sym.length(); i++) {
				if (d1.sym[i] != d2.sym[i]) {
					return d1.sym[i] < d2.sym[i];
				}
			}
			return d1.sym[0] < d2.sym[0];
		}
	}
}
//from AutoTools end

class MyParser
{
private:
	map<DicIndex, string>* SLR1_table;
	vector<AST*>* symStack;//分析符号栈
	vector<int>* stateStack;//状态符号栈
	int stackTop = -1;
	vector<Symbol*>* sourceSymList;//词法分析获得的符号串
	
	map<string, int>* pro2index = nullptr;

	string empty_str;
	vector<Production*>* productions;
	map<Token, string>* token2strP;
	//静态语义检查 start
	map<string, StructDecAST*>* definedStructs;//已经定义了的结构体
	map<string, vector<FuncDefineAST*>>* definedFuncs;//已经定义了的函数
	map<VarIndex, Variable*>* var_table;//变量表    不同函数的变量  普通的变量      //结构体的变量
	
	map<string,int>* usedStructName;//在函数中使用过的结构体名，再和声明的结构体名进行对比   int 是使用时所在的行号
	//静态语义检查 end
	
	//语法分析相关
	void push(AST* symbol, int state);
	AST* pop();
	Action getAction(Symbol* symbol);
	Action getAction(Symbol* symbol, string value);
	void GoTo(AST*& ast, string value);
	void initToken2StrP();
	void readpro2index(const char* proFileName);
	//静态语义检查相关    重定义报错在这几个函数中实现
	void addVarFromVarDec(VarDecAST* varDec);
	void addStructFromStructDec(StructDecAST* structDec);
	void addFuncFromFuncDefine(FuncDefineAST* funcDefine);
	void addDecExp(DecExpAST* decExp);


	//IR start
	AST* root = nullptr;
	//IR end

	//DFAtools 里面的
	static map<DicIndex, string>* readSLR1Table(const char* sourceFileName);

	void clearVarLevelMoreThan(int level);
public:

	MyParser(const char* sourceCodeFile, const char* SLR1File, const char* proFileName,string empty_str,bool isFile);
	virtual ~MyParser();

	void Parse();
	void CodeGen();
	void RunJIT();
};

