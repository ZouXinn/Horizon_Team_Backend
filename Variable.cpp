#include "Variable.h"




Variable::Variable(zx::Type type, std::string name, bool isArray)
{
	this->type = type;
	this->name = name;
	this->isArray = isArray;
}

Variable::Variable(zx::Type type, std::string name, std::string structName, bool isArray):Variable(type,name, isArray)
{
	this->structName = structName;
}

Variable::Variable(zx::Type type, std::string name, zx::Type toType, bool isArray):Variable(type, name, isArray)
{
	this->toType = toType;
}

void Variable::setValue(std::string val)
{
	this->value = val;
}

zx::Type Variable::getType()
{
	return this->type;
}

std::string Variable::getValue()
{
	return this->value;
}

std::string Variable::getName()
{
	return this->name;
}

std::string Variable::getStructName()
{
	return this->structName;
}

zx::Type Variable::getFinalToType()
{
	return this->finalToType;
}

int Variable::getPointerNum()
{
	return this->pointerNum;
}

std::string Variable::getFinalStructName()
{
	return this->finalStructName;
}

int Variable::getArrayLength()
{
	return this->arrayLength;
}

zx::Type Variable::getToType()
{
	return this->toType;
}

void Variable::setFinalToType(zx::Type finalToType)
{
	this->finalToType = finalToType;
}

void Variable::setFinalStructName(std::string name)
{
	this->finalStructName = name;
}
void Variable::setArray()
{
	this->isArray = true;
}
void Variable::setPointerNum(int num)
{
	this->pointerNum = num;
}

bool Variable::IsArray()
{
	return this->isArray;
}

void Variable::setArratLength(int length)
{
	if (isArray)
	{
		this->arrayLength = length;
	}
}