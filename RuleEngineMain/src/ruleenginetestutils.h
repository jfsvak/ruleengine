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

#include "ruleengine/Constant.h"

void printVector(const std::vector<std::string>&);
void printVector(const std::vector<std::shared_ptr<sbx::Constant>>&);
int testConstant(void);
void makeDummyConstants(std::vector<sbx::Constant> &constants);

#endif /* RULEENGINETESTUTILS_H_ */
