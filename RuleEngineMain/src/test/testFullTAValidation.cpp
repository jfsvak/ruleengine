#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "ruleenginetestutils.h"

#include "../ruleengine/KonceptInfo_sbx.h"
#include "../ruleengine/RuleEngine_sbx.h"
#include "../ruleengine/TA_sbx.h"
#include "../ruleengine/Utils.h"
#include "../ruleengine/ValidationResult.h"
#include "../ruleengine/ValidationResults.h"


using namespace std;
using namespace sbx;

class Full_TA_CONTEXT_KI_OSV_25_50 : public ::testing::Test  {
protected:
    virtual void SetUp() {
        re = RuleEngine();
        re.initConstants(get_file_contents("basedata-ruleconstants.json"));
        re.initKoncepts(get_file_contents("koncepts.json"));
        re.parseRuleCatalogueJSON(get_file_contents("rule-catalogue.json"));

        KonceptInfo ki {4, 30, 0, // UnderkonceptOid:OSV 25-49
        	{
        	  {  1, "true" }, // Solidarisk faellestarif
			  {  6, "true" }, // SEB Firmapensionspulje
        	  { 11, "true" }, // Parameter-Basis
			  { 15, "true" } // FG-Spaend
        	} };
        re.initContext(ki, OUTSIDE);
    }

    RuleEngine re;
};


TEST_F(Full_TA_CONTEXT_KI_OSV_25_50, Full_TA_POSITIVE) {
	TA ta { "15124040", 4}; // KonceptOid 4 - OSV
	RuleEngine::_printDebugAtValidation = true;
	int total {136};

	auto r = re.validate(ta);
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(total, r.sizeValidationResults());
	cout << r;

	// union agreement
	ta.setValue(kUnionAgreementRelationship, sbx::kOUTSIDE);
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;

	//
	//
	cout << "GENERAL PAGE:" << endl;
	//
	ta.setValue(kAftaleIkraftdato, 20150601);

	cout << "Indmeldelseskriterier:" << endl;
	ta.setValue(kIndmeldelsesAlder, (long) 20);
	ta.setValue(kAnciennitet, (long) 3);
	ta.setValue(kAnciennitetTidlArbejdsplads, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=4, r.sizeValidationResults());
	cout << r;

	cout << "Fravalgte grupper:" << endl;
	ta.setValue(kGrpElever, false);
	ta.setValue(kGrpStuderende, false);
	ta.setValue(kGrpVikarer, false);
	ta.setValue(kGrpLaerlinge, false);
	ta.setValue(kGrpSeniorMedarbejdere, false);
	ta.setValue(kEjIndmeldelseOverAlder, 60);

	r = re.validate(ta);
	EXPECT_FALSE(r.isAllOk());
	EXPECT_EQ(total-=6, r.sizeValidationResults());
	cout << r;

	cout << "Omtegning:" << endl;
	ta.setValue(kOmtegning_MK, false); // setting omtegning_mk == false, should make Omtegndato and Omtegnperiode not required
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;
	ta.setValue(kOmtegning_MK, true); // setting omtegning_mk == true, should make Omtegndato and Omtegnperiode required
	r = re.validate(ta);
	EXPECT_EQ(total+=2, r.sizeValidationResults());
	cout << r;

	ta.setValue(kOmtegndato, (long) 20150615);
	ta.setValue(kOmtegnperiode, (long) 4);
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;

	ta.setValue(kModetype, re.getDefaultValue(kModetype)->stringValue());
	ta.setValue(kModearrangor, re.getDefaultValue(kModearrangor)->stringValue());
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;


	//
	//
	// PRODUCT PAGE
	//
	cout << "Udlobsalder:" << endl;
	ta.setValue(kUdlobsalder_Pension, 67);
	ta.setValue(kFravalgRisiko_MK, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;
	ta.setValue(kFravalgRisiko_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total+=1, r.sizeValidationResults());
	cout << r;
	ta.setValue(kFravalgRisikoAlder, 65);
	r = re.validate(ta);
	EXPECT_EQ(total-=1, r.sizeValidationResults());
	cout << r;



	//
	//
	// PRODUCT PAGE
	//
	cout << "Opsparingsprodukter:" << endl;
	ta.setValue(kTraditionel_MK, true);
	ta.setValue(kTidspensionMedGaranti_MK, false);
	ta.setValue(kTidspensionUdenGaranti_MK, false);
	ta.setValue(kLink_MK, true);
	ta.setValue(kMarkedspension_MK, true);
	ta.setValue(kStandardProduct, "Traditionel_MK");
	r = re.validate(ta);
	EXPECT_EQ(total-=6, r.sizeValidationResults());
	cout << r;

	ta.setValue(kMinAndelTraditionelPct, (long) 6);
	ta.setValue(kMinAndelTraditionelPctType, "% af gage");
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;

	ta.remove(kMinAndelTraditionelPct);
	ta.remove(kMinAndelTraditionelPctType);
	ta.setValue(kTidspensionMedGaranti_MK, true);
	ta.setValue(kTidspensionUdenGaranti_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total, r.sizeValidationResults());
	cout << r;


	ta.setValue(kUdbetalingsform, re.getDefaultValue(kUdbetalingsform)->stringValue());
	ta.setValue(kUdbetalingsperiode, re.getDefaultValue(kUdbetalingsperiode)->stringValue());
	ta.setValue(kMaxAndelLinkBidragPCT, 100);
	ta.setValue(kMaxAndelMarkedspensionBidragPct, 100);
	ta.setValue(kLinkKunForEgetBidrag_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total-=5, r.sizeValidationResults());
	cout << r;



	//
	//
	// PRODUCT PAGE
	//
	cout << "Invaliditetsdaekning:" << endl;
	ta.setValue(kTAEReguleringskode, "Gage");
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;
	ta.setValue(kTAEReguleringskode, "Ingen");
	r = re.validate(ta);
	EXPECT_EQ(total-=1, r.sizeValidationResults());
	cout << r;
	ta.setValue(kTAEReguleringskode, "Gage");
	ta.setValue(kTAEPctGrMin, (long) 30);
	ta.setValue(kTAEPctOblMax, (long) 40);
	ta.setValue(kTAEBlGrMin, (long) 100000);
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;

	ta.setValue(kTAE_Udlobsforskellig_MK, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;
	ta.setValue(kTAE_Udlobsforskellig_MK, true); // requires to be set
	r = re.validate(ta);
	EXPECT_EQ(total+=1, r.sizeValidationResults());
	cout << r;

	ta.setValue(kTAE_Udlobsalder, (long) 60);
	ta.setValue(kTAESoliMax, "Ingen"); // requires kTAEDaekningstype to be set
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;

	ta.setValue(kTAEDaekningstype, re.getDefaultValue(kTAEDaekningstype)->stringValue());
	ta.setValue(kTAETilfirma_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;

	ta.setValue(kRetVedInv, 50);
	ta.setValue(kFagspec_mk, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;

	ta.setValue(kKortTAE_Daekning, "Ingen");
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;
	ta.setValue(kKortTAE_Daekning, "Frivillig");
	r = re.validate(ta);
	EXPECT_EQ(total+=2, r.sizeValidationResults());
	cout << r;
	ta.setValue(kKortTAE_Min_obl_faktor, (long) 0);
	ta.setValue(kKortTAE_Daekningsperiode, "9");
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;



	//
	//
	// PRODUCT PAGE
	//
	cout << "Invalidesum:" << endl;
	ta.setValue(kInvalidesumReguleringskode, "Ingen");
	r = re.validate(ta);
	EXPECT_EQ(total-=4, r.sizeValidationResults());
	cout << r;
	ta.setValue(kInvalidesumReguleringskode, "Gage");
	r = re.validate(ta);
	EXPECT_EQ(total+=1, r.sizeValidationResults());
	cout << r;
	ta.setValue(kInvalidesumPctMin, (long) 40);
	ta.setValue(kInvalidesumPctMax, (long) 50);
	ta.setValue(kInvalidesumBlMin, (long) 20000);
	r = re.validate(ta);
	EXPECT_EQ(total-=4, r.sizeValidationResults());
	cout << r;
	ta.setValue(kInvalidesumSoliMax, re.getDefaultValue(kInvalidesumSoliMax)->stringValue());
	ta.setValue(kInvalidesumSkattekode, re.getDefaultValue(kInvalidesumSkattekode)->stringValue());
	ta.setValue(kInvalidesumUdlobsalder, 60);
	ta.setValue(kInvalidesumSuppldaekn_MK, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=4, r.sizeValidationResults());
	cout << r;



	//
	//
	// PRODUCT PAGE
	//
	cout << "Dodfaldsdaekning:" << endl;
	ta.setValue(kDoedReguleringskode, "Gage");
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;
	ta.setValue(kDoedReguleringskode, "Ingen");
	r = re.validate(ta);
	EXPECT_EQ(total-=1, r.sizeValidationResults());
	cout << r;
	ta.setValue(kDoedBlGrMin, (long) 200000);
	ta.setValue(kDoedBlOblMax, (long) 300000);
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;
	ta.setValue(kDoedSoliMax, "Obligatorisk maks");
	r = re.validate(ta);
	EXPECT_EQ(total-=1, r.sizeValidationResults());
	cout << r;
	ta.setValue(kDoedSoliMax, "Tegningsmaks");
	r = re.validate(ta);
	EXPECT_EQ(total-=1, r.sizeValidationResults());
	cout << r;
	ta.setValue(kDoedSoliMax, "Obligatorisk maks");
	ta.setValue(kDoedDaekningstype, re.getDefaultValue(kDoedDaekningstype)->stringValue());
	ta.setValue(kDoedSkattekode, re.getDefaultValue(kDoedSkattekode)->stringValue());
	r = re.validate(ta);
	EXPECT_EQ(total-=1, r.sizeValidationResults());
	cout << r;

	//
	//
	// PRODUCT PAGE
	//
	cout << "Boernerente:" << endl;
	ta.setValue(kBoernerente_Reguleringstype, "Pristal");
	r = re.validate(ta);
	EXPECT_EQ(total-=4, r.sizeValidationResults());
	cout << r;
	ta.setValue(kBoernerente_Reguleringstype, "Gage");
	r = re.validate(ta);
	EXPECT_EQ(total, r.sizeValidationResults());
	cout << r;
	ta.setValue(kBoerneRentePctMin, (long) 20);
	ta.setValue(kBoerneRentePctMax, (long) 20);
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults()); // falls with span aswell
	cout << r;
	ta.setValue(kBoerneRenteSoliMax, "Tegningsmaks");
	ta.setValue(kBoerneUdloebsalder, (long) 24);
	ta.setValue(kBoerneSumBlMin, (long) 20000);
	ta.setValue(kBoerneSumBlMax, (long) 20000); // +span
	r = re.validate(ta);
	EXPECT_EQ(total-=5, r.sizeValidationResults());
	cout << r;


	//
	//
	// PRODUCT PAGE
	//
	cout << "Kritisk sygdom:" << endl;
	ta.setValue(kKritiskSygdom_i_FG_mk, false); // minus type, min/max/span x 2, skattetype, suppldaekning
	r = re.validate(ta);
	EXPECT_EQ(total-=10, r.sizeValidationResults());
	cout << r;

	ta.setValue(kKritiskSygdom_i_FG_mk, true);
	r = re.validate(ta);
	EXPECT_EQ(total+=9, r.sizeValidationResults());
	cout << r;
	ta.setValue(kKritiskSygReguleringskode, "Gage");
	ta.setValue(kKritiskSygPctMin, (long) 20);
	ta.setValue(kKritiskSygPctMax, (long) 30);
	ta.setValue(kKritiskSygSkattekode, re.getDefaultValue(kKritiskSygSkattekode)->stringValue());
	ta.setValue(kKritiskSygSuppldaekn_mk, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=9, r.sizeValidationResults());
	cout << r;

	cout << "Kritisk sygdom bornesum:" << endl;
	ta.setValue(kKritiskSygBornesum_i_FG_mk, false); // minus type, min/max/span x 2, skattekode, suppldaekning
	r = re.validate(ta);
	EXPECT_EQ(total-=10, r.sizeValidationResults());
	cout << r;
	ta.setValue(kKritiskSygBornesum_i_FG_mk, true); // minus type, min/max/span x 2, skattekode, suppldaekning
	r = re.validate(ta);
	EXPECT_EQ(total+=10, r.sizeValidationResults());
	cout << r;
	ta.setValue(kKritiskSygBornesumReguleringskode, "Ingen");
	ta.setValue(kKritiskSygBornesumBlMin, (long) 2000);
	ta.setValue(kKritiskSygBornesumBlMax, (long) 3000);
	ta.setValue(kKritiskSygBornesumSkattekode, re.getDefaultValue(kKritiskSygBornesumSkattekode)->stringValue());
	ta.setValue(kKritiskSygBornesumSuppldaekn_mk, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=9, r.sizeValidationResults());
	cout << r;
	ta.setValue(kKritiskSygBornesum_i_FG_mk, false); // minus type, min/max/span x 2, skattekode, suppldaekning
	ta.remove(kKritiskSygBornesumReguleringskode);
	ta.remove(kKritiskSygBornesumBlMin);
	ta.remove(kKritiskSygBornesumBlMax);
	ta.remove(kKritiskSygBornesumSkattekode);
	ta.remove(kKritiskSygBornesumSuppldaekn_mk);
	r = re.validate(ta);
	EXPECT_EQ(total-=1, r.sizeValidationResults());
	cout << r;


	cout << "Behandlingsforsikring:" << endl;
	ta.setValue(kHospitalsdaekning_MK, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=3, r.sizeValidationResults());
	cout << r;
	ta.setValue(kHospitalsdaekning_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total+=2, r.sizeValidationResults());
	cout << r;
	ta.setValue(kHospitalsdaekningLeverandoer, "Codan");
	ta.setValue(kHospitalsdaekningFrivillig_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;



	cout << "Eksisterende ordninger:" << endl;
	ta.setValue(kEksisterendeOrdning_MK, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=5, r.sizeValidationResults());
	cout << r;
	ta.setValue(kEksisterendeOrdning_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total+=4, r.sizeValidationResults());
	cout << r;
	ta.setValue(kPensionsudbyder, true);
	ta.setValue(kIndtraedelseSenere_MK, true);
	ta.setValue(kIndtraedelsesdato, 20150801);
	ta.setValue(kOblDaekningSammeStoerrelse_MK, false);
	ta.setValue(kHelbred_leverandorskift, "FP7");
	r = re.validate(ta);
	EXPECT_EQ(total-=5, r.sizeValidationResults());
	cout << r;


	cout << "Virksomhedsomdannelse:" << endl;
	ta.setValue(kFusion_MK, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=5, r.sizeValidationResults());
	cout << r;
	ta.setValue(kFusion_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total+=4, r.sizeValidationResults());
	cout << r;
	ta.setValue(kFusionTidligerePensionsordning_MK, true);
	ta.setValue(kFusionIndtraedelsesdato, 20150601);
	ta.setValue(kFusionPensionsselskab, "ATP");
	ta.setValue(kFusionOblDaekningSammeStoerrelse_MK, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=4, r.sizeValidationResults());
	cout << r;


	//
	//
	// SALARY PAGE
	//
	cout << "Lon og betaling:" << endl;
	ta.setValue(kLoenDefinition, re.getDefaultValue(kLoenDefinition)->stringValue());
	ta.setValue(kLoenRegulering, "januar");
	ta.setValue(kLoenreguleringsfrekvens, re.getDefaultValue(kLoenreguleringsfrekvens)->stringValue());
	ta.setValue(kBidragsstigningsform, "Alder");
	r = re.validate(ta);
	EXPECT_EQ(total-=4, r.sizeValidationResults());
	cout << r;
	ta.setValue(kPrivate_Taxed_MK, false);
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;
	ta.setValue(kPrivate_Taxed_MK, true);
	r = re.validate(ta);
	EXPECT_EQ(total+=1, r.sizeValidationResults());
	cout << r;
	ta.setValue(kPrivate_Premium_BL, (long) 200000);
	ta.setValue(kbagud_mk, "j");
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;
	ta.setValue(kPrivate_Taxed_MK, false);
	ta.remove(kPrivate_Premium_BL);
	r = re.validate(ta);
	EXPECT_EQ(total, r.sizeValidationResults());
	cout << r;

	ta.addContributionStep( {20, 4, 4} );
	ta.addContributionStep( {23, 4, 4} );
	r = re.validate(ta);
	EXPECT_EQ(total-=1, r.sizeValidationResults());
	cout << r;

	// misc
	ta.setValue(kSeneste_Underskriftsdato, (long) 20150801);
	ta.setValue(kKortTAE_Tegningsmax_bl, re.getDefaultValue(kKortTAE_Tegningsmax_bl)->longValue());
	ta.setValue(kKortTAE_Tegningsmax_faktor, re.getDefaultValue(kKortTAE_Tegningsmax_faktor)->longValue());
	ta.setValue(kKortTAE_Max_obl_faktor, re.getDefaultValue(kKortTAE_Max_obl_faktor)->longValue());
	ta.setValue(kKritiskSygdom_MK, false);
	ta.setValue(kTransomkostninger, "Ja");
	ta.setValue(kMaks_Pensionsgivende_Loen_MK, false);
	ta.setValue(kMaks_Pensionsgivende_Loen, (long) 360000);

	r = re.validate(ta);
	EXPECT_EQ(total-=8, r.sizeValidationResults());
	cout << r;

	ta.setValue(kGrpIndgaarIDepotsikring_MK, 0);
	ta.setValue(kIndslusning_MK, 0);
	r = re.validate(ta);
	EXPECT_EQ(total-=2, r.sizeValidationResults());
	cout << r;

//	re.getContainer().printContainerOverview(17);
//	re.getContainer().printProducts();
//	re.getContainer().printParametersToProducts(17);
}
