%{
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<cstdlib>
#include<vector>
#include "1705013_SymbolTable.h"
#define YYSTYPE SymbolInfo*
using namespace std;

int yyparse(void);
int yylex(void);

extern FILE* yyin;

int line_count = 1; 
int error_count = 0;
int scope_count = 1;
int func_error_recovery = 0;
int error_line;
int dec_check;
int already_got = 0;

SymbolTable symbolTable;
string cv;
FILE* input;
ofstream log;
ofstream error;
string undefined_func="blank";
string type, type_final; 
string name, name_final;

struct var{
    string var_name;
    int var_size; 
} temp_var;

vector<var> var_list;

class parameter {
public:
    string param_type;
    string param_name;
    parameter(){
    
    }  
};

vector<string> array_list;

vector<parameter> param_list;

vector<string> arg_list;
void errorprint(string s) {
    error << "Error at line " << line_count << ": " << s<<endl;
    log << "Error at line " << line_count << ": " << s<<endl;
    error_count++;
    return ;
}
void yyerror(char *s){
	error<<"Error at line "<<error_line<<": "<<s<<endl<<endl;
	error_count++;
	return ;
}
void multiple_Dec_check(){
	SymbolInfo* temp = symbolTable.lookUpAllScope(name_final);

            if(temp != NULL) {
                errorprint("multiple declaration of "+name_final+"\n");
            } else {
    		SymbolInfo* symbolInfo = new SymbolInfo(name_final, "ID");
    		symbolInfo->set_Type(type_final);  
    		symbolInfo->set_arrSize(-2); 
    		cout<<"Hi"<<endl;
    		for(int i=0; i<param_list.size(); i++) {
        		symbolInfo->addParam(param_list[i].param_type, param_list[i].param_name);
    		}

    		symbolTable.insertSymbol(symbolInfo);
            }
}
void inconsistant_def_check(){
	SymbolInfo* temp = symbolTable.lookUpAllScope(name_final);

            if(temp == NULL) {
            cout<<line_count<<" "<<name_final<<" "<<param_list.size()<<endl;
    		SymbolInfo* symbolInfo = new SymbolInfo(name_final, "ID");
    		symbolInfo->set_Type(type_final);  
    		symbolInfo->set_arrSize(-3); 
    		
    		for(int i=0; i<param_list.size(); i++) {
        		symbolInfo->addParam(param_list[i].param_type, param_list[i].param_name);
    		}

    		symbolTable.insertSymbol(symbolInfo);

            } else if(temp->get_arrSize() != -2) {
                errorprint("multiple declaration of "+name_final+"\n");

            } else {

                if(temp->get_Type() != type_final) {
                    errorprint("Return type mismatch with function declaration in function "+name_final+"\n");
                    temp->set_arrSize(-3); 

                } else if(temp->get_paramSize()==1 && param_list.size()==0 && temp->getParam(0).param_type=="void") {
                    temp->set_arrSize(-3); 

                } else if(temp->get_paramSize()==0 && param_list.size()==1 && param_list[0].param_type=="void") {
                    temp->set_arrSize(-3); 

                } else if(temp->get_paramSize() != param_list.size()) {
                    errorprint("Total number of arguments mismatch with declaration in function "+name_final+"\n");
                    temp->set_arrSize(-3); 
                    for(int i=0; i<param_list.size(); i++) {
                    	temp->clearParam_list();
        		temp->addParam(param_list[i].param_type, param_list[i].param_name);
    			}

                } else {
                    int i;

                    for(i=0; i<param_list.size(); i++) {
                        if(temp->getParam(i).param_type != param_list[i].param_type) {
                            break;
                        }
                    }

                    if(i == param_list.size()) {
                        temp->set_arrSize(-3); 
                    } else {
                        errorprint("inconsistant function definition with its declaration for "+name_final+"\n");
                    }
                }
            }
}
void Declaration_list_check(string name,string _size){
	temp_var.var_name = name;
	if(dec_check==1)
        	temp_var.var_size = -1;
        else if(dec_check==2){
        	stringstream temp_str(_size);
            	temp_str >> temp_var.var_size;
        }
        	
        var_list.push_back(temp_var);
        SymbolInfo* temp = symbolTable.lookUp(name);

        if(temp != NULL) {
            errorprint("multiple declaration of "+name+"\n");
        }
}
void insertParam(){
	if(param_list.size()==1 && param_list[0].param_type=="void") {
            } else {
                for(int i=0; i<param_list.size(); i++) {
                    cout<<param_list.size()<<endl;
                    temp_var.var_name = param_list[i].param_name;
                    temp_var.var_size = -1;
                    SymbolInfo* symbolInfo = new SymbolInfo(temp_var.var_name, "ID");
    		    symbolInfo->set_Type(param_list[i].param_type);  
    		    symbolInfo->set_arrSize(temp_var.var_size);
    		    symbolTable.insertSymbol(symbolInfo);
                }
            }

            param_list.clear(); 
}
%}

%token CONST_INT CONST_FLOAT ID
%token INT FLOAT VOID IF ELSE FOR WHILE PRINTLN RETURN
%token ASSIGNOP NOT INCOP DECOP LOGICOP RELOP ADDOP MULOP
%token LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON NEWLINE
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%%


start: program {
	    $$ = new SymbolInfo($1->getName(), "NON_TERMINAL");
            log << "Line " <<line_count << ": start : program" << "\n"  << endl; 
            log << $1->getName() << "\n"  << endl;
            
	}
	    ;

program: program unit {   
            $$ = new SymbolInfo($1->getName()+$2->getName(), "program_unit");
            log << "Line " << line_count << ": program : program unit" << "\n"  << endl;
            log << $1->getName()+$2->getName() << "\n"  << endl;
    }
	    | unit {
            $$ = new SymbolInfo($1->getName(), "unit");
            log << "Line " << line_count << ": program : unit" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
    }
	    ;
	
unit: var_declaration {
            $$ = new SymbolInfo($1->getName(), "var_declaration");
            log << "Line " << line_count << ": unit : var_declaration" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
    }
        | func_declaration {
            if(func_error_recovery==0){
            	$$ = new SymbolInfo($1->getName(), "func_declaration");
            	log << "Line " << line_count << ": unit : func_declaration" << "\n"  << endl;
            	log << $1->getName() << "\n"  << endl;
            }
            else
            	func_error_recovery = 0;
    }
        | func_definition {
            $$ = new SymbolInfo($1->getName(), "func_definition");
            log << "Line " << line_count << ": unit : func_definition" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
    }
        ;

func_declaration: type_specifier id Dummy LPAREN parameter_list RPAREN error {
	    func_error_recovery = 1;
	    $$ = new SymbolInfo("", "function_declaration_with_parameterlist");
	    error << "Error at line no: " << error_line << " " <<":Semicolon missing"<<endl;
	    log << "Error at line no: " << error_line << " " <<":Syntax Error"<<endl;
    	    log << "Error at line no: " << error_line << " " <<":Semicolon missing"<<endl;
	    yyerrok;
    }
        | type_specifier id Dummy LPAREN RPAREN error {
            func_error_recovery = 1;
	    $$ = new SymbolInfo("", "function_declaration_without_parameterlist");
            error << "Error at line no: " << error_line << " " <<":Semicolon missing"<<endl;
            log << "Error at line no: " << error_line << " " <<":Syntax Error"<<endl;
    	    log << "Error at line no: " << error_line << " " <<":Semicolon missing"<<endl;
            yyerrok;
    }
        ;
func_declaration: type_specifier id Dummy LPAREN parameter_list RPAREN {multiple_Dec_check();} SEMICOLON {
            $$ = new SymbolInfo($1->getName()+" "+$2->getName()+"("+$5->getName()+")"+";"+"\n", "function_declaration_with_parameterlist");
            log << "Line " << line_count << ": func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON" << "\n"  << endl;
            log << $1->getName()+" "+$2->getName()+"("+$5->getName()+")"+";" << "\n"  << endl;
            param_list.clear();
    }
        | type_specifier id Dummy LPAREN RPAREN {multiple_Dec_check();} SEMICOLON {
            $$ = new SymbolInfo($1->getName()+" "+$2->getName()+"();"+"\n", "function_declaration_without_parameterlist");
            log << "Line" << line_count << ": func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON" << "\n"  << endl;
            log << $1->getName()+" "+$2->getName()+"();" << "\n"  << endl;
            param_list.clear();
    }
        ;		 
func_definition: type_specifier id Dummy LPAREN parameter_list RPAREN {inconsistant_def_check();} compound_statement {
            $$ = new SymbolInfo($1->getName()+" "+$2->getName()+"("+$5->getName()+")"+$8->getName()+"\n", "function_definition_with_parameterlist");
            log << "Line " << line_count << ": func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement" << "\n"  << endl;
            log << $1->getName()+" "+$2->getName()+"("+$5->getName()+")"+$8->getName() << "\n"  << endl;
    }
        | type_specifier id Dummy LPAREN RPAREN {inconsistant_def_check();} compound_statement {
            $$ = new SymbolInfo($1->getName()+" "+$2->getName()+"()"+$7->getName()+"\n", "function_definition_without_parameterlist");
            log << "Line " << line_count << ": func_definition : type_specifier ID LPAREN RPAREN compound_statement" << "\n"  << endl;
            log << $1->getName()+" "+$2->getName()+"()"+$7->getName() << "\n"  << endl;
    }
        ;		

Dummy: {
	    error_line = line_count;
            type_final = type;
            name_final = name;
    }
        ;	
parameter_list: parameter_list COMMA type_specifier id {
            $$ = new SymbolInfo($1->getName()+","+$3->getName()+" "+(string)$4->getName(), "parameterlist_comma_type_id");
            for(int i=0;i<param_list.size();i++)
            {
            	if(param_list[i].param_name==$4->getName()){
            		errorprint("Multiple declaration of "+$4->getName()+" in parameter\n");
            		break;
            	}
            }
            log << "Line " << line_count << ": parameter_list : parameter_list COMMA type_specifier ID" << "\n"  << endl;
            log << $1->getName()+", "+$3->getName()+" "+$4->getName() << "\n"  << endl;
            parameter temp_parameter;
            temp_parameter.param_type = (string)$3->getName();
            temp_parameter.param_name = (string)$4->getName();
            param_list.push_back(temp_parameter);
    }
        | parameter_list COMMA type_specifier {
            $$ = new SymbolInfo($1->getName()+","+$3->getName(), "parameterlist_comma_type");
            log << "Line " << line_count << ": parameter_list : parameter_list COMMA type_specifier" << "\n"  << endl;
            log << $1->getName()+", "+$3->getName() << "\n"  << endl;
            parameter temp_parameter;
            temp_parameter.param_type = $3->getName();
            temp_parameter.param_name = "";

            param_list.push_back(temp_parameter);
    }
        | type_specifier id {
            $$ = new SymbolInfo($1->getName()+" "+$2->getName(), "type_id");
            log << "Line no: " << line_count << ": parameter_list : type_specifier ID" << "\n"  << endl;
            log << $1->getName()+" "+$2->getName() << "\n"  << endl;
            parameter temp_parameter;
            temp_parameter.param_type = $1->getName();
            temp_parameter.param_name = $2->getName();

            param_list.push_back(temp_parameter);
    }
        | type_specifier {
            $$ = new SymbolInfo($1->getName(), "type");
            log << "Line " << line_count << ": parameter_list : type_specifier" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
            parameter temp_parameter;
            temp_parameter.param_type = $1->getName();
            temp_parameter.param_name = "";

            param_list.push_back(temp_parameter);
    }
    	| parameter_list error {
	    $$ = new SymbolInfo($1->getName()+" ","param_error");
    	    log << "Error at line no "<<line_count<<": Syntax error"<<endl<<endl;
    	    log<< type<<endl<<endl;
    	    log <<"Error at line no "<<line_count<<": parameter's name not given in function definition"<< endl<<endl;
    	    error<<"Error at line no "<<line_count<<": parameter's name not given in function definition"<< endl<<endl;            
    }
        ;

compound_statement: LCURL {symbolTable.enterScope(scope_count++, 30, log);insertParam();} statements RCURL {
            $$ = new SymbolInfo("{ \n"+$3->getName()+"}"+"\n", "compound_statement_with_statements");  
            log << "Line " << line_count << ": compound_statement : LCURL statements RCURL" << "\n"  << endl;
            log << "{ \n"+$3->getName()+"}" << "\n"  << endl;
            symbolTable.printAllScope(log);
            symbolTable.exitScope(log);
    }
        | LCURL {symbolTable.enterScope(scope_count++, 30, log);insertParam();} RCURL {
            $$ = new SymbolInfo("{ \n\n}\n", "compound_statement_without_statements");  
            log << "Line " << line_count << ": compound_statement : LCURL RCURL" << "\n"  << endl;
            log << "{ }" << "\n"  << endl;
            symbolTable.printAllScope(log);
            symbolTable.exitScope(log);
    }
        ;	    
var_declaration: type_specifier declaration_list SEMICOLON {
            $$ = new SymbolInfo($1->getName()+" "+$2->getName()+";"+"\n", "type_declarationlist_semi");
            log << "Line " << line_count << ": var_declaration : type_specifier declaration_list SEMICOLON" << "\n"  << endl;
            log << $1->getName()+" "+$2->getName()+";" << "\n"  << endl;
            if($1->getName() == "void") {
		 errorprint("variable type can not be void\n");
                for(int i=0; i<var_list.size(); i++) {
                    SymbolInfo* symbolInfo = new SymbolInfo(var_list[i].var_name, "ID");
    		     symbolInfo->set_Type("float");  
    		     symbolInfo->set_arrSize(var_list[i].var_size);
    		     symbolTable.insertSymbol(symbolInfo);
                }
            } else {
                for(int i=0; i<var_list.size(); i++) {
                    //cout<<line_count<<" "<<var_list[i].var_name<<endl;
                    SymbolInfo* symbolInfo = new SymbolInfo(var_list[i].var_name, "ID");
    		     symbolInfo->set_Type((string)$1->getName());  
    		     symbolInfo->set_arrSize(var_list[i].var_size);
    		     symbolTable.insertSymbol(symbolInfo);
                }
            }

            var_list.clear();
    }
 		;
 		 
type_specifier: INT {
            $$ = new SymbolInfo("int", "INT");
            log << "Line no: " << line_count << ": type_specifier : INT" << "\n"  << endl;
            log << "int" << "\n"  << endl;

            type = "int";
    }
 		| FLOAT {
            $$ = new SymbolInfo("float", "FLOAT");
            log << "Line " << line_count << ": type_specifier : FLOAT" << "\n"  << endl;
            log << "float" << "\n"  << endl;

            type = "float";
    }
 		| VOID {
            $$ = new SymbolInfo("void", "VOID");
            log << "Line " << line_count << ": type_specifier : VOID" << "\n"  << endl;
            log << "void" << "\n"  << endl;

            type = "void";
    }
 		;

id: ID {
            $$ = new SymbolInfo((string)$1->getName(), "ID");
            name = $1->getName();
    }
        ;
		
declaration_list: declaration_list COMMA id {
            $$ = new SymbolInfo($1->getName()+","+$3->getName(), "declarationlist_comma_id");
            log << "Line " << line_count << ": declaration_list : declaration_list COMMA ID" << "\n"  << endl;
            log << $1->getName()+","+$3->getName() << "\n"  << endl;
            dec_check = 1;
            Declaration_list_check($3->getName(),"");
    }
 		| declaration_list COMMA id LTHIRD CONST_INT RTHIRD {
            $$ = new SymbolInfo($1->getName()+","+$3->getName()+"["+$5->getName()+"]", "declarationlist_comma_id[INT]");
            log << "Line " << line_count << ": declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD" << "\n"  << endl;
            log << $1->getName()+" , "+$3->getName()+"["+$5->getName()+"]" << "\n"  << endl;
            dec_check = 2;
            Declaration_list_check($3->getName(),$5->getName());
    }
 		| id {
            $$ = new SymbolInfo($1->getName(), "ID");
            log << "Line " << line_count << ": declaration_list : ID" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
            dec_check = 1;
            Declaration_list_check((string)$1->getName(),"");
    }
    		|declaration_list error id {
            $$ = new SymbolInfo((string)$1->getName(), "ID");
            log <<"Error at line no: "<<line_count<<" Syntax error\n"<<endl; 
            log<<$1->getName()<<endl<<endl;
            dec_check = 1;
            Declaration_list_check((string)$1->getName(),"");   	     
    }
 		| id LTHIRD CONST_INT RTHIRD {
            $$ = new SymbolInfo($1->getName()+"["+$3->getName()+"]", "id[INT]");
            log << "Line " << line_count << ": declaration_list : ID LTHIRD CONST_INT RTHIRD" << "\n"  << endl;
            log << $1->getName()+"["+$3->getName()+"]" << "\n"  << endl;
	    dec_check = 2;
            Declaration_list_check($1->getName(),$3->getName());
            array_list.push_back($1->getName());
    }
 		;
 		  
statements: statement {
            $$ = new SymbolInfo($1->getName(), "statement");
            log << "Line " << line_count << ": statements : statement" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
    }
	    | statements statement {
            $$ = new SymbolInfo($1->getName()+$2->getName(), "statements_statements");
            log << "Line " << line_count << ": statements : statements statement" << "\n"  << endl;
            log << $1->getName()+$2->getName() << "\n"  << endl;
    }
	    ;
	   
statement: var_declaration {
            $$ = new SymbolInfo($1->getName(), "var_declaration");
            log << "Line " << line_count << ": statement : var_declaration" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
    }
        | expression_statement {
            $$ = new SymbolInfo($1->getName(), "expression_statement");
            log << "Line " << line_count << ": statement : expression_statement" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
    }
        | compound_statement {
            $$ = new SymbolInfo($1->getName()+"\n", "compound_statement");
            log << "Line " << line_count << ": statement : compound_statement" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
    }
        | FOR LPAREN expression_statement error_exp error_void expression_statement error_exp error_void expression error_exp error_void RPAREN statement {
            $$ = new SymbolInfo("for("+$3->getName()+$6->getName()+$9->getName()+")"+$13->getName()+"\n", "for");
            log << "Line " << line_count << ": statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement" << "\n"  << endl;
            log << "for("+$3->getName()+$6->getName()+$9->getName()+")"+$13->getName() << "\n"  << endl;
    }
        | IF LPAREN expression error_exp RPAREN error_void statement %prec LOWER_THAN_ELSE {
            $$ = new SymbolInfo("if("+$3->getName()+")"+$7->getName()+"\n", "if_prec");
            log << "Line " << line_count << ": statement : IF LPAREN expression RPAREN statement" << "\n"  << endl;
            log << "if("+$3->getName()+")"+$7->getName() << "\n"  << endl;
    }
        | IF LPAREN expression error_exp RPAREN error_void statement ELSE statement {
            $$ = new SymbolInfo("if("+$3->getName()+")"+$7->getName()+" else"+$9->getName()+"\n", "if");
            log << "Line " << line_count << ": statement : IF LPAREN expression RPAREN statement ELSE statement" << "\n"  << endl;
            log << "if("+$3->getName()+")"+$7->getName()+" else"+$9->getName() << "\n"  << endl;
    }
        | WHILE LPAREN expression error_exp RPAREN error_void statement {
            $$ = new SymbolInfo("while("+$3->getName()+")"+$7->getName()+"\n", "while");
            log << "Line no: " << line_count << ": statement : WHILE LPAREN expression RPAREN statement" << "\n"  << endl;
            log << "while("+$3->getName()+")"+$7->getName() << "\n"  << endl;
    }
        | PRINTLN LPAREN id RPAREN SEMICOLON {
            SymbolInfo* temp = symbolTable.lookUpAllScope($3->getName());
            //cout<<"r valo lage na "<<$3->getName()<<endl;
            if(temp==NULL){
            	errorprint("Undeclared variable "+$3->getName()+"\n");
            }
            $$ = new SymbolInfo("printf("+$3->getName()+")"+";"+"\n", "println");
            log << "Line " << line_count << ": statement : PRINTLN LPAREN ID RPAREN SEMICOLON" << "\n"  << endl;
            log << "printf("+$3->getName()+")"+";" << "\n"  << endl;
    }
    	| PRINTLN LPAREN id RPAREN error {
            $$ = new SymbolInfo("printf("+$3->getName()+")\n", "println");
            log << "Error at line no " << line_count << ": Syntax error" << "\n"  << endl;
            log << "printf("+$3->getName()+")" << "\n"  << endl;
            log << "Error at line no " << line_count << ": Semicolon missing after println statement" << "\n"  << endl;
            error << "Error at line no " << line_count << ": Semicolon missing after println statement" << "\n"  << endl;            
    }
        | RETURN expression SEMICOLON {
            $$ = new SymbolInfo("return "+$2->getName()+";"+"\n", "return");
            log << "Line " << line_count << ": statement : RETURN expression SEMICOLON" << "\n"  << endl;
            log << "return "+$2->getName()+";" << "\n"  << endl;
            if($2->get_Type() == "void") {
                errorprint("void function used in expression\n");

            } 
    }
    	| RETURN expression error {
    	    $$ = new SymbolInfo("return "+$2->getName()+"\n", "return");
            log << "Error at line no" << line_count << ": Syntax error" << "\n"  << endl;
            log << "return "+$2->getName() << "\n"  << endl;
            log<<"Error at line no "<<line_count<<": Semicolon missing after return statement"<<endl<<endl;
            error<<"Error at line no "<<line_count<<": Semicolon missing after return statement"<<endl<<endl;
            if($2->get_Type() == "void") {
                errorprint("void function used in expression\n");

            } 
    }
        ;

error_exp: {
            type_final = type;
    }
        ;	

error_void: {
            
            if(type_final == "void") {
                errorprint("void function used in expression\n");
            } 
    }
        ;	
	  
expression_statement: SEMICOLON {
            $$ = new SymbolInfo((string)";"+"\n", "semicolon");
            log << "Line " << line_count << ": expression_statement : SEMICOLON" << "\n"  << endl;
            log << ";" << "\n"  << endl;
            $$->set_Type("int");
            type = "int";
    }		
        | expression SEMICOLON {
            $$ = new SymbolInfo($1->getName()+";"+"\n", "expression_semi");
            log << "Line " << line_count << ": expression_statement : expression SEMICOLON" << "\n"  << endl;
            log << $1->getName()+";" << "\n"  << endl;
           $$->set_Type($1->get_Type());
           type = $1->get_Type();
    }
    	| expression error {
            $$ = new SymbolInfo($1->getName()+"\n", "expression_semi");
            log << "Error at line no "<<line_count<<": Syntax error\n"<<endl;
            log << $1->getName()<<endl<<endl;
            log <<"Error at line no "<<line_count<<": Semicolon missing after expression"<<endl;
            error <<"Error at line no "<<line_count<<": Semicolon missing after expression"<<endl;
    }
        ;
	  
variable: id {
            $$ = new SymbolInfo($1->getName(), "id");
            log << "Line " << line_count << " variable: ID" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;           
            SymbolInfo* temp = symbolTable.lookUpAllScope($1->getName());

            if(temp == NULL) {
                errorprint("undeclared variable "+$1->getName()+"\n");

                $$->set_Type("float");  
            } else {
                if(temp->get_Type() != "void") {
                    $$->set_Type(temp->get_Type());
                } else {
                    $$->set_Type("float");  
                }
            }
            if((temp!=NULL) && (temp->get_arrSize()!=-1)) {
                errorprint("type mismatch, "+$1->getName()+" is not a variable\n");
                already_got = 1;
            }
            cv=$1->getName();
    }
        | id LTHIRD expression RTHIRD {
            $$ = new SymbolInfo($1->getName()+"["+$3->getName()+"]", "id[expression]");
            log << "Line " << line_count << ": variable : ID LTHIRD expression RTHIRD" << "\n"  << endl;
            log << $1->getName()+"["+$3->getName()+"]" << "\n"  << endl;
            SymbolInfo* temp = symbolTable.lookUpAllScope($1->getName());

            if(temp == NULL) {
                errorprint("undeclared variable "+$1->getName()+"\n");

                $$->set_Type("float"); 
            } else {
                if(temp->get_Type() != "void") {
                    $$->set_Type(temp->get_Type());
                } else {
                    $$->set_Type("float"); 
                }
            }

            if((temp!=NULL) && (temp->get_arrSize()<=-1)) {
                errorprint("type mismatch, "+$1->getName()+" is not a variable\n");
                already_got = 1;
            }

            if($3->get_Type() != "int") {
                errorprint("Expression inside third brackets not an integer\n");
            }            

            if($3->get_Type() == "void") {
                errorprint("void function used in expression\n");
            } 
             cv=$1->getName();
    }
        ;
	 
expression: logic_expression {
            $$ = new SymbolInfo($1->getName(), "logic_expression");
            log << "Line " << line_count << ": expression : logic_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  

            $$->set_Type($1->get_Type());
            type = $1->get_Type();
    }
        | variable ASSIGNOP logic_expression {
            $$ = new SymbolInfo($1->getName()+" = "+$3->getName(), "variable=logic_expression");
            log << "Line " << line_count << ": expression : variable ASSIGNOP logic_expression" << "\n"  << endl;
            log << $1->getName()+" = "+$3->getName() << "\n"  << endl; 
            if($3->get_Type() == "void") {
                errorprint("void function used in expression\n");
                $3->set_Type("float"); 
            } 
	     
            else if($1->get_Type() != $3->get_Type() && already_got!=1) {
		if(symbolTable.lookUpAllScope(cv)!=NULL){
			if(undefined_func=="blank"){
				if($1->get_Type()=="float" && $3->get_Type()=="int"){}
				else{
                			errorprint("type mismatch\n");
                		}
                	}
                	else
                	{
                		undefined_func = "blank";
                	}
                }
            }
            already_got = 0;
            $$->set_Type($1->get_Type());
            type = $1->get_Type();
    }
        ;
			
logic_expression: rel_expression {
            $$ = new SymbolInfo($1->getName(), "rel_expression");
            log << "Line " << line_count << ": logic_expression : rel_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl; 
            $$->set_Type($1->get_Type());
    }
        | rel_expression LOGICOP rel_expression {
            $$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(), "rel_expression_logic_rel_expression");
            log << "Line " << line_count << ": logic_expression : rel_expression LOGICOP rel_expression" << "\n"  << endl;
            log << $1->getName()+$2->getName()+$3->getName() << "\n"  << endl; 
            if($1->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            } 

            if($3->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            }
            $$->set_Type("int");
    }
        ;
			
rel_expression: simple_expression {
            $$ = new SymbolInfo($1->getName(), "simple_expression");
            log << "Line " << line_count << ": rel_expression : simple_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
    }
		| simple_expression RELOP simple_expression	{
            $$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(), "simple_expression_relop_simple_expression");
            log << "Line " << line_count << ": rel_expression : simple_expression RELOP simple_expression" << "\n"  << endl;
            log << $1->getName()+$2->getName()+$3->getName() << "\n"  << endl;
            if($1->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            } 

            if($3->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            }

            $$->set_Type("int");
    };
				
simple_expression: term {
            $$ = new SymbolInfo($1->getName(), "term");
            log << "Line " << line_count << ": simple_expression : term" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
    }
        | simple_expression ADDOP term {
            $$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(), "simple_expression+term");
            log << "Line " << line_count << ": simple_expression : simple_expression ADDOP term" << "\n"  << endl;
            log << $1->getName()+$2->getName()+$3->getName() << "\n"  << endl; 
            if($1->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                $1->set_Type("float"); 
            } 

            if($3->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                $3->set_Type("float"); 
            }

            if($1->get_Type()=="float" || $3->get_Type()=="float") {
                $$->set_Type("float");
            } else {
                $$->set_Type($1->get_Type()); 
            }
    }
        ;
					
term: unary_expression {
            $$ = new SymbolInfo($1->getName(), "unary_expression");
            log << "Line " << line_count << ": term : unary_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
    }
        |  term MULOP unary_expression {
            $$ = new SymbolInfo($1->getName()+$2->getName()+$3->getName(), "term_unary_expression");
            log << "Line " << line_count << ": term : term MULOP unary_expression" << "\n"  << endl;
            log << $1->getName()+$2->getName()+$3->getName() << "\n"  << endl; 
            if($1->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                $1->set_Type("float"); 
            } 

            if($3->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                $3->set_Type("float");  
            } 
            if(($2->getName() == "%") && $3->getName()=="0"){
            	errorprint("Modulus by zero\n");
            	log << $1->getName()+$2->getName()+$3->getName() << "\n"  << endl;
            	$$->set_Type($1->get_Type());
            }
            if(($2->getName() == "/") && $3->getName()=="0"){
            	errorprint("divided by zero\n");
            	log << $1->getName()+$2->getName()+$3->getName() << "\n"  << endl;
            	$$->set_Type($1->get_Type());
            }
            if(($2->getName() == "%") && ($1->get_Type() != "int" || $3->get_Type() != "int")) {
		 errorprint("Integer operand on modulus operator\n");
                $$->set_Type("int"); 
            } else if(($2->getName() != "%") && ($1->get_Type() == "float" || $3->get_Type() == "float")) {
                $$->set_Type("float"); 
            }
            else {
                $$->set_Type($1->get_Type());
            }
    }
        ;

unary_expression: ADDOP unary_expression {
            $$ = new SymbolInfo($1->getName()+$2->getName(), "+unary_expresssion");
            log << "Line " << line_count << ": unary_expression : ADDOP unary_expression" << "\n"  << endl;
            log << $1->getName()+$2->getName() << "\n"  << endl; 
            if($2->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                $$->set_Type("float");
            } else {
                $$->set_Type($2->get_Type());
            }
    }  
        | NOT unary_expression {
            $$ = new SymbolInfo("!"+$2->getName(), "!unary_expression");
            log << "Line " << line_count << ": unary_expression : NOT unary_expression" << "\n"  << endl;
            log << "!"+$2->getName() << "\n"  << endl;  
            if($2->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            }
            $$->set_Type("int");
    }
        | factor {
            $$ = new SymbolInfo($1->getName(), "factor");
            log << "Line " << line_count << ": unary_expression : factor" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
    }
        ;
	
factor: variable {
            $$ = new SymbolInfo($1->getName(), "variable");
            log << "Line " << line_count << ": factor : variable" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
    }
        | id LPAREN argument_list RPAREN {
            $$ = new SymbolInfo($1->getName()+"("+$3->getName()+")", "id(argument_list)");
            log << "Line " << line_count << ": factor : ID LPAREN argument_list RPAREN" << "\n"  << endl;
            log << $1->getName()+"("+$3->getName()+")" << "\n"  << endl;
            SymbolInfo* temp = symbolTable.lookUpAllScope($1->getName());
            if(temp == NULL) {
		errorprint("Undeclared Function "+$1->getName()+"\n");
		undefined_func = $1->getName();
                $$->set_Type("float"); 

            } else if(temp->get_arrSize() != -3) {
            	cout<<"factor"<<endl;
		errorprint("no such function definition found named "+$1->getName()+"\n");
		undefined_func = $1->getName();
                $$->set_Type("float"); 

            } else {
            	$$->set_Type(temp->get_Type());
                if(temp->get_paramSize()==1 && arg_list.size()==0 && temp->getParam(0).param_type=="void") {

                } else if(temp->get_paramSize() != arg_list.size()) {
		     errorprint("Total number of arguments mismatch in function "+$1->getName()+"\n");

                } else {
                    int i;

                    for(i=0; i<arg_list.size(); i++) {
                        if(temp->getParam(i).param_type != arg_list[i]) {
                            break;
                        }
                    }

                    if(i != arg_list.size() && already_got==0) { 
			 log<<"Error at line no "<<line_count<<": "<<i+1<<"th argument mismatch in function "<<$1->getName()<<"\n"<<endl;
			 error<<"Error at line no "<<line_count<<": "<<i+1<<"th argument mismatch in function "<<$1->getName()<<"\n"<<endl;
			 error_count++;

                    }
                    else if(i != arg_list.size() && already_got==1){already_got = 0;}
                }
            }

            arg_list.clear(); 
    }
        | LPAREN expression RPAREN {
            $$ = new SymbolInfo("("+$2->getName()+")", "(expression)");
            log << "Line " << line_count << ": factor : LPAREN expression RPAREN" << "\n"  << endl;
            log << "("+$2->getName()+")" << "\n"  << endl;
            if($2->get_Type() == "void") {
		errorprint("void function used in expression\n");
		already_got = 1;
                $2->set_Type("float"); 
            } 
            $$->set_Type($2->get_Type());
    }
        | CONST_INT {
            $$ = new SymbolInfo($1->getName(), "CONST_INT");
            log << "Line " << line_count << ": factor : CONST_INT" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type("int");
    }
        | CONST_FLOAT {
            $$ = new SymbolInfo($1->getName(), "CONST_FLOAT");
            log << "Line no: " << line_count << ": factor : CONST_FLOAT" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type("float");
    }
        | variable INCOP {
            $$ = new SymbolInfo($1->getName()+"++", "variable++");
            log << "Line " << line_count << ": factor : variable INCOP" << "\n"  << endl;
            log << $1->getName()+"++" << "\n"  << endl; 
            $$->set_Type($1->get_Type());
    }
        | variable DECOP {
            $$ = new SymbolInfo($1->getName()+"--", "variable--");
            log << "Line " << line_count << ": factor : variable DECOP" << "\n"  << endl;
            log << $1->getName()+"--" << "\n"  << endl; 
            $$->set_Type($1->get_Type());
    }
    	| id LPAREN argument_list error{
    	    $$ = new SymbolInfo($1->getName()+" ("+$3->getName(),"error_arg");
	    log<<"Error at line no "<<line_count<<": Syntax error\n"<<endl;
	    log << $1->getName()+" ("<<$3->getName()+" "<<endl<<endl;
	    log<<"Error at line no "<<line_count<<": Right paranthesis missing"<<endl<<endl;
	    error<<"Error at line no "<<line_count<<": Right paranthesis missing"<<endl<<endl;    	        
    }
        ;
	
argument_list: arguments {
            $$ = new SymbolInfo($1->getName(), "arguments");
            log << "Line " << line_count << ": argument_list : arguments" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
    }
        | {
            $$ = new SymbolInfo("", "empty");
            log << "Line " << line_count << ": argument_list : <epsilon-production>" << "\n"  << endl;
            log << "" << "\n"  << endl;  
    }
        ;
	
arguments: arguments COMMA logic_expression {
            $$ = new SymbolInfo($1->getName()+", "+$3->getName(), "argument,logic_expression");
            log << "Line " << line_count << ": arguments : arguments COMMA logic_expression" << "\n"  << endl;
            log << $1->getName()+", "+$3->getName() << "\n"  << endl;  
            if($3->get_Type() == "void") {
                errorprint("void function called within argument of function\n");
                $3->set_Type("float"); 
            } 
            arg_list.push_back($3->get_Type());
    }
        | logic_expression {
            $$ = new SymbolInfo($1->getName(), "logic_expression");
            log << "Line " << line_count << ": arguments : logic_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            if($1->get_Type() == "void") {
                errorprint("void function used in argument of function\n");
                $1->set_Type("float"); 
            } 
            arg_list.push_back($1->get_Type());
    }
        ;
 

%%


int main(int argc, char* argv[]) {
	if(argc != 2) {
		cout << "Please provide an input file and try again" << endl;
		return 0;
	}

    input = fopen(argv[1], "r");

    if(input == NULL) {
		cout << "input file not opened properly" << endl;
		exit(EXIT_FAILURE);
	}

	log.open("log.txt", ios::out);
	error.open("error.txt", ios::out);
	
	symbolTable.enterScope(scope_count++, 30, log);  
	
	yyin = input;
    	yyparse(); 

    	log << endl;
	symbolTable.printAllScope(log);
	symbolTable.exitScope(log);

	log << "Total Lines: " << line_count << endl; 
	log << "Total Errors: " << error_count << endl;
    	//error << "Total Errors: " << error_count << endl;
	
	fclose(yyin);
	log.close();
	error.close();
	
	return 0;
} 

