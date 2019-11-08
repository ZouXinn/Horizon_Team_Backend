#pragma once
#include "Enums_Structs.h"
#include <string>

class Variable
{
private:
	zx::Type type;
	std::string name;
	std::string value = "";
	
	//结构体定义指针

	//
	bool isArray;
	int arrayLength = 0;
	std::string structName = "";//如果type是STRUCT，则存此struct的名称
	zx::Type toType = zx::Type::VOID;//如果type是指针，则存指向的目标的类型
	zx::Type finalToType = zx::Type::VOID;//
	std::string finalStructName = "";//如果finalToType是STRUCT，则存此struct的名称
	int pointerNum = 0;
public:
	Variable(zx::Type type, std::string name,bool isArray);
	Variable(zx::Type type, std::string name, std::string structName, bool isArray);
	Variable(zx::Type type, std::string name, zx::Type toType, bool isArray);

	void setValue(std::string val);
	void setArratLength(int length);
	zx::Type getType();
	std::string getValue();
	std::string getName();
	std::string getStructName();
	zx::Type getFinalToType();
	int getPointerNum();
	zx::Type getToType();
	std::string getFinalStructName();
	int getArrayLength();
	void setPointerNum(int num);
	void setFinalToType(zx::Type finalToType);
	void setFinalStructName(std::string name);
	void setArray();
	bool IsArray();
};

