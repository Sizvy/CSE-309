%option noyywrap yylineno

%{
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include"symbolTable.h"
SymbolTable *st = new SymbolTable(7);

int line_count=1;
int error=0;
FILE *logout;
FILE *tokenout;

%}
NEWLINE \r?\n
NOTNEWLINE [^\r\n]
Letter [a-zA-Z]
Digit [0-9]
Float ({Digit}+)?(\.({Digit}+)?)?((E[+-]?{Digit}+))?
Variable ([a-zA-Z_])([a-zA-Z0-9_]*)
Character \'[ -~]{-}['\\]\'
Relop "=="|"!="|"<"|"<="|">"|">="
ADDOP [-+]
Incop "++"|"--"
MULOP [*/%]
Tab \'\\t\' 
CommentSlash \/\/((.*\\\n.*)(.*))*
DecimalError ({Digit}+)?(\.)+{Digit}+((\.)+{Digit}+)*
IllFormed ({Digit}+)?(\.{Digit}+)?([E]*[+-]?(({Digit}+)?(\.){Digit}+)(((\.){Digit}+)*)?)
MultiChar \'[^\r\n]{NOTNEWLINE}+\'
InvalidPrefix {Digit}+{Letter}+({Letter}|{Digit})*
CHAR_CONST ((\\{NOTNEWLINE})|[^\\'(\r\n)])
ERROR_CHARACTER '({CHAR_CONST}|\\)*
CommentError "/*"([^\*]|([\*]+[^*/]))*
CommentStar ([/][\*]([^*]|[\r\n]|(\*([^/]|[\r\n])))*[\*][/\\])|[/][/]+(.*)
ALL ([^\r\n]|{NEWLINE})
StringError \"([^"\\\r\n]|[\\]{ALL})*
StringLiteral {StringError}\"
%%
[ \t\v]+ {}
{NEWLINE} {line_count++;}
"if" {
	fprintf(tokenout,"<IF>");
	fprintf(logout,"Line no %d: TOKEN <IF> Lexeme %s found\n\n",yylineno,yytext);
     }
"else" {
	fprintf(tokenout,"<ELSE>");
	fprintf(logout,"Line no %d: TOKEN <ELSE> Lexeme %s found\n\n",yylineno,yytext);
	}
"for" {
	fprintf(tokenout,"<FOR>");
	fprintf(logout,"Line no %d: TOKEN <FOR> Lexeme %s found\n\n",yylineno,yytext);
	}
"int" {
	fprintf(tokenout,"<INT>");
	fprintf(logout,"Line no %d: TOKEN <INT> Lexeme %s found\n\n",yylineno,yytext);
	}
"float" {
	fprintf(tokenout,"<FLOAT>");
	fprintf(logout,"Line no %d: TOKEN <FLOAT> Lexeme %s found\n\n",yylineno,yytext);
	}
"void" {
	fprintf(tokenout,"<VOID>");
	fprintf(logout,"Line no %d: TOKEN <VOID> Lexeme %s found\n\n",yylineno,yytext);
	}
"switch" {
	fprintf(tokenout,"<SWITCH>");
	fprintf(logout,"Line no %d: TOKEN <SWTICH> Lexeme %s found\n\n",yylineno,yytext);
	}
"default" {
	fprintf(tokenout,"<DEFAULT>");
	fprintf(logout,"Line no %d: TOKEN <DEFAULT> Lexeme %s found\n\n",yylineno,yytext);
	}
"while" {
	fprintf(tokenout,"<WHILE>");
	fprintf(logout,"Line no %d: TOKEN <WHILE> Lexeme %s found\n\n",yylineno,yytext);
	}
"break" {
	fprintf(tokenout,"<BREAK>");
	fprintf(logout,"Line no %d: TOKEN <BREAK> Lexeme %s found\n\n",yylineno,yytext);
	}
"char" {
	fprintf(tokenout,"<CHAR>");
	fprintf(logout,"Line no %d: TOKEN <CHAR> Lexeme %s found\n\n",yylineno,yytext);
	}
"double" {
	fprintf(tokenout,"<DOUBLE>");
	fprintf(logout,"Line no %d: TOKEN <DOUBLE> Lexeme %s found\n\n",yylineno,yytext);
	}
"return" {
	fprintf(tokenout,"<RETURN>");
	fprintf(logout,"Line no %d: TOKEN <RETURN> Lexeme %s found\n\n",yylineno,yytext);
	}
"do" {
	fprintf(tokenout,"<DO>");
	fprintf(logout,"Line no %d: TOKEN <DO> Lexeme %s found\n\n",yylineno,yytext);
	}
"case" {
	fprintf(tokenout,"<CASE>");
	fprintf(logout,"Line no %d: TOKEN <CASE> Lexeme %s found\n\n",yylineno,yytext);
	}
"continue" {
	fprintf(tokenout,"<CONTINUE>");
	fprintf(logout,"Line no %d: TOKEN <CONTINUE> Lexeme %s found\n\n",yylineno,yytext);
	}
{Relop} {
	fprintf(tokenout,"<RELOP, %s>", yytext);
	fprintf(logout,"Line no %d: TOKEN <RELOP> Lexeme %s found\n\n",yylineno,yytext);
	}
{Incop} {
	fprintf(tokenout,"<INCOP, %s>",yytext);
	fprintf(logout,"Line no %d: TOKEN <INCOP> Lexeme %s found\n\n",yylineno,yytext);
	}
{ADDOP} {
	fprintf(tokenout,"<ADDOP, %s>",yytext);
	fprintf(logout,"Line no %d: TOKEN <ADDOP> Lexeme %s found\n\n",yylineno,yytext);
	}
{MULOP} {
	fprintf(tokenout,"<MULOP, %s>",yytext);
	fprintf(logout,"Line no %d: TOKEN <MULOP> Lexeme %s found\n\n",yylineno,yytext);
	}
"=" {
	fprintf(tokenout,"<ASSIGNOP, =>");
	fprintf(logout,"Line no %d: TOKEN <ASSIGNOP> Lexeme %s found\n\n",yylineno,yytext);
	}
"&&" {
	fprintf(tokenout,"<LOGICOP, &&>");
	fprintf(logout,"Line no %d: TOKEN <LOGICOP> Lexeme %s found\n\n",yylineno,yytext);
	}
"||" {
	fprintf(tokenout,"<LOGICOP, ||>");
	fprintf(logout,"Line no %d: TOKEN <LOGICOP> Lexeme %s found\n\n",yylineno,yytext);
	}
"!" {
	fprintf(tokenout,"<NOT, !>");
	fprintf(logout,"Line no %d: TOKEN <NOT> Lexeme %s found\n\n",yylineno,yytext);
	}
"(" {
	fprintf(tokenout,"<LPAREN, (>");
	fprintf(logout,"Line no %d: TOKEN <LPAREN> Lexeme %s found\n\n",yylineno,yytext);
	}
")" {
	fprintf(tokenout,"<RPAREN, )>");
	fprintf(logout,"Line no %d: TOKEN <RPAREN> Lexeme %s found\n\n",yylineno,yytext);
	}
"{" {
	st->EnterScope(7);
	fprintf(tokenout,"<LCURL, {>");
	fprintf(logout,"Line no %d: TOKEN <LCURL> Lexeme %s found\n\n",yylineno,yytext);
	}
"}" {	
	st->ExitScope();
	fprintf(tokenout,"<RCURL, }>");
	fprintf(logout,"Line no %d: TOKEN <RCURL> Lexeme %s found\n\n",yylineno,yytext);
	}
"[" {
	fprintf(tokenout,"<LTHIRD, [>");
	fprintf(logout,"Line no %d: TOKEN <LTHIRD> Lexeme %s found\n\n",yylineno,yytext);
	}
"]" {
	fprintf(tokenout,"<RTHIRD, ]>");
	fprintf(logout,"Line no %d: TOKEN <RTHIRD> Lexeme %s found\n\n",yylineno,yytext);
	}
"," {
	fprintf(tokenout,"<COMMA, ,>");
	fprintf(logout,"Line no %d: TOKEN <COMMA> Lexeme %s found\n\n",yylineno,yytext);
	}
";" {
	fprintf(tokenout,"<SEMICOLON, ;>");
	fprintf(logout,"Line no %d: TOKEN <SEMICOLON> Lexeme %s found\n\n",yylineno,yytext);
	}
\'\' {
      error++;
      fprintf(logout,"Error at Line no %d: Empty character const %s\n\n",yylineno,yytext);
     }
\'\\\' {
	error++;
	fprintf(logout,"Error at Line no %d: Unterminated Character %s\n\n",yylineno,yytext);	
	}
{Character} {
	   bool IsSuccess = false;
	   fprintf(tokenout,"<CONST_CHAR, %s>",yytext);
	   char temp[strlen(yytext)];
	   strcpy(temp,yytext);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,temp[1]);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   	
	   }
	   
	}
{Digit}+ {
           bool IsSuccess = false;
	   fprintf(tokenout,"<CONST_INT, %s>",yytext);
	   fprintf(logout,"Line no %d: TOKEN <CONST_INT> Lexeme %s found\n\n", yylineno,yytext);
	   IsSuccess=st->insertSymbol(yytext,"CONST_INT");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   	
	   }

	}
{Float} {
	   bool IsSuccess = false;
	   fprintf(tokenout,"<CONST_FLOAT, %s>",yytext);
	   fprintf(logout,"Line no %d: TOKEN <CONST_FLOAT> Lexeme %s found\n\n", yylineno,yytext);
	   IsSuccess=st->insertSymbol(yytext,"CONST_FLOAT");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   	
	   } 
	}
{Variable} {
	   bool IsSuccess = false;
	   fprintf(tokenout,"<ID, %s>",yytext);
	   fprintf(logout,"Line no %d: TOKEN <ID> Lexeme %s found\n\n", yylineno,yytext);
	   IsSuccess=st->insertSymbol(yytext,"ID");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }
	      
	}
{Tab} {
	   bool IsSuccess = false;
	   char tab = '\t';
	   fprintf(tokenout,"<CONST_CHAR, %c>",tab);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,tab);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   } 
	}
\'\\n\' {
	   bool IsSuccess = false;
	   char es = '\n';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\a\' {
	   bool IsSuccess = false;
	   char es = '\a';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\f\' {
	   bool IsSuccess = false;
	   char es = '\f';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\r\' {
	   bool IsSuccess = false;
	   char es = '\r';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\b\' {
	   bool IsSuccess = false;
	   char es = '\b';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\v' {
	   bool IsSuccess = false;
	   char es = '\v';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CHAR_CONST, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\0\' {
	   bool IsSuccess = false;
	   char es = '\n';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\\\\' {
	   bool IsSuccess = false;
	   char es = '\\';
	   cout<<"Prothom"<<endl;
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\\'\' {
	   bool IsSuccess = false;
	   char es = '\'';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
\'\\\"\' {
	   bool IsSuccess = false;
	   char es = '\"';
	   fprintf(tokenout,"<CONST_CHAR, %c>",es);
	   fprintf(logout,"Line no %d: TOKEN <CONST_CHAR> Lexeme %s found --><CONST_CHAR, %c>\n\n", yylineno,yytext,es);
	   IsSuccess=st->insertSymbol(yytext,"CONST_CHAR");
	   if(IsSuccess==false){
	   	fprintf(logout,"%s already exists in current Scope Table\n\n",yytext);
	   }
	   else{
	   	st->PrintAllScope(logout);
	   }  	
	}
{StringLiteral} {
		  char temp[strlen(yytext)];
	   	  strcpy(temp,yytext);
	   	  int i;
	   	  char quotes;
	   	  fprintf(tokenout,"<STRING, ");
	   	  fprintf(logout,"Line no %d: TOKEN <STRING> Lexeme %s found --><STRING, ",yylineno,yytext);
	   	  for(i=0;i<strlen(yytext);i++){
	   	  	if(temp[i]=='\\'&&temp[i+1]=='t'){
	   	  		quotes='\t';
	   	  		fprintf(tokenout,"%c",quotes);
	   	  		fprintf(logout, "%c",quotes);
	   	  		i++;
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\\'&&temp[i+1]=='n'){
	   	  		quotes='\n';
	   	  		fprintf(tokenout,"%c",quotes);
	   	  		fprintf(logout, "%c",quotes);
	   	  		i++;
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\\'&&temp[i+1]=='b'){
	   	  		quotes='\b';
	   	  		fprintf(tokenout,"%c",quotes);
	   	  		fprintf(logout, "%c",quotes);
	   	  		i++;
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\\'&&temp[i+1]=='f'){
	   	  		quotes='\f';
	   	  		fprintf(tokenout,"%c",quotes);
	   	  		fprintf(logout, "%c",quotes);
	   	  		i++;
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\\'&&temp[i+1]=='v'){
	   	  		quotes='\v';
	   	  		fprintf(tokenout,"%c",quotes);
	   	  		fprintf(logout, "%c",quotes);
	   	  		i++;
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\\'&&temp[i+1]=='a'){
	   	  		quotes='\a';
	   	  		fprintf(tokenout,"%c",quotes);
	   	  		fprintf(logout, "%c",quotes);
	   	  		i++;
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\\'&&temp[i+1]=='r'){
	   	  		quotes='\r';
	   	  		fprintf(tokenout,"%c",quotes);
	   	  		fprintf(logout, "%c",quotes);
	   	  		i++;
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\\'&&temp[i+1]=='\''){
	   	  		quotes='\'';
	   	  		fprintf(tokenout,"%c",quotes);
	   	  		fprintf(logout, "%c",quotes);
	   	  		i++;
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\\'){
	   	  		continue;
	   	  	}
	   	  	else if(temp[i]=='\n'){
	   	  		continue;
	   	  	}
	   	  	fprintf(tokenout,"%c",temp[i]);
	   	  	fprintf(logout, "%c",temp[i]);
	   	  }
	   	  fprintf(tokenout," >");
	   	  fprintf(logout," >\n\n");
		}
{CommentSlash} {
	         fprintf(logout,"Line no %d: TOKEN <COMMENT> Lexeme %s found\n\n", yylineno,yytext);
	       }
{CommentStar} {
	    fprintf(logout,"Line no %d: TOKEN <COMMENT> Lexeme %s found\n\n", yylineno,yytext);
	}
{DecimalError} {
		 error++;
		 fprintf(logout,"Error at Line no %d: Too many decimal points %s\n\n",yylineno,yytext);
		}
{IllFormed} {
		 error++;
		 fprintf(logout,"Error at Line no %d: Ill formed number %s\n\n",yylineno,yytext);
	    }
{MultiChar} {
	      error++;
	      fprintf(logout,"Error at Line no %d: Multi character constant error %s\n\n",yylineno,yytext);
	    } 
{ERROR_CHARACTER} {
			 error++;
			 fprintf(logout,"Error at Line no %d: Unterminated Character %s\n\n",yylineno,yytext);			
			}
{CommentError} {
		 error++;
		 fprintf(logout,"Error at Line no %d: Unterminated comment %s\n\n",yylineno,yytext);		
		}
{InvalidPrefix} {
		  error++;
		  fprintf(logout,"Error at Line no %d: Invalid prefix on ID or invalid suffix on Number %s\n\n",yylineno,yytext);
		 }
{StringError} {
		  error++;
		  fprintf(logout,"Error at Line no %d: Unterminated String %s\n\n",yylineno,yytext);
		 }		 	
<<EOF>> {
	st->PrintAllScope(logout);
	fprintf(logout, "Total Lines: %d\n",yylineno-1);
	fprintf(logout, "Total Errors: %d", error); 
	return 0;
	}
. {	error++;
	fprintf(logout,"Error at Line no %d: Unrecognised character %s\n\n",yylineno,yytext);
	}
%%

int main(int argc,char *argv[]){
	
	if(argc!=2){
		printf("Please provide input file name and try again\n");
		return 0;
	}
	
	FILE *fin=fopen(argv[1],"r");
	if(fin==NULL){
		printf("Cannot open specified file\n");
		return 0;
	}
	
	logout= fopen("1705013_log.txt","w");
	tokenout= fopen("1705013_token.txt","w");
	fprintf(logout,"\n");
	yyin= fin;
	yylineno = 1;
	yylex();
	fclose(yyin);
	fclose(tokenout);
	fclose(logout);
	return 0;
}
