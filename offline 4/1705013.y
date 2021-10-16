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
int scope_count = 0;
int func_error_recovery = 0;
int error_line;
int dec_check;
int already_got = 0;

// Newly added from here 
int l_count = 0;
int temp_var_count = 0;

string newLabel() {
    string label = "Label";
    string label_no = to_string(l_count);
    label+= label_no;
    l_count++;
    return label;
}

string newTemp() {
    string temp_var = "t";
    string temp_var_no = to_string(temp_var_count);
    temp_var += temp_var_no;
    temp_var_count++;
    return temp_var;
}

SymbolTable symbolTable;
string cv;
FILE* input;
ofstream log;
ofstream error;
ofstream asmFile;
ofstream optasmFile;
string undefined_func="blank";
string type, type_final; 
string name, name_final;
string final_code="";

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
vector<string> declaration_list;
vector<parameter> param_list;
vector<string> list_arg;
vector<string> pm_list;
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
string insertVar(string _type, var var_in) {
 
    string str = var_in.var_name, temp;
    temp = to_string(scope_count);
    str += temp;

    if(var_in.var_size == -1) {
        declaration_list.push_back(str+(string)" dw ?");

    }
    return str;
}
void multiple_Dec_check(){
	SymbolInfo* temp = symbolTable.lookUpAllScope(name_final);

            if(temp != NULL) {
                errorprint("multiple declaration of "+name_final+"\n");
            } else {
    		SymbolInfo* symbolInfo = new SymbolInfo(name_final, "ID");
    		symbolInfo->set_Type(type_final);  
    		symbolInfo->set_arrSize(-2); 
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
    		    string lst = insertVar(param_list[i].param_type, temp_var);
    		    symbolInfo->setcode_symbol(lst);
    		    pm_list.push_back(lst);
                }
            }

            param_list.clear(); 
}
void optimization(string code){
	string temp;
    stringstream ss(code);
    vector<string> tokens, tokens_1, tokens_2;

    while(getline(ss, temp, '\n')) {
        if(temp[1]!=';')
    		tokens.push_back(temp);
    }

    int line_count = tokens.size();

    for(int i=0; i<line_count; i++) {
        if((tokens[i].length() < 4) || (tokens[i+1].length() < 4)) {
            optasmFile << tokens[i] << endl;
            continue;
        }

        if((tokens[i].substr(1,3) == "mov") && (tokens[i+1].substr(1,3) == "mov")) {
            stringstream ss_1(tokens[i]), ss_2(tokens[i+1]);

            while(getline(ss_1, temp, ' ')) {
                tokens_1.push_back(temp);
            }

            while(getline(ss_2, temp, ' ')) {
                tokens_2.push_back(temp);
            }
            if((tokens_1[1].substr(0, tokens_1[1].length()-1) == tokens_2[2]) && (tokens_2[1].substr(0, tokens_2[1].length()-1) == tokens_1[2])) {
                optasmFile << tokens[i] << endl;
                i++;
            } else {
                optasmFile << tokens[i] << endl;
            }

            tokens_1.clear();
            tokens_2.clear();
        } else {
            optasmFile << tokens[i] << endl;
        }
    }

    tokens.clear();
    return ;
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
            if(error_count == 0) {
                final_code = "";
                final_code += (string)".model small\n.stack 100h\n.data\n\n";

                for(int i=0; i<declaration_list.size(); i++) {
                    final_code += (string)"\t"+(string)declaration_list[i]+(string)"\n";
                }

                declaration_list.clear();
                final_code += (string)"\n\taddress dw 0\n";
                final_code += (string)"\n.code\n\n";
                final_code += $1->getCode();
                
                final_code+= 
                "println PROC \n\
 		 PUSH AX \n\ 
    		 PUSH BX \n\ 
                PUSH CX \n\ 
                PUSH DX  \n\ 
                CMP AX,0 \n\ 
                JGE BEGIN \n\
                PUSH AX \n\ 
                MOV DL,'-' \n\ 
                MOV AH,2 \n\ 
                INT 21H \n\
                POP AX \n\ 
                NEG AX \n\ 
     		 BEGIN: \n\ 
    		XOR CX,CX \n\ 
    		MOV BX,10 \n\ 
    		REPEAT: \n\ 
    		XOR DX,DX \n\ 
    		IDIV BX \n\ 
    		PUSH DX \n\ 
    		INC CX \n\ 
    		OR AX,AX \n\ 
    		JNE REPEAT \n\ 
    		MOV AH,2 \n\ 
    		PRINT_LOOP: \n\ 
    		POP DX \n\ 
    		ADD DL,30H \n\ 
    		INT 21H \n\ 
    		LOOP PRINT_LOOP \n\    
    		MOV AH,2\n\
    		MOV DL,10\n\
    		INT 21H\n\
    		MOV DL,13\n\
    		INT 21H\n\
    		POP DX \n\ 
    		POP CX \n\ 
    		POP BX \n\ 
    		POP AX \n\ 
    		ret \n\ 
    		println ENDP \n";
                
		final_code += (string)"end main";
                $$->setCode(final_code);
                asmFile << $$->getCode() << endl;
                optimization($$->getCode());

            } else {
               
            }    
	}
	    ;

program: program unit {   
            $$ = new SymbolInfo($1->getName()+$2->getName(), "program_unit");
            log << "Line " << line_count << ": program : program unit" << "\n"  << endl;
            log << $1->getName()+$2->getName() << "\n"  << endl;
            $$->setCode($1->getCode()+$2->getCode());
    }
	    | unit {
            $$ = new SymbolInfo($1->getName(), "unit");
            log << "Line " << line_count << ": program : unit" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
            $$->setCode($1->getCode());
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
            $$->setCode($1->getCode());
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
            string temp="";
            if(($2->getName() == "main")) {
            	cout<<line_count<<" "<<endl;
                temp += (string)"main proc\n\tmov ax, @data\n\tmov ds ,ax\n\n";
                temp += $8->getCode();
                temp += (string)"\n\n\tmov ah, 4ch\n\tint 21h\nmain endp\n\n";

            }
            else {
                
                temp += $2->getName()+(string)" proc\n\tpop address\n";

                for(int i=(pm_list.size()-1); i>=0; i--) {
                    temp += (string)"\tpop "+pm_list[i]+(string)"\n";
                }

                temp += $8->getCode();

                temp += (string)"\tpush address\n\tret\n";
                temp += $2->getName()+(string)" endp\n\n";
            }
            pm_list.clear(); 
            $$->setCode(temp);
    }
        | type_specifier id Dummy LPAREN RPAREN {inconsistant_def_check();} compound_statement {
            $$ = new SymbolInfo($1->getName()+" "+$2->getName()+"()"+$7->getName()+"\n", "function_definition_without_parameterlist");
            log << "Line " << line_count << ": func_definition : type_specifier ID LPAREN RPAREN compound_statement" << "\n"  << endl;
            log << $1->getName()+" "+$2->getName()+"()"+$7->getName() << "\n"  << endl;
            string temp="";
            if(($2->getName() == "main")) {
            	cout<<line_count<<endl;
                temp += (string)"main proc\n\tmov ax, @data\n\tmov ds ,ax\n\n";
                temp += $7->getCode();
                temp += (string)"\n\n\tmov ah, 4ch\n\tint 21h\nmain endp\n\n";

            }
            else {
                
                temp += $2->getName()+(string)" proc\n\tpop address\n";

                for(int i=(pm_list.size()-1); i>=0; i--) {
                    temp += (string)"\tpop "+pm_list[i]+(string)"\n";
                }

                temp += $7->getCode();

                temp += (string)"\tpush address\n\tret\n";
                temp += $2->getName()+(string)" endp\n\n";
            }

            $$->setCode(temp);
            pm_list.clear();  
            $$->setCode(temp);
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
            $$->setCode($3->getCode());
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
            } else {
                for(int i=0; i<var_list.size(); i++) {
                    SymbolInfo* symbolInfo = new SymbolInfo(var_list[i].var_name, "ID");
    		     symbolInfo->set_Type((string)$1->getName());  
    		     symbolInfo->set_arrSize(var_list[i].var_size);
    	   
    		     string str = var_list[i].var_name, temp;
    		     temp = to_string(scope_count);
                    str += temp;
                    symbolInfo->setcode_symbol(str);

    		    if(var_list[i].var_size == -1) {
        		declaration_list.push_back(str+(string)" dw ?");

    		     } else {
        		str += " dw ";
        		temp = to_string(var_list[i].var_size);
        		str += temp;
        		str += " dup (?)";
        		declaration_list.push_back(str);
    		     }
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
            $$->setCode($1->getCode());
    }
	    | statements statement {
            $$ = new SymbolInfo($1->getName()+$2->getName(), "statements_statements");
            log << "Line " << line_count << ": statements : statements statement" << "\n"  << endl;
            log << $1->getName()+$2->getName() << "\n"  << endl;
            $$->setCode($1->getCode()+$2->getCode());
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
            $$->setCode($1->getCode());
    }
        | compound_statement {
            $$ = new SymbolInfo($1->getName()+"\n", "compound_statement");
            log << "Line " << line_count << ": statement : compound_statement" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;
            $$->setCode($1->getCode());
    }
        | FOR LPAREN expression_statement error_exp error_void expression_statement error_exp error_void expression error_exp error_void RPAREN statement {
            $$ = new SymbolInfo("for("+$3->getName()+$6->getName()+$9->getName()+")"+$13->getName()+"\n", "for");
            log << "Line " << line_count << ": statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement" << "\n"  << endl;
            log << "for("+$3->getName()+$6->getName()+$9->getName()+")"+$13->getName() << "\n"  << endl;
            
            if(($3->getcode_symbol() != ";") && ($6->getcode_symbol() != ";")) {
                string label1 = newLabel();
                string label2 = newLabel();
		            	cout<<line_count<<endl;
                $$->setCode($3->getCode()+"\t;Line "+to_string(line_count)+" for loop\n");
                $$->setCode($$->getCode()+(string)"\t"+label1+(string)":\n"+$6->getCode()+(string)"\tmov ax, "+$6->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label2+(string)"\n");
                $$->setCode($$->getCode()+$13->getCode()+$9->getCode()+(string)"\tjmp "+label1+(string)"\n\t"+label2+(string)":\n");
            }

    }
        | IF LPAREN expression error_exp RPAREN error_void statement %prec LOWER_THAN_ELSE {
            $$ = new SymbolInfo("if("+$3->getName()+")"+$7->getName()+"\n", "if_prec");
            log << "Line " << line_count << ": statement : IF LPAREN expression RPAREN statement" << "\n"  << endl;
            log << "if("+$3->getName()+")"+$7->getName() << "\n"  << endl;
            string label = newLabel();
		            	cout<<line_count<<endl;
            $$->setCode($3->getCode()+"\t;Line "+to_string(line_count)+" "+$1->getName()+" "+"("+$3->getName()+")\n"+(string)"\tmov ax, "+$3->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label+(string)"\n"+$7->getCode()+(string)"\t"+label+(string)":\n");
    }
        | IF LPAREN expression error_exp RPAREN error_void statement ELSE statement {
            $$ = new SymbolInfo("if("+$3->getName()+")"+$7->getName()+" else"+$9->getName()+"\n", "if");
            log << "Line " << line_count << ": statement : IF LPAREN expression RPAREN statement ELSE statement" << "\n"  << endl;
            log << "if("+$3->getName()+")"+$7->getName()+" else"+$9->getName() << "\n"  << endl;
            string label1 = newLabel();
            string label2 = newLabel();
		            	cout<<line_count<<endl;
            $$->setCode($3->getCode()+"\t;Line "+to_string(line_count)+" "+$1->getName()+" "+"("+$3->getName()+")"+"\n"+(string)"\tmov ax, "+$3->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label1+(string)"\n"+$7->getCode()+(string)"\tjmp "+label2+(string)"\n");
            $$->setCode($$->getCode()+(string)"\t"+label1+(string)":\n"+$9->getCode()+(string)"\t"+label2+(string)":\n");

    }
        | WHILE LPAREN expression error_exp RPAREN error_void statement {
            $$ = new SymbolInfo("while("+$3->getName()+")"+$7->getName()+"\n", "while");
            log << "Line no: " << line_count << ": statement : WHILE LPAREN expression RPAREN statement" << "\n"  << endl;
            log << "while("+$3->getName()+")"+$7->getName() << "\n"  << endl;
            string label1 = newLabel();
            string label2 = newLabel();
		            	cout<<line_count<<endl;
            $$->setCode("\t;Line "+to_string(line_count)+" while("+$3->getName()+")"+"\n"+(string)"\t"+label1+(string)":\n"+$3->getCode()+(string)"\tmov ax, "+$3->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label2+(string)"\n");
            $$->setCode($$->getCode()+$7->getCode()+(string)"\tjmp "+label1+(string)"\n\t"+label2+(string)":\n");
    }
        | PRINTLN LPAREN id RPAREN SEMICOLON {
            SymbolInfo* temp = symbolTable.lookUpAllScope($3->getName());
	     string var;
            if(temp==NULL){
            	errorprint("Undeclared variable "+$3->getName()+"\n");
            	var = "";
            }
            else{
            	if(temp->get_Type() != "void") {
                    var = temp->getcode_symbol();
                } else {
                    var = "";
                }
            }
            $$ = new SymbolInfo("printf("+$3->getName()+")"+";"+"\n", "println");
            log << "Line " << line_count << ": statement : PRINTLN LPAREN ID RPAREN SEMICOLON" << "\n"  << endl;
            log << "printf("+$3->getName()+")"+";" << "\n"  << endl;
                        	cout<<line_count<<endl;
            $$->setCode("\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+var+"\n\tcall println\n");
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
            $$->setCode($2->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tpush "+$2->getcode_symbol()+(string)"\n");
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
           $$->setcode_symbol($1->getcode_symbol());
           $$->setCode($1->getCode());
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
		 $$->set_arrSize(-1);
                $$->set_Type("float");  
            } else {
            	 $$->set_arrSize(-1);
                if(temp->get_Type() != "void") {
                    $$->set_Type(temp->get_Type());
                    $$->setcode_symbol(temp->getcode_symbol());
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
		 $$->set_arrSize(0);
                $$->set_Type("float"); 
            } else {
                if(temp->get_Type() != "void") {
                    $$->set_arrSize(temp->get_arrSize());
                    $$->set_Type(temp->get_Type());
                    $$->setcode_symbol(temp->getcode_symbol());
                } else {
                    $$->set_Type("float"); 
                    $$->set_arrSize(0);
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
            $$->setcode_symbol($1->getcode_symbol());
            $$->setCode($1->getCode());
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
            
            if($1->get_arrSize() > -1) {
                string temp = newTemp();
                declaration_list.push_back(temp+(string)" dw ?");
            	cout<<line_count<<endl;
                $$->setCode($3->getCode()+$1->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$3->getcode_symbol()+(string)"\n");
                $$->setCode($$->getCode()+(string)"\tmov "+$1->getcode_symbol()+(string)"[bx], ax\n\tmov "+temp+(string)", ax\n"); 
                $$->setcode_symbol(temp);
            } else {
                        	cout<<line_count<<endl;
                $$->setCode($1->getCode()+$3->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$3->getcode_symbol()+(string)"\n\tmov "+$1->getcode_symbol()+(string)", ax\n");
                $$->setcode_symbol($1->getcode_symbol());
            }

    }
        ;
			
logic_expression: rel_expression {
            $$ = new SymbolInfo($1->getName(), "rel_expression");
            log << "Line " << line_count << ": logic_expression : rel_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl; 
            $$->set_Type($1->get_Type());
            $$->setcode_symbol($1->getcode_symbol());
            $$->setCode($1->getCode());
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
            string label1 = newLabel();
            string label2 = newLabel();
            string temp = newTemp();
            declaration_list.push_back(temp+(string)" dw ?");

            $$->setCode($1->getCode()+$3->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n");
            
            if($2->getName() == "&&") {
                        	cout<<line_count<<endl;
                $$->setCode($$->getCode()+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label1+(string)"\n");
                $$->setCode($$->getCode()+(string)"\tmov ax, "+$3->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label1+(string)"\n");
                $$->setCode($$->getCode()+(string)"\tmov ax, 1\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n\t");
                $$->setCode($$->getCode()+label1+(string)":\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\t");
                $$->setCode($$->getCode()+label2+(string)":\n");
            } else {
                        	cout<<line_count<<endl;
                $$->setCode($$->getCode()+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tjne "+label1+(string)"\n");
                $$->setCode($$->getCode()+(string)"\tmov ax, "+$3->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tjne "+label1+(string)"\n");
                $$->setCode($$->getCode()+(string)"\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n\t");
                $$->setCode($$->getCode()+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t");
                $$->setCode($$->getCode()+label2+(string)":\n");
            }
            
            $$->setcode_symbol(temp);
    }
        ;
			
rel_expression: simple_expression {
            $$ = new SymbolInfo($1->getName(), "simple_expression");
            log << "Line " << line_count << ": rel_expression : simple_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
            $$->setcode_symbol($1->getcode_symbol());
            $$->setCode($1->getCode());
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
            string label1 = newLabel();
            string label2 = newLabel();
            string temp = newTemp();
            declaration_list.push_back(temp+(string)" dw ?");
		            	cout<<line_count<<endl;
            $$->setCode($1->getCode()+$3->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tcmp ax, "+$3->getcode_symbol()+(string)"\n");

            if($2->getName() == "<") {
                $$->setCode($$->getCode()+(string)"\tjl "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                $$->setCode($$->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else if($2->getName() == "<=") {
                $$->setCode($$->getCode()+(string)"\tjle "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                $$->setCode($$->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else if($2->getName() == ">") {
                $$->setCode($$->getCode()+(string)"\tjg "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                $$->setCode($$->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else if($2->getName() == ">=") {
                $$->setCode($$->getCode()+(string)"\tjge "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                $$->setCode($$->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else if($2->getName() == "==") {
                $$->setCode($$->getCode()+(string)"\tje "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                $$->setCode($$->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else {
                $$->setCode($$->getCode()+(string)"\tjne "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                $$->setCode($$->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            }

            $$->setcode_symbol(temp);
    };
				
simple_expression: term {
            $$ = new SymbolInfo($1->getName(), "term");
            log << "Line " << line_count << ": simple_expression : term" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
            $$->setcode_symbol($1->getcode_symbol());
            $$->setCode($1->getCode());
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
            string temp = newTemp();
            declaration_list.push_back(temp+(string)" dw ?");

            if($2->getName() == "+") {
                        	cout<<line_count<<endl;
                $$->setCode($1->getCode()+$3->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tadd ax, "+$3->getcode_symbol()+(string)"\n\tmov "+temp+(string)", ax\n");
                $$->setcode_symbol(temp);

            } else {
                        	cout<<line_count<<endl;
                $$->setCode($1->getCode()+$3->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tsub ax, "+$3->getcode_symbol()+(string)"\n\tmov "+temp+(string)", ax\n");
                $$->setcode_symbol(temp);
            }
    }
        ;
					
term: unary_expression {
            $$ = new SymbolInfo($1->getName(), "unary_expression");
            log << "Line " << line_count << ": term : unary_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
            $$->setcode_symbol($1->getcode_symbol());
            $$->setCode($1->getCode());
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
            string temp = newTemp();
            declaration_list.push_back(temp+(string)" dw ?");

            if($2->getName() == "*") {
                $$->setCode($1->getCode()+$3->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tmov bx, "+$3->getcode_symbol()+(string)"\n\timul bx\n\tmov "+temp+(string)", ax\n");
                $$->setcode_symbol(temp);

            } else {
                $$->setCode($1->getCode()+$3->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tcwd\n");
                $$->setCode($$->getCode()+(string)"\tmov bx, "+$3->getcode_symbol()+(string)"\n\tidiv bx\n");
                
                if($2->getName() == "/") {
                    $$->setCode($$->getCode()+(string)"\tmov "+temp+(string)", ax\n");
                } else {
                    $$->setCode($$->getCode()+(string)"\tmov "+temp+(string)", dx\n");
                }
                
                $$->setcode_symbol(temp);
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
            if($1->getName() == "-") {
                string temp = newTemp();
                declaration_list.push_back(temp+(string)" dw ?");

                $$->setCode($2->getCode()+(string)"\tmov ax, "+$2->getcode_symbol()+(string)"\n\tmov "+temp+(string)", ax\n\tneg "+temp+(string)"\n");
                $$->setcode_symbol(temp);
            } else {
                $$->setcode_symbol($2->getcode_symbol());
                $$->setCode($2->getCode());
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
            string tempCode="";
            tempCode += $2->getCode();
	    tempCode += "\tMOV AX," + $2->getcode_symbol()+"\n";
	    tempCode += "\tNOT AX\n";
            tempCode += "\tMOV " + $2->getcode_symbol()+",AX\n";

	    $$->setCode(tempCode);
	    $$->setcode_symbol($2->getcode_symbol());
    }
        | factor {
            $$ = new SymbolInfo($1->getName(), "factor");
            log << "Line " << line_count << ": unary_expression : factor" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
            $$->setcode_symbol($1->getcode_symbol());
            $$->setCode($1->getCode());
    }
        ;
	
factor: variable {
            $$ = new SymbolInfo($1->getName(), "variable");
            log << "Line " << line_count << ": factor : variable" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type($1->get_Type());
            $$->set_arrSize($1->get_arrSize());

            $$->setcode_symbol($1->getcode_symbol());
            $$->setCode($1->getCode());

            if($$->get_arrSize() > -1) {
                string temp = newTemp();
                declaration_list.push_back(temp+(string)" dw ?");

                $$->setCode($$->getCode()+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"[bx]\n\tmov "+temp+(string)", ax\n");
                $$->setcode_symbol(temp);
            }

    }
        | id LPAREN argument_list RPAREN {
            $$ = new SymbolInfo($1->getName()+"("+$3->getName()+")", "id(argument_list)");
            log << "Line " << line_count << ": factor : ID LPAREN argument_list RPAREN" << "\n"  << endl;
            log << $1->getName()+"("+$3->getName()+")" << "\n"  << endl;
            SymbolInfo* temp = symbolTable.lookUpAllScope($1->getName());
            bool valid =false;
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
                	$$->set_Type(temp->get_Type());
                	valid =true;

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
                    else{
                    	$$->set_Type(temp->get_Type());
                       valid = true;
                    }
                }
            }
	     if(valid == true) {
                string _temp = newTemp();
                declaration_list.push_back(_temp+(string)" dw ?");

                $$->setCode($3->getCode());
                $$->setCode($$->getCode()+(string)"\tpush address\n");

                for(int i=0; i<list_arg.size(); i++) {
                    $$->setCode($$->getCode()+(string)"\tpush "+list_arg[i]+(string)"\n");
                }

                $$->setCode($$->getCode()+(string)"\tcall "+temp->getName()+(string)"\n");

                if(temp->get_Type() != "void") {
                    $$->setCode($$->getCode()+(string)"\tpop "+_temp+(string)"\n");
                }

                $$->setCode($$->getCode()+(string)"\tpop address\n");
                $$->setcode_symbol(_temp);
            }
            arg_list.clear(); 
            list_arg.clear();
    }
        | LPAREN expression RPAREN {
            $$ = new SymbolInfo("("+$2->getName()+")", "(expression)");
            log << "Line " << line_count << ": factor : LPAREN expression RPAREN" << "\n"  << endl;
            log << "("+$2->getName()+")" << "\n"  << endl;
            $$->setcode_symbol($2->getcode_symbol());
            $$->setCode($2->getCode());
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
            $$->setcode_symbol($1->getName());
    }
        | CONST_FLOAT {
            $$ = new SymbolInfo($1->getName(), "CONST_FLOAT");
            log << "Line no: " << line_count << ": factor : CONST_FLOAT" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl;  
            $$->set_Type("float");
            $$->setcode_symbol($1->getName());
    }
        | variable INCOP {
            $$ = new SymbolInfo($1->getName()+"++", "variable++");
            log << "Line " << line_count << ": factor : variable INCOP" << "\n"  << endl;
            log << $1->getName()+"++" << "\n"  << endl; 
            $$->set_Type($1->get_Type());
	     
	     
            if($1->get_arrSize() > -1) {
                string temp1 = newTemp();
                declaration_list.push_back(temp1+(string)" dw ?");
                $$->setCode($1->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"[bx]\n\tmov "+temp1+(string)", ax\n");
                $$->setCode($$->getCode()+(string)"\tinc "+$1->getcode_symbol()+(string)"[bx]\n");

                $$->setcode_symbol(temp1);

            } else {
                string temp1 = newTemp();
                declaration_list.push_back(temp1+(string)" dw ?");
                $$->setCode($1->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tmov "+temp1+(string)", ax\n\tinc "+$1->getcode_symbol()+(string)"\n");

                $$->setcode_symbol(temp1);
            }

    }
        | variable DECOP {
            $$ = new SymbolInfo($1->getName()+"--", "variable--");
            log << "Line " << line_count << ": factor : variable DECOP" << "\n"  << endl;
            log << $1->getName()+"--" << "\n"  << endl; 
            $$->set_Type($1->get_Type());
            if($1->get_arrSize() > -1) {
                string temp1 = newTemp();
                declaration_list.push_back(temp1+(string)" dw ?");
                $$->setCode($1->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"[bx]\n\tmov "+temp1+(string)", ax\n");
                $$->setCode($$->getCode()+(string)"\tdec "+$1->getcode_symbol()+(string)"[bx]\n");

                $$->setcode_symbol(temp1);

            } else {
                string temp1 = newTemp();
                declaration_list.push_back(temp1+(string)" dw ?");
                $$->setCode($1->getCode()+"\t;Line "+to_string(line_count)+" "+$$->getName()+"\n"+(string)"\tmov ax, "+$1->getcode_symbol()+(string)"\n\tmov "+temp1+(string)", ax\n\tdec "+$1->getcode_symbol()+(string)"\n");

                $$->setcode_symbol(temp1);
            }
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
            $$->setCode($1->getCode()); 
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
            $$->setCode($1->getCode()+$3->getCode()); 
            if($3->get_Type() == "void") {
                errorprint("void function called within argument of function\n");
                $3->set_Type("float"); 
            } 
            arg_list.push_back($3->get_Type());
            list_arg.push_back($3->getcode_symbol());
    }
        | logic_expression {
            $$ = new SymbolInfo($1->getName(), "logic_expression");
            log << "Line " << line_count << ": arguments : logic_expression" << "\n"  << endl;
            log << $1->getName() << "\n"  << endl; 
            $$->setCode($1->getCode()); 
            if($1->get_Type() == "void") {
                errorprint("void function used in argument of function\n");
                $1->set_Type("float"); 
            } 
            arg_list.push_back($1->get_Type());
            list_arg.push_back($1->getcode_symbol());
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
	asmFile.open("code.asm",ios::out);
	optasmFile.open("optimized_code.asm",ios::out);
	
	symbolTable.enterScope(scope_count++, 30, log);  
	
	yyin = input;
    	yyparse(); 

    	log << endl;
	symbolTable.printAllScope(log);
	symbolTable.exitScope(log);

	log << "Total Lines: " << line_count << endl; 
	log << "Total Errors: " << error_count << endl;
	
	fclose(yyin);
	log.close();
	error.close();
	
	return 0;
} 

