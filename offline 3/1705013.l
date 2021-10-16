%option noyywrap yylineno
%{
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include"1705013_SymbolTable.h"
#define YYSTYPE SymbolInfo*
#include "y.tab.h"

using namespace std;

void yyerror(char*);  
extern YYSTYPE yylval;  
extern int line_count;
extern int error_count;
extern ofstream log;
extern ofstream error;
%}
WHITESPACE [ \t\v\f\r]+
NEWLINE \n
NOTNEWLINE [^\r\n]
Letter [a-zA-Z]
Digit [0-9]
Float ({Digit}+)?(\.({Digit}+)?)?((E[+-]?{Digit}+))?
Variable ([a-zA-Z_])([a-zA-Z0-9_]*)
Relop "=="|"!="|"<"|"<="|">"|">="
ADDOP [-+]
MULOP [*/%]
DecimalError ({Digit}+)?(\.)+{Digit}+((\.)+{Digit}+)*
IllFormed ({Digit}+)?(\.{Digit}+)?([E]*[+-]?(({Digit}+)?(\.){Digit}+)(((\.){Digit}+)*)?)
%%
[ \t\v]+ {}
{NEWLINE} {line_count++;}
"if" {
	return IF;
}
"else" {
	return ELSE;
}
"for" {
	return FOR;
}
"int" {
	return INT;
}
"float"	{
	return FLOAT;
}
"void" {
	return VOID;
}
"while" {
	return WHILE;
}
"return" {
	return RETURN;
}
"printf" {
    	return PRINTLN;
}
{Relop} {
	SymbolInfo* symbolInfo = new  SymbolInfo(yytext, "RELOP");
    	yylval = (YYSTYPE) symbolInfo;
    	return RELOP;
}
"++" 	{return INCOP;}
"--" 	{return DECOP;}
{ADDOP} {
	SymbolInfo* symbolInfo = new  SymbolInfo(yytext, "ADDOP");
    	yylval = (YYSTYPE) symbolInfo;
    	return ADDOP;
}
{MULOP} {
	SymbolInfo* symbolInfo = new  SymbolInfo(yytext, "MULOP");
    	yylval = (YYSTYPE) symbolInfo;
    	return MULOP;
}
"=" {
	return ASSIGNOP;
}
"&&"|"||" {
	SymbolInfo* symbolInfo = new  SymbolInfo(yytext, "LOGICOP");
    	yylval = (YYSTYPE) symbolInfo;
    	return LOGICOP;
}
"!" {
	return NOT;
}
"(" {
	return LPAREN;
}
")" {
	return RPAREN;
}
"{" {
	return LCURL;
}
"}" {	
	return RCURL;
}
"[" {
	return LTHIRD;
}
"]" {
	return RTHIRD;
}
"," {
	return COMMA;
}
";" {
	return SEMICOLON;
}
{Digit}+ {
           SymbolInfo* symbolInfo = new  SymbolInfo(yytext, "CONST_INT");
    	   yylval = (YYSTYPE) symbolInfo;
    	   return CONST_INT;

}
{Float} {
	   SymbolInfo* symbolInfo = new  SymbolInfo(yytext, "CONST_FLOAT");
    	   yylval = (YYSTYPE) symbolInfo;
    	   return CONST_FLOAT;
}
{Variable} {
	   SymbolInfo* symbolInfo = new  SymbolInfo(yytext, "ID");
    	   yylval = (YYSTYPE) symbolInfo;
    	   return ID;   
}
{DecimalError} {
	   log<<"Error at line no: "<<yylineno<<" Too many decimal points\n"<<endl;
	   error<<"Error at line no: "<<yylineno<<" Too many decimal points\n"<<endl;
	   error_count++;
	   SymbolInfo* symbolInfo = new  SymbolInfo(yytext, "CONST_FLOAT");
    	   yylval = (YYSTYPE) symbolInfo;
	   return CONST_FLOAT;
}
{WHITESPACE} {
    /* ignore them */
}
. {	error_count++;
	log<<"Error at line no: "<<yylineno<<" Unrecognized Character "<<yytext<<endl<<endl;
	error<<"Error at line no: "<<yylineno<<" Unrecognised Character "<<yytext<<endl<<endl;
}
%%
