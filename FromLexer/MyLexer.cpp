#include "MyLexer.h"
#include <fstream>
#include <cstring>
#include <atlstr.h>
using namespace std;

int MyLexer::keyNum = 11;
Token MyLexer::keyTokens[] = { Void,Int,Real,Char,Struct,If,Else,While,Return,Continue,Break};
string MyLexer::keyWords[] = {"void","int","real","char","struct","if","else","while","return","continue","break"};

Token MyLexer::inKey(string str)
{
	for (int i = 0; i < keyNum; i++)
	{
		if (str == keyWords[i])
		{
			return keyTokens[i];
		}
	}
	return Tag;
}

MyLexer::MyLexer(const char* fileName, bool isFile)
{
	try {
		this->mySourceCode = readSourceCode(fileName,isFile);
		this->buildSymbols();
	}
	catch (Exception e) {
		throw e;
	}

	if (symVecPointer->at(symVecPointer->size() - 1)->token != End)
	{
		throw Exception(LexerEx, symVecPointer->at(symVecPointer->size() - 1)->row, "没有结束符号");
	}
}

MyLexer::~MyLexer()
{
	delete mySourceCode;
	for (int i = 0; i < symVecPointer->size(); i++)
	{
		delete symVecPointer->at(i);
	}
	delete symVecPointer;
}

bool isBlank(char c) //是否是空白
{
	return c == ' ' || c == '\t' || c == '\n';
}
bool isAlpha(char c) // 是否是字母
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool isNumber(char c) // 是否是数字
{
	return c >= '0' && c <= '9';
}
bool isNoZeroNumber(char c) // 是否是非零数字
{
	return c >= '1' && c <= '9';
}
void MyLexer::buildSymbols()
{
	this->symVecPointer = new vector<Symbol*>();
	Symbol* symPointer = nullptr;
	string s = "";
	int row = 1,j=0;
	for (int i = 0; i < mySourceCode->length;)
	{
		while (isBlank(this->mySourceCode->buffer[i]))
		{
			if (this->mySourceCode->buffer[i] == '\n')
			{
				row++;
			}
			i++;
		}
		switch (this->mySourceCode->buffer[i])
		{
		case '_':
			throw Exception(LexerEx, row, "不能以下划线开头");
		case '!':
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = Token::Not;
			symPointer->value = "!";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '$': // 结束
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = End;
			symPointer->value = "$";
			this->symVecPointer->push_back(symPointer);
			symPointer = nullptr;
			return;
		case ';':
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = Semi;
			symPointer->value = ";";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '{': //LB
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = LB;
			symPointer->value = "{";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '}': //RB
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = RB;
			symPointer->value = "}";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '[': //LS
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = LS;
			symPointer->value = "[";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case ']': //RS
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = RS;
			symPointer->value = "]";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '(': //LP
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = LP;
			symPointer->value = "(";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case ')': //RP
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = RP;
			symPointer->value = ")";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '.': //Dot
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = Token::Dot;
			symPointer->value = ".";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case ',': //Comma
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = Comma;
			symPointer->value = ",";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '+': // +   ++
			if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '+')
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::PlusPlus;
				symPointer->value = "++";
				i+=2;
			}
			else
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Plus;
				symPointer->value = "+";
				i++;
			}
			this->symVecPointer->push_back(symPointer);
			symPointer = nullptr;
			break;
		case '-': // -   --
			//if (i + 1 < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + 1])) //负数
			//{
			//	i++;
			//	s += '-';
			//	if (this->mySourceCode->buffer[i] == '0')
			//	{
			//		if (i + 1 < this->mySourceCode->length && this->mySourceCode->buffer[i + 1] != '.')
			//		{
			//			symPointer = new Symbol();
			//			symPointer->row = row;
			//			symPointer->token = Const_int;
			//			symPointer->value = "-0";
			//			i++;
			//			if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
			//			{
			//				throw Exception(LexerEx, row, "number can not be followed by letter or _");
			//			}
			//			this->symVecPointer->push_back(symPointer);
			//			symPointer = nullptr;
			//			break;
			//			//if (i + 2 < this->mySourceCode->length && !isNumber(this->mySourceCode->buffer[i + 2])) // 0
			//			//{
			//			//	symPointer = new Symbol();
			//			//	symPointer->row = row;
			//			//	symPointer->token = Const_int;
			//			//	symPointer->value = "-0";
			//			//	i++;
			//			//	if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
			//			//	{
			//			//		throw Exception(LexerEx, row, "number can not be followed by letter or _");
			//			//	}
			//			//	this->symVecPointer->push_back(symPointer);
			//			//	symPointer = nullptr;
			//			//	break;
			//			//}
			//			//else
			//			//{
			//			//	throw Exception(LexerEx, row, "非法符号0");
			//			//}
			//		}
			//		else if (i + 1 == this->mySourceCode->length)
			//		{
			//			throw Exception(LexerEx, row, "非法符号\"0\"");
			//		}
			//		else // 小数
			//		{
			//			j = 2;
			//			if (i + j < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + j]))
			//			{
			//				s += "0.";
			//				while (i + j < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + j]))
			//				{
			//					s += this->mySourceCode->buffer[i + j];
			//					j++;
			//				}
			//				if (j == 2)
			//				{
			//					throw Exception(LexerEx, row, "非法字符");
			//				}
			//				symPointer = new Symbol();
			//				symPointer->row = row;
			//				symPointer->token = Const_real;
			//				symPointer->value = s;
			//				s = "";
			//				i += j;
			//				if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
			//				{
			//					throw Exception(LexerEx, row, "number can not be followed by letter or _");
			//				}
			//				j = 0;
			//				this->symVecPointer->push_back(symPointer);
			//				symPointer = nullptr;
			//				break;
			//			}
			//			else
			//			{
			//				throw Exception(LexerEx, row, "非法符号\"-0.\"");
			//			}
			//		}
			//	}
			//	else // 不以0开头
			//	{
			//		j = 0;
			//		while (i + j < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + j]))
			//		{
			//			s += this->mySourceCode->buffer[i + j];
			//			j++;
			//		}
			//		if (i + j == this->mySourceCode->length)
			//		{
			//			throw Exception(LexerEx, row, "");
			//		}
			//		else// char不是数字
			//		{
			//			if (this->mySourceCode->buffer[i + j] == '.')//浮点数
			//			{
			//				j++; // i+j 指向.后面的字符
			//				int curJ = j;
			//				s += '.';
			//				if (i + j == this->mySourceCode->length)
			//				{
			//					throw Exception(LexerEx, row, "");
			//				}
			//				else
			//				{
			//					while (i + j < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + j]))
			//					{
			//						s += this->mySourceCode->buffer[i + j];
			//						j++;
			//					}
			//					if (i + j == this->mySourceCode->length)
			//					{
			//						throw Exception(LexerEx, row, "");
			//					}
			//					else // 浮点数
			//					{
			//						if (j == curJ)
			//						{
			//							throw Exception(LexerEx, row, "非法字符");
			//						}
			//						symPointer = new Symbol();
			//						symPointer->row = row;
			//						symPointer->token = Const_real;
			//						symPointer->value = s;
			//						s = "";
			//						i += j;
			//						if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
			//						{
			//							throw Exception(LexerEx, row, "number can not be followed by letter or _");
			//						}
			//						this->symVecPointer->push_back(symPointer);
			//						symPointer = nullptr;
			//						break;
			//					}
			//				}
			//			}
			//			else//整数
			//			{
			//				symPointer = new Symbol();
			//				symPointer->row = row;
			//				symPointer->token = Const_int;
			//				symPointer->value = s;
			//				s = "";
			//				i += j;
			//				if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
			//				{
			//					throw Exception(LexerEx, row, "number can not be followed by letter or _");
			//				}
			//				this->symVecPointer->push_back(symPointer);
			//				symPointer = nullptr;
			//				break;
			//			}
			//		}
			//	}
			//}
			if (i + 1 < this->mySourceCode->length && this->mySourceCode->buffer[i + 1] == '-') // --
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::MinusMinus;
				symPointer->value = "--";
				this->symVecPointer->push_back(symPointer);
				i += 2;
				symPointer = nullptr;
				break;
			}
			else if (i + 1 < this->mySourceCode->length && this->mySourceCode->buffer[i + 1] == '>')
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::Pointer_Sign;
				symPointer->value = "->";
				this->symVecPointer->push_back(symPointer);
				i += 2;
				symPointer = nullptr;
				break;
			}
			else
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::Minus;
				symPointer->value = "-";
				this->symVecPointer->push_back(symPointer);
				i++;
				symPointer = nullptr;
				break;
			}
		case '*':
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = Token::Star;
			symPointer->value = "*";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '/': // / 或 // 或 /**/
			if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '/') // 单行注释
			{
				j = 2;
				while (i+j < mySourceCode->length&& this->mySourceCode->buffer[i+j] != '\n')
				{
					j++;
				}
				if (i+j == mySourceCode->length)
				{
					throw Exception(LexerEx, row, "源程序应当以$符号结束");
				}
				else
				{
					row++;
					i += j + 1;
				}
			}
			else if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '*') // 多行注释
			{
				j = 2;
				while (true)
				{
					while (i + j < mySourceCode->length && this->mySourceCode->buffer[i + j] != '*')
					{
						if (this->mySourceCode->buffer[i + j] == '\n')
						{
							row++;
						}
						j++;
					}
					if (i + j == mySourceCode->length)
					{
						throw Exception(LexerEx, row, "源程序应当以$符号结束");
					}
					else if(i + j + 1 == mySourceCode->length)
					{
						throw Exception(LexerEx, row, "结束符号$不能在注释中");
					}
					else
					{
						if (this->mySourceCode->buffer[i + j + 1] != '/')
						{
							j++;
						}
						else
						{
							i += j + 2;
							break;
						}
					}
				}
			}
			else
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::Divide;
				symPointer->value = "/";
				this->symVecPointer->push_back(symPointer);
				i++;
				symPointer = nullptr;
			}
			break;
		case '%':
			symPointer = new Symbol();
			symPointer->row = row;
			symPointer->token = Token::Mode;
			symPointer->value = "%";
			this->symVecPointer->push_back(symPointer);
			i++;
			symPointer = nullptr;
			break;
		case '>': // > >=
			if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '=')
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::MoreOrEq;
				symPointer->value = ">=";
				i += 2;
			}
			else
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::More;
				symPointer->value = ">";
				i++;
			}
			this->symVecPointer->push_back(symPointer);
			symPointer = nullptr;
			break;
		case '<': // < <= <>
			if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '=')
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::LessOrEq;
				symPointer->value = "<=";
				i += 2;
			}
			else if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '>')
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::Neq;
				symPointer->value = "<>";
				i += 2;
			}
			else
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::Less;
				symPointer->value = ">";
				i++;
			}
			this->symVecPointer->push_back(symPointer);
			symPointer = nullptr;
			break;
		case '=': // = ==
			if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '=')
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::Eq;
				symPointer->value = "==";
				i += 2;
			}
			else
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Assign;
				symPointer->value = "=";
				i++;
			}
			this->symVecPointer->push_back(symPointer);
			symPointer = nullptr;
			break;
		case '|':
			if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '|')
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::Or;
				symPointer->value = "||";
				i += 2;
				this->symVecPointer->push_back(symPointer);
				symPointer = nullptr;
				break;
			}
			else
			{
				throw Exception(LexerEx, row, "非法字符|");
			}
		case '&':
			if (i + 1 < mySourceCode->length && this->mySourceCode->buffer[i + 1] == '&')
			{
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Token::And;
				symPointer->value = "&&";
				i += 2;
				this->symVecPointer->push_back(symPointer);
				symPointer = nullptr;
				break;
			}
			else
			{
				throw Exception(LexerEx, row, "非法字符&");
			}
		case '\'': // Const_char
			//if (i + 2 < mySourceCode->length && isAlpha(this->mySourceCode->buffer[i + 1]) && this->mySourceCode->buffer[i + 2] == '\'')
			if (i + 2 < mySourceCode->length && this->mySourceCode->buffer[i + 2] == '\'')
			{
				s += this->mySourceCode->buffer[i + 1];
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Const_char;
				symPointer->value = s;
				s = "";
				i += 3;
				this->symVecPointer->push_back(symPointer);
				symPointer = nullptr;
				break;
			}
			else
			{
				throw Exception(LexerEx, row, "");
			}
		case '\"': // Const_string
			j = 1;
			while (i + j < mySourceCode->length && this->mySourceCode->buffer[i + j] != '\"')
			{
				s += this->mySourceCode->buffer[i + j];
				if (this->mySourceCode->buffer[i + j] == '\n')
				{
					row++;
				}
				j++;
			}
			if (i + j == mySourceCode->length)
			{
				throw Exception(LexerEx, row, "程序不完整");
			}
			else
			{
				//s += this->mySourceCode->buffer[i + 1];
				symPointer = new Symbol();
				symPointer->row = row;
				symPointer->token = Const_string;
				symPointer->value = s;
				s = "";
				i += j + 1;
				this->symVecPointer->push_back(symPointer);
				symPointer = nullptr;
				break;
			}
		//常量  Const_int Const_real Tag  关键字
		default:
			if (isAlpha(this->mySourceCode->buffer[i]))//标识符：以字母开头，中间是字母或数字或下划线，不能以下划线结尾  或 关键字
			{
				j = 0;
				while (i + j < this->mySourceCode ->length && (isAlpha(this->mySourceCode->buffer[i + j]) || isNumber(this->mySourceCode->buffer[i + j]) || this->mySourceCode->buffer[i + j] == '_'))
				{
					s += this->mySourceCode->buffer[i + j];
					j++;
				}
				if (i + j == this->mySourceCode->length)
				{
					throw Exception(LexerEx, row, "Error");
				}
				else
				{
					if (this->mySourceCode->buffer[i + j - 1] == '_')
					{
						throw Exception(LexerEx, row,"标识符不能以下划线结尾！");
					}
					else
					{
						Token tk = inKey(s);
						symPointer = new Symbol();
						symPointer->row = row;
						symPointer->token = tk;
						symPointer->value = s;
						s = "";
						i += j;
						this->symVecPointer->push_back(symPointer);
						symPointer = nullptr;
						break;
					}
				}
			}
			else if (isNumber(this->mySourceCode->buffer[i]))//正数
			{
				if (this->mySourceCode->buffer[i] == '0')
				{
					if (i + 1 < this->mySourceCode->length && this->mySourceCode->buffer[i + 1] != '.')
					{
						symPointer = new Symbol();
						symPointer->row = row;
						symPointer->token = Const_int;
						symPointer->value = "0";
						i++;
						if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
						{
							throw Exception(LexerEx, row, "number can not be followed by letter or _");
						}
						this->symVecPointer->push_back(symPointer);
						symPointer = nullptr;
						break;

						//if (i + 2 < this->mySourceCode->length && !isNumber(this->mySourceCode->buffer[i + 2])) // 0
						//{
						//	symPointer = new Symbol();
						//	symPointer->row = row;
						//	symPointer->token = Const_int;
						//	symPointer->value = "0";
						//	i++;
						//	if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
						//	{
						//		throw Exception(LexerEx, row, "number can not be followed by letter or _");
						//	}
						//	this->symVecPointer->push_back(symPointer);
						//	symPointer = nullptr;
						//	break;
						//}
						//else
						//{
						//	throw Exception(LexerEx, row, "非法符号0");
						//}
					}
					else if (i + 1 == this->mySourceCode->length)
					{
						throw Exception(LexerEx, row, "非法符号\"0\"");
					}
					else // 小数
					{
						j = 2;
						if (i + j < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + j]))
						{
							s = "0.";
							while (i + j < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + j]))
							{
								s += this->mySourceCode->buffer[i + j];
								j++;
							}
							if (j == 2)
							{
								throw Exception(LexerEx, row, "非法字符");
							}
							symPointer = new Symbol();
							symPointer->row = row;
							symPointer->token = Const_real;
							symPointer->value = s;
							s = "";
							i += j;
							if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
							{
								throw Exception(LexerEx, row, "number can not be followed by letter or _");
							}
							this->symVecPointer->push_back(symPointer);
							symPointer = nullptr;
							break;
						}
						else
						{
							throw Exception(LexerEx, row, "非法符号\"0.\"");
						}
					}
				}
				else // 不以0开头
				{
					j = 0;
					while (i + j < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + j]))
					{
						s += this->mySourceCode->buffer[i + j];
						j++;
					}
					if (i + j == this->mySourceCode->length)
					{
						throw Exception(LexerEx, row, "");
					}
					else// char不是数字
					{
						if (this->mySourceCode->buffer[i + j] == '.')//浮点数
						{
							j++; // i+j 指向.后面的字符
							int curJ = j;
							s += '.';
							if (i + j == this->mySourceCode->length)
							{
								throw Exception(LexerEx, row, "");
							}
							else
							{
								while (i + j < this->mySourceCode->length && isNumber(this->mySourceCode->buffer[i + j]))
								{
									s += this->mySourceCode->buffer[i + j];
									j++;
								}
								if (i + j == this->mySourceCode->length)
								{
									throw Exception(LexerEx, row, "");
								}
								else // 浮点数
								{
									if (j == curJ)
									{
										throw Exception(LexerEx, row, "非法字符");
									}
									symPointer = new Symbol();
									symPointer->row = row;
									symPointer->token = Const_real;
									symPointer->value = s;
									s = "";
									i += j;
									if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
									{
										throw Exception(LexerEx, row, "number can not be followed by letter or _");
									}
									this->symVecPointer->push_back(symPointer);
									symPointer = nullptr;
									break;
								}
							}
						}
						else//整数
						{
							symPointer = new Symbol();
							symPointer->row = row;
							symPointer->token = Const_int;
							symPointer->value = s;
							s = "";
							i += j;
							if (isAlpha(this->mySourceCode->buffer[i]) || this->mySourceCode->buffer[i] == '_')
							{
								throw Exception(LexerEx, row, "number can not be followed by letter or _");
							}
							this->symVecPointer->push_back(symPointer);
							symPointer = nullptr;
							break;
						}
					}
				}
			}
			else
			{
				throw Exception(LexerEx, row, "非法字符");
			}
		}
	}
}

SourceCode* MyLexer::readSourceCode(const char* fileName,bool isFile)
{
	if (isFile) 
	{
		fstream in;
		in.open(fileName, ios::in);
		SourceCode* sc = new SourceCode();
		in.seekg(0, ios::end);//移动到末尾
		sc->length = in.tellg();//获得长度
		in.seekg(0, ios::beg);//移动到开头
		sc->buffer = new char[sc->length];
		in.read(sc->buffer, sc->length);
		in.close();
		char* tCp = sc->buffer;
		if (sc->length > 0)
		{
			bool find = false;
			for (int i = 0; i < sc->length; i++)
			{
				if (tCp[i] == '$')
				{
					find = true;
					sc->length = i + 1;
					break;
				}
			}
			if (find)
			{

			}
			else
			{
				int i = 0,row = 1;
				while (i < sc->length) {
					if (tCp[i] == '\n') {
						row++;
					}
					i++;
				}
				throw Exception(LexerEx, row, "没有结束符号");
			}
		}
		else
		{
			throw Exception(LexerEx, -1, "没有内容");
		}
		return sc;
	}
	else
	{
		SourceCode* sc = new SourceCode();
		int length = strlen(fileName);
		/*while (fileName[length] != '$')
		{
			length++;
		}
		length++;*/
		sc->length = length;


		//string str(fileName);
		//CString temp = str.c_str();
		
		//char* s = temp.GetBuffer(temp.ReleaseBuffer());

		sc->buffer = const_cast<char*>(fileName);

		char* tCp = sc->buffer;
		if (sc->length > 0)
		{
			bool find = false;
			for (int i = 0; i < sc->length; i++)
			{
				if (tCp[i] == '$')
				{
					find = true;
					sc->length = i + 1;
					break;
				}
			}
			if (find)
			{

			}
			else
			{
				int i = 0, row = 1;
				while (i < sc->length) {
					if (tCp[i] == '\n') {
						row++;
					}
					i++;
				}
				throw Exception(LexerEx, row, "没有结束符号");
			}
		}
		else
		{
			throw Exception(LexerEx, -1, "没有内容");
		}
		return sc;
	}
}
void MyLexer::printResult()
{ 
	string tokens[] = { "Plus","Minus","Star","Divide","Mode","PlusPlus","MinusMinus","More","Less","MoreOrEq","LessOrEq","Eq",
					"Neq","Or","And","Void","Int","Real","Char","LB","RB","LS","RS","LP","RP","Comma","Semi","Dot","Assign",
					"Struct","If","Else","While","Return","Continue","Break","Tag","Const_int","Const_real","Const_char","Const_string","Pointer_Sign","End","Not" };
	
	for (int i = 0; i < this->symVecPointer->size(); i++)
	{
		Symbol* symPointer = this->symVecPointer->at(i);
		cout << "row: " << symPointer->row << "\t\t\t" << "value: " << symPointer->value << "\t\t\t" << "token: " << tokens[symPointer->token] << endl;
	}
}


vector<Symbol*>* MyLexer::getSymbols()
{
	return this->symVecPointer;
}