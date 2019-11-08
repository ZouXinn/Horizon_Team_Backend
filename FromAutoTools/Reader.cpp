#include "Reader.h"
#include <fstream>
#include <iostream>

char* Reader::buffer = nullptr;
int Reader::length = 0;


void Reader::readFile(const char* fileName)
{
	fstream in;
	in.open(fileName, ios::in);
	in.seekg(0, ios::end);//移动到末尾
	Reader::length = in.tellg();//获得长度
	in.seekg(0, ios::beg);//移动到开头
	Reader::buffer = new char[length];
	in.read(Reader::buffer, length);
	in.close();
}


vector<Production*>* Reader::readProductions(const char* fileName)//读取Production 的 vector
{
	// $结束   @表示推导出
	int row = 1;
	Reader::readFile(fileName);
	vector<Production*>* vectorPointer = new vector<Production*>();
	string s = "";
	/*if (buffer[length - 1] != '#')
	{
		cout << buffer[length - 1];
		Error("文法格式错误，应当以#结尾",row);
	}*/
	Production* pro = nullptr;
	vector<ProductionSym*>* right = new vector<ProductionSym*>();
	int i = 0;
	//for (int i = 0; i < Reader::length; i++)
	while(buffer[i] != '$')
	{
		pro = new Production();
		ProductionSym* left = nullptr;
		while (buffer[i] != ' ')
		{
			/*if ((buffer[i] < '0' || buffer[i] > '9') && (buffer[i] < 'a' || buffer[i] > 'z') && (buffer[i] < 'A' || buffer[i] > 'Z') && buffer[i] != '_' && buffer[i] != '<' && buffer[i] != '>')
			{
				Error("非法字符", row);
			}*/
			s += buffer[i];
			i++;
		}
		if (s[0] != '<' || s[s.length() - 1] != '>')
		{
			Error("非终结符必须以<>括起来",row);
		}
		//left = new ProductionSym(s.substr(1, s.length() - 2), true);
		left = new ProductionSym(s, true);
		pro->setLeft(left);
		left = nullptr;
		s = "";
		i++;
		if (buffer[i] != '@')
		{
			Error("必须用@表示推导符号",row);
		}
		i++;
		if (buffer[i] != ' ')
		{
			Error("各符号之间缺少空格",row);
		}
		i++;
		if (buffer[i] == '$') // 结束
		{
			Error("非法结束", row);
			/*delete right;
			right = nullptr;
			return vectorPointer;*/
		}
		while (buffer[i] != '\n')
		{
			while (buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '$' && buffer[i] != '@')
			{
				/*if ((buffer[i] < '0' || buffer[i] > '9') && (buffer[i] < 'a' || buffer[i] > 'z') && (buffer[i] < 'A' || buffer[i] > 'Z') && buffer[i] != '_' && buffer[i] != '<' && buffer[i] != '>')
				{
					Error("非法字符", row);
				}*/
				s += buffer[i];
				i++;
			}
			if (buffer[i] == ' ')
			{
				i++;
			}
			else if(buffer[i]!='\n')
			{
				Error("$和@在此处非法！", row);
			}
			if (s[0] == '<' && s[s.length() - 1] == '>')
			{
				//ProductionSym* tsym = new ProductionSym(s.substr(1, s.length() - 2), true);
				if (s.length() == 2)
				{
					ProductionSym* tsym = new ProductionSym(s, false);
					right->push_back(tsym);
					s = "";
				}
				else
				{
					ProductionSym* tsym = new ProductionSym(s, true);
					right->push_back(tsym);
					s = "";
				}
			}
			else
			{
				ProductionSym* tsym = new ProductionSym(s, false);
				right->push_back(tsym);
				s = "";
			}
		}
		i++;
		pro->setRight(right);
		vectorPointer->push_back(pro);
		
		right = new vector<ProductionSym*>();
		row++;
	}

	// 清理内存
	delete Reader::buffer;
	Reader::buffer = nullptr;
	Reader::length = 0;
	return vectorPointer;
}

void Reader::Error(const char* detail,int row)
{
	cout << "文件" << "第" << row << "行出错；";
	cout << detail<< endl;
	exit(0);
}
