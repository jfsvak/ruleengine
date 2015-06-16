#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "../ruleengine/Constant_sbx.h"
#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/ProductElementValue_sbx.h"
#include "../ruleengine/RuleConstantContainer_sbx.h"
#include "../ruleengine/RuleEngine_sbx.h"
#include "../ruleengine/RuleCatalogue.h"
#include "../ruleengine/Rule.h"
#include "../ruleengine/sbxTypes.h"
#include "../ruleengine/TA_sbx.h"
#include "../ruleengine/ValidationResult.h"
#include "../ruleengine/muParser/mpError.h"
#include "../ruleengine/mubridge/utils.h"

#include "ruleenginetestutils.h"

using namespace std;
using namespace sbx;

// Common data set for all test cases in this file
class ProductElementValidationTest : public ::testing::Test  {
protected:
    virtual void SetUp() {        
        re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.initKoncepts(get_file_contents("koncepts.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {4, 20, 0, { {11, "true"} }};
        re.initContext(ki, OUTSIDE);
    }

    RuleEngine re;
};

// Test cases
TEST_F(ProductElementValidationTest, aPositiveScenario) {
    TA ta { "15124040", 4};
    RuleEngine::_printDebugAtValidation = true;
    ta.setValue(kDoedReguleringskode, std::string {"Gage"})
    			.setValue(kDoedPctGrMin, (long) 200)
				.setValue(kDoedPctOblMax, (long) 300);
//    ta.setValue(kDoedReguleringskode, std::string {"Gage"}).setValue(kDoedPctGrMin, (long) 200).setValue(kDoedPctOblMax, (long) 300);
    
    auto result = re.validate(ta, (unsigned short) kDoedPctGrMin);

    EXPECT_EQ(0, result.getValidationResults().size());
    if (result.getValidationResults().size() != 0)
    	cout << result;
}

TEST_F(ProductElementValidationTest, peValueIsOverTheLimit) {
    TA ta { "15124040", 4};
    ta.setValue(kDoedReguleringskode, std::string {"Gage"});
    ta.getValue(kDoedPctGrMin).setValue("700");
    ta.getValue(kDoedPctOblMax).setValue("801");

    auto result = re.validate(ta, {kDoedPctGrMin, kDoedPctOblMax} );
    auto v = result.getValidationResults(kDoedPctOblMax);

    EXPECT_EQ(1, v.size());
    EXPECT_EQ(sbx::ValidationCode::kValueOverLimit, v.at(0).getValidationCode());
    
    if (v.at(0).getValidationCode() != sbx::ValidationCode::kValueOverLimit)
    	cout << result;
}

TEST_F(ProductElementValidationTest, relatedPEValueSpaendIsOverTheLimit) {
    RuleEngine::_printDebugAtValidation = true;
    TA ta { "15124040", 4};
    ta.setValue(kDoedReguleringskode, std::string {"Gage"});
    ta.getValue(kDoedPctGrMin).setValue("100");
    ta.getValue(kDoedPctOblMax).setValue("700");
	auto result = re.validate(ta, (unsigned short) kDoedSpaendPct);

	auto v = result.getValidationResults(kDoedSpaendPct);
	cout << result;
	ASSERT_EQ(1, v.size());
	EXPECT_EQ(kValueOverLimit, v.at(0).getValidationCode());
}
