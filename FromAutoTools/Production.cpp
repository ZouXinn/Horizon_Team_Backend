#include "Production.h"


bool Production::setLeft(ProductionSym* left)
{
	if (this->left != nullptr)
	{
		return false;
	}
	else
	{
		this->left = left;
		return true;
	}
}

bool Production::setRight(vector<ProductionSym*>* right) 
{
	if (this->right != nullptr)
	{
		return false;
	}
	else
	{
		this->right = right;
		return true;
	}
}

Production::Production()
{
	this->left = nullptr;
	this->right = nullptr;
}

ProductionSym* Production::getLeft()
{
	return this->left;
}

vector<ProductionSym*>* Production::getRight()
{
	return this->right;
}

Production::Production(ProductionSym* left, vector<ProductionSym*>* right)
{
	this->left = left;
	this->right = right;
}

Production::Production(const Production& pro)//拷贝构造函数，重写为深拷贝
{
	this->left = new ProductionSym(pro.left);
	this->right = new vector<ProductionSym*>();
	for (int i = 0; i < pro.right->size(); i++)
	{
		ProductionSym* tSym = pro.right->at(i);
		this->right->push_back(new ProductionSym(tSym));
	}
	this->dotIndex = pro.dotIndex;
}

Production::~Production()
{
	// delete left
	delete this->left;
	this->left = nullptr;
	// delete right and Syms that the vector elements point to

	//(1)
	this->right->clear();
	if (this->right != nullptr)
	{
		delete this->right;
		this->right = nullptr;
	}

	//(2)
	/*
	for (int i = 0; i < this->right->size(); i++)
	{
		delete right[i];
	}
	*/
}


bool Production::theSameAs(Production* pro)//返回两个产生式是否相同
{
	if (this->dotIndex == pro->dotIndex&& left->getVal()==pro->left->getVal()&&right->size()==pro->right->size())
	{
		for (int i = 0; i < right->size(); i++)
		{
			if (right->at(i)->getVal() != pro->right->at(i)->getVal())
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

int Production::getDotIndex()
{
	return this->dotIndex;
}

void Production::addDotIndex()
{
	this->dotIndex++;
}