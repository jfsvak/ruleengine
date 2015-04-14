//============================================================================
// Name        : helloworld2.cpp
// Author      : xjes
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <sstream>
#include <vector>
#include <iostream>
#include <string>

#include "RuleEngine.h"
#include "sbxTypes.h"
#include "muParser/mpParser.h"

using namespace std;
using namespace mup;

namespace sbx {

  void RuleEngine::initConstants(const std::vector<sbx::Constant> &allConstants)
  {
	  _container.initGlobalConstants(allConstants);
//	  loadRuleCatalogue();
  }

  /**
   * Initialises local context that is used for following operations on RuleEngine
   */
  void RuleEngine::initContext(const short underkoncept_oid, const short unionagreement_oid)
  {
	  _container.initContext(underkoncept_oid, unionagreement_oid);
  }

  /**
   * Delegate call to RuleConstantContainer::getOptionsList(...)
   */
  vector<string> RuleEngine::getOptions(const ProductElementNames productElement)
  {
	  return _container.getOptions(productElement);
  }

  /**
   * Delegate call to RuleConstantContainer::getOptionsList(...)
   */
  std::vector<std::shared_ptr<Constant>> RuleEngine::getOptionsList(const sbx::ProductElementNames productElement)
  {
	  return _container.getOptionsList(productElement);
  }

  /**
   * Delegate call to RuleConstantContainer::getConstant(...)
   */
  std::shared_ptr<sbx::Constant> RuleEngine::getConstant(const sbx::ProductElementNames productElement, const sbx::ComparisonTypes comparisonType)
  {
	  return _container.getConstant(productElement, comparisonType);
  }

  /**
   *
   */
  void RuleEngine::loadRuleCatalogue()
  {
	  _ruleCatalogue[kTaeSpaendBl] = {"rc_taespaendbl_max >= (a-b)" };
	  _ruleCatalogue[kTaeBlGrMin] = {"taeblgrmin <= taebloblmax" };
  }

  /**
   *
   */
  void RuleEngine::loadRuleConstants(ParserX &p)
  {
	  // load rule constants - for now, set dummy values
	  ProductElement pe = {kTaeBlGrMin, "taeblgrmin", kCurr};

//	rc_cont["rc_taeblgrmin_min"] = MakeRuleConstant(pe, kMin, 100000);
//	rc_cont["rc_taeblgrmin_max"] = MakeRuleConstant(pe, kMax, 800000);
//	rc_cont["rc_taebloblmax_min"] = MakeRuleConstant(pe, kMin, 100000);
//	rc_cont["rc_taebloblmax_max"] = MakeRuleConstant(pe, kMax, 800000);

	  pe = {kTaeSpaendBl, "taespaendbl", kCurr};
//	  rc_cont["rc_taespaendbl_max"] = MakeRuleConstant(pe, kMax, "200000");

	  try {
		  p.EnableAutoCreateVar(true);
		  p.SetExpr("rc_taeblgrmin_min=100000");
		  p.Eval();
		  p.SetExpr("rc_taeblgrmin_max=800000");
		  p.Eval();
		  p.SetExpr("rc_taebloblmax_min=100000");
		  p.Eval();
		  p.SetExpr("rc_taebloblmax_max=800000");
		  p.Eval();
		  p.SetExpr("rc_taespaendbl_max=200000");
		  p.Eval();
	  } catch(ParserError &e) {
		  cout << "Exception: [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetExpr() << "]" << endl;
	  }
  }

  /*
   * Validates a single productElementValue using rules specific for the productElementType
   */
  int RuleEngine::validate(const sbx::ProductElementValue &peValue)
  {
	  switch (peValue.getProductElementType())
	  {
		  case kText:
		  {
			  // check in optionsList is this value is allowed
			  const vector<string> v = _container.getOptions(peValue.getProductElement());
			  const auto result = find(std::begin(v), std::end(v), peValue.stringValue());

			  // if we are not at the end, we found it, so we return 1
			  return (result != std::end(v) ? RuleEngine::VALID : RuleEngine::INVALID);
		  }
	  	  case kLong:
	  	  case kCurr:
		  case kPercent:
		  {
	  		  // Initialise a muParser and run the necessary evals.
			  cout << "Will do double validation" << endl;
			  ParserX p;
			  p.EnableAutoCreateVar(true);
			  // setup constants
			  std::ostringstream sstream;
			  sstream << _container.getConstant(peValue.getProductElement(), kMin)->doubleValue();
			  std::string varAsString = sstream.str();
			  p.SetExpr("min_constant=" + varAsString);
			  p.Eval();
			  sstream.str("");

			  sstream << _container.getConstant(peValue.getProductElement(), kMax)->doubleValue();
			  varAsString = sstream.str();
			  p.SetExpr("max_constant=" + varAsString);
			  p.Eval();
			  sstream.str("");

			  sstream << peValue.doubleValue();
			  varAsString = sstream.str();
			  p.SetExpr("variable=" + varAsString);
			  p.Eval();
			  sstream.str("");

			  printVariables(p);
			  printExpressions(p);

			  string minExpr = "variable >= min_constant";
			  p.SetExpr(minExpr);
			  Value result = p.Eval();
			  cout << "Result of min-value validation : [" << result << "]" << endl;

			  string maxExpr = "variable <= max_constant";
			  p.SetExpr(maxExpr);
			  result = p.Eval();
			  cout << "Result of max-value validation : [" << result << "]" << endl;

			  break;
		  }
	  	  default:
	  		  break;
	  }

	  return -1;
  }

  /*
   * Validates a single product element using the specific rules for that product element
   */
  int RuleEngine::validate(sbx::ProductElementNames p_element, std::vector<std::pair<std::string,long>> p_operands) {
	  cout << "The Start of Validate" << endl;

	  try {
		  ParserX p;
		  p.EnableAutoCreateVar(true);
		  loadRuleConstants(p);

		  for (auto & element : p_operands) {
			  stringstream sstm;
			  sstm << element.first << "=" << element.second;
			  string s = sstm.str();
			  cout << "Will add [" << s << "]" << endl;
			  p.SetExpr(s);
			  Value result = p.Eval();
			  cout << "Result of adding it: [" << result << "]" << endl;
		  }

		  printVariables(p);

		  int no_of_failures;

		  for (auto & rule : _ruleCatalogue[p_element]) {
			  p.SetExpr(rule);
			  Value result = p.Eval();
			  printExpressions(p);
			  if (!result) {
				  no_of_failures++;
			  }
		  }

		  cout << "The End of Validate" << endl;

		  return no_of_failures;
	  } catch(ParserError &e) {
		  cout << "Exception: [" << e.GetCode() << "]-[" << e.GetMsg() << "]-[" << e.GetExpr() << "]" << endl;
	  }

	  return -1;
  }

  /*
   * Validates the generic rules for the ComparisonType
   */
  const int RuleEngine::validate(sbx::ComparisonTypes comparisonType) const {
	  return -1;
  }

  void RuleEngine::printVariables(ParserX p) {
	  cout << "------------- Variables initialised BEGIN ------------" << endl;
	  var_maptype vmap = p.GetVar();
	  for (var_maptype::iterator item = vmap.begin();item!=vmap.end(); ++item) {
		  cout << item->first << "=" << (Variable&)(*(item->second)) << endl;
	  }
	  cout << "------------- Variables initialised END ------------" << endl << endl;
  }

  void RuleEngine::printExpressions(ParserX p) {
	  cout << "------------- Expressions BEGIN ------------" << endl;
	  var_maptype vmap = p.GetExprVar();
	  for (var_maptype::iterator item = vmap.begin(); item!=vmap.end(); ++item)
		  cout << "  " << item->first << " =  " << (Variable&)(*(item->second)) << "\n";
	  cout << "------------- Expressions END ------------" << endl;
  }

} // sbx namespace end
