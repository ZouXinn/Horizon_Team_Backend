#pragma once

enum Token {
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
	Pointer_Sign,
	End,//$
	Not// !
};




