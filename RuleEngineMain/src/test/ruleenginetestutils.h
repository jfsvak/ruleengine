//============================================================================
// Name        : ruleenginetestutils.h
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef RULEENGINETESTUTILS_H_
#define RULEENGINETESTUTILS_H_

#include <memory>
#include <vector>
#include <iostream>

#include "../json/json.h"
#include "../ruleengine/Constant_sbx.h"

void printVector(const std::vector<std::string>&);
void printVector(const std::vector<std::shared_ptr<sbx::Constant>>&);
int testConstant(void);
void PrintJSONValue( const Json::Value &val );
bool PrintJSONTree(Json::Value &root, unsigned short depth);
std::string get_file_contents(const char *filename);
int testJsonLoad();

#endif /* RULEENGINETESTUTILS_H_ */
