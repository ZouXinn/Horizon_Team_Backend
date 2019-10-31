#include "MyParser.h"
#include <sstream>
#include <fstream>
#include <iostream>
#define STATIC 0
double str2Double(string s)
{
	double d;
	stringstream ss;
	ss << s;
	ss >> d;
	return d;
}
//start  from   DAFTools
int strToInt(string s) {
	int ret = 0;
	for (int i = 0; i < s.length(); i++) {
		if (s[i] > '9' || s[i] < '0') {
			std::cout << "something wrong while reading SLR" << endl;
			system("pause");
			exit(0);
		}
		else {
			int t = s[i] - '0';
			ret *= 10;
			ret += t;
		}
	}
	return ret;
}
map<DicIndex, string>* MyParser::readSLR1Table(const char* sourceFileName)
{
	map<DicIndex, string>* table = new map<DicIndex, string>();
	fstream in;
	in.open(sourceFileName, ios::in);
	in.seekg(0, ios::end);//移动到末尾
	int length = in.tellg();//获得长度
	in.seekg(0, ios::beg);//移动到开头
	char* buffer = new char[length];
	in.read(buffer, length);
	in.close();
	int num = 0;
	char c = *buffer;
	while (c != '$') {
		if (c != '@') {
			std::cout << "@ worng , at func DFATools::readSLR1Table()" << endl;
			std::cout << "something wrong while reading SLR" << endl;
			system("pause");
			exit(0);
		}
		buffer++;
		num++;
		c = *buffer;
		string s = "";
		while (c != '#') {
			s += c;
			buffer++;
			num++;
			c = *buffer;
		}
		DicIndex dicIndex;
		dicIndex.state = strToInt(s);
		s = "";
		buffer++;
		num++;
		c = *buffer;
		while (c != '#') {
			s += c;
			buffer++;
			num++;
			c = *buffer;
		}
		dicIndex.sym = s;
		s = "";
		buffer++;
		num++;
		c = *buffer;
		while (c != '@') {
			s += c;
			buffer++;
			num++;
			c = *buffer;
		}
		buffer++;
		num++;
		if (*buffer != '\n') {
			std::cout << "enter wrong" << endl;
			std::cout << "something wrong while reading SLR" << endl;
			system("pause");
			exit(0);
		}
		else {
			(*table)[dicIndex] = s;
			buffer++;
			num++;
			c = *buffer;
		}
	}
	return table;
}

//end  from   DAFTools
void MyParser::addVarFromVarDec(VarDecAST* varDec)
{

}

void MyParser::addStructFromStructDec(StructDecAST* structDec)
{

}

void MyParser::addFuncFromFuncDefine(FuncDefineAST* funcDefine)
{

}

void MyParser::addDecExp(DecExpAST* decExp)
{

}

MyParser::MyParser(const char* sourceCodeFile, const char* SLR1File, const char* proFileName, string empty_str,bool isFile)
{
	//SLR1_table = DFATools::readSLR1Table("SLR1.txt");
	SLR1_table = MyParser::readSLR1Table(SLR1File);
	symStack = new vector<AST*>();
	stateStack = new vector<int>();
	productions = Reader::readProductions(proFileName);
	readpro2index(proFileName);
	var_table = new map<VarIndex, Variable*>();
	MyLexer* lexer = nullptr;
	
	try {
		lexer = new MyLexer(sourceCodeFile,isFile);
		sourceSymList = lexer->getSymbols();
	}
	catch (Exception e) {
		throw e;
	}

	initToken2StrP();
	definedStructs = new map<string, StructDecAST*>();
	definedFuncs = new map<string, vector<FuncDefineAST*>>();
	var_table = new map<VarIndex, Variable*>();
}


void MyParser::readpro2index(const char* proFileName)
{
	pro2index = new map<string, int>();
	string s = "";
	fstream in;
	in.open(proFileName, ios::in);
	in.seekg(0, ios::end);//移动到末尾
	int length = in.tellg();//获得长度
	in.seekg(0, ios::beg);//移动到开头
	char* buffer = new char[length];
	in.read(buffer, length);
	in.close();
	char* tCp = buffer;
	for (int i = 0; i < productions->size(); i++)
	{
		while (*tCp != '\n')
		{
			s += *tCp;
			tCp++;
		}
		(*pro2index)[s] = i;
		s = "";
		tCp++;
	}
}

MyParser::~MyParser()
{
}
void MyParser::push(AST* ast, int state)
{
	stackTop++;
	symStack->push_back(ast);
	stateStack->push_back(state);
}
AST* MyParser::pop()
{
	AST* ret = (*symStack)[stackTop];
	symStack->pop_back();
	stateStack->pop_back();
	stackTop--;
	return ret;
}
void funcDataHandle(TypeSpecifyAST*& funcTypeAST, IdentifierAST*& funcId, FormalParaListAST*& funcFparaList, int& state, Token token)
{
	switch (state)//0 表示开始 ， 1表示移进type ， 2表示移进id ，3表示移进了(,确认了此为函数定义
	{
	case 0:
		switch (token)
		{
		case Token::Int:
		case Token::Real:
		case Token::Void:
		case Token::Char:
			state = 1;
			break;
		default:
			state = 0;
			funcTypeAST = nullptr;
			funcId = nullptr;
			funcFparaList = nullptr;
			break;
		}
		break;
	case 1:
		switch (token)
		{
		case Token::Star:
			break;
		case Token::Tag:
			state = 2;
			break;
		default:
			state = 0;
			funcTypeAST = nullptr;
			funcId = nullptr;
			funcFparaList = nullptr;
			break;
		}
		break;
	case 2:
		switch (token)
		{
		case Token::LP:
			state = 3;
			break;
		default:
			state = 0;
			funcTypeAST = nullptr;
			funcId = nullptr;
			funcFparaList = nullptr;
			break;
		}
		break;
	case 3:
		break;
	}
}



void reset(TypeSpecifyAST*& funcTypeAST, IdentifierAST*& funcId, FormalParaListAST*& funcFparaList, int& state)
{
	funcTypeAST = nullptr;
	funcId = nullptr;
	funcFparaList = nullptr;
	state = 0;
}

void MyParser::Parse()
{
	int endPointer = sourceSymList->size() - 1;//指向$
	int curPointer = 0;//当前正在分析的符号
	int curLevel = 0;//层数:每经过{ 就自增，经过}就自减
	//静态语义分析部分 start

	TypeSpecifyAST* funcTypeAST = nullptr;
	IdentifierAST* funcId = nullptr;//是 nullptr则证明此时正在最外层，要定义的是全局变量
	FormalParaListAST* funcFparaList = nullptr;//当有规约产生FormalParaListAST的时候将此变量置为其指针
	//当规约产生FuncDefineAST时将state置0，当从1，2，..等状态无法顺利转换到下一状态时，将state置0
	int state = 0;//0 表示开始 ， 1表示移进type ， 2表示移进id ，3表示移进了(,确认了此为函数定义

	int dotOrPointerSign = 0;


	bool justMoveStruct = false;
	string curStructName = "";
	//静态语义分析部分 end
	AST* tree = new EndAST();
	tree->setRow(-1);
	this->push(tree, 0);
	Action action = this->getAction((*sourceSymList)[curPointer]);
	while (action.behavior != acc)//没有接受项目
	{
		if (action.behavior == S)//移进项目
		{
			AST* ast = nullptr;
			string value = (*sourceSymList)[curPointer]->value;
			Token token = (*sourceSymList)[curPointer]->token;
			int row = (*sourceSymList)[curPointer]->row;
			/*if (row == 51)
			{
				cout << endl;
			}*/
			switch (token)
			{
			case Token::Plus:
				ast = new OperatorAST(Op::ADD);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Minus:
				ast = new OperatorAST(Op::MINUS);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Star:
				ast = new OperatorAST(Op::STAR);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Divide:
				ast = new OperatorAST(Op::DIVIDE);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Mode:
				ast = new OperatorAST(Op::MODE);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::PlusPlus:
				ast = new OperatorAST(Op::PLUSPLUS);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::MinusMinus:
				ast = new OperatorAST(Op::MINUSMINUS);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::More:
				ast = new OperatorAST(Op::MORE);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Less:
				ast = new OperatorAST(Op::LESS);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::MoreOrEq:
				ast = new OperatorAST(Op::MOREOREQ);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::LessOrEq:
				ast = new OperatorAST(Op::LESSOREQ);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Eq:
				ast = new OperatorAST(Op::EQ);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Neq:
				ast = new OperatorAST(Op::NEQ);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Or:
				ast = new OperatorAST(Op::OR);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::And:
				ast = new OperatorAST(Op::AND);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Void:
				ast = new TypeAST(zx::Type::VOID);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Int:
				ast = new TypeAST(zx::Type::INT);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Real:
				ast = new TypeAST(zx::Type::REAL);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Char:
				ast = new TypeAST(zx::Type::CHAR);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::LB:
				ast = new OtherSymAST(Token::LB);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				curLevel++;
				break;
			case Token::RB:
				ast = new OtherSymAST(Token::RB);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				curLevel--;
				break;
			case Token::LS:
				ast = new OtherSymAST(Token::LS);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::RS:
				ast = new OtherSymAST(Token::RS);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::LP:
				ast = new OtherSymAST(Token::LP);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::RP:
				ast = new OtherSymAST(Token::RP);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Comma:
				ast = new OtherSymAST(Token::Comma);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Semi:
				ast = new OtherSymAST(Token::Semi);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Dot:
				ast = new OperatorAST(Op::DOT);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Assign:
				ast = new OtherSymAST(Token::Assign);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Struct:
				ast = new OtherSymAST(Token::Struct);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::If:
				ast = new OtherSymAST(Token::If);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Else:
				ast = new OtherSymAST(Token::Else);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::While:
				ast = new OtherSymAST(Token::While);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Return:
				ast = new OtherSymAST(Token::Return);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Continue:
				ast = new OtherSymAST(Token::Continue);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Break:
				ast = new OtherSymAST(Token::Break);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Tag:
				ast = new IdentifierAST(value);
				ast->setRow(row);
				this->push(ast, action.aim);
				if (curLevel == 0)
				{
					funcId = (IdentifierAST*)ast;
				}
				if (curLevel == 0 && justMoveStruct)
				{
					IdentifierAST* id = (IdentifierAST*)(ast);
					curStructName = id->identifier;
				}
				ast = nullptr;
				break;
			case Token::Const_int:
				ast = new IntAST(std::stoi(value, nullptr, 0));
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Const_real:
				ast = new RealAST(str2Double(value));
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Const_char:
				ast = new CharAST(value[0]);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Const_string:
				ast = new StringAST(value);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Pointer_Sign:
				ast = new OperatorAST(Op::POINTERSIGN);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::End:
				ast = new EndAST();
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			case Token::Not:
				ast = new OperatorAST(Op::NOT);
				ast->setRow(row);
				this->push(ast, action.aim);
				ast = nullptr;
				break;
			}
			if (token == Token::Struct)
			{
				justMoveStruct = true;
			}
			else
			{
				justMoveStruct = false;
			}
			if (token == Token::Pointer_Sign || token == Token::Dot)
			{
				dotOrPointerSign = 2;
			}
			else
			{
				dotOrPointerSign--;
				if (dotOrPointerSign < 0)
				{
					dotOrPointerSign = 0;
				}
			}
			curPointer++;
			funcDataHandle(funcTypeAST, funcId, funcFparaList, state, token);//每次移进更新函数状态
			/*if (token == Token::Semi)
			{
				reset(funcTypeAST, funcId, funcFparaList, state);
			}*/
		}
		else if (action.behavior == r)//规约项目
		{
			int length = productions->at(action.aim)->getRight()->size();
			string left = productions->at(action.aim)->getLeft()->getVal();
			if (action.aim == (*pro2index)["<program> @ <dec_exp> <program>"])// 可以检查是否重定义
			{
				//get <program>
				ProgramAST* program = (ProgramAST*)pop();
				//get <dec_exp>
				DecExpAST* decExp = (DecExpAST*)pop();
				program->addDecExpAST(decExp);
				tree = program;
				GoTo(tree, "<program>");
			}
			else if (action.aim == (*pro2index)["<program> @ <func_define> <program>"])// 可以检查是否重定义
			{
				//get <program>
				ProgramAST* program = (ProgramAST*)pop();
				//get <func_define>
				FuncDefineAST* funcDef = (FuncDefineAST*)pop();
				program->addFuncDefineAST(funcDef);
				tree = program;
				GoTo(tree, "<program>");
			}
			else if (action.aim == (*pro2index)["<program> @ <dec_exp>"])//   可以检查是否重定义
			{
				//get <dec_exp>
				DecExpAST* decExp = (DecExpAST*)pop();
				//create <program>
				ProgramAST* program = new ProgramAST();
				program->addDecExpAST(decExp);
				tree = program;
				GoTo(tree, "<program>");
			}
			else if (action.aim == (*pro2index)["<program> @ <func_define>"])//  可以检查是否重定义
			{
				//get <func_define>
				FuncDefineAST* funcDef = (FuncDefineAST*)pop();
				//create <program>
				ProgramAST* program = new ProgramAST();
				program->addFuncDefineAST(funcDef);
				tree = program;
				GoTo(tree, "<program>");
			}
			else if (action.aim == (*pro2index)["<dec_exp> @ <struct_dec>"])//  可以检查是否重定义
			{
				//get <struct_dec>
				StructDecAST* structDec = (StructDecAST*)pop();
				//set son = 1
				structDec->son = 1;
				tree = structDec;
				GoTo(tree, "<dec_exp>");
			}
			else if (action.aim == (*pro2index)["<dec_exp> @ <var_dec>"])//
			{
				//get <var_dec>
				VarDecAST* varDec = (VarDecAST*)pop();
				//set son = 0
				varDec->son = 0;
				tree = varDec;
				GoTo(tree, "<dec_exp>");
			}
			else if (action.aim == (*pro2index)["<func_define> @ <type_specify> identifier ( <formal_para_list> ) { <stmts> }"])//
			{//还可以将定义加入表中并检查是否重定义
				//get }
				OtherSymAST* rB = (OtherSymAST*)pop();
				//get <stmts>
				StmtsAST* stmts = (StmtsAST*)pop();
				//get {
				OtherSymAST* lB = (OtherSymAST*)pop();
				//get )
				OtherSymAST* rP = (OtherSymAST*)pop();
				//get <formal_para_list>
				FormalParaListAST* fparaList = (FormalParaListAST*)pop();
				//get (
				OtherSymAST* lP = (OtherSymAST*)pop();
				//get identifier
				IdentifierAST* id = (IdentifierAST*)pop();
				//get <type_specify>
				TypeSpecifyAST* typeSpecify = (TypeSpecifyAST*)pop();
				//create <func_define>
				FuncDefineAST* funDefine = new FuncDefineAST(typeSpecify, fparaList, stmts, id);
				tree = funDefine;
				
				//静态语义检查 start
#ifdef STATIC
				//检查重定义
				string funcName = id->identifier;
				if (definedFuncs->count(funcName) == 1)//已存在
				{
					for (int i = 0; i < (*definedFuncs)[funcName].size(); i++)//对每个函数定义进行遍历
					{
						FuncDefineAST* funcDef = (*definedFuncs)[funcName][i];
						if (funcDef->formalParaListAST->formalParaItemASTs->size() != funDefine->formalParaListAST->formalParaItemASTs->size())
						{//如果参数个数不同，则continue
							continue;
						}
						else
						{
							if (funDefine->formalParaListAST->formalParaItemASTs->size() == 0) {
								throw Exception(StaticSemaEx, lP->row, "函数重定义");
							}
							for (int j = 0; j < funcDef->formalParaListAST->formalParaItemASTs->size(); j++)//对一个函数定义中的形参列表进行遍历
							{
								FormalParaItemAST* existItem = funcDef->formalParaListAST->formalParaItemASTs->at(j);
								FormalParaItemAST* defItem = funDefine->formalParaListAST->formalParaItemASTs->at(j);
								if (existItem->typeSpecifyAST->son == defItem->typeSpecifyAST->son)
								{
									if (existItem->typeSpecifyAST->son == 0)//direct
									{
										DirectTypeSpecifyAST* directExt = (DirectTypeSpecifyAST*)(existItem->typeSpecifyAST);
										DirectTypeSpecifyAST* directDef = (DirectTypeSpecifyAST*)(defItem->typeSpecifyAST);
										if(directDef->type->type == directExt->type->type)
										{
											if (directExt->type->type == zx::Type::STRUCT)
											{
												if (directExt->structNameIdentifier->identifier == directDef->structNameIdentifier->identifier)
												{//如果结构体名相同，则表示此项相同，继续看下一个参数
													if (j == funcDef->formalParaListAST->formalParaItemASTs->size() - 1)
													{
														throw Exception(StaticSemaEx, lP->row, "函数重定义");
													}
													continue;
												}
												else//有一项不同，则跟当前函数定义不冲突，转向下一个函数定义
												{
													break;
												}
											}
											else//不是结构体，表示这一项相同，则继续看下一个参数
											{
												if (j == funcDef->formalParaListAST->formalParaItemASTs->size() - 1)
												{
													throw Exception(StaticSemaEx, lP->row, "函数重定义");
												}
												continue;
											}
										}
										else//类型不同，转向下一个函数定义
										{
											break;
										}
									}
									else//pointer
									{
										PointerTypeSpecifyAST* pointerTypeSpecifyExt = (PointerTypeSpecifyAST*)(existItem->typeSpecifyAST);
										PointerTypeSpecifyAST* pointerTypeSpecifyDef = (PointerTypeSpecifyAST*)(defItem->typeSpecifyAST);
										if (pointerTypeSpecifyExt->pointerAST->starNum != pointerTypeSpecifyDef->pointerAST->starNum)
										{//指针层数不同，此参数不同，转向下一个函数定义
											break;
										}
										else//指针层数相同
										{
											DirectTypeSpecifyAST* directExt = pointerTypeSpecifyExt->directTypeSpecifyAST;
											DirectTypeSpecifyAST* directDef = pointerTypeSpecifyDef->directTypeSpecifyAST;
											if (directDef->type->type == directExt->type->type)
											{
												if (directExt->type->type == zx::Type::STRUCT)
												{
													if (directExt->structNameIdentifier->identifier == directDef->structNameIdentifier->identifier)
													{//如果结构体名相同，则表示此项相同，继续看下一个参数
														if (j == funcDef->formalParaListAST->formalParaItemASTs->size() - 1)
														{
															throw Exception(StaticSemaEx, lP->row, "函数重定义");
														}
														continue;
													}
													else//有一项不同，则跟当前函数定义不冲突，转向下一个函数定义
													{
														break;
													}
												}
												else//不是结构体，表示这一项相同，则继续看下一个参数
												{
													if (j == funcDef->formalParaListAST->formalParaItemASTs->size() - 1)
													{
														throw Exception(StaticSemaEx, lP->row, "函数重定义");
													}
													continue;
												}
											}
											else//类型不同，转向下一个函数定义
											{
												break;
											}
										}
									}
								}
								else//参数类型不同，break
								{
									break;
								}
							}
						}
					}
				}
				/*
				//检查所有return 语句
				for (int i = 0; i < stmts->stmtASTs->size(); i++)
				{
					StmtAST* stmt = stmts->stmtASTs->at(i);
					if (stmt->son == 4)
					{
						ReturnStmtAST* returnStmt = (ReturnStmtAST*)stmt;
						if (returnStmt->expAST == nullptr)
						{

						}
						else
						{

						}
					}
					else
					{
						continue;
					}
				}
				*/


				//如果没有重定义，则加入definedFuncs
				(*definedFuncs)[funcName].push_back(funDefine);
				//规约之后,退出了该函数的声明，设置state为0
				state = 0;
				funcTypeAST = nullptr;
				funcId = nullptr;
				funcFparaList = nullptr;
#endif // STATIC
				//静态语义检查 end
				delete rB;
				delete lB;
				delete rP;
				delete lP;
				funDefine->setRow(id->row);
				GoTo(tree, "<func_define>");
			}
			else if (action.aim == (*pro2index)["<type_specify> @ <direct_type_specify>"])//
			{
				//get <direct_type_specify>
				DirectTypeSpecifyAST* directTypeSpecify = (DirectTypeSpecifyAST*)pop();
				//set son = 0
				directTypeSpecify->son = 0;
				tree = directTypeSpecify;
				//静态语义检查 start
#ifdef STATIC
				if (state != 3)
				{
					funcTypeAST = directTypeSpecify;
				}
#endif // STATIC
				//静态语义检查 end
				tree->setRow(directTypeSpecify->row);
				GoTo(tree, "<type_specify>");
			}
			else if (action.aim == (*pro2index)["<type_specify> @ <pointer_type_specify>"])//
			{
				//get <pointer_type_specify>
				PointerTypeSpecifyAST* pointerTypeSpecify = (PointerTypeSpecifyAST*)pop();
				//set son = 1
				pointerTypeSpecify->son = 1;
				tree = pointerTypeSpecify;
				//静态语义检查 start
#ifdef STATIC
				if (state != 3)
				{
					funcTypeAST = pointerTypeSpecify;
				}
#endif
				//静态语义检查 end
				GoTo(tree, "<type_specify>");
			}
			else if (action.aim == (*pro2index)["<formal_para_list> @ <formal_para_item> , <formal_para_list>"])//
			{// item 变量名重复的检查在addFormalParaItemAST函数中做
				//get <formal_para_list>
				FormalParaListAST* fparaList = (FormalParaListAST*)pop();
				//get Comma
				OtherSymAST* comma = (OtherSymAST*)pop();
				//get <formal_para_item>
				FormalParaItemAST* fparaItem = (FormalParaItemAST*)pop();
				fparaList->addFormalParaItemAST(fparaItem);
				fparaList->setRow(comma->row);
				//静态语义检查 start
#ifdef STATIC
				VarIndex index;
				index.formalParaList = fparaList;
				index.funcName = funcId->identifier;
				index.level = curLevel + 1;
				index.name = fparaItem->decVarNameAST->identifier->identifier;
				if (var_table->count(index) == 1)
				{
					throw Exception(StaticSemaEx, fparaItem->row, "变量已存在");
				}
				else
				{
					TypeSpecifyAST* typeSpecify = fparaItem->typeSpecifyAST;
					bool isArr = fparaItem->decVarNameAST->LSRS;
					string name = fparaItem->decVarNameAST->identifier->identifier;
					if (typeSpecify->son == 0)//非指针
					{
						DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)typeSpecify;
						if (dTypeSpecify->type->type == zx::Type::STRUCT)//结构体
						{
							string structName = dTypeSpecify->structNameIdentifier->identifier;
							if (definedStructs->count(structName) == 1)
							{
								Variable* var = new Variable(zx::Type::STRUCT, name, structName, isArr);
								(*var_table)[index] = var;
							}
							else
							{
								throw Exception(StaticSemaEx, fparaItem->row, "结构体未定义");
							}
						}
						else//非结构体
						{
							if (dTypeSpecify->type->type == zx::Type::VOID)
							{
								throw Exception(StaticSemaEx, dTypeSpecify->row, "不能定义void类型的变量");
							}
							Variable* var = new Variable(dTypeSpecify->type->type, name, isArr);
							(*var_table)[index] = var;
						}
					}
					else//指针
					{
						PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)typeSpecify;
						DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
						PointerAST* pointer = pTypeSpecify->pointerAST;
						if (dTypeSpecify->type->type == zx::Type::VOID)
						{
							throw Exception(StaticSemaEx, dTypeSpecify->row, "不能定义void类型的指针变量");
						}
						else if (dTypeSpecify->type->type == zx::Type::STRUCT)
						{
							string finalToStructName = dTypeSpecify->structNameIdentifier->identifier;
							if (definedStructs->count(finalToStructName) == 1)
							{

								if (pointer->starNum > 1)
								{
									Variable* var = new Variable(zx::Type::POINTER, name, zx::Type::POINTER, isArr);
									var->setPointerNum(pointer->starNum);
									var->setFinalToType(zx::Type::STRUCT);
									var->setFinalStructName(finalToStructName);
									(*var_table)[index] = var;
								}
								else
								{
									Variable* var = new Variable(zx::Type::POINTER, name, zx::Type::STRUCT, isArr);
									var->setPointerNum(pointer->starNum);
									var->setFinalToType(zx::Type::STRUCT);
									var->setFinalStructName(finalToStructName);
									(*var_table)[index] = var;
								}
							}
							else
							{
								throw Exception(StaticSemaEx, fparaItem->row, "结构体未定义");
							}
						}
						else//一般类型
						{
							if (pointer->starNum > 1)
							{
								Variable* var = new Variable(zx::Type::POINTER, name, zx::Type::POINTER, isArr);
								var->setPointerNum(pointer->starNum);
								var->setFinalToType(dTypeSpecify->type->type);
								(*var_table)[index] = var;
							}
							else
							{
								Variable* var = new Variable(zx::Type::POINTER, name, dTypeSpecify->type->type, isArr);
								var->setPointerNum(pointer->starNum);
								var->setFinalToType(dTypeSpecify->type->type);
								(*var_table)[index] = var;
							}
						}
					}
				}

				funcFparaList = fparaList;
#endif
				//静态语义检查 end
				tree = fparaList;
				//静态语义检查 start
#ifdef STATIC
				funcFparaList = fparaList;
#endif
				//静态语义检查 end
				delete comma;
				GoTo(tree, "<formal_para_list>");
			}
			else if (action.aim == (*pro2index)["<formal_para_list> @ <formal_para_item>"])//
			{
				//get <formal_para_item>
				FormalParaItemAST* fparaItem = (FormalParaItemAST*)pop();
				//create <formal_para_list>
				FormalParaListAST* fparaList = new FormalParaListAST();
				fparaList->addFormalParaItemAST(fparaItem);
				fparaList->setRow(fparaItem->row);
				tree = fparaList;
				//静态语义检查 start
#ifdef STATIC
				VarIndex index;
				index.formalParaList = fparaList;
				index.funcName = funcId->identifier;
				index.level = curLevel + 1;
				index.name = fparaItem->decVarNameAST->identifier->identifier;
				if (var_table->count(index) == 1)
				{
					throw Exception(StaticSemaEx, fparaItem->row, "变量已存在");
				}
				else
				{
					TypeSpecifyAST* typeSpecify = fparaItem->typeSpecifyAST;
					bool isArr = fparaItem->decVarNameAST->LSRS;
					string name = fparaItem->decVarNameAST->identifier->identifier;
					if (typeSpecify->son == 0)//非指针
					{
						DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)typeSpecify;
						if (dTypeSpecify->type->type == zx::Type::STRUCT)//结构体
						{
							string structName = dTypeSpecify->structNameIdentifier->identifier;
							if (definedStructs->count(structName) == 1)
							{
								Variable* var = new Variable(zx::Type::STRUCT, name, structName, isArr);
								(*var_table)[index] = var;
							}
							else
							{
								throw Exception(StaticSemaEx, fparaItem->row, "结构体未定义");
							}
						}
						else//非结构体
						{
							if (dTypeSpecify->type->type == zx::Type::VOID)
							{
								throw Exception(StaticSemaEx, dTypeSpecify->row, "不能定义void类型的变量");
							}
							Variable* var = new Variable(dTypeSpecify->type->type, name, isArr);
							(*var_table)[index] = var;
						}
					}
					else//指针
					{
						PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)typeSpecify;
						DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
						PointerAST* pointer = pTypeSpecify->pointerAST;
						if (dTypeSpecify->type->type == zx::Type::VOID)
						{
							throw Exception(StaticSemaEx, dTypeSpecify->row, "不能定义void类型的指针变量");
						}
						else if (dTypeSpecify->type->type == zx::Type::STRUCT)
						{
							string finalToStructName = dTypeSpecify->structNameIdentifier->identifier;
							if (definedStructs->count(finalToStructName) == 1)
							{
								
								if (pointer->starNum > 1)
								{
									Variable* var = new Variable(zx::Type::POINTER, name, zx::Type::POINTER, isArr);
									var->setPointerNum(pointer->starNum);
									var->setFinalToType(zx::Type::STRUCT);
									var->setFinalStructName(finalToStructName);
									(*var_table)[index] = var;
								}
								else
								{
									Variable* var = new Variable(zx::Type::POINTER, name, zx::Type::STRUCT, isArr);
									var->setPointerNum(pointer->starNum);
									var->setFinalToType(zx::Type::STRUCT);
									var->setFinalStructName(finalToStructName);
									(*var_table)[index] = var;
								}
							}
							else
							{
								throw Exception(StaticSemaEx, fparaItem->row, "结构体未定义");
							}
						}
						else//一般类型
						{
							if (pointer->starNum > 1)
							{
								Variable* var = new Variable(zx::Type::POINTER, name, zx::Type::POINTER, isArr);
								var->setPointerNum(pointer->starNum);
								var->setFinalToType(dTypeSpecify->type->type);
								(*var_table)[index] = var;
							}
							else
							{
								Variable* var = new Variable(zx::Type::POINTER, name, dTypeSpecify->type->type, isArr);
								var->setPointerNum(pointer->starNum);
								var->setFinalToType(dTypeSpecify->type->type);
								(*var_table)[index] = var;
							}
						}
					}
				}

				funcFparaList = fparaList;
#endif
				//静态语义检查 end
				GoTo(tree, "<formal_para_list>");
			}
			else if (action.aim == (*pro2index)["<formal_para_list> @ empty"])//
			{
				//create <formal_para_list>
				FormalParaListAST* fparaList = new FormalParaListAST();
				tree = fparaList;
				//静态语义检查 start
#ifdef STATIC
				funcFparaList = fparaList;
#endif
				//静态语义检查 end
				GoTo(tree, "<formal_para_list>");
			}
			else if (action.aim == (*pro2index)["<formal_para_item> @ <type_specify> <dec_var_name>"])//
			{
				//get <dec_var_name>
				DecVarNameAST* decVarName = (DecVarNameAST*)pop();
				//get <type_specify>
				TypeSpecifyAST* typeSpecify = (TypeSpecifyAST*)pop();
				//create <formal_para_item>
				FormalParaItemAST* fparaItem = new FormalParaItemAST(typeSpecify, decVarName);
				fparaItem->setRow(typeSpecify->row);
				tree = fparaItem;
				GoTo(tree, "<formal_para_item>");
			}
			else if (action.aim == (*pro2index)["<direct_type_specify> @ void"])//
			{
				//get void
				TypeAST* type = (TypeAST*)pop();
				//create <direct_type_specify>
				DirectTypeSpecifyAST* directTypeSpecify = new DirectTypeSpecifyAST(type);
				directTypeSpecify->setRow(type->row);
				tree = directTypeSpecify;
				GoTo(tree, "<direct_type_specify>");
			}
			else if (action.aim == (*pro2index)["<direct_type_specify> @ int"])//
			{
				//get int
				TypeAST* type = (TypeAST*)pop();
				//create <direct_type_specify>
				DirectTypeSpecifyAST* directTypeSpecify = new DirectTypeSpecifyAST(type);
				directTypeSpecify->setRow(type->row);
				tree = directTypeSpecify;
				GoTo(tree, "<direct_type_specify>");
			}
			else if (action.aim == (*pro2index)["<direct_type_specify> @ real"])//
			{
				//get real
				TypeAST* type = (TypeAST*)pop();
				//create <direct_type_specify>
				DirectTypeSpecifyAST* directTypeSpecify = new DirectTypeSpecifyAST(type);
				directTypeSpecify->setRow(type->row);
				tree = directTypeSpecify;
				GoTo(tree, "<direct_type_specify>");
			}
			else if (action.aim == (*pro2index)["<direct_type_specify> @ char"])//
			{
				//get char
				TypeAST* type = (TypeAST*)pop();
				//create <direct_type_specify>
				DirectTypeSpecifyAST* directTypeSpecify = new DirectTypeSpecifyAST(type);
				directTypeSpecify->setRow(type->row);
				tree = directTypeSpecify;
				GoTo(tree, "<direct_type_specify>");
			}
			else if (action.aim == (*pro2index)["<direct_type_specify> @ struct identifier"])//
			{
				//get identifier
				IdentifierAST* id = (IdentifierAST*)pop();
				//get struct
				OtherSymAST* structSym = (OtherSymAST*)pop();
				//create <direct_type_specify>
				DirectTypeSpecifyAST* directTypeSpecify = new DirectTypeSpecifyAST(id);
				directTypeSpecify->setRow(structSym->row);
				TypeAST* type = new TypeAST(zx::Type::STRUCT);
				directTypeSpecify->type = type;
				//静态语义分析 start
#ifdef STATIC
				string name = id->identifier;
				if (definedStructs->count(name) == 0&&name != curStructName)
				{
					string detail = "结构体" + name + "未定义";
					throw Exception(StaticSemaEx, structSym->row, "结构体未定义");
				}
#endif // STATIC
				//静态语义分析 end
				tree = directTypeSpecify;
				delete structSym;
				GoTo(tree, "<direct_type_specify>");
			}
			else if (action.aim == (*pro2index)["<pointer_type_specify> @ <direct_type_specify> <pointer>"])//
			{
				//get <pointer>
				PointerAST* pointer = (PointerAST*)pop();
				//get <direct_type_specify>
				DirectTypeSpecifyAST* directTypeSpecify = (DirectTypeSpecifyAST*)pop();
				//create <pointer_type_specify>
				PointerTypeSpecifyAST* pointerTypeSpecify = new PointerTypeSpecifyAST(directTypeSpecify, pointer);
				pointerTypeSpecify->setRow(pointer->row);
				tree = pointerTypeSpecify;
				GoTo(tree, "<pointer_type_specify>");
			}
			else if (action.aim == (*pro2index)["<pointer> @ *"])//
			{
				//get *
				TypeAST* star = (TypeAST*)pop();
				//create <pointer>
				PointerAST* pointer = new PointerAST(1);
				pointer->setRow(star->row);
				tree = pointer;
				GoTo(tree, "<pointer>");
			}
			else if (action.aim == (*pro2index)["<pointer> @ * <pointer>"])//
			{
				//get <pointer>
				PointerAST* pointer = (PointerAST*)pop();
				//get *
				TypeAST* star = (TypeAST*)pop();
				pointer->addStarNum();
				pointer->setRow(star->row);
				delete star;
				tree = pointer;
				GoTo(tree, "<pointer>");
			}
			else if (action.aim == (*pro2index)["<var_dec> @ <type_specify> <id_list> ;"])//
			{
				//get Semi
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get <id_list>
				IdListAST* idList = (IdListAST*)pop();
				//get <type_specify>
				TypeSpecifyAST* typeSpecify = (TypeSpecifyAST*)pop();
				//create <var_dec>
				VarDecAST* varDec = new VarDecAST(typeSpecify, idList);
				
				//静态语义检查 start
#ifdef STATIC
				if (typeSpecify->son == 0)//非指针
				{
					DirectTypeSpecifyAST* dtypeSpecify = (DirectTypeSpecifyAST*)typeSpecify;
					zx::Type type = dtypeSpecify->type->type;
					if (type == zx::Type::VOID)
					{
						throw Exception(StaticSemaEx, typeSpecify->row, "不能定义void类型的变量");
					}
					if (type == zx::Type::STRUCT)//是结构体
					{
						for (int i = 0; i < idList->idItemASTs->size(); i++)
						{
							IdItemAST* item = idList->idItemASTs->at(i);
							string name = item->decVarNameAST->identifier->identifier;//变量名
							string structName = dtypeSpecify->structNameIdentifier->identifier;
							bool isArr = item->decVarNameAST->LSRS;//是否是数组
							VarIndex index;
							index.name = name;
							index.level = curLevel;
							index.funcName = funcId == nullptr?"":funcId->identifier;
							index.formalParaList = funcFparaList;
							if (var_table->count(index) == 1)//已存在，变量重定义，报错
							{
								throw Exception(ExType::StaticSemaEx, semi->row, "变量重定义");
							}
							if (isArr)//是数组
							{
								if (item->decVarNameAST->intAST == nullptr && item->exps == nullptr)
								{
									throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "未给出数组长度!");
								}
								else if (item->decVarNameAST->intAST != nullptr)
								{
									int length = item->decVarNameAST->intAST->integer;
									if (length <= 0) {
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "数组长度必须大于0!");
									}
									if (item->exps != nullptr)
									{
										if (length < item->exps->exps->size())
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初始化数值太多!");
										}
									}
								}
								Variable* variable = new Variable(type, name, structName, isArr);
								if (item->exps != nullptr)
								{
									if (item->exps->expType != zx::Type::STRUCT || item->exps->structName != dtypeSpecify->structNameIdentifier->identifier)
									{
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
									}
									int arrayLength = 0;
									arrayLength = item->exps->exps->size();
									variable->setArratLength(arrayLength);
								}
								//if (item->exps != nullptr)//没有对数组进行 {}的初始化
								//{
								//	throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "对数组的初始化有问题!");
								//}
								//检查初始化类型是否正确
								//variable->setArray();
								(*var_table)[index] = variable;
							}
							else//不是数组
							{
								if (item->exp != nullptr)
								{
									if (item->exp->expType != zx::Type::STRUCT || item->exp->structName != dtypeSpecify->structNameIdentifier->identifier)
									{
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
									}
								}
								if (item->exps != nullptr)
								{
									throw Exception(StaticSemaEx, item->row, "不能为非数组变量赋数组初值");
								}
								Variable* variable = new Variable(type, name, structName, isArr);
								(*var_table)[index] = variable;
							}
						}
					}
					else//普通变量
					{
						for (int i = 0; i < idList->idItemASTs->size(); i++)
						{
							IdItemAST* item = idList->idItemASTs->at(i);
							string name = item->decVarNameAST->identifier->identifier;//变量名
							bool isArr = item->decVarNameAST->LSRS;//是否是数组
							VarIndex index;
							index.name = name;
							index.level = curLevel;
							index.funcName = funcId == nullptr?"":funcId->identifier;
							index.formalParaList = funcFparaList;
							if (var_table->count(index) == 1)//已存在，变量重定义，报错
							{
								throw Exception(ExType::StaticSemaEx, semi->row, "变量重定义");
							}
							if (isArr)//是数组
							{
								//if (item->exps == nullptr)//没有对数组进行 {}的初始化
								//{
								//	throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "对数组的初始化有问题!");
								//}
								if (item->decVarNameAST->intAST == nullptr && item->exps == nullptr)
								{
									throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "未给出数组长度!");
								}
								else if (item->decVarNameAST->intAST != nullptr)
								{
									int length = item->decVarNameAST->intAST->integer;
									if (length <= 0) {
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "数组长度必须大于0!");
									}
									if (item->exps != nullptr)
									{
										if (length < item->exps->exps->size())
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初始化数值太多!");
										}
									}
								}
								Variable* variable = new Variable(type, name, isArr);
								if (item->exps != nullptr)
								{
									if (item->exps->expType != dtypeSpecify->type->type)
									{
										if (!(dtypeSpecify->type->type == zx::Type::REAL && item->exps->expType == zx::Type::INT))
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
									}
									int arrayLength = 0;
									arrayLength = item->exps->exps->size();
									variable->setArratLength(arrayLength);
								}
								//variable->setArray();
								(*var_table)[index] = variable;
							}
							else//不是数组
							{
								if (item->exp != nullptr)
								{
									if (item->exp->expType != dtypeSpecify->type->type)
									{
										if (!(dtypeSpecify->type->type == zx::Type::REAL && item->exp->expType == zx::Type::INT))
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
									}
								}
								if (item->exps != nullptr)
								{
									throw Exception(StaticSemaEx, item->row, "不能为非数组变量赋数组初值");
								}
								Variable* variable = new Variable(type, name, isArr);
								(*var_table)[index] = variable;
							}
						}
					}
				}
				else if (typeSpecify->son == 1)//指针
				{
					PointerTypeSpecifyAST* ptypeSpecify = (PointerTypeSpecifyAST*)typeSpecify;
					DirectTypeSpecifyAST* dtypeSpecify = ptypeSpecify->directTypeSpecifyAST;
					if (dtypeSpecify->type->type == zx::Type::VOID)
					{
						throw Exception(StaticSemaEx, typeSpecify->row, "不能定义void类型的指针变量");
					}
					PointerAST* pointer = ptypeSpecify->pointerAST;
					if (pointer->starNum > 1)//指针指向的对象是指针
					{
						zx::Type type = zx::Type::POINTER;
						zx::Type toType = zx::Type::POINTER;
						zx::Type finalToType = dtypeSpecify->type->type;
						if (finalToType == zx::Type::STRUCT)//最终指向结构体
						{
							string finalStructName = dtypeSpecify->structNameIdentifier->identifier;
							for (int i = 0; i < idList->idItemASTs->size(); i++)
							{
								IdItemAST* item = idList->idItemASTs->at(i);
								string name = item->decVarNameAST->identifier->identifier;//变量名
								bool isArr = item->decVarNameAST->LSRS;//是否是数组
								VarIndex index;
								index.name = name;
								index.level = curLevel;
								index.funcName = funcId == nullptr ? "" : funcId->identifier;
								index.formalParaList = funcFparaList;
								if (var_table->count(index) == 1)//已存在，变量重定义，报错
								{
									throw Exception(ExType::StaticSemaEx, semi->row, "变量重定义");
								}
								if(isArr)//是数组
								{
									//if (item->exps == nullptr)//没有对数组进行 {}的初始化
									//{
									//	throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "对数组的初始化有问题!");
									//}

									if (item->decVarNameAST->intAST == nullptr && item->exps == nullptr)
									{
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "未给出数组长度!");
									}
									else if (item->decVarNameAST->intAST != nullptr)
									{
										int length = item->decVarNameAST->intAST->integer;
										if (length <= 0) {
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "数组长度必须大于0!");
										}
										if (item->exps != nullptr)
										{
											if (length < item->exps->exps->size())
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初始化数值太多!");
											}
										}
									}
									Variable* variable = new Variable(type, name, toType, isArr);
									if (item->exps != nullptr)
									{
										if (item->exps->pointerNum != pointer->starNum)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										if (item->exps->expType != zx::Type::POINTER)//没有对具体指向什么类型做检查
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exps->finalToType != zx::Type::STRUCT || item->exps->structName != dtypeSpecify->structNameIdentifier->identifier)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										int arrayLength = 0;
										arrayLength = item->exps->exps->size();
										variable->setArratLength(arrayLength);
									}
									variable->setPointerNum(pointer->starNum);
									variable->setFinalToType(zx::Type::STRUCT);
									variable->setFinalStructName(finalStructName);
									(*var_table)[index] = variable;
								}
								else//不是数组
								{
									if (item->exp != nullptr)
									{
										if (item->exp->expType != zx::Type::POINTER)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exp->finalToType != zx::Type::STRUCT || item->exp->structName != dtypeSpecify->structNameIdentifier->identifier)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										if (item->exp->pointerNum != pointer->starNum)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
									}
									if (item->exps != nullptr)
									{
										throw Exception(StaticSemaEx, item->row, "不能为非数组变量赋数组初值");
									}
									Variable* variable = new Variable(type, name, toType, isArr);
									variable->setPointerNum(pointer->starNum);
									variable->setFinalToType(zx::Type::STRUCT);
									variable->setFinalStructName(finalStructName);
									(*var_table)[index] = variable;
								}
							}
						}
						else//最终指向的不是结构体
						{
							for (int i = 0; i < idList->idItemASTs->size(); i++)
							{
								IdItemAST* item = idList->idItemASTs->at(i);
								string name = item->decVarNameAST->identifier->identifier;//变量名
								bool isArr = item->decVarNameAST->LSRS;//是否是数组
								VarIndex index;
								index.name = name;
								index.level = curLevel;
								index.funcName = funcId == nullptr ? "" : funcId->identifier;
								index.formalParaList = funcFparaList;
								if (var_table->count(index) == 1)//已存在，变量重定义，报错
								{
									throw Exception(ExType::StaticSemaEx, semi->row, "变量重定义");
								}
								if (isArr)//是数组
								{
									//if (item->exps == nullptr)//没有对数组进行 {}的初始化
									//{
									//	throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "对数组的初始化有问题!");
									//}
									if (item->decVarNameAST->intAST == nullptr && item->exps == nullptr)
									{
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "未给出数组长度!");
									}
									else if (item->decVarNameAST->intAST != nullptr)
									{
										int length = item->decVarNameAST->intAST->integer;
										if (length <= 0) {
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "数组长度必须大于0!");
										}
										if (item->exps != nullptr)
										{
											if (length < item->exps->exps->size())
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初始化数值太多!");
											}
										}
									}
									Variable* variable = new Variable(type, name, toType, isArr);
									if (item->exps != nullptr)
									{
										if (item->exps->pointerNum != pointer->starNum)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										if (item->exps->expType != zx::Type::POINTER)//没有对具体指向什么类型做检查
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exps->finalToType != dtypeSpecify->type->type)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										int arrayLength = 0;
										arrayLength = item->exps->exps->size();
										variable->setArratLength(arrayLength);
									}
									variable->setPointerNum(pointer->starNum);
									variable->setFinalToType(finalToType);
									(*var_table)[index] = variable;
								}
								else//不是数组
								{
									if (item->exp != nullptr)
									{
										if (item->exp->expType != zx::Type::POINTER)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exp->finalToType != dtypeSpecify->type->type)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										if (item->exp->pointerNum != pointer->starNum)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
									}
									if (item->exps != nullptr)
									{
										throw Exception(StaticSemaEx, item->row, "不能为非数组变量赋数组初值");
									}
									Variable* variable = new Variable(type, name, toType, isArr);
									variable->setPointerNum(pointer->starNum);
									variable->setFinalToType(finalToType);
									(*var_table)[index] = variable;
								}
							}
						}
						//string finalStructName = dtypeSpecify->
					}
					else if(pointer->starNum == 1)//指向的对象不是指针
					{
						zx::Type type = zx::Type::POINTER;
						zx::Type toType = dtypeSpecify->type->type;
						zx::Type finalToType = toType;
						if (finalToType == zx::Type::STRUCT)//最终指向结构体
						{
							string finalStructName = dtypeSpecify->structNameIdentifier->identifier;
							for (int i = 0; i < idList->idItemASTs->size(); i++)
							{
								IdItemAST* item = idList->idItemASTs->at(i);
								string name = item->decVarNameAST->identifier->identifier;//变量名
								bool isArr = item->decVarNameAST->LSRS;//是否是数组
								VarIndex index;
								index.name = name;
								index.level = curLevel;
								index.funcName = funcId == nullptr ? "" : funcId->identifier;
								index.formalParaList = funcFparaList;
								if (var_table->count(index) == 1)//已存在，变量重定义，报错
								{
									throw Exception(ExType::StaticSemaEx, semi->row, "变量重定义");
								}
								if (isArr)//是数组
								{
									//if (item->exps == nullptr)//没有对数组进行 {}的初始化
									//{
									//	throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "对数组的初始化有问题!");
									//}
									if (item->decVarNameAST->intAST == nullptr && item->exps == nullptr)
									{
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "未给出数组长度!");
									}
									else if (item->decVarNameAST->intAST != nullptr)
									{
										int length = item->decVarNameAST->intAST->integer;
										if (length <= 0) {
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "数组长度必须大于0!");
										}
										if (item->exps != nullptr)
										{
											if (length < item->exps->exps->size())
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初始化数值太多!");
											}
										}
									}
									Variable* variable = new Variable(type, name, toType, isArr);
									if (item->exps != nullptr)
									{
										if (item->exps->expType != zx::Type::POINTER)//没有对具体指向什么类型做检查
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exps->finalToType != zx::Type::STRUCT || item->exps->structName != dtypeSpecify->structNameIdentifier->identifier)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exps->pointerNum != pointer->starNum)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										int arrayLength = 0;
										arrayLength = item->exps->exps->size();
										variable->setArratLength(arrayLength);
									}
									variable->setPointerNum(pointer->starNum);
									variable->setFinalToType(zx::Type::STRUCT);
									variable->setFinalStructName(finalStructName);
									(*var_table)[index] = variable;
								}
								else//不是数组
								{
									if (item->exp != nullptr)
									{
										if (item->exp->expType != zx::Type::POINTER)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exp->finalToType != zx::Type::STRUCT || item->exp->structName != dtypeSpecify->structNameIdentifier->identifier)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exp->pointerNum != pointer->starNum)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
									}
									if (item->exps != nullptr)
									{
										throw Exception(StaticSemaEx, item->row, "不能为非数组变量赋数组初值");
									}
									Variable* variable = new Variable(type, name, toType, isArr);
									variable->setPointerNum(pointer->starNum);
									variable->setFinalToType(zx::Type::STRUCT);
									variable->setFinalStructName(finalStructName);
									(*var_table)[index] = variable;
								}
							}
						}
						else if (finalToType == zx::Type::CHAR) {
							for (int i = 0; i < idList->idItemASTs->size(); i++) {
								IdItemAST* item = idList->idItemASTs->at(i);
								string name = item->decVarNameAST->identifier->identifier;//变量名
								bool isArr = item->decVarNameAST->LSRS;//是否是数组
								VarIndex index;
								index.name = name;
								index.level = curLevel;
								index.funcName = funcId == nullptr ? "" : funcId->identifier;
								index.formalParaList = funcFparaList;
								if (var_table->count(index) == 1)//已存在，变量重定义，报错
								{
									throw Exception(ExType::StaticSemaEx, semi->row, "变量重定义");
								}
								if (isArr) {
									if (item->decVarNameAST->intAST == nullptr && item->exps == nullptr)
									{
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "未给出数组长度!");
									}
									else if (item->decVarNameAST->intAST != nullptr)
									{
										int length = item->decVarNameAST->intAST->integer;
										if (length <= 0) {
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "数组长度必须大于0!");
										}
										if (item->exps != nullptr)
										{
											if (length < item->exps->exps->size())
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初始化数值太多!");
											}
										}
									}
									if (item->exps!= nullptr && item->exps->expType == zx::Type::STRING) {//char* cArr[] = {"aa","bb"}
										Variable* variable = new Variable(type, name, toType, isArr);
										int arrayLength = 0;
										arrayLength = item->exps->exps->size();
										variable->setArratLength(arrayLength);
										variable->setPointerNum(pointer->starNum);
										variable->setFinalToType(finalToType);
										(*var_table)[index] = variable;
									}
									else {//char *
										Variable* variable = new Variable(type, name, toType, isArr);
										if (item->exps != nullptr)
										{
											if (item->exps->expType != zx::Type::POINTER)//没有对具体指向什么类型做检查
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
											}
											else if (item->exps->finalToType != dtypeSpecify->type->type)
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
											}
											else if (item->exps->pointerNum != pointer->starNum)
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
											}
											int arrayLength = 0;
											arrayLength = item->exps->exps->size();
											variable->setArratLength(arrayLength);
										}
										variable->setPointerNum(pointer->starNum);
										variable->setFinalToType(finalToType);
										(*var_table)[index] = variable;
									}
								}
								else {//不是数组
									if (item->exp != nullptr && item->exp->expType == zx::Type::STRING) {//char * a = string
										Variable* variable = new Variable(type, name, toType, isArr);
										variable->setPointerNum(pointer->starNum);
										variable->setFinalToType(finalToType);
										(*var_table)[index] = variable;
									}
									else {//char * a = char*
										if (item->exp != nullptr)
										{
											if (item->exp->expType != zx::Type::POINTER)
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
											}
											else if (item->exp->finalToType != dtypeSpecify->type->type)
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
											}
											else if (item->exp->pointerNum != pointer->starNum)
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
											}
										}
										if (item->exps != nullptr)
										{
											throw Exception(StaticSemaEx, item->row, "不能为非数组变量赋数组初值");
										}
										Variable* variable = new Variable(type, name, toType, isArr);
										variable->setPointerNum(pointer->starNum);
										variable->setFinalToType(finalToType);
										(*var_table)[index] = variable;
									}
								}
							}

						}
						else//最终指向的不是结构体
						{
							for (int i = 0; i < idList->idItemASTs->size(); i++)
							{
								IdItemAST* item = idList->idItemASTs->at(i);
								string name = item->decVarNameAST->identifier->identifier;//变量名
								bool isArr = item->decVarNameAST->LSRS;//是否是数组
								VarIndex index;
								index.name = name;
								index.level = curLevel;
								index.funcName = funcId == nullptr ? "" : funcId->identifier;
								index.formalParaList = funcFparaList;
								if (var_table->count(index) == 1)//已存在，变量重定义，报错
								{
									throw Exception(ExType::StaticSemaEx, semi->row, "变量重定义");
								}
								if (isArr)//是数组
								{
									//if (item->exps == nullptr)//没有对数组进行 {}的初始化
									//{
									//	throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "对数组的初始化有问题!");
									//}
									if (item->decVarNameAST->intAST == nullptr && item->exps == nullptr)
									{
										throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "未给出数组长度!");
									}
									else if (item->decVarNameAST->intAST != nullptr)
									{
										int length = item->decVarNameAST->intAST->integer;
										if (length <= 0) {
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "数组长度必须大于0!");
										}
										if (item->exps != nullptr)
										{
											if (length < item->exps->exps->size())
											{
												throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初始化数值太多!");
											}
										}
									}
									Variable* variable = new Variable(type, name, toType, isArr);
									if (item->exps != nullptr)
									{
										if (item->exps->expType != zx::Type::POINTER)//没有对具体指向什么类型做检查
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exps->finalToType != dtypeSpecify->type->type)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exps->pointerNum != pointer->starNum)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										int arrayLength = 0;
										arrayLength = item->exps->exps->size();
										variable->setArratLength(arrayLength);
									}
									variable->setPointerNum(pointer->starNum);
									variable->setFinalToType(finalToType);
									(*var_table)[index] = variable;
								}
								else//不是数组
								{
									if (item->exp != nullptr)
									{
										if (item->exp->expType != zx::Type::POINTER)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exp->finalToType != dtypeSpecify->type->type)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
										else if (item->exp->pointerNum != pointer->starNum)
										{
											throw Exception(ExType::StaticSemaEx, item->decVarNameAST->identifier->row, "初值类型不匹配!");
										}
									}
									if (item->exps != nullptr)
									{
										throw Exception(StaticSemaEx, item->row, "不能为非数组变量赋数组初值");
									}
									Variable* variable = new Variable(type, name, toType, isArr);
									variable->setPointerNum(pointer->starNum);
									variable->setFinalToType(finalToType);
									(*var_table)[index] = variable;
								}
							}
						}
					}
				}
#endif
				//静态语义检查 end
				varDec->setRow(semi->row);
				tree = varDec;
				delete semi;
				GoTo(tree, "<var_dec>");
			}
			else if (action.aim == (*pro2index)["<id_list> @ <id_item> , <id_list>"])//    //add进去的顺序与实际顺序相反
			{
				//get <id_list>
				IdListAST* idList = (IdListAST*)pop();
				//get comma
				OtherSymAST* comma = (OtherSymAST*)pop();
				//get <id_item>
				IdItemAST* item = (IdItemAST*)pop();
				idList->addIdItem(item);
				idList->setRow(comma->row);
				tree = idList;
				GoTo(tree, "<id_list>");
			}
			else if (action.aim == (*pro2index)["<id_list> @ <id_item>"])//
			{
				//get <id_item>
				IdItemAST* item = (IdItemAST*)pop();
				//create <id_list>
				IdListAST* idList = new IdListAST(item);
				idList->setRow(item->row);
				tree = idList;
				GoTo(tree, "<id_list>");
			}
			else if (action.aim == (*pro2index)["<id_item> @ <dec_var_name>"])//
			{
				//get <dec_var_name>
				DecVarNameAST* decVarName = (DecVarNameAST*)pop();
				//create <id_item>
				IdItemAST* idItem = new IdItemAST(decVarName);
				idItem->setRow(decVarName->row);
				tree = idItem;
				GoTo(tree, "<id_item>");
			}
			else if (action.aim == (*pro2index)["<id_item> @ <dec_var_name> = <exp>"])//
			{
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//get =
				OtherSymAST* assign = (OtherSymAST*)pop();
				//get <dec_var_name>
				DecVarNameAST* decVarName = (DecVarNameAST*)pop();
				//create <id_item>
				IdItemAST* idItem = new IdItemAST(decVarName, exp);
				idItem->setRow(assign->row);
				tree = idItem;
				delete assign;
				GoTo(tree, "<id_item>");
			}
			else if (action.aim == (*pro2index)["<id_item> @ <dec_var_name> = { <exp_list> }"])//
			{
				//get }
				OtherSymAST* rB = (OtherSymAST*)pop();
				//get <val_list>
				ExpListAST* expList = (ExpListAST*)pop();
				//get {
				OtherSymAST* lB = (OtherSymAST*)pop();
				//get =
				OtherSymAST* assign = (OtherSymAST*)pop();
				//get <dec_var_name>
				DecVarNameAST* decVarName = (DecVarNameAST*)pop();
				//create <id_item>
				IdItemAST* idItem = new IdItemAST(decVarName, expList);
				idItem->setRow(assign->row);
				delete rB;
				delete lB;
				delete assign;
				tree = idItem;
				GoTo(tree, "<id_item>");
			}
			else if (action.aim == (*pro2index)["<dec_var_name> @ identifier"])//
			{
				//get identifier
				IdentifierAST* id = (IdentifierAST*)pop();
				//create <dec_var_name>
				DecVarNameAST* decVarName = new DecVarNameAST(id);
				decVarName->setRow(id->row);
				tree = decVarName;
				GoTo(tree, "<dec_var_name>");
			}
			else if (action.aim == (*pro2index)["<dec_var_name> @ identifier [ ]"])//
			{
				//get ]
				OtherSymAST* rS = (OtherSymAST*)pop();
				//get [
				OtherSymAST* lS = (OtherSymAST*)pop();
				//get identifier
				IdentifierAST* id = (IdentifierAST*)pop();
				//create <dec_var_name>
				DecVarNameAST* decVarName = new DecVarNameAST(id, true);
				decVarName->setRow(id->row);
				tree = decVarName;
				delete rS;
				delete lS;
				GoTo(tree, "<dec_var_name>");
			}
			else if (action.aim == (*pro2index)["<dec_var_name> @ identifier [ integer ]"])//
			{
				//get ]
				OtherSymAST* rS = (OtherSymAST*)pop();
				//get integer
				IntAST* integer = (IntAST*)pop();
				//get [
				OtherSymAST* lS = (OtherSymAST*)pop();
				//get identifier
				IdentifierAST* id = (IdentifierAST*)pop();
				//create <dec_var_name>
				DecVarNameAST* decVarName = new DecVarNameAST(id, true,integer);
				decVarName->setRow(id->row);
				tree = decVarName;
				delete rS;
				delete lS;
				GoTo(tree, "<dec_var_name>");
			}
			else if (action.aim == (*pro2index)["<struct_dec> @ struct identifier { <var_list> } ;"])//
			{
				//get Semi
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get }
				OtherSymAST* rB = (OtherSymAST*)pop();
				//get <var_list>
				VarListAST* varList = (VarListAST*)pop();
				//get {
				OtherSymAST* lB = (OtherSymAST*)pop();
				// get identifier
				IdentifierAST* id = (IdentifierAST*)pop();
				//get struct
				OtherSymAST* structSym = (OtherSymAST*)pop();
				//create <struct_dec>
				StructDecAST* structDec = new StructDecAST(id, varList);
				//静态语义检查 start
#ifdef STATIC
				string structName = id->identifier;
				if (definedStructs->count(structName) == 1)//重定义
				{
					throw Exception(StaticSemaEx, structSym->row, "结构体重定义");
				}
				else//没有重定义，加入definedStructs
				{
					map<string, int> alreadyVar = map<string, int>();
					for (int i = 0; i < varList->singleVarDecASTs->size(); i++)
					{
						string name = varList->singleVarDecASTs->at(i)->decVarNameAST->identifier->identifier;
						if (alreadyVar.count(name) == 1)
						{
							throw Exception(StaticSemaEx, varList->singleVarDecASTs->at(i)->row, "成员变量重定义");
						}
						else
						{
							alreadyVar[name] = 1;
						}
					}

					(*definedStructs)[structName] = structDec;
				}
				curStructName = "";
#endif // STATIC
				//静态语义检查 end
				structDec->setRow(structSym->row);
				delete rB;
				delete lB;
				delete structSym;
				tree = structDec;
				GoTo(tree, "<struct_dec>");
			}
			else if (action.aim == (*pro2index)["<var_list> @ <single_var_dec> <var_list>"])//
			{
				//get <var_list>
				VarListAST* varList = (VarListAST*)pop();
				//get <single_var_dec>
				SingleVarDecAST* singleVarDec = (SingleVarDecAST*)pop();
				varList->addSingleVarDecAST(singleVarDec);
				tree = varList;
				GoTo(tree, "<var_list>");
			}
			else if (action.aim == (*pro2index)["<var_list> @ <single_var_dec>"])//
			{
				//get <single_var_dec>
				SingleVarDecAST* singleVarDec = (SingleVarDecAST*)pop();
				//create <var_list>
				VarListAST* varList = new VarListAST(singleVarDec);
				varList->setRow(singleVarDec->row);
				tree = varList;
				GoTo(tree, "<var_list>");
			}
			else if (action.aim == (*pro2index)["<single_var_dec> @ <type_specify> <dec_var_name> ;"])//
			{
				//get Semi
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get <dec_var_name> 
				DecVarNameAST* decVarName = (DecVarNameAST*)pop();
				//get <type_specify>
				TypeSpecifyAST* type = (TypeSpecifyAST*)pop();
				//create <single_var_dec>
				SingleVarDecAST* singleVarDec = new SingleVarDecAST(type, decVarName);
				//静态语义分析 start
#ifdef STATIC
				if (type->son == 0)
				{
					DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)type;
					if (dTypeSpecify->type->type == zx::Type::VOID)
					{
						throw Exception(StaticSemaEx, type->row, "不能定义void类型的变量");
					}
					else if (dTypeSpecify->type->type == zx::Type::STRUCT)
					{
						throw Exception(StaticSemaEx, semi->row, "结构体内不能声明结构体变量!");
					}
				}
				else
				{
					PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)type;
					DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
					if (dTypeSpecify->type->type == zx::Type::VOID)
					{
						throw Exception(StaticSemaEx, type->row, "不能定义void类型的指针变量");
					}
				}
#endif // STATIC
				//静态语义分析 end
				singleVarDec->setRow(semi->row);
				tree = singleVarDec;
				delete semi;
				GoTo(tree, "<single_var_dec>");
			}
			else if (action.aim == (*pro2index)["<single_var_dec> @ <type_specify> <dec_var_name> = <val> ;"])//
			{
				//get Semi
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get <val>
				ValAST* val = (ValAST*)pop();
				//get assign
				OtherSymAST* assign = (OtherSymAST*)pop();
				//get <dec_var_name> 
				DecVarNameAST* decVarName = (DecVarNameAST*)pop();
				//get <type_specify>
				TypeSpecifyAST* type = (TypeSpecifyAST*)pop();
				//create <single_var_dec>
				SingleVarDecAST* singleVarDec = new SingleVarDecAST(type, decVarName,val);
#ifdef STATIC
				if (type->son == 1)
				{
					if (val->stringAST != nullptr) {
						PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)type;
						DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
						PointerAST* pointer = pTypeSpecify->pointerAST;
						if (pointer->starNum != 1) {
							throw Exception(StaticSemaEx, assign->row, "结构体内的二维指针变量无法赋初值!");
						}
						else if (val->stringAST == nullptr) {
							throw Exception(StaticSemaEx, assign->row, "赋值符号两边类型不匹配!");
						}
					}
					else {
						throw Exception(StaticSemaEx, assign->row, "结构体内的指针变量(char* 除外)无法赋初值!");
					}
				}
				else
				{
					DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)type;
					if (dTypeSpecify->type->type == zx::Type::VOID)
					{
						throw Exception(StaticSemaEx, type->row, "不能定义void类型的变量");
					}
					if (dTypeSpecify->type->type == zx::Type::STRUCT)
					{
						throw Exception(StaticSemaEx, assign->row, "结构体内不能声明结构体变量!");
					}
				}
				if (val->charAST != nullptr)
				{
					if (type->son != 0 || ((DirectTypeSpecifyAST*)type)->type->type != zx::Type::CHAR)
					{
						throw Exception(StaticSemaEx, assign->row, "赋值符号两边类型不匹配！");
					}
				}
				else if (val->numberAST != nullptr)
				{
					if (val->numberAST->integer == nullptr)//real
					{
						if (type->son != 0 || ((DirectTypeSpecifyAST*)type)->type->type != zx::Type::REAL)
						{
							throw Exception(StaticSemaEx, assign->row, "赋值符号两边类型不匹配！");
						}
					}
					else//int
					{
						if (type->son != 0 || ((DirectTypeSpecifyAST*)type)->type->type != zx::Type::INT && ((DirectTypeSpecifyAST*)type)->type->type != zx::Type::REAL)
						{
							throw Exception(StaticSemaEx, assign->row, "赋值符号两边类型不匹配！");
						}
					}
				}
				else if (val->stringAST != nullptr)
				{
					if (type->son == 0)
					{
						throw Exception(StaticSemaEx, assign->row, "赋值符号两边类型不匹配！");
					}
					else// == 1
					{
						PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)type;
						DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
						PointerAST* pointer = pTypeSpecify->pointerAST;
						if (pointer->starNum != 1 || dTypeSpecify->type->type != zx::Type::CHAR)
						{
							throw Exception(StaticSemaEx, assign->row, "赋值符号两边类型不匹配！");
						}
					}
				}
#endif
				singleVarDec->setRow(assign->row);
				tree = singleVarDec;
				delete semi;
				delete assign;
				GoTo(tree, "<single_var_dec>");
			}
			else if (action.aim == (*pro2index)["<single_var_dec> @ <type_specify> <dec_var_name> = { <val_list> } ;"])
			{
				//get Semi
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get rB
				OtherSymAST* rB = (OtherSymAST*)pop();
				//get <val_list>
				ValListAST* valList = (ValListAST*)pop();
				//get lB
				OtherSymAST* lB = (OtherSymAST*)pop();
				//get assign
				OtherSymAST* assign = (OtherSymAST*)pop();
				//get <dec_var_name> 
				DecVarNameAST* decVarName = (DecVarNameAST*)pop();
				//get <type_specify>
				TypeSpecifyAST* type = (TypeSpecifyAST*)pop();
				//create <single_var_dec>
				SingleVarDecAST* singleVarDec = new SingleVarDecAST(type, decVarName, valList);
#ifdef STATIC
				if (!decVarName->LSRS)
				{
					throw Exception(StaticSemaEx, assign->row, "无法将非数组变量赋值为数组");
				}
				if (decVarName->intAST != nullptr)
				{
					int arrLength = decVarName->intAST->integer;
					if (arrLength <= 0) {
						throw Exception(StaticSemaEx, lB->row, "数组长度必须大于0!");
					}
					if (valList->vals->size() > arrLength)
					{
						throw Exception(StaticSemaEx, lB->row, "给出的初始值过多!");
					}
				}
				if (type->son == 1)//指针
				{
					throw Exception(StaticSemaEx, assign->row, "指针类型的数组变量在结构体声明中无法赋初值!");
				}
				else
				{
					DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)type;
					if (dTypeSpecify->type->type == zx::Type::VOID)
					{
						throw Exception(StaticSemaEx, type->row, "不能定义void类型的变量");
					}
					if (dTypeSpecify->type->type == zx::Type::STRUCT)//是结构体
					{
						throw Exception(StaticSemaEx, assign->row, "结构体内无法嵌套定义结构体，如有需要，请定义该类型的指针!");
					}
					else
					{
						if (dTypeSpecify->type->type == zx::Type::INT)
						{
							if (valList->vals->size() <= 0)//应该不会进入
							{
								throw Exception(StaticSemaEx, assign->row, "赋值列表为空");
							}
							else
							{
								ValAST* val = valList->vals->at(0);
								if (val->numberAST == nullptr || val->numberAST->integer == nullptr)
								{
									throw Exception(StaticSemaEx, assign->row, "赋值号两边类型不匹配");
								}
							}
						}
						else if (dTypeSpecify->type->type == zx::Type::REAL)
						{
							if (valList->vals->size() <= 0)//应该不会进入
							{
								throw Exception(StaticSemaEx, assign->row, "赋值列表为空");
							}
							else
							{
								if (!valList->isReal && !valList->isInt)
								{
									throw Exception(StaticSemaEx, assign->row, "赋值号两边类型不匹配");
								}
								/*ValAST* val = valList->vals->at(0);
								if (val->numberAST == nullptr || val->numberAST->real_number == nullptr)
								{
									throw Exception(StaticSemaEx, assign->row, "赋值号两边类型不匹配");
								}*/
							}
						}
						else if (dTypeSpecify->type->type == zx::Type::CHAR)
						{
							if (valList->vals->size() <= 0)//应该不会进入
							{
								throw Exception(StaticSemaEx, assign->row, "赋值列表为空");
							}
							else
							{
								ValAST* val = valList->vals->at(0);
								if (val->charAST == nullptr)
								{
									throw Exception(StaticSemaEx, assign->row, "赋值号两边类型不匹配");
								}
							}
						}
						else if (dTypeSpecify->type->type == zx::Type::STRING)
						{
							if (valList->vals->size() <= 0)//应该不会进入
							{
								throw Exception(StaticSemaEx, assign->row, "赋值列表为空");
							}
							else
							{
								ValAST* val = valList->vals->at(0);
								if (val->stringAST == nullptr)
								{
									throw Exception(StaticSemaEx, assign->row, "赋值号两边类型不匹配");
								}
							}
						}
					}
				}

#endif 
				singleVarDec->setRow(assign->row);
				tree = singleVarDec;
				delete semi;
				delete assign;
				GoTo(tree, "<single_var_dec>");
			}
			else if (action.aim == (*pro2index)["<val_list> @ <val> , <val_list>"])
			{
				//get <val_list>
				ValListAST* valList = (ValListAST*)pop();
				//get comma
				OtherSymAST* comma = (OtherSymAST*)pop();
				//get <val>
				ValAST* val = (ValAST*)pop();
				//add
				valList->addVal(val);
				valList->setRow(comma->row);
#ifdef STATIC

				//已经在ValListAST类内部实现

#endif 
				tree = valList;
				delete comma;
				GoTo(tree, "<val_list>");
			}
			else if (action.aim == (*pro2index)["<val_list> @ <val>"])
			{
				//get <val>
				ValAST* val = (ValAST*)pop();
				//create <val_list>
				ValListAST* valList = new ValListAST(val);
				valList->setRow(val->row);
				tree = valList;
				GoTo(tree, "<val_list>");
			}
			else if (action.aim == (*pro2index)["<val> @ <number>"])//
			{
				//get <number>
				NumberAST* number = (NumberAST*)pop();
				//create <val>
				ValAST* val = new ValAST(number);
				val->setRow(number->row);
				tree = val;
				GoTo(tree, "<val>");
			}
			else if (action.aim == (*pro2index)["<val> @ string"])//
			{
				//get string
				StringAST* str = (StringAST*)pop();
				//create <val>
				ValAST* val = new ValAST(str);
				val->setRow(str->row);
				tree = val;
				GoTo(tree, "<val>");
			}
			else if (action.aim == (*pro2index)["<val> @ Achar"])//
			{
				//get Achar
				CharAST* Achar = (CharAST*)pop();
				//create <val>
				ValAST* val = new ValAST(Achar);
				val->setRow(Achar->row);
				tree = val;
				GoTo(tree, "<val>");
			}
			else if (action.aim == (*pro2index)["<var_name> @ identifier"])//
			{
				//get identifier
				IdentifierAST* id = (IdentifierAST*)pop();
				//create <var_name>
				VarNameAST* varName = new VarNameAST(id);
				//静态语义检查 start
#ifdef STATIC
				if (dotOrPointerSign == 1)//identifier前面有->或.符号
				{
					//nothing to do?
				}
				else
				{
					bool find = false;
					for (int i = curLevel; i >= 0; i--)
					{
						VarIndex index;
						index.name = id->identifier;
						index.funcName = i==0?"":funcId->identifier;
						index.level = i;
						index.formalParaList = i == 0?nullptr:funcFparaList;
						if (var_table->count(index) == 0)
						{
							continue;
						}
						else
						{
							find = true;
							Variable* var = var_table->at(index);
							varName->setType(var->getType());
							varName->isArray = var->IsArray();
							if (var->getType() == zx::Type::POINTER)
							{
								varName->setPointerNum(var->getPointerNum());
								varName->setFinalToType(var->getFinalToType());
								if (var->getFinalToType() == zx::Type::STRUCT)
								{
									varName->setStructName(var->getFinalStructName());
								}
							}
							else if (var->getType() == zx::Type::STRUCT)
							{
								varName->setStructName(var->getStructName());
							}
							break;
						}
					}
					if (!find)//////////////////有问题???
					{
						throw Exception(StaticSemaEx, id->row, "变量不存在");
					}
				}
#endif
				//静态语义检查 end
				varName->setRow(id->row);
				tree = varName;
				GoTo(tree, "<var_name>");
			}
			else if (action.aim == (*pro2index)["<var_name> @ <var_name> -> <var_name>"])//
			{
				//get <var_name> r
				VarNameAST* rightName = (VarNameAST*)pop();
				//get ->
				OperatorAST* op = (OperatorAST*)pop();
				//get <var_name> l
				VarNameAST* leftName = (VarNameAST*)pop();
				//create <var_name>
				VarNameAST* varName = new VarNameAST(leftName, rightName, 2);
				//静态语义检查 start
#ifdef STATIC
				if (rightName->type == 0)//右边的<var_name>为identifier
				{
					//左边的<var_name>已经检查过变量是否存在，故只需要检查右边的变量是否是左边的结构体的成员变量
					if (leftName->varType != zx::Type::POINTER)
					{
						throw Exception(StaticSemaEx, leftName->row, "不是指针的变量不能使用\"->\"符号");
					}
					else if (leftName->pointerNum != 1)
					{
						throw Exception(StaticSemaEx, leftName->row, "不是一维指针的变量不能使用\"->\"符号");
					}
					else
					{
						string structName = leftName->structName;
						bool find = false;
						StructDecAST* structDec = definedStructs->at(structName);
						for (int i = 0; i < structDec->varList->singleVarDecASTs->size(); i++)
						{
							SingleVarDecAST* varDec = structDec->varList->singleVarDecASTs->at(i);
							if(varDec->decVarNameAST->identifier->identifier == rightName->identifier->identifier)
							{
								find = true;
								if (varDec->typeSpecifyAST->son == 0)//direct
								{
									DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)(varDec->typeSpecifyAST);
									rightName->setType(dTypeSpecify->type->type);
									varName->setType(dTypeSpecify->type->type);
									/*varName->type = rightName->type;
									if (varName->type == 0)
									{
										varName->identifier = rightName->identifier;
									}*/
									if (rightName->varType == zx::Type::STRUCT)
									{
										rightName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
										varName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
									}
									varName->isArray = varDec->decVarNameAST->LSRS;
								}
								else
								{
									PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)(varDec->typeSpecifyAST);
									DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
									PointerAST* pointer = pTypeSpecify->pointerAST;
									rightName->setType(zx::Type::POINTER);
									varName->setType(zx::Type::POINTER);
									rightName->setPointerNum(pointer->starNum);
									varName->setPointerNum(pointer->starNum);
									rightName->setFinalToType(dTypeSpecify->type->type);
									varName->setFinalToType(dTypeSpecify->type->type);
									if (rightName->finalToType == zx::Type::STRUCT)
									{
										rightName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
										varName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
									}
									varName->isArray = varDec->decVarNameAST->LSRS;
								}
								break;
							}
						}
						if (!find)
						{
							throw Exception(StaticSemaEx, rightName->row, "结构体中没有此变量");
						}
					}
				}
				else//应该不会跳转到这里
				{
					throw Exception(StaticSemaEx, rightName->row, "");
				}
#endif
				//静态语义检查 end
				varName->setRow(op->row);
				tree = varName;
				delete op;
				GoTo(tree, "<var_name>");
			}
			else if (action.aim == (*pro2index)["<var_name> @ ( <var_name> )"])
			{
				//get )
				OtherSymAST* rP = (OtherSymAST*)pop();
				//get <var_name>
				tree = pop();
				//get (
				OtherSymAST* lP = (OtherSymAST*)pop();
				tree->setRow(lP->row);
				delete lP, rP;
				GoTo(tree, "<var_name>");
			}
			else if (action.aim == (*pro2index)["<var_name> @ * <exp>"])
			{
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//get *
				OperatorAST* op = (OperatorAST*)pop();
				//create <var_name>
				VarNameAST* varName = new VarNameAST(exp);
				//静态语义检查 start
				//检查exp是否是指针类型，然后看pointerNum是1还是大于1？....
#ifdef STATIC
				if (exp->expType != zx::Type::POINTER)
				{
					throw Exception(StaticSemaEx, op->row, "只有指针变量才能使用\"*\"符号");
				}
				else
				{
					if (exp->pointerNum > 1)
					{
						varName->setType(zx::Type::POINTER);
						varName->setPointerNum(exp->pointerNum - 1);
						varName->setFinalToType(exp->finalToType);
						if (varName->finalToType == zx::Type::STRUCT)
						{
							varName->setStructName(exp->structName);
						}
					}
					else
					{
						varName->setType(exp->finalToType);
						if (varName->varType == zx::Type::STRUCT)
						{
							varName->setStructName(exp->structName);
						}
					}
				}
#endif // STATIC
				//静态语义检查 end
				varName->setRow(op->row);
				tree = varName;
				GoTo(tree, "<var_name>");
			}
			else if (action.aim == (*pro2index)["<var_name> @ <var_name> . <var_name>"])//
			{
				//get <var_name> r
				VarNameAST* rightName = (VarNameAST*)pop();
				//get .
				OperatorAST* op = (OperatorAST*)pop();
				//get <var_name> l
				VarNameAST* leftName = (VarNameAST*)pop();
				//create <var_name>
				VarNameAST* varName = new VarNameAST(leftName, rightName, 3);
				//静态语义检查 start
#ifdef STATIC
				if (rightName->type == 0)//右边的<var_name>为identifier
				{
					//左边的<var_name>已经检查过变量是否存在，故只需要检查右边的变量是否是左边的结构体的成员变量
					if (leftName->varType != zx::Type::STRUCT)
					{
						throw Exception(StaticSemaEx, leftName->row, "不是结构体的变量不能使用\".\"符号");
					}
					else
					{
						string structName = leftName->structName;
						bool find = false;
						StructDecAST* structDec = definedStructs->at(structName);
						for (int i = 0; i < structDec->varList->singleVarDecASTs->size(); i++)
						{
							SingleVarDecAST* varDec = structDec->varList->singleVarDecASTs->at(i);
							if (varDec->decVarNameAST->identifier->identifier == rightName->identifier->identifier)
							{
								find = true;
								if (varDec->typeSpecifyAST->son == 0)//direct
								{
									DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)(varDec->typeSpecifyAST);
									rightName->setType(dTypeSpecify->type->type);
									varName->setType(dTypeSpecify->type->type);
									if (rightName->varType == zx::Type::STRUCT)
									{
										rightName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
										varName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
									}
									varName->isArray = varDec->decVarNameAST->LSRS;
								}
								else
								{
									PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)(varDec->typeSpecifyAST);
									DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
									PointerAST* pointer = pTypeSpecify->pointerAST;
									rightName->setType(zx::Type::POINTER);
									varName->setType(zx::Type::POINTER);
									rightName->setPointerNum(pointer->starNum);
									varName->setPointerNum(pointer->starNum);
									rightName->setFinalToType(dTypeSpecify->type->type);
									varName->setFinalToType(dTypeSpecify->type->type);
									if (rightName->finalToType == zx::Type::STRUCT)
									{
										rightName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
										varName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
									}
									varName->isArray = varDec->decVarNameAST->LSRS;
								}
								break;
							}
						}
						if (!find)
						{
							throw Exception(StaticSemaEx, rightName->row, "结构体中没有此变量");
						}
					}
				}
				else//应该不会跳转到这里
				{
					throw Exception(StaticSemaEx, rightName->row, "");
				}
#endif
				//静态语义检查 end
				varName->setRow(op->row);
				tree = varName;
				delete op;
				GoTo(tree, "<var_name>");
			}
			else if (action.aim == (*pro2index)["<var_name> @ <var_name> [ <exp> ]"])//
			{
				//get ]
				OtherSymAST* rS = (OtherSymAST*)pop();
				//get <exp>
				ExpAST* intExp = (ExpAST*)pop();
				//get [
				OtherSymAST* lS = (OtherSymAST*)pop();
				//get <var_name>
				VarNameAST* leftName = (VarNameAST*)pop();
				//create <var_name>
				VarNameAST* varName = new VarNameAST(leftName, intExp);
				//静态语义检查 start   检查数组越界？
#ifdef STATIC
				if (intExp->expType != zx::Type::INT)
				{
					throw Exception(StaticSemaEx, lS->row, "\"[]\"内必须是整数表达式");
				}
				if (leftName->type == 0)//是变量
				{
					string name = leftName->identifier->identifier;
					VarIndex index;
					index.name = name;
					bool find = false;
					for (int i = curLevel; i >= 0; i--)
					{
						index.funcName = i == 0 ? "" : funcId->identifier;
						index.formalParaList = i == 0 ? nullptr : funcFparaList;
						index.level = i;
						if (var_table->count(index) == 0)
						{
							continue;
						}
						else
						{
							Variable* var = var_table->at(index);
							//int arrayLength = var->getArrayLength();
							if (!var->IsArray())
							{
								throw Exception(StaticSemaEx, lS->row, "只有数组变量才能使用符号\"[]\"");
							}
							else//不检查数组越界，因为后面可能会改成exp而不是单纯的integer
							{

							}
							varName->setType(var->getType());
							if (varName->varType == zx::Type::POINTER)
							{
								varName->setPointerNum(var->getPointerNum());
								varName->setFinalToType(var->getFinalToType());
								if (varName->finalToType == zx::Type::STRUCT)
								{
									varName->setStructName(var->getFinalStructName());
								}
							}
							else if (varName->varType == zx::Type::STRUCT)
							{
								varName->setStructName(var->getStructName());
							}
							find = true;
							break;
						}
					}
					if (!find)
					{
						throw Exception(StaticSemaEx, lS->row, "变量未定义");
					}
				}
				else if(leftName->type == 1)//二维数组，不支持
				{
					throw Exception(StaticSemaEx, lS->row, "暂不支持二维数组");
				}
				else if (leftName->type == 2 || leftName->type == 3)//->     .
				{
					string structName = leftName->left->structName;
					if (definedStructs->count(structName) == 0)//应该不会进入
					{
						throw Exception(StaticSemaEx, lS->row, "结构体不存在");
					}
					else
					{
						StructDecAST* structDef = definedStructs->at(structName);
						bool find = false;
						for (int i = 0; i < structDef->varList->singleVarDecASTs->size(); i++)
						{
							SingleVarDecAST* singleVarDec = structDef->varList->singleVarDecASTs->at(i);
							if (singleVarDec->decVarNameAST->identifier->identifier == leftName->right->identifier->identifier)
							{
								if (singleVarDec->decVarNameAST->LSRS)//是数组
								{
									varName->isArray = false;
									if (singleVarDec->typeSpecifyAST->son == 0)
									{
										DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)(singleVarDec->typeSpecifyAST);
										varName->varType = dTypeSpecify->type->type;
										if (varName->varType == zx::Type::STRUCT)
										{
											varName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
										}
									}
									else
									{
										PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)(singleVarDec->typeSpecifyAST);
										DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
										PointerAST* pointer = pTypeSpecify->pointerAST;
										varName->varType = zx::Type::POINTER;
										varName->setPointerNum(pointer->starNum);
										varName->setFinalToType(dTypeSpecify->type->type);
										if (varName->finalToType == zx::Type::STRUCT)
										{
											varName->setStructName(dTypeSpecify->structNameIdentifier->identifier);
										}
									}
								}
								else//不是数组
								{
									string detail = "变量" + leftName->right->identifier->identifier + "不是数组";
									throw Exception(StaticSemaEx, lS->row, detail.c_str());
								}
							}
							else
							{
								continue;
							}
						}
						if (!find)
						{
							string detail = "结构体中没有变量" + leftName->right->identifier->identifier;
							throw Exception(StaticSemaEx, lS->row, detail.c_str());
						}
					}
				}
#endif
				//静态语义检查 end
				varName->setRow(leftName->row);
				tree = varName;
				delete lS;
				delete rS;
				GoTo(tree, "<var_name>");
			}
			else if (action.aim == (*pro2index)["<stmts> @ <stmt> <stmts>"])//
			{
				//get <stmts>
				StmtsAST* stmts = (StmtsAST*)pop();
				//get Semi
				//OtherSymAST* semi = (OtherSymAST*)pop();
				//get <stmt>
				StmtAST* stmt = (StmtAST*)pop();
				stmts->addStmtAST(stmt);
				stmts->setRow(stmt->row);
				tree = stmts;
				//delete semi;
				GoTo(tree, "<stmts>");
			}
			else if (action.aim == (*pro2index)["<stmts> @ empty"])//
			{
				//create <stmts>
				tree = new StmtsAST();
				GoTo(tree, "<stmts>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <assign_stmt>"])//
			{
				//get <assign_stmt>
				AssignStmtAST* assignStmt = (AssignStmtAST*)pop();
				//set son = 0
				assignStmt->son = 0;
				tree = assignStmt;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <func_call_stmt> ;"])//
			{
				//get ;
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get <func_call_stmt>
				FuncCallStmtAST* funcCall = (FuncCallStmtAST*)pop();
				//set son = 1
				funcCall->son = 1;
				tree = funcCall;
				delete semi;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <if_stmt>"])//
			{
				//get <if_stmt>
				IfStmtAST* ifStmt = (IfStmtAST*)pop();
				//set son = 2
				ifStmt->son = 2;
				tree = ifStmt;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <if_else_stmt>"])//
			{
				//get <if_else_stmt>
				IfStmtAST* ifElseStmt = (IfStmtAST*)pop();
				//set son = 2
				ifElseStmt->son = 2;
				tree = ifElseStmt;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <while_stmt>"])//
			{
				//get <while_stmt>
				WhileStmtAST* whileStmt = (WhileStmtAST*)pop();
				//set son = 3
				whileStmt->son = 3;
				tree = whileStmt;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <return_stmt>"])//
			{
				//get <return_stmt>
				ReturnStmtAST* returnStmt = (ReturnStmtAST*)pop();
				//set son = 4
				returnStmt->son = 4;
				tree = returnStmt;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <break_stmt>"])//
			{
				//get <break_stmt>
				BreakStmtAST* breakStmt = (BreakStmtAST*)pop();
				//set son = 5
				breakStmt->son = 5;
				tree = breakStmt;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <continue_stmt>"])//
			{
				//get <continue_stmt>
				ContinueStmtAST* continueStmt = (ContinueStmtAST*)pop();
				//set son = 6
				continueStmt->son = 6;
				tree = continueStmt;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<stmt> @ <var_dec>"])//
			{
				//get <var_dec>
				VarDecAST* varDec = (VarDecAST*)pop();
				//set son = 7
				varDec->son = 7;
				tree = varDec;
				GoTo(tree, "<stmt>");
			}
			else if (action.aim == (*pro2index)["<if_stmt> @ if ( <exp> ) { <stmts> }"])//
			{
				//get }
				OtherSymAST* rB = (OtherSymAST*)pop();
				//get <stmts>
				StmtsAST* stmts = (StmtsAST*)pop();
				//get {
				OtherSymAST* lB = (OtherSymAST*)pop();
				//get )
				OtherSymAST* rP = (OtherSymAST*)pop();
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//get (
				OtherSymAST* lP = (OtherSymAST*)pop();
				//get if
				OtherSymAST* ifSym = (OtherSymAST*)pop();
				//create <if_stmt>
				IfStmtAST* ifStmt = new IfStmtAST(exp, stmts);
				//静态语义分析 start
#ifdef STATIC
				if (exp->expType != zx::Type::INT)
				{
					throw Exception(StaticSemaEx, exp->row, "分支条件表达式应该是INT类型");
				}
#endif // STATIC
				//静态语义分析 end
				ifStmt->setRow(ifSym->row);
				tree = ifStmt;
				delete rB; delete lB; delete rP; delete lP; delete ifSym;
				GoTo(tree, "<if_stmt>");
			}
			else if (action.aim == (*pro2index)["<if_else_stmt> @ if ( <exp> ) { <stmts> } else { <stmts> }"])//
			{
				//get }
				OtherSymAST* rB1 = (OtherSymAST*)pop();
				//get <stmts>
				StmtsAST* elseStmts = (StmtsAST*)pop();
				//get {
				OtherSymAST* lB1 = (OtherSymAST*)pop();
				//get else
				OtherSymAST* elseSym = (OtherSymAST*)pop();
				//get }
				OtherSymAST* rB = (OtherSymAST*)pop();
				//get <stmts>
				StmtsAST* thenStmts = (StmtsAST*)pop();
				//get {
				OtherSymAST* lB = (OtherSymAST*)pop();
				//get )
				OtherSymAST* rP = (OtherSymAST*)pop();
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//get (
				OtherSymAST* lP = (OtherSymAST*)pop();
				//get if
				OtherSymAST* ifSym = (OtherSymAST*)pop();
				//create <if_stmt>
				IfStmtAST* ifElseStmt = new IfStmtAST(exp, thenStmts,elseStmts);
				//静态语义分析 start
#ifdef STATIC
				if (exp->expType != zx::Type::INT)
				{
					throw Exception(StaticSemaEx, exp->row, "分支条件表达式应该是INT类型");
				}
#endif // STATIC
				//静态语义分析 end
				ifElseStmt->setRow(ifSym->row);
				tree = ifElseStmt;
				delete rB; delete lB; delete rP; delete lP; delete ifSym; delete elseSym; delete rB1; delete lB1;
				GoTo(tree, "<if_else_stmt>");
			}
			else if (action.aim == (*pro2index)["<while_stmt> @ while ( <exp> ) { <stmts> }"])//
			{
				//get }
				OtherSymAST* rB = (OtherSymAST*)pop();
				//get <stmts>
				StmtsAST* stmts = (StmtsAST*)pop();
				//get {
				OtherSymAST* lB = (OtherSymAST*)pop();
				//get )
				OtherSymAST* rP = (OtherSymAST*)pop();
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//get (
				OtherSymAST* lP = (OtherSymAST*)pop();
				//get while
				OtherSymAST* whileSym = (OtherSymAST*)pop();
				//create <while_stmt>
				WhileStmtAST* whileStmt = new WhileStmtAST(exp, stmts);
				//静态语义分析 start
#ifdef STATIC
				if (exp->expType != zx::Type::INT)
				{
					throw Exception(StaticSemaEx, exp->row, "循环条件表达式应该是INT类型");
				}
#endif // STATIC
				//静态语义分析 end
				whileStmt->setRow(whileSym->row);
				tree = whileStmt;
				delete rB; delete lB; delete rP; delete lP; delete whileSym;
				GoTo(tree, "<while_stmt>");
			}
			else if (action.aim == (*pro2index)["<assign_stmt> @ <var_name> ++ ;"])//
			{
				//get Semi
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get ++
				OperatorAST* plusplus = (OperatorAST*)pop();
				//get <var_name>
				VarNameAST* varName = (VarNameAST*)pop();
				//create <assign_stmt>
				AssignStmtAST* assignStmt = new AssignStmtAST(varName, plusplus);
				//静态语义检查 start
#ifdef STATIC
				if (varName->varType != zx::Type::INT && varName->varType != zx::Type::REAL && varName->varType != zx::Type::POINTER)
				{
					throw Exception(StaticSemaEx, plusplus->row, "只有整数，实数和指针类型的变量才能使用\"++\"运算符");
				}
#endif
				//静态语义检查 end
				assignStmt->setRow(semi->row);
				tree = assignStmt;
				delete semi;
				GoTo(tree, "<assign_stmt>");
			}
			else if (action.aim == (*pro2index)["<assign_stmt> @ <var_name> -- ;"])//
			{
				//get Semi
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get --
				OperatorAST* minusminus = (OperatorAST*)pop();
				//get <var_name>
				VarNameAST* varName = (VarNameAST*)pop();
				//create <assign_stmt>
				AssignStmtAST* assignStmt = new AssignStmtAST(varName, minusminus);
				//静态语义检查 start
#ifdef STATIC
				if (varName->varType != zx::Type::INT && varName->varType != zx::Type::REAL && varName->varType != zx::Type::POINTER)
				{
					throw Exception(StaticSemaEx, minusminus->row, "只有整数，实数和指针类型的变量才能使用\"--\"运算符");
				}
#endif
				//静态语义检查 end
				assignStmt->setRow(semi->row);
				tree = assignStmt;
				delete semi;
				GoTo(tree, "<assign_stmt>");
			}
			else if (action.aim == (*pro2index)["<assign_stmt> @ <var_name> = <exp> ;"])//
			{
				//get Semi
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//get =
				OtherSymAST* assignSym = (OtherSymAST*)pop();
				//get <var_name>
				VarNameAST* varName = (VarNameAST*)pop();
				//create <assign_stmt>
				AssignStmtAST* assignStmt = new AssignStmtAST(varName, exp);
				//静态语义检查 start
#ifdef STATIC
				//是不是数组的问题?        <exp>不可能是数组
				if (varName->isArray)
				{
					throw Exception(StaticSemaEx, assignSym->row, "不能直接给数组赋值");
				}
				if (varName->varType == exp->expType)
				{
					if (varName->varType == zx::Type::POINTER)
					{
						if (varName->pointerNum == exp->pointerNum && varName->finalToType==exp->finalToType)
						{
							if (varName->finalToType == zx::Type::STRUCT)
							{
								if (varName->structName != exp->structName)
								{
									throw Exception(StaticSemaEx, assignSym->row, "赋值语句左右部类型不匹配");
								}
							}
						}
						else
						{
							throw Exception(StaticSemaEx, assignSym->row, "赋值语句左右部类型不匹配");
						}
					}
					else if (varName->varType == zx::Type::STRUCT)
					{
						if (varName->structName != exp->structName)
						{
							throw Exception(StaticSemaEx, assignSym->row, "赋值语句左右部类型不匹配");
						}
					}
				}
				else
				{
					if (!(varName->varType == zx::Type::REAL && exp->expType == zx::Type::INT))
					{
						throw Exception(StaticSemaEx, assignSym->row, "赋值语句左右部类型不匹配");
					}
				}
#endif
				//静态语义检查 end
				assignStmt->setRow(semi->row);
				tree = assignStmt;
				delete assignSym;
				GoTo(tree, "<assign_stmt>");
			}
			else if (action.aim == (*pro2index)["<return_stmt> @ return ;"])//
			{
				//get ;
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get return
				OtherSymAST* returnSym = (OtherSymAST*)pop();
				//create <return_stmt>
				ReturnStmtAST* returnStmt = new ReturnStmtAST();
				//静态语义分析 start
#ifdef STATIC
				if (funcTypeAST != nullptr)
				{
					if (funcTypeAST->son == 1 || ((DirectTypeSpecifyAST*)funcTypeAST)->type->type != zx::Type::VOID)
					{
						throw Exception(StaticSemaEx, semi->row, "返回值与函数定义的类型不匹配");
					}
				}
				else
				{
					throw Exception(StaticSemaEx, semi->row, "<return_stmt> @ return ;");
				}
#endif // STATIC
				//静态语义分析 end
				returnStmt->setRow(semi->row);
				tree = returnStmt;
				delete semi; delete returnSym;
				GoTo(tree, "<return_stmt>");
			}
			else if (action.aim == (*pro2index)["<return_stmt> @ return <exp> ;"])//
			{
				//get ;
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//get return
				OtherSymAST* returnSym = (OtherSymAST*)pop();
				//create <return_stmt>
				ReturnStmtAST* returnStmt = new ReturnStmtAST(exp);
#ifdef STATIC
				if (funcTypeAST != nullptr)
				{
					if (funcTypeAST->son == 0)//direct
					{
						DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)funcTypeAST;
						if (dTypeSpecify->type->type == exp->expType)
						{
							if (dTypeSpecify->type->type == zx::Type::STRUCT)
							{
								if (dTypeSpecify->structNameIdentifier->identifier != exp->structName)
								{
									throw Exception(StaticSemaEx, semi->row, "返回值与函数定义的类型不匹配");
								}
							}
						}
						else
						{
							if (!(dTypeSpecify->type->type == zx::Type::REAL && exp->expType == zx::Type::INT))
							{
								throw Exception(StaticSemaEx, semi->row, "返回值与函数定义的类型不匹配");
							}
						}
					}
					else//pointer
					{
						PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)funcTypeAST;
						DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
						PointerAST* pointer = pTypeSpecify->pointerAST;
						if (!(exp->expType == zx::Type::POINTER && exp->pointerNum == pointer->starNum && exp->finalToType == dTypeSpecify->type->type))
						{
							throw Exception(StaticSemaEx, semi->row, "返回值与函数定义的类型不匹配");
						}
						else
						{
							if (exp->finalToType == zx::Type::STRUCT)
							{
								if (exp->structName != dTypeSpecify->structNameIdentifier->identifier)
								{
									throw Exception(StaticSemaEx, semi->row, "返回值与函数定义的类型不匹配");
								}
							}
						}
					}
				}
				else
				{
					throw Exception(StaticSemaEx, semi->row, "<return_stmt> @ return ;");
				}
#endif // STATIC
				returnStmt->setRow(semi->row);
				tree = returnStmt;
				delete semi; delete returnSym;
				GoTo(tree, "<return_stmt>");
			}
			else if (action.aim == (*pro2index)["<break_stmt> @ break ;"])//
			{
				//get ;
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get break
				OtherSymAST* breakSym = (OtherSymAST*)pop();
				//create <break_stmt>
				BreakStmtAST* breakStmt = new BreakStmtAST();
				breakStmt->setRow(breakSym->row);
				tree = breakStmt;
				delete semi; delete breakSym;
				GoTo(tree, "<break_stmt>");
			}
			else if (action.aim == (*pro2index)["<continue_stmt> @ continue ;"])//
			{
				//get ;
				OtherSymAST* semi = (OtherSymAST*)pop();
				//get continue
				OtherSymAST* continueSym = (OtherSymAST*)pop();
				//create <continue_stmt>
				ContinueStmtAST* continueStmt = new ContinueStmtAST();
				continueStmt->setRow(continueSym->row);
				tree = continueStmt;
				delete semi; delete continueSym;
				GoTo(tree, "<continue_stmt>");
			}
			else if (action.aim == (*pro2index)["<exp_list> @ <exp> , <exp_list>"])//
			{
				//get <exp_list>
				ExpListAST* expList = (ExpListAST*)pop();
				//get ,
				OtherSymAST* comma = (OtherSymAST*)pop();
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				expList->addExp(exp);
				expList->setRow(comma->row);
				tree = expList;
				delete comma;
				GoTo(tree, "<exp_list>");
			}
			else if (action.aim == (*pro2index)["<exp_list> @ <exp>"])//
			{
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//create <exp_list>
				ExpListAST* expList = new ExpListAST(exp);
				//静态语义检查 start
#ifdef STATIC
				//在ExpList类中实现

#endif
				//静态语义检查 end
				expList->setRow(exp->row);
				tree = expList;
				GoTo(tree, "<exp_list>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> + <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get +
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
				//静态语义分析 start
#ifdef STATIC
				if (rightExp->expType == leftExp->expType)//类型相同
				{
					if (leftExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL)//
					{
						exp->setType(leftExp->expType);
					}
					else//暂时不允许字符相加
					{
						throw Exception(StaticSemaEx, op->row, "两个加数类型相同时，必须是int或者real");
					}
				}
				else//类型不同
				{
					if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT)
					{//一个 int  一个  real
						exp->setType(zx::Type::REAL);
					}
					else if (leftExp->expType == zx::Type::POINTER && rightExp->expType == zx::Type::INT)
					{
						exp->setType(zx::Type::POINTER);
					}
					else
					{
						throw Exception(StaticSemaEx, op->row, "运算符\"+\"的操作数不合法");
					}
				}
#endif // STATIC
				//静态语义分析 end
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> - <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get -
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
				//静态语义分析 start
#ifdef STATIC
				if (rightExp->expType == leftExp->expType)//类型相同
				{
					if (leftExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL)//
					{
						exp->setType(leftExp->expType);
					}
					else//暂时不允许字符相减
					{
						throw Exception(StaticSemaEx, op->row, "减数与被减数类型相同时，必须是int或者real");
					}
				}
				else//类型不同
				{
					if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT)
					{//一个 int  一个  real
						exp->setType(zx::Type::REAL);
					}
					else if (leftExp->expType == zx::Type::POINTER && rightExp->expType == zx::Type::INT)
					{
						exp->setType(zx::Type::POINTER);
					}
					else
					{
						throw Exception(StaticSemaEx, op->row, "运算符\"-\"的操作数不合法");
					}
				}
#endif
				//静态语义分析 end
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> * <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get *
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
				//静态语义分析 start
#ifdef STATIC
				if (rightExp->expType == leftExp->expType)//类型相同
				{
					if (leftExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL)//
					{
						exp->setType(leftExp->expType);
					}
					else//暂时不允许字符相乘
					{
						throw Exception(StaticSemaEx, op->row, "乘数类型相同时，必须是int或者real");
					}
				}
				else//类型不同
				{
					if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT)
					{//一个 int  一个  real
						exp->setType(zx::Type::REAL);
					}
					else
					{
						throw Exception(StaticSemaEx, op->row, "运算符\"*\"的操作数不合法");
					}
				}
#endif
				//静态语义分析 end
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> / <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get /
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
				//静态语义分析 start
#ifdef STATIC
				if (rightExp->expType == leftExp->expType)//类型相同
				{
					if (leftExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL)//
					{
						exp->setType(leftExp->expType);
					}
					else//暂时不允许字符相除
					{
						throw Exception(StaticSemaEx, op->row, "除数与被除数类型相同时，必须是int或者real");
					}
				}
				else//类型不同
				{
					if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT)
					{//一个 int  一个  real
						exp->setType(zx::Type::REAL);
					}
					else
					{
						throw Exception(StaticSemaEx, op->row, "运算符\"/\"的操作数不合法");
					}
				}
#endif
				//静态语义分析 end
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> % <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get %
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
				//静态语义分析 start
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "运算符\"%\"的两个操作数都必须是整数");
				}
#endif
				//静态语义分析 end
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> > <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get >
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT||
					leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT||leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::REAL)
				{
					exp->setType(zx::Type::INT);
				}
				else if (leftExp->expType == zx::Type::CHAR && rightExp->expType == zx::Type::CHAR)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\">\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> < <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get <
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT ||
					leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::REAL)
				{
					exp->setType(zx::Type::INT);
				}
				else if (leftExp->expType == zx::Type::CHAR && rightExp->expType == zx::Type::CHAR)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"<\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> >= <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get >=
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT ||
					leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::REAL)
				{
					exp->setType(zx::Type::INT);
				}
				else if (leftExp->expType == zx::Type::CHAR && rightExp->expType == zx::Type::CHAR)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\">=\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> <= <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get <=
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT ||
					leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::REAL)
				{
					exp->setType(zx::Type::INT);
				}
				else if (leftExp->expType == zx::Type::CHAR && rightExp->expType == zx::Type::CHAR)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"<=\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> == <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get ==
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT ||
					leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::REAL)
				{
					exp->setType(zx::Type::INT);
				}
				else if (leftExp->expType == zx::Type::CHAR && rightExp->expType == zx::Type::CHAR)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"==\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> <> <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get <>
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::REAL || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::INT ||
					leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT || leftExp->expType == zx::Type::REAL && rightExp->expType == zx::Type::REAL)
				{
					exp->setType(zx::Type::INT);
				}
				else if (leftExp->expType == zx::Type::CHAR && rightExp->expType == zx::Type::CHAR)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"<>\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> && <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get &&
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"&&\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <exp> || <exp>"])//
			{
				//get <exp> r
				ExpAST* rightExp = (ExpAST*)pop();
				//get ||
				OperatorAST* op = (OperatorAST*)pop();
				//get <exp> l
				ExpAST* leftExp = (ExpAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(leftExp, rightExp, op);
#ifdef STATIC
				if (leftExp->expType == zx::Type::INT && rightExp->expType == zx::Type::INT)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"||\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ ! <exp>"])//
			{
				//get <exp>
				ExpAST* exp0 = (ExpAST*)pop();
				//get !
				OperatorAST* op = (OperatorAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(exp0, op);
#ifdef STATIC
				if (exp0->expType == zx::Type::INT)
				{
					exp->setType(zx::Type::INT);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"!\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			/*else if (action.aim == (*pro2index)["<exp> @ * <exp>"])//删除
			{
				//get <exp>
				ExpAST* exp0 = (ExpAST*)pop();
				//get *
				OperatorAST* op = (OperatorAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(exp0, op);
#ifdef STATIC
				if (exp0->expType == zx::Type::POINTER)
				{
					if (exp0->pointerNum > 1)
					{
						exp->setType(zx::Type::POINTER);
						exp->setPointerNum(exp0->pointerNum - 1);
						exp->setFinalToType(exp0->finalToType);
					}
					else
					{
						exp->setType(exp0->finalToType);
					}
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"*\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}*/
			else if (action.aim == (*pro2index)["<exp> @ - <exp>"])//
			{
				//get <exp>
				ExpAST* exp0 = (ExpAST*)pop();
				//get -
				OperatorAST* op = (OperatorAST*)pop();
				//create exp
				ExpAST* exp = new ExpAST(exp0, op);
#ifdef STATIC
				if (exp0->expType == zx::Type::INT|| exp0->expType == zx::Type::REAL)
				{
					exp->setType(exp0->expType);
				}
				else
				{
					throw Exception(StaticSemaEx, op->row, "\"-\"运算符的操作数类型错误");
				}
#endif
				exp->setRow(op->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ ( <exp> )"])//
			{
				//get )
				OtherSymAST* rP = (OtherSymAST*)pop();
				//get <exp>
				tree = pop();
				tree->setRow(rP->row);
				//get (
				OtherSymAST* lP = (OtherSymAST*)pop();
				delete lP; delete rP;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <func_call_stmt>"])
			{
				//get <func_call_stmt>
				FuncCallStmtAST* funcCallStmt = (FuncCallStmtAST*)pop();
				//create <exp>
				ExpAST* exp = new ExpAST(funcCallStmt);
				//静态语义分析 start
#ifdef STATIC
				//根据funcName和realParaList在definedFuncs中找到函数定义，再根据定义找到返回值，这些在规约生成<func_call_stmt>时已经做了
				//函数是否定义已经检查过了
				TypeSpecifyAST* retTypeSpecify = funcCallStmt->retTypeSpecify;
				if (retTypeSpecify->son == 0)//非指针
				{
					DirectTypeSpecifyAST* dTypeSpecify = (DirectTypeSpecifyAST*)(retTypeSpecify);
					exp->expType = dTypeSpecify->type->type;
					if (dTypeSpecify->type->type == zx::Type::STRUCT)
					{
						exp->setStructName(dTypeSpecify->structNameIdentifier->identifier);
					}
				}
				else//指针
				{
					PointerTypeSpecifyAST* pTypeSpecify = (PointerTypeSpecifyAST*)(retTypeSpecify);
					DirectTypeSpecifyAST* dTypeSpecify = pTypeSpecify->directTypeSpecifyAST;
					PointerAST* pointer = pTypeSpecify->pointerAST;
					exp->expType = zx::Type::POINTER;
					exp->setPointerNum(pointer->starNum);
					exp->setFinalToType(dTypeSpecify->type->type);
					if (dTypeSpecify->type->type == zx::Type::STRUCT)
					{
						exp->setStructName(dTypeSpecify->structNameIdentifier->identifier);
					}
				}
#endif
				//静态语义分析 end

				exp->setRow(funcCallStmt->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <var_name>"])//
			{
				//get <var_name>
				VarNameAST* varName = (VarNameAST*)pop();
				//create <exp>
				ExpAST* exp = new ExpAST(varName);
				//在var_table中找到变量定义，获得变量类型,如果是指针，还要设置exp的指针层数，以及最终指向的类型数
#ifdef STATIC
				if (varName->isArray)//???????????????????????????????????????????????????????????????????????????????????????????????????????????????
				{
					throw Exception(StaticSemaEx, varName->row, "数组使用错误");
				}
				if (varName->type == 0)//id
				{
					bool find = false;
					for (int i = curLevel; i >= 0; i--)
					{
						VarIndex index;
						index.formalParaList = i==0?nullptr:funcFparaList;
						index.level = i;
						index.funcName = i == 0 ? "" : funcId->identifier;
						index.name = varName->identifier->identifier;
						if (var_table->count(index) == 0)
						{
							continue;
						}
						else
						{
							Variable* var = var_table->at(index);
							exp->expType = var->getType();
							if (exp->expType == zx::Type::POINTER)
							{
								exp->setPointerNum(var->getPointerNum());
								exp->setFinalToType(var->getFinalToType());
								if (exp->finalToType == zx::Type::STRUCT)
								{
									exp->setStructName(var->getFinalStructName());
								}
							}
							else if(exp->expType == zx::Type::STRUCT)
							{
								exp->setStructName(var->getStructName());
							}
							find = true;
							break;
						}
					}
					if (!find)
					{
						string detail = "变量" + varName->identifier->identifier + "不存在";
						throw Exception(StaticSemaEx, varName->row, detail.c_str());
					}
				}
				else if (varName->type == 1)//id[int]
				{
					VarNameAST* leftVarName = varName->left;
					exp->expType = leftVarName->varType;
					if (exp->expType == zx::Type::POINTER)
					{
						exp->setPointerNum(leftVarName->pointerNum);
						exp->setFinalToType(leftVarName->finalToType);
						if (exp->finalToType == zx::Type::STRUCT)
						{
							exp->setStructName(leftVarName->structName);
						}
					}
					else if (exp->expType == zx::Type::STRUCT)
					{
						exp->setStructName(leftVarName->structName);
					}
				}
				else if (varName->type == 2 || varName->type == 3)//->   .
				{
					exp->expType = varName->varType;
					if (exp->expType == zx::Type::POINTER)
					{
						exp->setPointerNum(varName->pointerNum);
						exp->setFinalToType(varName->finalToType);
						if (exp->finalToType == zx::Type::STRUCT)
						{
							exp->setStructName(varName->structName);
						}
					}
					else if (exp->expType == zx::Type::STRUCT)
					{
						exp->setStructName(varName->structName);
					}
				}
				else if (varName->type == 4)
				{
					exp->expType = varName->varType;
					if (exp->expType == zx::Type::POINTER)
					{
						exp->setPointerNum(varName->pointerNum);
						exp->finalToType = varName->finalToType;
						if (exp->finalToType == zx::Type::STRUCT)
						{
							exp->setStructName(varName->structName);
						}
					}
					else
					{
						if (exp->expType == zx::Type::STRUCT)
						{
							exp->setStructName(varName->structName);
						}
					}
				}
#endif
				exp->setRow(varName->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<exp> @ <val>"])//
			{
				//get <val>
				ValAST* val = (ValAST*)pop();
				//create <exp>
				ExpAST* exp = new ExpAST(val);
#ifdef STATIC
				if (val->charAST != nullptr)
				{
					exp->setType(zx::Type::CHAR);
				}
				else if (val->numberAST != nullptr)
				{
					if (val->numberAST->type == 0)//int
					{
						exp->setType(zx::Type::INT);
					}
					else//real
					{
						exp->setType(zx::Type::REAL);
					}
				}
				else if (val->stringAST != nullptr)
				{
					exp->setType(zx::Type::STRING);
				}
				else
				{
					throw Exception(StaticSemaEx, val->row, "编译器出错 at <exp> @ <val>");
				}
#endif
				exp->setRow(val->row);
				tree = exp;
				GoTo(tree, "<exp>");
			}
			else if (action.aim == (*pro2index)["<func_call_stmt> @ identifier ( <real_para_list> )"])//
			{
				//get )
				OtherSymAST* rP = (OtherSymAST*)pop();
				//get <real_para_list>
				RealParaListAST* rparaList = (RealParaListAST*)pop();
				//get (
				OtherSymAST* lP = (OtherSymAST*)pop();
				//get identifier
				IdentifierAST* id = (IdentifierAST*)pop();
				//create <func_call_stmt>
				FuncCallStmtAST* funcCallStmt = new FuncCallStmtAST(id, rparaList);
				//静态语义检查 start
#ifdef STATIC
				string funcName = id->identifier;
				if (definedFuncs->count(funcName) == 1)
				{
					bool find = false;
					for (int i = 0; i < (*definedFuncs)[funcName].size(); i++)
					{
						if (find)
						{
							break;
						}
						FuncDefineAST* func = (*definedFuncs)[funcName][i];
						if (rparaList->realParaItemASTs->size() == func->formalParaListAST->formalParaItemASTs->size())
						{
							if (rparaList->realParaItemASTs->size() == 0)
							{
								find = true;
								funcCallStmt->retTypeSpecify = func->typeSpecifyAST;
								break;
							}
							for (int j = 0; j < rparaList->realParaItemASTs->size(); j++)
							{
								FormalParaItemAST* fParaItem = func->formalParaListAST->formalParaItemASTs->at(j);
								RealParaItemAST* rParaItem = rparaList->realParaItemASTs->at(j);
								ExpAST* exp = rParaItem->expAST;
								if (fParaItem->typeSpecifyAST->son == 0)
								{
									if (exp->expType != zx::Type::POINTER)
									{
										DirectTypeSpecifyAST* dtypeSpecify = (DirectTypeSpecifyAST*)(fParaItem->typeSpecifyAST);
										if (exp->expType == dtypeSpecify->type->type)
										{
											if (exp->expType == zx::Type::STRUCT)
											{
												if (exp->structName == dtypeSpecify->structNameIdentifier->identifier)
												{
													if (j == rparaList->realParaItemASTs->size() - 1)
													{
														find = true;
														funcCallStmt->retTypeSpecify = func->typeSpecifyAST;
														break;
													}
													continue;
												}
												else
												{
													break;
												}
											}
											else
											{
												if (j == rparaList->realParaItemASTs->size() - 1)
												{
													find = true;
													funcCallStmt->retTypeSpecify = func->typeSpecifyAST;
													break;
												}
												continue;
											}
										}
										else
										{
											if (dtypeSpecify->type->type == zx::Type::REAL && exp->expType == zx::Type::INT)
											{
												if (j == rparaList->realParaItemASTs->size() - 1)
												{
													find = true;
													funcCallStmt->retTypeSpecify = func->typeSpecifyAST;
													break;
												}
												continue;
											}
											else 
											{
												break;
											}
										}
									}
									else
									{
										break;
									}
								}
								else//指针类型
								{
									PointerTypeSpecifyAST* pointerTypeSpecify = (PointerTypeSpecifyAST*)(fParaItem->typeSpecifyAST);
									if (exp->expType == zx::Type::POINTER)
									{
										if (exp->pointerNum == pointerTypeSpecify->pointerAST->starNum)
										{
											if (exp->finalToType == pointerTypeSpecify->directTypeSpecifyAST->type->type)
											{
												if (exp->expType == zx::Type::STRUCT)
												{
													if (exp->structName == pointerTypeSpecify->directTypeSpecifyAST->structNameIdentifier->identifier)
													{
														if (j == rparaList->realParaItemASTs->size() - 1)
														{
															find = true;
															funcCallStmt->retTypeSpecify = func->typeSpecifyAST;
															break;
														}
														continue;
													}
													else
													{
														break;
													}
												}
												else
												{
													if (j == rparaList->realParaItemASTs->size() - 1)
													{
														find = true;
														funcCallStmt->retTypeSpecify = func->typeSpecifyAST;
														break;
													}
													continue;
												}
											}
											else
											{
												if (j == rparaList->realParaItemASTs->size() - 1)
												{
													find = true;
													break;
												}
												continue;
											}
										}
										else
										{
											break;
										}
									}
									else
									{
										break;
									}
								}
								
							}
						}
						else
						{
							continue;
						}
					}
					if (!find)
					{
						throw Exception(StaticSemaEx, lP->row, "函数参数列表类型不匹配");
					}
				}
				else
				{
					throw Exception(StaticSemaEx, lP->row, "函数未定义");
				}
#endif
				//静态语义检查 end
				funcCallStmt->setRow(id->row);
				tree = funcCallStmt;
				delete lP; delete rP;
				GoTo(tree, "<func_call_stmt>");
			}
			else if (action.aim == (*pro2index)["<real_para_list> @ <real_para_item> , <real_para_list>"])//
			{
				//get <real_para_list>
				RealParaListAST* rparaList = (RealParaListAST*)pop();
				//get ,
				OtherSymAST* comma = (OtherSymAST*)pop();
				//get <real_para_item>
				RealParaItemAST* rparaItem = (RealParaItemAST*)pop();
				//add
				rparaList->addRealParaListAST(rparaItem);
				rparaList->setRow(comma->row);
				tree = rparaList;
				delete comma;
				GoTo(tree, "<real_para_list>");
			}
			else if (action.aim == (*pro2index)["<real_para_list> @ <real_para_item>"])//
			{
				//get <real_para_item>
				RealParaItemAST* rparaItem = (RealParaItemAST*)pop();
				//create <real_para_list>
				RealParaListAST* rparaList = new RealParaListAST(rparaItem);
				rparaList->setRow(rparaItem->row);
				tree = rparaList;
				GoTo(tree, "<real_para_list>");
			}
			else if (action.aim == (*pro2index)["<real_para_list> @ empty"])//
			{
				//create <real_para_list>
				RealParaListAST* rparaList = new RealParaListAST();
				tree = rparaList;
				GoTo(tree, "<real_para_list>");
			}
			else if (action.aim == (*pro2index)["<real_para_item> @ <exp>"])//
			{
				//get <exp>
				ExpAST* exp = (ExpAST*)pop();
				//create <real_para_item>
				RealParaItemAST* rparaItem = new RealParaItemAST(exp);
				rparaItem->setRow(exp->row);
				tree = rparaItem;
				GoTo(tree, "<real_para_item>");
			}
			else if (action.aim == (*pro2index)["<number> @ integer"])
			{
				//get integer
				IntAST* integer = (IntAST*)pop();
				//create <number>
				NumberAST* number = new NumberAST(integer);
				number->setRow(integer->row);
				tree = number;
				GoTo(tree, "<number>");
			}
			else if (action.aim == (*pro2index)["<number> @ real_number"])
			{
				//get real_number
				RealAST* real = (RealAST*)pop();
				//create <number>
				NumberAST* number = new NumberAST(real);
				number->setRow(real->row);
				tree = number;
				GoTo(tree, "<number>");
			}
		}
		action = getAction((*sourceSymList)[curPointer]);
	}
	//acc
#ifdef STATIC
	std::cout << "静态语义分析成功!" << endl;
#else
	std::cout << "语法分析成功!" << endl;
#endif
	tree->codegen();
}
void MyParser::GoTo(AST*& ast, string value)
{
	Symbol* symbol = new Symbol();
	symbol->value = value;
	symbol->row = ast->row;
	Action action = getAction(symbol, symbol->value);
	if (action.behavior != GOTO)
	{
		throw Exception(ParserEx, symbol->row, "At:  MyParser::GoTo() ");
	}
	else
	{
		delete symbol;
		push(ast, action.aim);
	}
}

Action MyParser::getAction(Symbol* symbol, string value)//非终结符使用
{
	DicIndex index;
	index.state = (*stateStack)[stackTop];
	index.sym = value;
	Action action;
	if (SLR1_table->count(index) == 0)//不在表中，报错
	{
		throw Exception(ParserEx, symbol->row, "语法错误");
	}
	else
	{
		string s = (*SLR1_table)[index];
		int length = s.length();
		if (length == 0)
		{
			throw Exception(OtherEx, -1, "SLR1表有误");
		}
		int aimState = 0;
		int aimProduction = 0;
		switch (s[0])
		{

		case 'S'://移进
			action.behavior = S;
			aimState = 0;
			for (int i = 1; i < length; i++)
			{
				aimState *= 10;
				aimState += (int(s[i]) - 48);
			}
			action.aim = aimState;
			return action;
		case 'r'://归约
			action.behavior = r;
			for (int i = 1; i < length; i++)
			{
				aimProduction *= 10;
				aimProduction += (int(s[i]) - 48);
			}
			action.aim = aimProduction;
			return action;
		case 'a'://接受
			action.behavior = acc;
			action.aim = -1;
			return action;
		default://GOTO
			action.behavior = GOTO;
			aimState = 0;
			for (int i = 0; i < length; i++)
			{
				aimState *= 10;
				aimState += (int(s[i]) - 48);
			}
			action.aim = aimState;
			return action;
		}
	}
}

Action MyParser::getAction(Symbol* symbol)//终结符使用
{
	DicIndex index;
	index.state = (*stateStack)[stackTop];
	index.sym = (*token2strP)[symbol->token];
	Action action;
	if (SLR1_table->count(index) == 0)//不在表中，报错
	{
		throw Exception(ParserEx, symbol->row, "语法错误");
	}
	else
	{
		string s = (*SLR1_table)[index];
		int length = s.length();
		if (length == 0)
		{
			throw Exception(OtherEx, -1, "SLR1表有误");
		}
		int aimState = 0;
		int aimProduction = 0;
		switch (s[0])
		{
		
		case 'S'://移进
			action.behavior = S;
			aimState = 0;
			for (int i = 1; i < length; i++)
			{
				aimState *= 10;
				aimState += (int(s[i]) - 48);
			}
			action.aim = aimState;
			return action;
		case 'r'://归约
			action.behavior = r;
			for (int i = 1; i < length; i++)
			{
				aimProduction *= 10;
				aimProduction += (int(s[i]) - 48);
			}
			action.aim = aimProduction;
			return action;
		case 'a'://接受
			action.behavior = acc;
			action.aim = -1;
			return action;
		default://GOTO
			action.behavior = GOTO;
			aimState = 0;
			for (int i = 0; i < length; i++)
			{
				aimState *= 10;
				aimState += (int(s[i]) - 48);
			}
			action.aim = aimState;
			return action;
		}
	}
	
}






void MyParser::initToken2StrP()
{
	token2strP = new map<Token, string>();
	(*token2strP)[Token::Tag] = "identifier";
	(*token2strP)[Token::Plus] = "+";
	(*token2strP)[Token::Minus] = "-";
	(*token2strP)[Token::Star] = "*";
	(*token2strP)[Token::Divide] = "/";
	(*token2strP)[Token::Mode] = "%";
	(*token2strP)[Token::PlusPlus] = "++";
	(*token2strP)[Token::MinusMinus] = "--";
	(*token2strP)[Token::More] = ">";
	(*token2strP)[Token::Less] = "<";
	(*token2strP)[Token::MoreOrEq] = ">=";
	(*token2strP)[Token::LessOrEq] = "<=";
	(*token2strP)[Token::Eq] = "==";
	(*token2strP)[Token::Neq] = "<>";
	(*token2strP)[Token::Or] = "||";
	(*token2strP)[Token::And] = "&&";
	(*token2strP)[Token::Void] = "void";
	(*token2strP)[Token::Int] = "int";
	(*token2strP)[Token::Real] = "real";
	(*token2strP)[Token::Char] = "char";
	(*token2strP)[Token::LB] = "{";
	(*token2strP)[Token::RB] = "}";
	(*token2strP)[Token::LS] = "[";
	(*token2strP)[Token::RS] = "]";
	(*token2strP)[Token::LP] = "(";
	(*token2strP)[Token::RP] = ")";
	(*token2strP)[Token::Comma] = ",";
	(*token2strP)[Token::Semi] = ";";
	(*token2strP)[Token::Dot] = ".";
	(*token2strP)[Token::Assign] = "=";
	(*token2strP)[Token::Struct] = "struct";
	(*token2strP)[Token::If] = "if";
	(*token2strP)[Token::Else] = "else";
	(*token2strP)[Token::While] = "while";
	(*token2strP)[Token::Return] = "return";
	(*token2strP)[Token::Continue] = "continue";
	(*token2strP)[Token::Break] = "break";
	(*token2strP)[Token::Const_int] = "integer";
	(*token2strP)[Token::Const_real] = "real_number";
	(*token2strP)[Token::Const_char] = "Achar";
	(*token2strP)[Token::Const_string] = "string";
	(*token2strP)[Token::Pointer_Sign] = "->";
	(*token2strP)[Token::End] = "$";
	(*token2strP)[Token::Not] = "!";
}


















inline bool operator < (const struct VarIndex& v1, const struct VarIndex& v2) {
	if (v1.level != v2.level) {
		return v1.level < v2.level;
	}
	else if (v1.formalParaList != v2.formalParaList)
	{
		return v1.formalParaList < v2.formalParaList;
	}
	else {
		if (v1.name.length() != v2.name.length()) {
			return v1.name.length() < v2.name.length();
		}
		else {
			for (int i = 0; i < v1.name.length(); i++) {
				if (v1.name[i] != v2.name[i]) {
					return v1.name[i] < v2.name[i];
				}
			}
			for (int i = 0; i < v1.funcName.length(); i++)
			{
				if (v1.funcName[i] != v2.funcName[i]) {
					return v1.funcName[i] < v2.funcName[i];
				}
			}
			return v1.name[0] < v2.name[0];
		}
	}
}
