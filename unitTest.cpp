#include "MyParser.h"
#include "../src/FromLexer/MyLexer.h"
#include "addJIT/addJIT.h"
void testLexer()
{
	MyLexer* lexer = nullptr;
	try {
		lexer = new MyLexer("..//Files//sourceCode.txt",true);
		lexer->printResult();
	}
	catch (Exception e) {
		cout<<e.print();
	}
}

void testParser()
{
	const char* sourceCodePath = "..//Files//sourceCode.txt";
	try {
		MyParser* parser = new MyParser(sourceCodePath, "..//Files//SLR1.txt", "..//Files//MyProductions.txt", "empty",true);
		parser->Parse();
		parser->CodeGen();
	}
	catch (Exception e)
	{
		cout<<e.print();
	}
	system("pause");
}

void testParser2()
{
	const char* sourceCode = "int a = 10,b = 1.0;\n$";
	try {
		MyParser* parser = new MyParser(sourceCode, "..//Files//SLR1.txt", "..//Files//MyProductions.txt", "empty",false);
		parser->Parse();
		addJIT();
	}
	catch (Exception e)
	{
		cout<<e.print();
	}
	system("pause");
}
