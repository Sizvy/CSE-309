#include<bits/stdc++.h>
using namespace std;
fstream out;

class SymbolInfo
{
private:
    string Name;
    string type;
    SymbolInfo *nextInfo;
public:
    SymbolInfo()
    {

    }
    SymbolInfo(string Name, string type)
    {
        this->Name = Name;
        this->type = type;
        nextInfo = nullptr;
    }
    void setName(string Name)
    {
        this->Name = Name;
    }
    void setType(string type)
    {
        this->type = type;
    }
    string getName()
    {
        return Name;
    }
    string getType()
    {
        return type;
    }
    void setNext(SymbolInfo *nextInfo)
    {
        this->nextInfo = nextInfo;
    }
    SymbolInfo* getNext()
    {
        return nextInfo;
    }
    ~SymbolInfo()
    {

    }
};

class ScopeTable
{
    //SymbolInfo *symbolinfo = new SymbolInfo();
    ScopeTable *parentScope;
    SymbolInfo **hashTable;
    int size,parent_id=1,current_id=0;
    string ID="1";
public:
    ScopeTable()
    {

    }
    ScopeTable(int n)
    {
        size = n;
        hashTable = new SymbolInfo*[size];
        for(int i=0; i<size; i++)
        {
            hashTable[i]=nullptr;
        }
    }
    void setParent_id(int parent_id)
    {
        this->parent_id=parent_id;
    }
    void setCurrent_id(int current_id)
    {
        this->current_id=current_id;
    }
    int getParent_id()
    {
        return parent_id;
    }
    int getCurrent_id()
    {
        return current_id;
    }
    void setID(string ID)
    {
        this->ID = ID;
    }
    string getID()
    {
        return ID;
    }
    void setParentScope(ScopeTable *table)
    {
        this->parentScope = table;
    }
    ScopeTable* getParentScope()
    {
        return parentScope;
    }
    int HashFunction(int buckets, string Name)
    {
        int sum = 0;
        for(int i=0; i<Name.size(); i++)
        {
            sum+=(int)Name[i];
        }
        return sum%buckets;
    }
    SymbolInfo* Lookup(string Name)
    {

        int idx = 0;
        int hashValue = HashFunction(size,Name);
        SymbolInfo *temp=hashTable[hashValue];
        while(temp!=nullptr)
        {
            if(temp->getName()==Name)
            {
                out<<"Found in ScopeTable# "<<ID<<" at position "<<hashValue<<", "<<idx<<endl;
                cout<<"Found in ScopeTable# "<<ID<<" at position "<<hashValue<<", "<<idx<<endl;
                return temp;
            }
            temp=temp->getNext();
            idx++;
        }
        return nullptr;
    }
    bool insertScope(string Name, string type)
    {
    	
        if(Lookup(Name)==nullptr)
        {
            int idx=0;
            int hashValue = HashFunction(size,Name);
            SymbolInfo *temp1 = new SymbolInfo(Name,type);
            SymbolInfo *temp = hashTable[hashValue];
            if(temp==nullptr)
            {
                hashTable[hashValue]=temp1;
                out<<"Inserted in ScopeTable# "<<ID<<" at position "<<hashValue<<", "<<idx<<endl;
                
                cout<<"Inserted in ScopeTable# "<<ID<<" at position "<<hashValue<<", "<<idx<<endl;
                
                return true;
            }
            else
            {
                idx++;
                while(temp->getNext()!=nullptr)
                {
                    temp = temp->getNext();
                    idx++;
                }
                temp->setNext(temp1);
                out<<"Inserted in ScopeTable# "<<ID<<" at position "<<hashValue<<", "<<idx<<endl;
                
                cout<<"Inserted in ScopeTable# "<<ID<<" at position "<<hashValue<<", "<<idx<<endl;
                
                return true;
            }

        }
        out<<"<"<<Name<<","<<type<<">"<<" already exists in current scope table"<<endl;
        cout<<"<"<<Name<<","<<type<<">"<<" already exists in current scope table"<<endl;
        
        return false;
    }
    bool DeleteScope(string name)
    {
        if(Lookup(name)==nullptr)
        {
            out<<name<<" not found"<<endl;
            cout<<name<<" not found"<<endl;
            return false;
        }
        else
        {
            int idx=0;
            int hashValue = HashFunction(size,name);
            SymbolInfo *temp=hashTable[hashValue];
            SymbolInfo *prev = nullptr;
            while(temp!=nullptr)
            {
                if(temp->getName()==name)
                {
                    out<<"Deleted Entry "<<hashValue<<", "<<idx<<" from ScopeTable# "<<ID<<endl;
                    cout<<"Deleted Entry "<<hashValue<<", "<<idx<<" from ScopeTable# "<<ID<<endl;
                    break;
                }
                prev = temp;
                temp=temp->getNext();
                idx++;
            }
            if(prev==nullptr)
            {
                hashTable[hashValue]=temp->getNext();
                delete temp;
                return true;
            }
            prev->setNext(temp->getNext());
            delete temp;
            return true;
        }
    }
    void PrintScope(FILE *logout)
    {
        out<<"Scopetable# "<<ID<<endl;
        char temp3[ID.size()+1];
        strcpy(temp3,ID.c_str());
        fprintf(logout,"Scopetable# %s\n",temp3);
        cout<<"Scopetable# "<<ID<<endl;
        for(int i=0; i<size; i++)
        {
            SymbolInfo *temp = hashTable[i];
            if(temp==nullptr)
            	continue;
            fprintf(logout,"%d -->",i);
            cout<<i<<" -->";
            while(temp!=nullptr)
            {
            	 char temp1[temp->getName().size()+1];
            	 strcpy(temp1,temp->getName().c_str());
            	 char temp2[temp->getType().size()+1];
            	 strcpy(temp2,temp->getType().c_str());
                fprintf(logout,"<%s : %s>",temp1,temp2);
                cout<<" < "<<temp->getName()<<" : "<<temp->getType()<<" > ";
                temp=temp->getNext();
            }
            out<<endl;
            fprintf(logout,"\n");
            cout<<endl;
        }
    }
    ~ScopeTable()
    {

        for(int i=0; i<size; i++)
        {
            SymbolInfo *temp = hashTable[i];
            while(temp!=nullptr)
            {
                delete temp;
                temp=temp->getNext();
            }
        }
        delete []hashTable;
    }
};

class SymbolTable
{
    ScopeTable *current;
    int countScope=1;
public:
    SymbolTable(int size)
    {
    	
        current = new ScopeTable(size);
    }
    void setCurrentScope(ScopeTable *CurrTable)
    {
        this->current = CurrTable;
    }
    ScopeTable *getCurrentScope()
    {
        return current;
    }
    void EnterScope(int n)
    {
        countScope++;
        ScopeTable *Table = new ScopeTable(n);
        Table->setParentScope(getCurrentScope());
        setCurrentScope(Table);
        current->setID(current->getParentScope()->getID()+"."+std::to_string(current->getParentScope()->getParent_id()+current->getParentScope()->getCurrent_id()));
        out<<"New ScopeTable with id "<<current->getID()<<" created"<<endl;
        cout<<"New ScopeTable with id "<<current->getID()<<" created"<<endl;
    }
    void ExitScope()
    {
        countScope--;
        out<<"ScopeTable with id "<<current->getID()<<" removed"<<endl;
        cout<<"ScopeTable with id "<<current->getID()<<" removed"<<endl;
        ScopeTable *temp = current->getParentScope();
        delete current;
        setCurrentScope(temp);
        int curr = current->getCurrent_id();
        current->setCurrent_id(curr+1);
    }
    bool insertSymbol(string Name, string type)
    {
        //cout<<"main"<<endl;
        return current->insertScope(Name,type);
    }
    bool RemoveSymbol(string Name)
    {
        if(current->DeleteScope(Name)==0)
        {
            out<<"Not found"<<endl;
            cout<<"Not found"<<endl;
            return false;
        }
        else
            return true;
    }
    //void PrintCurrentScope()
    //{
    //    current->PrintScope();
    //}
    SymbolInfo* Lookup(string name)
    {
        ScopeTable *temp = current;
        int tempcount = countScope;
        while(tempcount>0)
        {
            if(temp->Lookup(name)!=nullptr)
            {
                return temp->Lookup(name);
            }
            else
                temp=temp->getParentScope();
            tempcount--;
        }
        out<<"Not Found"<<endl;
        cout<<"Not Found"<<endl;
        return nullptr;
    }
    void PrintAllScope(FILE *logout)
    {
        ScopeTable *temp = current;
        int tempcount = countScope;
        while(tempcount>0)
        {
            temp->PrintScope(logout);
            fprintf(logout,"\n");
            temp = temp->getParentScope();
            tempcount--;
        }
    }
    ~SymbolTable()
    {
        while(countScope>0)
        {
            delete current;
            current = current->getParentScope();
            countScope--;
        }
    }

};

