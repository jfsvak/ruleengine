#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "ruleengine/Constant_sbx.h"
#include "ruleengine/KonceptInfo_sbx.h"
#include "ruleengine/ProductElementValue_sbx.h"
#include "ruleengine/RuleConstantContainer_sbx.h"
#include "ruleengine/RuleEngine_sbx.h"
#include "ruleengine/RuleCatalogue.h"
#include "ruleengine/Rule.h"
#include "ruleengine/sbxTypes.h"
#include "ruleengine/TA_sbx.h"
#include "ruleengine/ValidationResult.h"
#include "ruleenginetestutils.h"

using namespace std;
using namespace sbx;

// Common data set for all test cases in this file
class ProductElementValidationTest : public ::testing::Test  {
protected:
    virtual void SetUp() {
        re = {  };
        re.initConstants(get_file_contents("RuleEngineMain/basedata-ruleconstants.json"));
        re.parseRuleCatalogueJSON(get_file_contents("RuleEngineMain/rule-catalogue.json"));

        KonceptInfo ki {27, { {11, "true"} }};
        re.initContext(ki);
    }

    RuleEngine re;
};

// Test cases
TEST_F(ProductElementValidationTest, aPositiveScenario) {
    TA ta { "15124040", 4};
    ta.setValue(kDoedReguleringskode, std::string {"Gage"})
        .setValue(kDoedPctGrMin, (long) 200)
        .setValue(kDoedPctOblMax, (long) 300);

    sbx::ValidationResult result = re.validate(ta, kDoedPctGrMin);

    EXPECT_EQ(result.getValidationResults().size(), 0);
}

TEST_F(ProductElementValidationTest, aNegativeScenario) {
    TA ta { "15124040", 4};
    ta.setValue(kDoedReguleringskode, std::string {"Gage"})
        .setValue(kDoedPctGrMin, (long) 200)
        .setValue(kDoedPctOblMax, (long) 300);

    ta.getValue(kDoedPctGrMin).setValue("700");
    ta.getValue(kDoedPctOblMax).setValue("801");

    sbx::ValidationResult result = re.validate(ta, kDoedPctGrMin);

    EXPECT_NE(result.getValidationResults().size(), 0);
}
