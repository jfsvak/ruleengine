//============================================================================
// Name        : ruleenginetestutils.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include "../ruleengine/Constant_sbx.h"
#include "../json/json.h"

#include "ruleenginetestutils.h"

using namespace std;
using namespace sbx;

int testConstant(void) {
	//	sbx::Constant* c_ptr;
	//	std::vector<Constant> global_constants;
	//	sbx::ConstantTemplate<std::string> c {"hello"};
	sbx::Constant c { 1, 0, sbx::kTaeBlGrMin, sbx::kMin, "123.45" };
	cout << "String: " << c.stringValue() << endl;
	cout << "Long: " << c.longValue() << endl;
	cout << "Double: " << c.doubleValue()*2 << endl;
	return 0;
}

void printVector(const vector<string> &v) {
	cout << "------------- Vector Content BEGIN ------------" << endl;

	for (const string option : v) {
	  cout << "Option=" << option << endl;
	}
	cout << "------------- Vector Content END ------------" << endl;
}

void printVector(const vector<shared_ptr<Constant>> &v) {
	cout << "------------- Vector Content BEGIN ------------" << endl;

	for (const shared_ptr<Constant> constant : v) {
		constant->printValues();
	}

	cout << "------------- Vector Content END ------------" << endl;
}

bool PrintJSONTree(Json::Value &root, unsigned short depth) {
	depth += 1;
	cout << " {type=["<< root.type()<< "], size="<< root.size() << "}";
	if( root.size() > 0 ) {
		cout << "\n";
        
		for(Json::ValueIterator itr = root.begin(); itr != root.end(); itr++) {
            
			for( int tab = 0 ; tab < depth; tab++) {
				cout << "-";
			}
			cout << " ";
			PrintJSONValue(itr.key());
			cout << " -";
			PrintJSONTree( *itr, depth);
		}
		return true;
	} else {
		cout << " ";
		PrintJSONValue(root);
		cout << "\n" ;
	}
	return true;
}

void PrintJSONValue( const Json::Value &val )
{
    if( val.isString() ) {
        cout << "string("<< val.asString().c_str() << ")";
    } else if( val.isBool() ) {
    	cout << "bool(" << val.asBool() << ")";
    } else if( val.isInt() ) {
        cout << "int(" << val.asInt() << ")";
    } else if( val.isUInt() ) {
        cout << "uint(" << val.asUInt() << ")";
    } else if( val.isDouble() ) {
        cout << "double(" << val.asDouble() << ")";
    }
    else
    {
        cout << "unknown type=[" << val.type() << "]";
    }
}


std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(1);
}


int getValuesFromCIN() {
//	  sbx::RuleEngine re;
	  //	re.Init();

	  string s;
	  //	std::vector<std::pair<std::string, long>> operands;
//
//	do {
//		int v;
//		s.clear();
//		cout << "Enter product element: ";
//		cin >> s;
//		if (s == "exit")
//			break;
//		cout << "Enter value :" << endl;
//		cin >> v;
//		cout << "Product Element: " << s << " Value: " << v << endl;
//		pair<string, long> p(s,v);
//		operands.push_back(p);
//	} while (s != "exit");
//
//	for (auto & element : operands) {
//		stringstream sstm;
//		sstm << element.first << "=" << element.second;
//		string s = sstm.str();
//		cout << "Added " << s << endl;
//	}
//
//	int result = re.Validate(sbx::kTaeSpaendBl, operands);
//	cout << "No of validation exceptions: [" << result << "]" << endl;

	  return 0;
}

int testJsonLoad() {
	cout << "Load file: " << "rc-small.json" << endl;
	string json = get_file_contents("rc-small.json");
    
    //	cout << "Loading Json from string:" << endl << json << endl;
    
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(json, root);
    
	if (parsingSuccessful)
	{
        //		PrintJSONTree(root["data"], 0);
        
        //		Json::Value data = root["data"];
        
		Json::Value ruleConstants = root["data"].get("ruleConstant", 0);
        
		cout << "Printing RC " << endl;
		PrintJSONValue(ruleConstants);
		cout << "\nRC size: " << ruleConstants.size() << endl;
        
		Json::Value rc0 = ruleConstants.get(0u, 0);
		cout << "RC0 type: " << rc0.type() << " size: " << rc0.size() << endl;
        
		Json::Value::Members rcMembers = rc0.getMemberNames();
        
		for ( unsigned int i=0; i<rcMembers.size();++i) {
			string member = rcMembers[i];
			Json::Value value = rc0[member];
			cout << "  " << member << "=" << value.asString() << endl;
		}
        
		for (Json::ValueIterator ruleConstantElement = ruleConstants.begin(); ruleConstantElement != ruleConstants.end(); ruleConstantElement++) {
			cout << "Key:" << ruleConstantElement.key().asInt() << "Type:" << ruleConstantElement->type() << "Size:" << ruleConstantElement->size() << endl;
            
			int peOid = ruleConstantElement->get("productElementOid", 0).asInt();
			cout << "ProductElement:" << peOid << endl;
            
            //			for (Json::ValueIterator elementItr = rcItr->begin(); elementItr != rcItr->end(); elementItr++) {
            //				cout << "  Key[" << elementItr.key().asString() << "] Value:";
            //				PrintJSONValue(*elementItr);
            //				cout << endl;
            //
            //			}
            
		}
        //		for ( Json::ValueIterator itr = data.begin(); itr != data.end(); itr++) {
        //			PrintJSONValue(itr.key());
        //
        //			for (Json::ValueIterator itr2 = itr->begin(); itr2 != itr->end(); itr2++) {
        //				cout << "Key:" << itr2.key().asInt() << "Size:" << itr2->size() << endl;
        ////				PrintJSONValue();
        //			}
        //		}
        
        //		Json::Value::Members members = root["data"].getMemberNames();
        //		cout << "Members found in data: " << members.size() << endl;
        //
        //		for (unsigned int i = 0; i<members.size();++i) {
        //			string name = members[i];
        //			Json::Value value = root[name];
        //			cout<<"Key: " << name <<endl;
        //			cout<<"Value: " << value.toStyledString() <<endl;
        //			cout<<"type: " << value.type() <<endl;
        //		}
        
	}
	else {
		cout<<"Failed to parse JSON"<< endl <<reader.getFormattedErrorMessages() << endl;
	}
	return 0;
}
