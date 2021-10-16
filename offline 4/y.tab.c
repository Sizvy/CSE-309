/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "1705013.y"

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

#line 324 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    CONST_INT = 258,
    CONST_FLOAT = 259,
    ID = 260,
    INT = 261,
    FLOAT = 262,
    VOID = 263,
    IF = 264,
    ELSE = 265,
    FOR = 266,
    WHILE = 267,
    PRINTLN = 268,
    RETURN = 269,
    ASSIGNOP = 270,
    NOT = 271,
    INCOP = 272,
    DECOP = 273,
    LOGICOP = 274,
    RELOP = 275,
    ADDOP = 276,
    MULOP = 277,
    LPAREN = 278,
    RPAREN = 279,
    LCURL = 280,
    RCURL = 281,
    LTHIRD = 282,
    RTHIRD = 283,
    COMMA = 284,
    SEMICOLON = 285,
    NEWLINE = 286,
    LOWER_THAN_ELSE = 287
  };
#endif
/* Tokens.  */
#define CONST_INT 258
#define CONST_FLOAT 259
#define ID 260
#define INT 261
#define FLOAT 262
#define VOID 263
#define IF 264
#define ELSE 265
#define FOR 266
#define WHILE 267
#define PRINTLN 268
#define RETURN 269
#define ASSIGNOP 270
#define NOT 271
#define INCOP 272
#define DECOP 273
#define LOGICOP 274
#define RELOP 275
#define ADDOP 276
#define MULOP 277
#define LPAREN 278
#define RPAREN 279
#define LCURL 280
#define RCURL 281
#define LTHIRD 282
#define RTHIRD 283
#define COMMA 284
#define SEMICOLON 285
#define NEWLINE 286
#define LOWER_THAN_ELSE 287

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   167

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  33
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  82
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  145

#define YYUNDEFTOK  2
#define YYMAXUTOK   287


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   266,   266,   336,   342,   350,   355,   364,   372,   380,
     389,   389,   395,   395,   402,   402,   430,   430,   462,   468,
     484,   494,   504,   514,   523,   523,   531,   531,   539,   574,
     581,   588,   597,   603,   610,   617,   624,   631,   641,   647,
     655,   660,   666,   672,   687,   695,   706,   716,   736,   743,
     753,   766,   771,   779,   786,   795,   804,   829,   866,   876,
     919,   927,   968,   976,  1021,  1029,  1066,  1074,  1130,  1152,
    1170,  1180,  1199,  1270,  1283,  1290,  1297,  1321,  1342,  1351,
    1357,  1364,  1376
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CONST_INT", "CONST_FLOAT", "ID", "INT",
  "FLOAT", "VOID", "IF", "ELSE", "FOR", "WHILE", "PRINTLN", "RETURN",
  "ASSIGNOP", "NOT", "INCOP", "DECOP", "LOGICOP", "RELOP", "ADDOP",
  "MULOP", "LPAREN", "RPAREN", "LCURL", "RCURL", "LTHIRD", "RTHIRD",
  "COMMA", "SEMICOLON", "NEWLINE", "LOWER_THAN_ELSE", "$accept", "start",
  "program", "unit", "func_declaration", "$@1", "$@2", "func_definition",
  "$@3", "$@4", "Dummy", "parameter_list", "compound_statement", "$@5",
  "$@6", "var_declaration", "type_specifier", "id", "declaration_list",
  "statements", "statement", "error_exp", "error_void",
  "expression_statement", "variable", "expression", "logic_expression",
  "rel_expression", "simple_expression", "term", "unary_expression",
  "factor", "argument_list", "arguments", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287
};
# endif

#define YYPACT_NINF (-91)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-27)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     150,   -91,   -91,   -91,     9,   150,   -91,   -91,   -91,   -91,
      11,   -91,   -91,   -91,    40,    18,    15,     4,    11,    11,
     -91,     2,   100,   -91,    53,   -91,    30,    50,    11,    58,
     -91,    64,    72,   -91,    32,   150,   -91,    61,   -91,    75,
     -91,   -91,    73,    72,    11,   -91,   107,    91,   -91,   -91,
     -91,   -91,   -91,    99,   106,   108,   113,   130,   130,   130,
     130,   -91,   -91,   -91,    11,    48,    79,   -91,   -91,    81,
      24,   -91,   121,    20,   120,   -91,   -91,   -91,   130,   122,
     130,    11,    28,    59,   -91,   -91,   123,   117,   130,   130,
     -91,   -91,   130,   -91,   -91,   -91,   -91,   130,   130,   130,
     130,   -91,   -91,   -91,   124,   -91,   -91,   -91,   -91,    13,
     125,   131,   -91,   -91,   129,   120,   -91,   136,   -91,   137,
      34,   -91,   -91,   130,   -91,   -91,   122,   -91,   -91,   -91,
     -91,   107,   -91,   107,   152,   -91,   -91,   107,   130,   -91,
     -91,   -91,   139,   107,   -91
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    29,    30,    31,     0,     2,     4,     6,     7,     5,
       0,     1,     3,    32,    35,     0,     0,     0,     0,     0,
      28,     0,     0,    36,    33,    37,     0,     0,    22,     0,
       9,     0,     0,    23,     0,     0,    21,     0,    13,    24,
      17,     8,     0,     0,    20,    34,     0,     0,    11,    15,
      19,    74,    75,     0,     0,     0,     0,     0,     0,     0,
       0,    53,    42,    40,     0,    56,     0,    38,    41,    71,
       0,    58,    60,    62,    64,    66,    70,    27,     0,     0,
       0,     0,     0,    71,    69,    68,     0,    35,    80,     0,
      25,    39,     0,    76,    77,    55,    54,     0,     0,     0,
       0,    51,    51,    51,     0,    50,    49,    73,    82,     0,
      79,     0,    59,    61,    63,    65,    67,     0,    52,     0,
       0,    78,    72,     0,    57,    52,     0,    52,    48,    47,
      81,     0,    51,     0,    44,    52,    46,     0,     0,    45,
      51,    52,     0,     0,    43
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -91,   -91,   -91,   159,   -91,   -91,   -91,   -91,   -91,   -91,
     -91,   -91,   -17,   -91,   -91,    65,    17,    -8,   -91,   -91,
     -65,   -90,    14,   -73,   -54,   -57,   -64,    68,    69,    67,
     -51,   -91,   -91,   -91
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,     6,     7,    42,    31,     8,    43,    32,
      17,    27,    62,    46,    47,    63,    64,    65,    15,    66,
      67,   117,   126,    68,    69,    70,    71,    72,    73,    74,
      75,    76,   109,   110
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      82,    91,    14,    86,    83,    83,   102,    84,    85,    11,
      23,    24,   118,   119,   121,    40,    13,    10,    21,    18,
      36,   101,    10,   103,   108,    95,    49,    22,   112,   105,
      25,    30,   111,    41,    83,   128,    50,   122,    83,    28,
      98,    99,   135,    83,    83,    83,    83,    19,    20,   116,
     141,    33,    44,   132,    96,   -16,    87,   -14,   106,   130,
     -12,    37,   -10,   -18,   129,     9,   134,    16,   136,    83,
       9,    88,   139,   104,    34,    89,    93,    94,   144,    35,
      29,   140,    51,    52,    13,     1,     2,     3,    53,    45,
      54,    55,    56,    57,    38,    58,    92,    39,    93,    94,
      59,   -26,    60,    48,    39,    90,     1,     2,     3,    61,
      51,    52,    13,     1,     2,     3,    53,    77,    54,    55,
      56,    57,    78,    58,    26,    51,    52,    13,    59,    79,
      60,    80,    39,    51,    52,    13,    81,    61,    58,   131,
      97,   133,   100,    59,    16,    60,    58,   107,   120,   138,
      99,    59,    61,    60,   123,   142,     1,     2,     3,   124,
     125,   127,   137,   143,    12,   113,   115,   114
};

static const yytype_uint8 yycheck[] =
{
      57,    66,    10,    60,    58,    59,    79,    58,    59,     0,
      18,    19,   102,   103,     1,    32,     5,     0,     3,     1,
      28,    78,     5,    80,    88,     1,    43,    23,    92,     1,
      28,     1,    89,     1,    88,     1,    44,    24,    92,    22,
      20,    21,   132,    97,    98,    99,   100,    29,    30,   100,
     140,     1,    35,   126,    30,    25,    64,    25,    30,   123,
      30,     3,    30,    23,    30,     0,   131,    27,   133,   123,
       5,    23,   137,    81,    24,    27,    17,    18,   143,    29,
      27,   138,     3,     4,     5,     6,     7,     8,     9,    28,
      11,    12,    13,    14,    30,    16,    15,    25,    17,    18,
      21,    26,    23,    30,    25,    26,     6,     7,     8,    30,
       3,     4,     5,     6,     7,     8,     9,    26,    11,    12,
      13,    14,    23,    16,    24,     3,     4,     5,    21,    23,
      23,    23,    25,     3,     4,     5,    23,    30,    16,   125,
      19,   127,    22,    21,    27,    23,    16,    24,    24,   135,
      21,    21,    30,    23,    29,   141,     6,     7,     8,    28,
      24,    24,    10,    24,     5,    97,    99,    98
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     6,     7,     8,    34,    35,    36,    37,    40,    48,
      49,     0,    36,     5,    50,    51,    27,    43,     1,    29,
      30,     3,    23,    50,    50,    28,    24,    44,    49,    27,
       1,    39,    42,     1,    24,    29,    50,     3,    30,    25,
      45,     1,    38,    41,    49,    28,    46,    47,    30,    45,
      50,     3,     4,     9,    11,    12,    13,    14,    16,    21,
      23,    30,    45,    48,    49,    50,    52,    53,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    26,    23,    23,
      23,    23,    58,    57,    63,    63,    58,    50,    23,    27,
      26,    53,    15,    17,    18,     1,    30,    19,    20,    21,
      22,    58,    56,    58,    50,     1,    30,    24,    59,    65,
      66,    58,    59,    60,    61,    62,    63,    54,    54,    54,
      24,     1,    24,    29,    28,    24,    55,    24,     1,    30,
      59,    55,    56,    55,    53,    54,    53,    10,    55,    53,
      58,    54,    55,    24,    53
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    33,    34,    35,    35,    36,    36,    36,    37,    37,
      38,    37,    39,    37,    41,    40,    42,    40,    43,    44,
      44,    44,    44,    44,    46,    45,    47,    45,    48,    49,
      49,    49,    50,    51,    51,    51,    51,    51,    52,    52,
      53,    53,    53,    53,    53,    53,    53,    53,    53,    53,
      53,    54,    55,    56,    56,    56,    57,    57,    58,    58,
      59,    59,    60,    60,    61,    61,    62,    62,    63,    63,
      63,    64,    64,    64,    64,    64,    64,    64,    64,    65,
      65,    66,    66
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     7,     6,
       0,     8,     0,     7,     0,     8,     0,     7,     0,     4,
       3,     2,     1,     2,     0,     4,     0,     3,     3,     1,
       1,     1,     1,     3,     6,     1,     3,     4,     1,     2,
       1,     1,     1,    13,     7,     9,     7,     5,     5,     3,
       3,     0,     0,     1,     2,     2,     1,     4,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     2,     2,
       1,     1,     4,     3,     1,     1,     2,     2,     4,     1,
       0,     3,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 266 "1705013.y"
               {
	    yyval = new SymbolInfo(yyvsp[0]->getName(), "NON_TERMINAL");
            log << "Line " <<line_count << ": start : program" << "\n"  << endl; 
            log << yyvsp[0]->getName() << "\n"  << endl;
            if(error_count == 0) {
                final_code = "";
                final_code += (string)".model small\n.stack 100h\n.data\n\n";

                for(int i=0; i<declaration_list.size(); i++) {
                    final_code += (string)"\t"+(string)declaration_list[i]+(string)"\n";
                }

                declaration_list.clear();
                final_code += (string)"\n\taddress dw 0\n";
                final_code += (string)"\n.code\n\n";
                final_code += yyvsp[0]->getCode();
                
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
                yyval->setCode(final_code);
                asmFile << yyval->getCode() << endl;
                optimization(yyval->getCode());

            } else {
               
            }    
	}
#line 1784 "y.tab.c"
    break;

  case 3:
#line 336 "1705013.y"
                      {   
            yyval = new SymbolInfo(yyvsp[-1]->getName()+yyvsp[0]->getName(), "program_unit");
            log << "Line " << line_count << ": program : program unit" << "\n"  << endl;
            log << yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl;
            yyval->setCode(yyvsp[-1]->getCode()+yyvsp[0]->getCode());
    }
#line 1795 "y.tab.c"
    break;

  case 4:
#line 342 "1705013.y"
                   {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "unit");
            log << "Line " << line_count << ": program : unit" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 1806 "y.tab.c"
    break;

  case 5:
#line 350 "1705013.y"
                      {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "var_declaration");
            log << "Line " << line_count << ": unit : var_declaration" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
    }
#line 1816 "y.tab.c"
    break;

  case 6:
#line 355 "1705013.y"
                           {
            if(func_error_recovery==0){
            	yyval = new SymbolInfo(yyvsp[0]->getName(), "func_declaration");
            	log << "Line " << line_count << ": unit : func_declaration" << "\n"  << endl;
            	log << yyvsp[0]->getName() << "\n"  << endl;
            }
            else
            	func_error_recovery = 0;
    }
#line 1830 "y.tab.c"
    break;

  case 7:
#line 364 "1705013.y"
                          {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "func_definition");
            log << "Line " << line_count << ": unit : func_definition" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 1841 "y.tab.c"
    break;

  case 8:
#line 372 "1705013.y"
                                                                             {
	    func_error_recovery = 1;
	    yyval = new SymbolInfo("", "function_declaration_with_parameterlist");
	    error << "Error at line no: " << error_line << " " <<":Semicolon missing"<<endl;
	    log << "Error at line no: " << error_line << " " <<":Syntax Error"<<endl;
    	    log << "Error at line no: " << error_line << " " <<":Semicolon missing"<<endl;
	    yyerrok;
    }
#line 1854 "y.tab.c"
    break;

  case 9:
#line 380 "1705013.y"
                                                      {
            func_error_recovery = 1;
	    yyval = new SymbolInfo("", "function_declaration_without_parameterlist");
            error << "Error at line no: " << error_line << " " <<":Semicolon missing"<<endl;
            log << "Error at line no: " << error_line << " " <<":Syntax Error"<<endl;
    	    log << "Error at line no: " << error_line << " " <<":Semicolon missing"<<endl;
            yyerrok;
    }
#line 1867 "y.tab.c"
    break;

  case 10:
#line 389 "1705013.y"
                                                                       {multiple_Dec_check();}
#line 1873 "y.tab.c"
    break;

  case 11:
#line 389 "1705013.y"
                                                                                                         {
            yyval = new SymbolInfo(yyvsp[-7]->getName()+" "+yyvsp[-6]->getName()+"("+yyvsp[-3]->getName()+")"+";"+"\n", "function_declaration_with_parameterlist");
            log << "Line " << line_count << ": func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON" << "\n"  << endl;
            log << yyvsp[-7]->getName()+" "+yyvsp[-6]->getName()+"("+yyvsp[-3]->getName()+")"+";" << "\n"  << endl;
            param_list.clear();
    }
#line 1884 "y.tab.c"
    break;

  case 12:
#line 395 "1705013.y"
                                                {multiple_Dec_check();}
#line 1890 "y.tab.c"
    break;

  case 13:
#line 395 "1705013.y"
                                                                                  {
            yyval = new SymbolInfo(yyvsp[-6]->getName()+" "+yyvsp[-5]->getName()+"();"+"\n", "function_declaration_without_parameterlist");
            log << "Line" << line_count << ": func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON" << "\n"  << endl;
            log << yyvsp[-6]->getName()+" "+yyvsp[-5]->getName()+"();" << "\n"  << endl;
            param_list.clear();
    }
#line 1901 "y.tab.c"
    break;

  case 14:
#line 402 "1705013.y"
                                                                      {inconsistant_def_check();}
#line 1907 "y.tab.c"
    break;

  case 15:
#line 402 "1705013.y"
                                                                                                                     {
            yyval = new SymbolInfo(yyvsp[-7]->getName()+" "+yyvsp[-6]->getName()+"("+yyvsp[-3]->getName()+")"+yyvsp[0]->getName()+"\n", "function_definition_with_parameterlist");
            log << "Line " << line_count << ": func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement" << "\n"  << endl;
            log << yyvsp[-7]->getName()+" "+yyvsp[-6]->getName()+"("+yyvsp[-3]->getName()+")"+yyvsp[0]->getName() << "\n"  << endl;
            string temp="";
            if((yyvsp[-6]->getName() == "main")) {
            	cout<<line_count<<" "<<endl;
                temp += (string)"main proc\n\tmov ax, @data\n\tmov ds ,ax\n\n";
                temp += yyvsp[0]->getCode();
                temp += (string)"\n\n\tmov ah, 4ch\n\tint 21h\nmain endp\n\n";

            }
            else {
                
                temp += yyvsp[-6]->getName()+(string)" proc\n\tpop address\n";

                for(int i=(pm_list.size()-1); i>=0; i--) {
                    temp += (string)"\tpop "+pm_list[i]+(string)"\n";
                }

                temp += yyvsp[0]->getCode();

                temp += (string)"\tpush address\n\tret\n";
                temp += yyvsp[-6]->getName()+(string)" endp\n\n";
            }
            pm_list.clear(); 
            yyval->setCode(temp);
    }
#line 1940 "y.tab.c"
    break;

  case 16:
#line 430 "1705013.y"
                                                {inconsistant_def_check();}
#line 1946 "y.tab.c"
    break;

  case 17:
#line 430 "1705013.y"
                                                                                               {
            yyval = new SymbolInfo(yyvsp[-6]->getName()+" "+yyvsp[-5]->getName()+"()"+yyvsp[0]->getName()+"\n", "function_definition_without_parameterlist");
            log << "Line " << line_count << ": func_definition : type_specifier ID LPAREN RPAREN compound_statement" << "\n"  << endl;
            log << yyvsp[-6]->getName()+" "+yyvsp[-5]->getName()+"()"+yyvsp[0]->getName() << "\n"  << endl;
            string temp="";
            if((yyvsp[-5]->getName() == "main")) {
            	cout<<line_count<<endl;
                temp += (string)"main proc\n\tmov ax, @data\n\tmov ds ,ax\n\n";
                temp += yyvsp[0]->getCode();
                temp += (string)"\n\n\tmov ah, 4ch\n\tint 21h\nmain endp\n\n";

            }
            else {
                
                temp += yyvsp[-5]->getName()+(string)" proc\n\tpop address\n";

                for(int i=(pm_list.size()-1); i>=0; i--) {
                    temp += (string)"\tpop "+pm_list[i]+(string)"\n";
                }

                temp += yyvsp[0]->getCode();

                temp += (string)"\tpush address\n\tret\n";
                temp += yyvsp[-5]->getName()+(string)" endp\n\n";
            }

            yyval->setCode(temp);
            pm_list.clear();  
            yyval->setCode(temp);
    }
#line 1981 "y.tab.c"
    break;

  case 18:
#line 462 "1705013.y"
       {
	    error_line = line_count;
            type_final = type;
            name_final = name;
    }
#line 1991 "y.tab.c"
    break;

  case 19:
#line 468 "1705013.y"
                                                       {
            yyval = new SymbolInfo(yyvsp[-3]->getName()+","+yyvsp[-1]->getName()+" "+(string)yyvsp[0]->getName(), "parameterlist_comma_type_id");
            for(int i=0;i<param_list.size();i++)
            {
            	if(param_list[i].param_name==yyvsp[0]->getName()){
            		errorprint("Multiple declaration of "+yyvsp[0]->getName()+" in parameter\n");
            		break;
            	}
            }
            log << "Line " << line_count << ": parameter_list : parameter_list COMMA type_specifier ID" << "\n"  << endl;
            log << yyvsp[-3]->getName()+", "+yyvsp[-1]->getName()+" "+yyvsp[0]->getName() << "\n"  << endl;
            parameter temp_parameter;
            temp_parameter.param_type = (string)yyvsp[-1]->getName();
            temp_parameter.param_name = (string)yyvsp[0]->getName();
            param_list.push_back(temp_parameter);
    }
#line 2012 "y.tab.c"
    break;

  case 20:
#line 484 "1705013.y"
                                              {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+","+yyvsp[0]->getName(), "parameterlist_comma_type");
            log << "Line " << line_count << ": parameter_list : parameter_list COMMA type_specifier" << "\n"  << endl;
            log << yyvsp[-2]->getName()+", "+yyvsp[0]->getName() << "\n"  << endl;
            parameter temp_parameter;
            temp_parameter.param_type = yyvsp[0]->getName();
            temp_parameter.param_name = "";

            param_list.push_back(temp_parameter);
    }
#line 2027 "y.tab.c"
    break;

  case 21:
#line 494 "1705013.y"
                            {
            yyval = new SymbolInfo(yyvsp[-1]->getName()+" "+yyvsp[0]->getName(), "type_id");
            log << "Line no: " << line_count << ": parameter_list : type_specifier ID" << "\n"  << endl;
            log << yyvsp[-1]->getName()+" "+yyvsp[0]->getName() << "\n"  << endl;
            parameter temp_parameter;
            temp_parameter.param_type = yyvsp[-1]->getName();
            temp_parameter.param_name = yyvsp[0]->getName();

            param_list.push_back(temp_parameter);
    }
#line 2042 "y.tab.c"
    break;

  case 22:
#line 504 "1705013.y"
                         {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "type");
            log << "Line " << line_count << ": parameter_list : type_specifier" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
            parameter temp_parameter;
            temp_parameter.param_type = yyvsp[0]->getName();
            temp_parameter.param_name = "";

            param_list.push_back(temp_parameter);
    }
#line 2057 "y.tab.c"
    break;

  case 23:
#line 514 "1705013.y"
                               {
	    yyval = new SymbolInfo(yyvsp[-1]->getName()+" ","param_error");
    	    log << "Error at line no "<<line_count<<": Syntax error"<<endl<<endl;
    	    log<< type<<endl<<endl;
    	    log <<"Error at line no "<<line_count<<": parameter's name not given in function definition"<< endl<<endl;
    	    error<<"Error at line no "<<line_count<<": parameter's name not given in function definition"<< endl<<endl;            
    }
#line 2069 "y.tab.c"
    break;

  case 24:
#line 523 "1705013.y"
                          {symbolTable.enterScope(scope_count++, 30, log);insertParam();}
#line 2075 "y.tab.c"
    break;

  case 25:
#line 523 "1705013.y"
                                                                                                           {
            yyval = new SymbolInfo("{ \n"+yyvsp[-1]->getName()+"}"+"\n", "compound_statement_with_statements");  
            log << "Line " << line_count << ": compound_statement : LCURL statements RCURL" << "\n"  << endl;
            log << "{ \n"+yyvsp[-1]->getName()+"}" << "\n"  << endl;
            symbolTable.printAllScope(log);
            symbolTable.exitScope(log);
            yyval->setCode(yyvsp[-1]->getCode());
    }
#line 2088 "y.tab.c"
    break;

  case 26:
#line 531 "1705013.y"
                {symbolTable.enterScope(scope_count++, 30, log);insertParam();}
#line 2094 "y.tab.c"
    break;

  case 27:
#line 531 "1705013.y"
                                                                                      {
            yyval = new SymbolInfo("{ \n\n}\n", "compound_statement_without_statements");  
            log << "Line " << line_count << ": compound_statement : LCURL RCURL" << "\n"  << endl;
            log << "{ }" << "\n"  << endl;
            symbolTable.printAllScope(log);
            symbolTable.exitScope(log);
    }
#line 2106 "y.tab.c"
    break;

  case 28:
#line 539 "1705013.y"
                                                           {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+" "+yyvsp[-1]->getName()+";"+"\n", "type_declarationlist_semi");
            log << "Line " << line_count << ": var_declaration : type_specifier declaration_list SEMICOLON" << "\n"  << endl;
            log << yyvsp[-2]->getName()+" "+yyvsp[-1]->getName()+";" << "\n"  << endl;
            if(yyvsp[-2]->getName() == "void") {
		 errorprint("variable type can not be void\n");
            } else {
                for(int i=0; i<var_list.size(); i++) {
                    SymbolInfo* symbolInfo = new SymbolInfo(var_list[i].var_name, "ID");
    		     symbolInfo->set_Type((string)yyvsp[-2]->getName());  
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
#line 2144 "y.tab.c"
    break;

  case 29:
#line 574 "1705013.y"
                    {
            yyval = new SymbolInfo("int", "INT");
            log << "Line no: " << line_count << ": type_specifier : INT" << "\n"  << endl;
            log << "int" << "\n"  << endl;

            type = "int";
    }
#line 2156 "y.tab.c"
    break;

  case 30:
#line 581 "1705013.y"
                        {
            yyval = new SymbolInfo("float", "FLOAT");
            log << "Line " << line_count << ": type_specifier : FLOAT" << "\n"  << endl;
            log << "float" << "\n"  << endl;

            type = "float";
    }
#line 2168 "y.tab.c"
    break;

  case 31:
#line 588 "1705013.y"
                       {
            yyval = new SymbolInfo("void", "VOID");
            log << "Line " << line_count << ": type_specifier : VOID" << "\n"  << endl;
            log << "void" << "\n"  << endl;

            type = "void";
    }
#line 2180 "y.tab.c"
    break;

  case 32:
#line 597 "1705013.y"
       {
            yyval = new SymbolInfo((string)yyvsp[0]->getName(), "ID");
            name = yyvsp[0]->getName();
    }
#line 2189 "y.tab.c"
    break;

  case 33:
#line 603 "1705013.y"
                                            {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+","+yyvsp[0]->getName(), "declarationlist_comma_id");
            log << "Line " << line_count << ": declaration_list : declaration_list COMMA ID" << "\n"  << endl;
            log << yyvsp[-2]->getName()+","+yyvsp[0]->getName() << "\n"  << endl;
            dec_check = 1;
            Declaration_list_check(yyvsp[0]->getName(),"");
    }
#line 2201 "y.tab.c"
    break;

  case 34:
#line 610 "1705013.y"
                                                                    {
            yyval = new SymbolInfo(yyvsp[-5]->getName()+","+yyvsp[-3]->getName()+"["+yyvsp[-1]->getName()+"]", "declarationlist_comma_id[INT]");
            log << "Line " << line_count << ": declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD" << "\n"  << endl;
            log << yyvsp[-5]->getName()+" , "+yyvsp[-3]->getName()+"["+yyvsp[-1]->getName()+"]" << "\n"  << endl;
            dec_check = 2;
            Declaration_list_check(yyvsp[-3]->getName(),yyvsp[-1]->getName());
    }
#line 2213 "y.tab.c"
    break;

  case 35:
#line 617 "1705013.y"
                     {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "ID");
            log << "Line " << line_count << ": declaration_list : ID" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
            dec_check = 1;
            Declaration_list_check((string)yyvsp[0]->getName(),"");
    }
#line 2225 "y.tab.c"
    break;

  case 36:
#line 624 "1705013.y"
                                           {
            yyval = new SymbolInfo((string)yyvsp[-2]->getName(), "ID");
            log <<"Error at line no: "<<line_count<<" Syntax error\n"<<endl; 
            log<<yyvsp[-2]->getName()<<endl<<endl;
            dec_check = 1;
            Declaration_list_check((string)yyvsp[-2]->getName(),"");   	     
    }
#line 2237 "y.tab.c"
    break;

  case 37:
#line 631 "1705013.y"
                                             {
            yyval = new SymbolInfo(yyvsp[-3]->getName()+"["+yyvsp[-1]->getName()+"]", "id[INT]");
            log << "Line " << line_count << ": declaration_list : ID LTHIRD CONST_INT RTHIRD" << "\n"  << endl;
            log << yyvsp[-3]->getName()+"["+yyvsp[-1]->getName()+"]" << "\n"  << endl;
	    dec_check = 2;
            Declaration_list_check(yyvsp[-3]->getName(),yyvsp[-1]->getName());
            array_list.push_back(yyvsp[-3]->getName());
    }
#line 2250 "y.tab.c"
    break;

  case 38:
#line 641 "1705013.y"
                      {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "statement");
            log << "Line " << line_count << ": statements : statement" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2261 "y.tab.c"
    break;

  case 39:
#line 647 "1705013.y"
                                   {
            yyval = new SymbolInfo(yyvsp[-1]->getName()+yyvsp[0]->getName(), "statements_statements");
            log << "Line " << line_count << ": statements : statements statement" << "\n"  << endl;
            log << yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl;
            yyval->setCode(yyvsp[-1]->getCode()+yyvsp[0]->getCode());
    }
#line 2272 "y.tab.c"
    break;

  case 40:
#line 655 "1705013.y"
                           {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "var_declaration");
            log << "Line " << line_count << ": statement : var_declaration" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
    }
#line 2282 "y.tab.c"
    break;

  case 41:
#line 660 "1705013.y"
                               {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "expression_statement");
            log << "Line " << line_count << ": statement : expression_statement" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2293 "y.tab.c"
    break;

  case 42:
#line 666 "1705013.y"
                             {
            yyval = new SymbolInfo(yyvsp[0]->getName()+"\n", "compound_statement");
            log << "Line " << line_count << ": statement : compound_statement" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2304 "y.tab.c"
    break;

  case 43:
#line 672 "1705013.y"
                                                                                                                                                          {
            yyval = new SymbolInfo("for("+yyvsp[-10]->getName()+yyvsp[-7]->getName()+yyvsp[-4]->getName()+")"+yyvsp[0]->getName()+"\n", "for");
            log << "Line " << line_count << ": statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement" << "\n"  << endl;
            log << "for("+yyvsp[-10]->getName()+yyvsp[-7]->getName()+yyvsp[-4]->getName()+")"+yyvsp[0]->getName() << "\n"  << endl;
            
            if((yyvsp[-10]->getcode_symbol() != ";") && (yyvsp[-7]->getcode_symbol() != ";")) {
                string label1 = newLabel();
                string label2 = newLabel();
		            	cout<<line_count<<endl;
                yyval->setCode(yyvsp[-10]->getCode()+"\t;Line "+to_string(line_count)+" for loop\n");
                yyval->setCode(yyval->getCode()+(string)"\t"+label1+(string)":\n"+yyvsp[-7]->getCode()+(string)"\tmov ax, "+yyvsp[-7]->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label2+(string)"\n");
                yyval->setCode(yyval->getCode()+yyvsp[0]->getCode()+yyvsp[-4]->getCode()+(string)"\tjmp "+label1+(string)"\n\t"+label2+(string)":\n");
            }

    }
#line 2324 "y.tab.c"
    break;

  case 44:
#line 687 "1705013.y"
                                                                                           {
            yyval = new SymbolInfo("if("+yyvsp[-4]->getName()+")"+yyvsp[0]->getName()+"\n", "if_prec");
            log << "Line " << line_count << ": statement : IF LPAREN expression RPAREN statement" << "\n"  << endl;
            log << "if("+yyvsp[-4]->getName()+")"+yyvsp[0]->getName() << "\n"  << endl;
            string label = newLabel();
		            	cout<<line_count<<endl;
            yyval->setCode(yyvsp[-4]->getCode()+"\t;Line "+to_string(line_count)+" "+yyvsp[-6]->getName()+" "+"("+yyvsp[-4]->getName()+")\n"+(string)"\tmov ax, "+yyvsp[-4]->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label+(string)"\n"+yyvsp[0]->getCode()+(string)"\t"+label+(string)":\n");
    }
#line 2337 "y.tab.c"
    break;

  case 45:
#line 695 "1705013.y"
                                                                                    {
            yyval = new SymbolInfo("if("+yyvsp[-6]->getName()+")"+yyvsp[-2]->getName()+" else"+yyvsp[0]->getName()+"\n", "if");
            log << "Line " << line_count << ": statement : IF LPAREN expression RPAREN statement ELSE statement" << "\n"  << endl;
            log << "if("+yyvsp[-6]->getName()+")"+yyvsp[-2]->getName()+" else"+yyvsp[0]->getName() << "\n"  << endl;
            string label1 = newLabel();
            string label2 = newLabel();
		            	cout<<line_count<<endl;
            yyval->setCode(yyvsp[-6]->getCode()+"\t;Line "+to_string(line_count)+" "+yyvsp[-8]->getName()+" "+"("+yyvsp[-6]->getName()+")"+"\n"+(string)"\tmov ax, "+yyvsp[-6]->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label1+(string)"\n"+yyvsp[-2]->getCode()+(string)"\tjmp "+label2+(string)"\n");
            yyval->setCode(yyval->getCode()+(string)"\t"+label1+(string)":\n"+yyvsp[0]->getCode()+(string)"\t"+label2+(string)":\n");

    }
#line 2353 "y.tab.c"
    break;

  case 46:
#line 706 "1705013.y"
                                                                        {
            yyval = new SymbolInfo("while("+yyvsp[-4]->getName()+")"+yyvsp[0]->getName()+"\n", "while");
            log << "Line no: " << line_count << ": statement : WHILE LPAREN expression RPAREN statement" << "\n"  << endl;
            log << "while("+yyvsp[-4]->getName()+")"+yyvsp[0]->getName() << "\n"  << endl;
            string label1 = newLabel();
            string label2 = newLabel();
		            	cout<<line_count<<endl;
            yyval->setCode("\t;Line "+to_string(line_count)+" while("+yyvsp[-4]->getName()+")"+"\n"+(string)"\t"+label1+(string)":\n"+yyvsp[-4]->getCode()+(string)"\tmov ax, "+yyvsp[-4]->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label2+(string)"\n");
            yyval->setCode(yyval->getCode()+yyvsp[0]->getCode()+(string)"\tjmp "+label1+(string)"\n\t"+label2+(string)":\n");
    }
#line 2368 "y.tab.c"
    break;

  case 47:
#line 716 "1705013.y"
                                             {
            SymbolInfo* temp = symbolTable.lookUpAllScope(yyvsp[-2]->getName());
	     string var;
            if(temp==NULL){
            	errorprint("Undeclared variable "+yyvsp[-2]->getName()+"\n");
            	var = "";
            }
            else{
            	if(temp->get_Type() != "void") {
                    var = temp->getcode_symbol();
                } else {
                    var = "";
                }
            }
            yyval = new SymbolInfo("printf("+yyvsp[-2]->getName()+")"+";"+"\n", "println");
            log << "Line " << line_count << ": statement : PRINTLN LPAREN ID RPAREN SEMICOLON" << "\n"  << endl;
            log << "printf("+yyvsp[-2]->getName()+")"+";" << "\n"  << endl;
                        	cout<<line_count<<endl;
            yyval->setCode("\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+var+"\n\tcall println\n");
    }
#line 2393 "y.tab.c"
    break;

  case 48:
#line 736 "1705013.y"
                                         {
            yyval = new SymbolInfo("printf("+yyvsp[-2]->getName()+")\n", "println");
            log << "Error at line no " << line_count << ": Syntax error" << "\n"  << endl;
            log << "printf("+yyvsp[-2]->getName()+")" << "\n"  << endl;
            log << "Error at line no " << line_count << ": Semicolon missing after println statement" << "\n"  << endl;
            error << "Error at line no " << line_count << ": Semicolon missing after println statement" << "\n"  << endl;            
    }
#line 2405 "y.tab.c"
    break;

  case 49:
#line 743 "1705013.y"
                                      {
            yyval = new SymbolInfo("return "+yyvsp[-1]->getName()+";"+"\n", "return");
            log << "Line " << line_count << ": statement : RETURN expression SEMICOLON" << "\n"  << endl;
            log << "return "+yyvsp[-1]->getName()+";" << "\n"  << endl;
            yyval->setCode(yyvsp[-1]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tpush "+yyvsp[-1]->getcode_symbol()+(string)"\n");
            if(yyvsp[-1]->get_Type() == "void") {
                errorprint("void function used in expression\n");

            } 
    }
#line 2420 "y.tab.c"
    break;

  case 50:
#line 753 "1705013.y"
                                  {
    	    yyval = new SymbolInfo("return "+yyvsp[-1]->getName()+"\n", "return");
            log << "Error at line no" << line_count << ": Syntax error" << "\n"  << endl;
            log << "return "+yyvsp[-1]->getName() << "\n"  << endl;
            log<<"Error at line no "<<line_count<<": Semicolon missing after return statement"<<endl<<endl;
            error<<"Error at line no "<<line_count<<": Semicolon missing after return statement"<<endl<<endl;
            if(yyvsp[-1]->get_Type() == "void") {
                errorprint("void function used in expression\n");

            } 
    }
#line 2436 "y.tab.c"
    break;

  case 51:
#line 766 "1705013.y"
           {
            type_final = type;
    }
#line 2444 "y.tab.c"
    break;

  case 52:
#line 771 "1705013.y"
            {
            
            if(type_final == "void") {
                errorprint("void function used in expression\n");
            } 
    }
#line 2455 "y.tab.c"
    break;

  case 53:
#line 779 "1705013.y"
                                {
            yyval = new SymbolInfo((string)";"+"\n", "semicolon");
            log << "Line " << line_count << ": expression_statement : SEMICOLON" << "\n"  << endl;
            log << ";" << "\n"  << endl;
            yyval->set_Type("int");
            type = "int";
    }
#line 2467 "y.tab.c"
    break;

  case 54:
#line 786 "1705013.y"
                               {
            yyval = new SymbolInfo(yyvsp[-1]->getName()+";"+"\n", "expression_semi");
            log << "Line " << line_count << ": expression_statement : expression SEMICOLON" << "\n"  << endl;
            log << yyvsp[-1]->getName()+";" << "\n"  << endl;
           yyval->set_Type(yyvsp[-1]->get_Type());
           type = yyvsp[-1]->get_Type();
           yyval->setcode_symbol(yyvsp[-1]->getcode_symbol());
           yyval->setCode(yyvsp[-1]->getCode());
    }
#line 2481 "y.tab.c"
    break;

  case 55:
#line 795 "1705013.y"
                           {
            yyval = new SymbolInfo(yyvsp[-1]->getName()+"\n", "expression_semi");
            log << "Error at line no "<<line_count<<": Syntax error\n"<<endl;
            log << yyvsp[-1]->getName()<<endl<<endl;
            log <<"Error at line no "<<line_count<<": Semicolon missing after expression"<<endl;
            error <<"Error at line no "<<line_count<<": Semicolon missing after expression"<<endl;
    }
#line 2493 "y.tab.c"
    break;

  case 56:
#line 804 "1705013.y"
             {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "id");
            log << "Line " << line_count << " variable: ID" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;           
            SymbolInfo* temp = symbolTable.lookUpAllScope(yyvsp[0]->getName());

            if(temp == NULL) {
                errorprint("undeclared variable "+yyvsp[0]->getName()+"\n");
		 yyval->set_arrSize(-1);
                yyval->set_Type("float");  
            } else {
            	 yyval->set_arrSize(-1);
                if(temp->get_Type() != "void") {
                    yyval->set_Type(temp->get_Type());
                    yyval->setcode_symbol(temp->getcode_symbol());
                } else {
                    yyval->set_Type("float");  
                }
            }
            if((temp!=NULL) && (temp->get_arrSize()!=-1)) {
                errorprint("type mismatch, "+yyvsp[0]->getName()+" is not a variable\n");
                already_got = 1;
            }
            cv=yyvsp[0]->getName();
    }
#line 2523 "y.tab.c"
    break;

  case 57:
#line 829 "1705013.y"
                                      {
            yyval = new SymbolInfo(yyvsp[-3]->getName()+"["+yyvsp[-1]->getName()+"]", "id[expression]");
            log << "Line " << line_count << ": variable : ID LTHIRD expression RTHIRD" << "\n"  << endl;
            log << yyvsp[-3]->getName()+"["+yyvsp[-1]->getName()+"]" << "\n"  << endl;
            SymbolInfo* temp = symbolTable.lookUpAllScope(yyvsp[-3]->getName());

            if(temp == NULL) {
                errorprint("undeclared variable "+yyvsp[-3]->getName()+"\n");
		 yyval->set_arrSize(0);
                yyval->set_Type("float"); 
            } else {
                if(temp->get_Type() != "void") {
                    yyval->set_arrSize(temp->get_arrSize());
                    yyval->set_Type(temp->get_Type());
                    yyval->setcode_symbol(temp->getcode_symbol());
                } else {
                    yyval->set_Type("float"); 
                    yyval->set_arrSize(0);
                }
            }

            if((temp!=NULL) && (temp->get_arrSize()<=-1)) {
                errorprint("type mismatch, "+yyvsp[-3]->getName()+" is not a variable\n");
                already_got = 1;
            }

            if(yyvsp[-1]->get_Type() != "int") {
                errorprint("Expression inside third brackets not an integer\n");
            }            

            if(yyvsp[-1]->get_Type() == "void") {
                errorprint("void function used in expression\n");
            } 
             cv=yyvsp[-3]->getName();
    }
#line 2563 "y.tab.c"
    break;

  case 58:
#line 866 "1705013.y"
                             {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "logic_expression");
            log << "Line " << line_count << ": expression : logic_expression" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;  

            yyval->set_Type(yyvsp[0]->get_Type());
            type = yyvsp[0]->get_Type();
            yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2578 "y.tab.c"
    break;

  case 59:
#line 876 "1705013.y"
                                             {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+" = "+yyvsp[0]->getName(), "variable=logic_expression");
            log << "Line " << line_count << ": expression : variable ASSIGNOP logic_expression" << "\n"  << endl;
            log << yyvsp[-2]->getName()+" = "+yyvsp[0]->getName() << "\n"  << endl; 
            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                yyvsp[0]->set_Type("float"); 
            } 
	     
            else if(yyvsp[-2]->get_Type() != yyvsp[0]->get_Type() && already_got!=1) {
		if(symbolTable.lookUpAllScope(cv)!=NULL){
			if(undefined_func=="blank"){
				if(yyvsp[-2]->get_Type()=="float" && yyvsp[0]->get_Type()=="int"){}
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
            yyval->set_Type(yyvsp[-2]->get_Type());
            type = yyvsp[-2]->get_Type();
            
            if(yyvsp[-2]->get_arrSize() > -1) {
                string temp = newTemp();
                declaration_list.push_back(temp+(string)" dw ?");
            	cout<<line_count<<endl;
                yyval->setCode(yyvsp[0]->getCode()+yyvsp[-2]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[0]->getcode_symbol()+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\tmov "+yyvsp[-2]->getcode_symbol()+(string)"[bx], ax\n\tmov "+temp+(string)", ax\n"); 
                yyval->setcode_symbol(temp);
            } else {
                        	cout<<line_count<<endl;
                yyval->setCode(yyvsp[-2]->getCode()+yyvsp[0]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[0]->getcode_symbol()+(string)"\n\tmov "+yyvsp[-2]->getcode_symbol()+(string)", ax\n");
                yyval->setcode_symbol(yyvsp[-2]->getcode_symbol());
            }

    }
#line 2624 "y.tab.c"
    break;

  case 60:
#line 919 "1705013.y"
                                 {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "rel_expression");
            log << "Line " << line_count << ": logic_expression : rel_expression" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl; 
            yyval->set_Type(yyvsp[0]->get_Type());
            yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2637 "y.tab.c"
    break;

  case 61:
#line 927 "1705013.y"
                                                {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName(), "rel_expression_logic_rel_expression");
            log << "Line " << line_count << ": logic_expression : rel_expression LOGICOP rel_expression" << "\n"  << endl;
            log << yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl; 
            if(yyvsp[-2]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            } 

            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            }
            yyval->set_Type("int");
            string label1 = newLabel();
            string label2 = newLabel();
            string temp = newTemp();
            declaration_list.push_back(temp+(string)" dw ?");

            yyval->setCode(yyvsp[-2]->getCode()+yyvsp[0]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n");
            
            if(yyvsp[-1]->getName() == "&&") {
                        	cout<<line_count<<endl;
                yyval->setCode(yyval->getCode()+(string)"\tmov ax, "+yyvsp[-2]->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label1+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\tmov ax, "+yyvsp[0]->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tje "+label1+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\tmov ax, 1\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n\t");
                yyval->setCode(yyval->getCode()+label1+(string)":\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\t");
                yyval->setCode(yyval->getCode()+label2+(string)":\n");
            } else {
                        	cout<<line_count<<endl;
                yyval->setCode(yyval->getCode()+(string)"\tmov ax, "+yyvsp[-2]->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tjne "+label1+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\tmov ax, "+yyvsp[0]->getcode_symbol()+(string)"\n\tcmp ax, 0\n\tjne "+label1+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n\t");
                yyval->setCode(yyval->getCode()+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t");
                yyval->setCode(yyval->getCode()+label2+(string)":\n");
            }
            
            yyval->setcode_symbol(temp);
    }
#line 2681 "y.tab.c"
    break;

  case 62:
#line 968 "1705013.y"
                                  {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "simple_expression");
            log << "Line " << line_count << ": rel_expression : simple_expression" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;  
            yyval->set_Type(yyvsp[0]->get_Type());
            yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2694 "y.tab.c"
    break;

  case 63:
#line 976 "1705013.y"
                                                                {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName(), "simple_expression_relop_simple_expression");
            log << "Line " << line_count << ": rel_expression : simple_expression RELOP simple_expression" << "\n"  << endl;
            log << yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl;
            if(yyvsp[-2]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            } 

            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            }

            yyval->set_Type("int");
            string label1 = newLabel();
            string label2 = newLabel();
            string temp = newTemp();
            declaration_list.push_back(temp+(string)" dw ?");
		            	cout<<line_count<<endl;
            yyval->setCode(yyvsp[-2]->getCode()+yyvsp[0]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-2]->getcode_symbol()+(string)"\n\tcmp ax, "+yyvsp[0]->getcode_symbol()+(string)"\n");

            if(yyvsp[-1]->getName() == "<") {
                yyval->setCode(yyval->getCode()+(string)"\tjl "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else if(yyvsp[-1]->getName() == "<=") {
                yyval->setCode(yyval->getCode()+(string)"\tjle "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else if(yyvsp[-1]->getName() == ">") {
                yyval->setCode(yyval->getCode()+(string)"\tjg "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else if(yyvsp[-1]->getName() == ">=") {
                yyval->setCode(yyval->getCode()+(string)"\tjge "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else if(yyvsp[-1]->getName() == "==") {
                yyval->setCode(yyval->getCode()+(string)"\tje "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            } else {
                yyval->setCode(yyval->getCode()+(string)"\tjne "+label1+(string)"\n\tmov ax, 0\n\tmov "+temp+(string)", ax\n\tjmp "+label2+(string)"\n");
                yyval->setCode(yyval->getCode()+(string)"\t"+label1+(string)":\n\tmov ax, 1\n\tmov "+temp+(string)", ax\n\t"+label2+(string)":\n");
            }

            yyval->setcode_symbol(temp);
    }
#line 2743 "y.tab.c"
    break;

  case 64:
#line 1021 "1705013.y"
                        {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "term");
            log << "Line " << line_count << ": simple_expression : term" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;  
            yyval->set_Type(yyvsp[0]->get_Type());
            yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2756 "y.tab.c"
    break;

  case 65:
#line 1029 "1705013.y"
                                       {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName(), "simple_expression+term");
            log << "Line " << line_count << ": simple_expression : simple_expression ADDOP term" << "\n"  << endl;
            log << yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl; 
            if(yyvsp[-2]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                yyvsp[-2]->set_Type("float"); 
            } 

            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                yyvsp[0]->set_Type("float"); 
            }

            if(yyvsp[-2]->get_Type()=="float" || yyvsp[0]->get_Type()=="float") {
                yyval->set_Type("float");
            } else {
                yyval->set_Type(yyvsp[-2]->get_Type()); 
            }
            string temp = newTemp();
            declaration_list.push_back(temp+(string)" dw ?");

            if(yyvsp[-1]->getName() == "+") {
                        	cout<<line_count<<endl;
                yyval->setCode(yyvsp[-2]->getCode()+yyvsp[0]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-2]->getcode_symbol()+(string)"\n\tadd ax, "+yyvsp[0]->getcode_symbol()+(string)"\n\tmov "+temp+(string)", ax\n");
                yyval->setcode_symbol(temp);

            } else {
                        	cout<<line_count<<endl;
                yyval->setCode(yyvsp[-2]->getCode()+yyvsp[0]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-2]->getcode_symbol()+(string)"\n\tsub ax, "+yyvsp[0]->getcode_symbol()+(string)"\n\tmov "+temp+(string)", ax\n");
                yyval->setcode_symbol(temp);
            }
    }
#line 2796 "y.tab.c"
    break;

  case 66:
#line 1066 "1705013.y"
                       {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "unary_expression");
            log << "Line " << line_count << ": term : unary_expression" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;  
            yyval->set_Type(yyvsp[0]->get_Type());
            yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2809 "y.tab.c"
    break;

  case 67:
#line 1074 "1705013.y"
                                       {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName(), "term_unary_expression");
            log << "Line " << line_count << ": term : term MULOP unary_expression" << "\n"  << endl;
            log << yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl; 
            if(yyvsp[-2]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                yyvsp[-2]->set_Type("float"); 
            } 

            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                yyvsp[0]->set_Type("float");  
            } 
            if((yyvsp[-1]->getName() == "%") && yyvsp[0]->getName()=="0"){
            	errorprint("Modulus by zero\n");
            	log << yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl;
            	yyval->set_Type(yyvsp[-2]->get_Type());
            }
            if((yyvsp[-1]->getName() == "/") && yyvsp[0]->getName()=="0"){
            	errorprint("divided by zero\n");
            	log << yyvsp[-2]->getName()+yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl;
            	yyval->set_Type(yyvsp[-2]->get_Type());
            }
            if((yyvsp[-1]->getName() == "%") && (yyvsp[-2]->get_Type() != "int" || yyvsp[0]->get_Type() != "int")) {
		 errorprint("Integer operand on modulus operator\n");
                yyval->set_Type("int"); 
            } else if((yyvsp[-1]->getName() != "%") && (yyvsp[-2]->get_Type() == "float" || yyvsp[0]->get_Type() == "float")) {
                yyval->set_Type("float"); 
            }
            else {
                yyval->set_Type(yyvsp[-2]->get_Type());
            }
            string temp = newTemp();
            declaration_list.push_back(temp+(string)" dw ?");

            if(yyvsp[-1]->getName() == "*") {
                yyval->setCode(yyvsp[-2]->getCode()+yyvsp[0]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-2]->getcode_symbol()+(string)"\n\tmov bx, "+yyvsp[0]->getcode_symbol()+(string)"\n\timul bx\n\tmov "+temp+(string)", ax\n");
                yyval->setcode_symbol(temp);

            } else {
                yyval->setCode(yyvsp[-2]->getCode()+yyvsp[0]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-2]->getcode_symbol()+(string)"\n\tcwd\n");
                yyval->setCode(yyval->getCode()+(string)"\tmov bx, "+yyvsp[0]->getcode_symbol()+(string)"\n\tidiv bx\n");
                
                if(yyvsp[-1]->getName() == "/") {
                    yyval->setCode(yyval->getCode()+(string)"\tmov "+temp+(string)", ax\n");
                } else {
                    yyval->setCode(yyval->getCode()+(string)"\tmov "+temp+(string)", dx\n");
                }
                
                yyval->setcode_symbol(temp);
            }
    }
#line 2868 "y.tab.c"
    break;

  case 68:
#line 1130 "1705013.y"
                                         {
            yyval = new SymbolInfo(yyvsp[-1]->getName()+yyvsp[0]->getName(), "+unary_expresssion");
            log << "Line " << line_count << ": unary_expression : ADDOP unary_expression" << "\n"  << endl;
            log << yyvsp[-1]->getName()+yyvsp[0]->getName() << "\n"  << endl; 
            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
                yyval->set_Type("float");
            } else {
                yyval->set_Type(yyvsp[0]->get_Type());
            }
            if(yyvsp[-1]->getName() == "-") {
                string temp = newTemp();
                declaration_list.push_back(temp+(string)" dw ?");

                yyval->setCode(yyvsp[0]->getCode()+(string)"\tmov ax, "+yyvsp[0]->getcode_symbol()+(string)"\n\tmov "+temp+(string)", ax\n\tneg "+temp+(string)"\n");
                yyval->setcode_symbol(temp);
            } else {
                yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
                yyval->setCode(yyvsp[0]->getCode());
            }
    }
#line 2895 "y.tab.c"
    break;

  case 69:
#line 1152 "1705013.y"
                               {
            yyval = new SymbolInfo("!"+yyvsp[0]->getName(), "!unary_expression");
            log << "Line " << line_count << ": unary_expression : NOT unary_expression" << "\n"  << endl;
            log << "!"+yyvsp[0]->getName() << "\n"  << endl;  
            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function used in expression\n");
                already_got = 1;
            }
            yyval->set_Type("int");
            string tempCode="";
            tempCode += yyvsp[0]->getCode();
	    tempCode += "\tMOV AX," + yyvsp[0]->getcode_symbol()+"\n";
	    tempCode += "\tNOT AX\n";
            tempCode += "\tMOV " + yyvsp[0]->getcode_symbol()+",AX\n";

	    yyval->setCode(tempCode);
	    yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
    }
#line 2918 "y.tab.c"
    break;

  case 70:
#line 1170 "1705013.y"
                 {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "factor");
            log << "Line " << line_count << ": unary_expression : factor" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;  
            yyval->set_Type(yyvsp[0]->get_Type());
            yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
            yyval->setCode(yyvsp[0]->getCode());
    }
#line 2931 "y.tab.c"
    break;

  case 71:
#line 1180 "1705013.y"
                 {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "variable");
            log << "Line " << line_count << ": factor : variable" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;  
            yyval->set_Type(yyvsp[0]->get_Type());
            yyval->set_arrSize(yyvsp[0]->get_arrSize());

            yyval->setcode_symbol(yyvsp[0]->getcode_symbol());
            yyval->setCode(yyvsp[0]->getCode());

            if(yyval->get_arrSize() > -1) {
                string temp = newTemp();
                declaration_list.push_back(temp+(string)" dw ?");

                yyval->setCode(yyval->getCode()+(string)"\tmov ax, "+yyvsp[0]->getcode_symbol()+(string)"[bx]\n\tmov "+temp+(string)", ax\n");
                yyval->setcode_symbol(temp);
            }

    }
#line 2955 "y.tab.c"
    break;

  case 72:
#line 1199 "1705013.y"
                                         {
            yyval = new SymbolInfo(yyvsp[-3]->getName()+"("+yyvsp[-1]->getName()+")", "id(argument_list)");
            log << "Line " << line_count << ": factor : ID LPAREN argument_list RPAREN" << "\n"  << endl;
            log << yyvsp[-3]->getName()+"("+yyvsp[-1]->getName()+")" << "\n"  << endl;
            SymbolInfo* temp = symbolTable.lookUpAllScope(yyvsp[-3]->getName());
            bool valid =false;
            if(temp == NULL) {
		errorprint("Undeclared Function "+yyvsp[-3]->getName()+"\n");
		undefined_func = yyvsp[-3]->getName();
                yyval->set_Type("float"); 

            } else if(temp->get_arrSize() != -3) {
            	cout<<"factor"<<endl;
		errorprint("no such function definition found named "+yyvsp[-3]->getName()+"\n");
		undefined_func = yyvsp[-3]->getName();
                yyval->set_Type("float"); 

            } else {
            	yyval->set_Type(temp->get_Type());
                if(temp->get_paramSize()==1 && arg_list.size()==0 && temp->getParam(0).param_type=="void") {
                	yyval->set_Type(temp->get_Type());
                	valid =true;

                } else if(temp->get_paramSize() != arg_list.size()) {
		     errorprint("Total number of arguments mismatch in function "+yyvsp[-3]->getName()+"\n");

                } else {
                    int i;

                    for(i=0; i<arg_list.size(); i++) {
                        if(temp->getParam(i).param_type != arg_list[i]) {
                            break;
                        }
                    }

                    if(i != arg_list.size() && already_got==0) { 
			 log<<"Error at line no "<<line_count<<": "<<i+1<<"th argument mismatch in function "<<yyvsp[-3]->getName()<<"\n"<<endl;
			 error<<"Error at line no "<<line_count<<": "<<i+1<<"th argument mismatch in function "<<yyvsp[-3]->getName()<<"\n"<<endl;
			 error_count++;

                    }
                    else if(i != arg_list.size() && already_got==1){already_got = 0;}
                    else{
                    	yyval->set_Type(temp->get_Type());
                       valid = true;
                    }
                }
            }
	     if(valid == true) {
                string _temp = newTemp();
                declaration_list.push_back(_temp+(string)" dw ?");

                yyval->setCode(yyvsp[-1]->getCode());
                yyval->setCode(yyval->getCode()+(string)"\tpush address\n");

                for(int i=0; i<list_arg.size(); i++) {
                    yyval->setCode(yyval->getCode()+(string)"\tpush "+list_arg[i]+(string)"\n");
                }

                yyval->setCode(yyval->getCode()+(string)"\tcall "+temp->getName()+(string)"\n");

                if(temp->get_Type() != "void") {
                    yyval->setCode(yyval->getCode()+(string)"\tpop "+_temp+(string)"\n");
                }

                yyval->setCode(yyval->getCode()+(string)"\tpop address\n");
                yyval->setcode_symbol(_temp);
            }
            arg_list.clear(); 
            list_arg.clear();
    }
#line 3031 "y.tab.c"
    break;

  case 73:
#line 1270 "1705013.y"
                                   {
            yyval = new SymbolInfo("("+yyvsp[-1]->getName()+")", "(expression)");
            log << "Line " << line_count << ": factor : LPAREN expression RPAREN" << "\n"  << endl;
            log << "("+yyvsp[-1]->getName()+")" << "\n"  << endl;
            yyval->setcode_symbol(yyvsp[-1]->getcode_symbol());
            yyval->setCode(yyvsp[-1]->getCode());
            if(yyvsp[-1]->get_Type() == "void") {
		errorprint("void function used in expression\n");
		already_got = 1;
                yyvsp[-1]->set_Type("float"); 
            } 
            yyval->set_Type(yyvsp[-1]->get_Type());
    }
#line 3049 "y.tab.c"
    break;

  case 74:
#line 1283 "1705013.y"
                    {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "CONST_INT");
            log << "Line " << line_count << ": factor : CONST_INT" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;  
            yyval->set_Type("int");
            yyval->setcode_symbol(yyvsp[0]->getName());
    }
#line 3061 "y.tab.c"
    break;

  case 75:
#line 1290 "1705013.y"
                      {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "CONST_FLOAT");
            log << "Line no: " << line_count << ": factor : CONST_FLOAT" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl;  
            yyval->set_Type("float");
            yyval->setcode_symbol(yyvsp[0]->getName());
    }
#line 3073 "y.tab.c"
    break;

  case 76:
#line 1297 "1705013.y"
                         {
            yyval = new SymbolInfo(yyvsp[-1]->getName()+"++", "variable++");
            log << "Line " << line_count << ": factor : variable INCOP" << "\n"  << endl;
            log << yyvsp[-1]->getName()+"++" << "\n"  << endl; 
            yyval->set_Type(yyvsp[-1]->get_Type());
	     
	     
            if(yyvsp[-1]->get_arrSize() > -1) {
                string temp1 = newTemp();
                declaration_list.push_back(temp1+(string)" dw ?");
                yyval->setCode(yyvsp[-1]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-1]->getcode_symbol()+(string)"[bx]\n\tmov "+temp1+(string)", ax\n");
                yyval->setCode(yyval->getCode()+(string)"\tinc "+yyvsp[-1]->getcode_symbol()+(string)"[bx]\n");

                yyval->setcode_symbol(temp1);

            } else {
                string temp1 = newTemp();
                declaration_list.push_back(temp1+(string)" dw ?");
                yyval->setCode(yyvsp[-1]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-1]->getcode_symbol()+(string)"\n\tmov "+temp1+(string)", ax\n\tinc "+yyvsp[-1]->getcode_symbol()+(string)"\n");

                yyval->setcode_symbol(temp1);
            }

    }
#line 3102 "y.tab.c"
    break;

  case 77:
#line 1321 "1705013.y"
                         {
            yyval = new SymbolInfo(yyvsp[-1]->getName()+"--", "variable--");
            log << "Line " << line_count << ": factor : variable DECOP" << "\n"  << endl;
            log << yyvsp[-1]->getName()+"--" << "\n"  << endl; 
            yyval->set_Type(yyvsp[-1]->get_Type());
            if(yyvsp[-1]->get_arrSize() > -1) {
                string temp1 = newTemp();
                declaration_list.push_back(temp1+(string)" dw ?");
                yyval->setCode(yyvsp[-1]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-1]->getcode_symbol()+(string)"[bx]\n\tmov "+temp1+(string)", ax\n");
                yyval->setCode(yyval->getCode()+(string)"\tdec "+yyvsp[-1]->getcode_symbol()+(string)"[bx]\n");

                yyval->setcode_symbol(temp1);

            } else {
                string temp1 = newTemp();
                declaration_list.push_back(temp1+(string)" dw ?");
                yyval->setCode(yyvsp[-1]->getCode()+"\t;Line "+to_string(line_count)+" "+yyval->getName()+"\n"+(string)"\tmov ax, "+yyvsp[-1]->getcode_symbol()+(string)"\n\tmov "+temp1+(string)", ax\n\tdec "+yyvsp[-1]->getcode_symbol()+(string)"\n");

                yyval->setcode_symbol(temp1);
            }
    }
#line 3128 "y.tab.c"
    break;

  case 78:
#line 1342 "1705013.y"
                                       {
    	    yyval = new SymbolInfo(yyvsp[-3]->getName()+" ("+yyvsp[-1]->getName(),"error_arg");
	    log<<"Error at line no "<<line_count<<": Syntax error\n"<<endl;
	    log << yyvsp[-3]->getName()+" ("<<yyvsp[-1]->getName()+" "<<endl<<endl;
	    log<<"Error at line no "<<line_count<<": Right paranthesis missing"<<endl<<endl;
	    error<<"Error at line no "<<line_count<<": Right paranthesis missing"<<endl<<endl;    	        
    }
#line 3140 "y.tab.c"
    break;

  case 79:
#line 1351 "1705013.y"
                         {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "arguments");
            log << "Line " << line_count << ": argument_list : arguments" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl; 
            yyval->setCode(yyvsp[0]->getCode()); 
    }
#line 3151 "y.tab.c"
    break;

  case 80:
#line 1357 "1705013.y"
          {
            yyval = new SymbolInfo("", "empty");
            log << "Line " << line_count << ": argument_list : <epsilon-production>" << "\n"  << endl;
            log << "" << "\n"  << endl;  
    }
#line 3161 "y.tab.c"
    break;

  case 81:
#line 1364 "1705013.y"
                                            {
            yyval = new SymbolInfo(yyvsp[-2]->getName()+", "+yyvsp[0]->getName(), "argument,logic_expression");
            log << "Line " << line_count << ": arguments : arguments COMMA logic_expression" << "\n"  << endl;
            log << yyvsp[-2]->getName()+", "+yyvsp[0]->getName() << "\n"  << endl;  
            yyval->setCode(yyvsp[-2]->getCode()+yyvsp[0]->getCode()); 
            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function called within argument of function\n");
                yyvsp[0]->set_Type("float"); 
            } 
            arg_list.push_back(yyvsp[0]->get_Type());
            list_arg.push_back(yyvsp[0]->getcode_symbol());
    }
#line 3178 "y.tab.c"
    break;

  case 82:
#line 1376 "1705013.y"
                           {
            yyval = new SymbolInfo(yyvsp[0]->getName(), "logic_expression");
            log << "Line " << line_count << ": arguments : logic_expression" << "\n"  << endl;
            log << yyvsp[0]->getName() << "\n"  << endl; 
            yyval->setCode(yyvsp[0]->getCode()); 
            if(yyvsp[0]->get_Type() == "void") {
                errorprint("void function used in argument of function\n");
                yyvsp[0]->set_Type("float"); 
            } 
            arg_list.push_back(yyvsp[0]->get_Type());
            list_arg.push_back(yyvsp[0]->getcode_symbol());
    }
#line 3195 "y.tab.c"
    break;


#line 3199 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1391 "1705013.y"



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

