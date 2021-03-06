#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "otherFunctions.h"
#include <string>
#include <vector>
#include <cstdio>
#include <iostream>

using namespace rapidjson;
using namespace std;

void Merge(Value& doc,Value::AllocatorType& all) { //combine duplicate key in json
    if(doc.IsObject()) {
        for(Value::MemberIterator key=doc.MemberBegin();key!=doc.MemberEnd();key++) {
            if(!key->value.IsArray())
                key->value=Value(kArrayType).Move().PushBack(key->value,all);
        }   
        Value::MemberIterator key=doc.MemberBegin();
        while(key!=doc.MemberEnd()) {
            Value::MemberIterator tempkey=doc.FindMember(key->name);
            if(key!=tempkey) {
                tempkey->value.PushBack(key->value[0],all);
                key=doc.EraseMember(key);
            }
            else
                key++;
        }
        for(key=doc.MemberBegin();key!=doc.MemberEnd();key++) {
            if(key->value.Size()==1)
                key->value=key->value[0];
        }
    }
}
int main(int argc, char *argv[]) {
    if(argc!=2) { //checking for correct input
        cout<<"Requires exactly one json file"<<endl;
        return 0;
    }
    //const char json[] = "{\"gcd\":\"unsigned long long a, unsigned long long b\",\"MysteryCrypto\":\"unsigned long long a, unsigned long long b, unsigned long long g, unsigned long long p\",\"MysteryCrypto\":\"unsigned long long p, unsigned long long q, unsigned long long m\"}";
    char buffer[1024];
    FILE *file=fopen(argv[1],"r");
    if(!file) { //checking for file errors
        cout<<"file "<<argv[1]<<" is not found"<<endl;
        return 0;
    }
    //initializing RapidJson document
    FileReadStream frs(file,buffer,sizeof(buffer));
    Document document;
    document.ParseStream<0, UTF8<>, FileReadStream>(frs);
    Merge(document,document.GetAllocator());
    
    //initializing variables
    string line;
    string name;
    string parse;
    string delimiter1="(";
    string delimiter2=",";
    string delimiter3=")";
    vector<string> para;
    size_t index=0;

    //getting stdin argument
    getline(cin,line);
    if((index=line.find(delimiter1))==std::string::npos) { //checking if stdin is valid
        cout<<"invalid input"<<endl;
        return 0;
    }
    parse=line.substr(0,index);
    line.erase(0,index+delimiter1.length());
    name=trim(parse);
    while((index=line.find(delimiter2))!=std::string::npos) {
        parse=line.substr(0,index);
        line.erase(0,index+delimiter2.length());
        para.push_back(trim(parse));
    }
    if((index=line.find(delimiter3))==std::string::npos) { //checking if stdin is valid
        cout<<"invalid input"<<endl;
        return 0;
    }
    parse=line.substr(0,index);
    para.push_back(trim(parse));

    //checking if stdin exist within document
    for(Value::MemberIterator key=document.MemberBegin();key!=document.MemberEnd();key++) {
        if(key->name.GetString()==name) {
            if(key->value.IsArray()) { //checking for keys that are duplicated
                for(auto& val:key->value.GetArray()) {
                    index=0;
                    int i=0;
                    string temp=val.GetString();
                    while((index=temp.find(delimiter2))!=std::string::npos) {
                        string parse=temp.substr(0,index);
                        parse=trim(parse);
                        temp.erase(0,index+delimiter2.length());
                        if(getVariable(parse)==para[i]) {
                            i++;
                            continue;
                        }
                    }
                    if(i+1==para.size()&&getVariable(temp)==para[i]) {
                        cout<<key->name.GetString()<<" : "<<val.GetString()<<endl;
                    }
                }
            }
            else { //TODO fix after finding a way to also check for data type
                cout<<key->name.GetString()<<" : "<<key->value.GetString()<<endl;
            }
        }
    }
    /*for(Value::MemberIterator key=document.MemberBegin();key!=document.MemberEnd();key++) {
        if(key->value.IsArray()) {
            for(auto& val:key->value.GetArray())
                cout<<key->name.GetString()<<" : "<<val.GetString()<<endl;
            cout<<endl;
        }
        else
            cout<<key->name.GetString()<<" : "<<key->value.GetString()<<endl;
        
    }*/
    return 0;
}
