#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "../ruleengine/RuleEngine_sbx.h"

#include "ruleenginetestutils.h"
#include "testRuleEngineInitialiser.cpp"

using namespace std;
using namespace sbx;

class RuleEngine_Koncept_Parsing : public RuleEngineInitialiser {
protected:
    virtual void SetUp() {
    	RuleEngineInitialiser::SetUp();

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{ {11, "true"}, // Parameter-Basis
        	  {1, "true"}, // Solidarisk faellestarif
			  {6, "true"} // SEB Firmapensionspulje
        	} };
        re.initContext(ki, OUTSIDE);
    }
};

TEST_F(RuleEngine_Koncept_Parsing, Parsing_Of_Koncepts) {
	re.getContainer().printKoncepts();

	EXPECT_EQ(28, re.getKoncept(4).getSubkoncept(5).getOid());
	EXPECT_EQ(16, re.getKoncept(4).getSubkoncept(4).getOid());
	EXPECT_EQ(28, re.getKoncept(4).getSubkoncept(9).getOid());
	EXPECT_EQ(27, re.getKoncept(4).getSubkoncept(10).getOid());
	EXPECT_EQ(27, re.getKoncept(4).getSubkoncept(24).getOid());
	EXPECT_EQ(17, re.getKoncept(4).getSubkoncept(25).getOid());

	EXPECT_EQ(31, re.getKoncept(7).getSubkoncept(1).getOid());
	EXPECT_EQ(29, re.getKoncept(7).getSubkoncept(3).getOid());
	EXPECT_EQ(29, re.getKoncept(7).getSubkoncept(9).getOid());
	EXPECT_EQ(30, re.getKoncept(7).getSubkoncept(10).getOid());

}
