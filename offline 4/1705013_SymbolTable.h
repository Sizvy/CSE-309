#include<iostream>
#include<string>
#include<fstream>
#include<vector>

using namespace std;


class SymbolInfo {
    string name;
    string type;

    SymbolInfo* next;
    string _type; 
    int _size;//(-1=variable, -2=function_declaration, -3=function_definition)

    struct param {
        string param_type;
        string param_name;
    } temp_param;
    
    vector<param> param_list;
    string code;
    string code_symbol;

public:
    SymbolInfo() {

    }

    SymbolInfo(string name, string type) {
        this->name = name;
        this->type = type;

        next = NULL;
    }
    string getName() const {
        return name;
    }

    string getType() const {
        return type;
    }

    void setNext(SymbolInfo* next) {
        this->next = next;
        return ;
    }

    SymbolInfo* getNext() const {
        return next;
    }

   ~SymbolInfo() {
        param_list.clear();
    }
//newly added
    void set_Type(string _type) {
        this->_type = _type;
        return ;
    }

    string get_Type() const {
        return _type;
    }

    void set_arrSize(int _size) {
        this->_size = _size;
        return ;
    }

    int get_arrSize() const {

        return _size;
    }

    int get_paramSize() const {
        return param_list.size();
    }

    void addParam(string param_type, string param_name) {
        temp_param.param_type = param_type;
        temp_param.param_name = param_name;

        param_list.push_back(temp_param);
        return ;
    }

    param getParam(int index) const {
        return param_list[index];
    }
    void clearParam_list(){
    	param_list.clear();
    }
    void setcode_symbol(string symbol){
    	this->code_symbol = symbol;
    }
    void setCode(string code){
    	this->code = code;
    }
    string getcode_symbol(){
    	return code_symbol;
    }
    string getCode(){
    	return code;
    }
};


class ScopeTable {
    int id;
    int length;

    SymbolInfo** bucketList;
    ScopeTable* parentScope;

   int hashFunction(string Name) {
        int sum = 0;
        for(int i=0; i<Name.size(); i++)
        {
            sum+=(int)Name[i];
        }
        return sum%length;
    } 

public:
    ScopeTable() {

    }

    ScopeTable(int id, int length, ScopeTable* parentScope) {
        this->id = id;
        this->length = length;

        bucketList = new SymbolInfo*[length];

        for(int i=0; i<length; i++) {
            bucketList[i] = NULL;
        }

        this->parentScope = parentScope;
    }

    int getID() const {
        return id;
    }

    int getlength() const {
        return length;
    }

    ScopeTable* getParentScope() const {
        return parentScope;
    }

    ~ScopeTable() {
        delete[] bucketList;
    }

    SymbolInfo* lookUp(string key) {
        int index = hashFunction(key);
        int position = 0;

        SymbolInfo* temp = bucketList[index];

        while(temp != NULL) {
            if(key == temp->getName()) {
                return temp;
            }

            position++;
            temp = temp->getNext();
        }

        return NULL;
    }

    bool insertSymbol(SymbolInfo* symbol) {
        if(lookUp(symbol->getName()) != NULL) {
            return false;
        }

        int index = hashFunction(symbol->getName());
        SymbolInfo* temp = bucketList[index];

        if(temp == NULL) {
            bucketList[index] = symbol;
            symbol->setNext(NULL);
            return true;
        }

        while(temp->getNext() != NULL) {
            temp = temp->getNext();
        }

        temp->setNext(symbol);
        symbol->setNext(NULL);

        return true;
    }

    bool deleteSymbol(string key) {
        if(lookUp(key) == NULL) {
            return false;
        }

        int index = hashFunction(key);
        SymbolInfo* temp = bucketList[index];
        SymbolInfo* previous = NULL;

        while(temp != NULL) {
            if(key == temp->getName()) {
                break;
            }

            previous = temp;
            temp = temp->getNext();
        }

        if(previous == NULL) {
            bucketList[index] = temp->getNext();
        } else {
            previous->setNext(temp->getNext());
        }

        delete temp; 
        return true;
    }

    void print(ofstream& log) const {
        log << "ScopeTable #" << id << endl;

        for(int i=0; i<length; i++) {
            if(bucketList[i] == NULL) {
                continue;
            }

            log<< i << " -->";

            SymbolInfo* temp = bucketList[i];

            while(temp != NULL) {
                log << " <" << temp->getName() << ", " << temp->getType() << ">";
                temp = temp->getNext();
            }

            log << endl;
        }

        return ;
    }
};


class SymbolTable {
    ScopeTable* current;

public:
    SymbolTable() {
        current = NULL;
    }

    ~SymbolTable() {

    }

    void enterScope(int id, int length, ofstream& log) {
        ScopeTable* temp = new ScopeTable(id, length, current);
        current = temp;
	if(id!=1)
        	log<< "New ScopeTable with id " << id << " created" << endl;
        return ;
    }

    void exitScope(ofstream& log) {
        if(current == NULL) {
            return ;
        }

        //log<< "ScopeTable with id " << current->getID() << " removed" << endl;

        current = current->getParentScope();
        return ;
    }

    bool insertSymbol(SymbolInfo* symbol) {
        if(current == NULL) {
            return false;
        }

        return current->insertSymbol(symbol);
    }

    bool deleteSymbol(string key) {
        if(current == NULL) {
            return false;
        }

        return current->deleteSymbol(key);
    }

    SymbolInfo* lookUp(string key) {
        if(current == NULL) {
            return NULL;
        }

        return current->lookUp(key);
    }

    SymbolInfo* lookUpAllScope(string key) {
        if(current == NULL) {
            return NULL;
        }

        ScopeTable* temp = current;
        SymbolInfo* result = NULL;

        while(temp != NULL) {
            result = temp->lookUp(key);

            if(result != NULL) {
                break;
            }

            temp = temp->getParentScope();
        }

        return result;
    }

    void printCurrent(ofstream& log) const {
        if(current == NULL) {
            return ;
        }

        current->print(log);
        return ;
    }

    void printAllScope(ofstream& log) const {
        if(current == NULL) {
            return ;
        }

        ScopeTable* temp = current;

        while(temp != NULL) {
            temp->print(log);
            log << endl;

            temp = temp->getParentScope();
        }

        return ;
    }
};
