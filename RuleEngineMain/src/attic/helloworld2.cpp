//============================================================================
// Name        : helloworld2.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>

#include "ruleengine/muParser/mpParser.h"

using namespace std;
using namespace mup;

int maina() {
	try {
		cout << "The Start" << endl;
		ParserX p;
		Value a = "Årlig";
		Value b = "Årlig";
		Value c = "Månedlig";

		Value arr(2, 0);
		arr.At(0) = Variable(&b);
		arr.At(1) = Variable(&c);

		p.DefineVar("a", Variable(&a));
		p.DefineVar("va", Variable(&arr));

		p.SetExpr("a==va[0] or a==va[1]");

		Value result = p.Eval();

		cout << "Eval result: [" << result << "]" << endl;

		var_maptype vmap = p.GetVar();
		for (var_maptype::iterator item = vmap.begin(); item!=vmap.end(); ++item) {
		  cout << item->first << "=" << (Variable&)(*(item->second)) << "\n";
		}
		cout << "The End" << endl;
	} catch(ParserError &e) {
		cout << "Exception: [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetExpr() << "]" << endl;
	}

	return 0;
}
