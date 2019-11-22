#include <iostream>
#include "unitTest.h"
#include "MyParser.h"


int main(int argc, char* argv[])
{
	//testParser();
	if (argc >= 2) {
		try {
			MyParser* parser = new MyParser(argv[1], "..//Files//SLR1.txt", "..//Files//MyProductions.txt", "empty", true);
			parser->Parse();
			parser->CodeGen();
			parser->RunJIT();
		}
		catch (Exception e)
		{
			cout << e.print();
		}

	}
	return 0;
}