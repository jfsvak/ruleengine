//
//  Bridge.m
//  REIntegration
//
//  Created by Anton Soloviev on 2015-05-14.
//  Copyright (c) 2015 Anton Soloviev. All rights reserved.
//

#import "Bridge.h"

#include "RuleEngine_sbx.h"
#include <iostream>
#include <fstream>
#include "KonceptInfo_sbx.h"
#include "Constant_sbx.h"
#include "TA_sbx.h"
#include "ValidationResults.h"
#include "ValidationResult.h"


@implementation ValidationResult
@end


@implementation Bridge {
    sbx::RuleEngine re;
    sbx::TA ta;
}

-(instancetype)initWithConstants:(NSString*)constantsFilePath ruleCatalog:(NSString*)ruleCatalogFilePath koncepts:(NSString*)koncepts {
    self = [super init];
    if (self) {
        re.initConstants(get_file_contents(constantsFilePath.UTF8String));
        re.parseRuleCatalogueJSON(get_file_contents(ruleCatalogFilePath.UTF8String));
        re.initKoncepts(get_file_contents(koncepts.UTF8String));
    }
    return self;
}

std::string get_file_contents(const char *filename)
{
    std::ifstream inf(filename, std::ios::in | std::ios::binary);
    if (inf)
    {
        std::string contents;
        inf.seekg(0, std::ios::end);
        contents.resize(inf.tellg());
        inf.seekg(0, std::ios::beg);
        inf.read(&contents[0], contents.size());
        inf.close();
        return(contents);
    }
    throw(1);
}

-(void)setKonceptOid:(NSInteger)konceptOid numOfEmpl:(NSInteger)numOfEmpl numOfRiskClass:(NSInteger)numOfRiscClassC parameters:(NSDictionary*)parametersDictionary {
    std::map<unsigned short, std::string> parameters;

    sbx::KonceptInfo ki = sbx::KonceptInfo(static_cast<unsigned short>(konceptOid), static_cast<unsigned short>(numOfEmpl),
                                           static_cast<unsigned short>(numOfRiscClassC), parameters);
    
    if (parametersDictionary) {
        for (NSNumber *oid in parametersDictionary) {
            short oidShort = [oid shortValue];
            if ([parametersDictionary[oid] isKindOfClass:[NSString class]]) {
                ki.addParameterValue(static_cast<unsigned short>(oidShort), std::string([parametersDictionary[oid] UTF8String]));
            } else if(oidShort == 14 || oidShort == 11 || oidShort == 15) { // BOOL values encoded as NSNumber
                ki.addParameterValue(oidShort, [parametersDictionary[oid] boolValue]);
            } else {
                NSAssert(YES, @"Can't add parameter to koncept info: %@", parametersDictionary);
            }
        }
    }
    
    re.initContext(ki, sbx::OUTSIDE);
}

-(void)createTA:(NSString*)CVR konceptOid:(unsigned short)konceptOid {
    ta = sbx::TA([CVR UTF8String], konceptOid);
}

-(void)setTAUnionAgreementRelation:(uint8_t)relation {
    ta.setUar(static_cast<sbx::UnionAgreementRelationship>(relation));
}

-(void)setTAUnionAgreement:(uint8_t)uaOid {
    ta.setValue(sbx::ProductElementOid::kUnionAgreementOid, uaOid);
}

-(void)initUAContribution:(unsigned short)oid employeePct:(double)employeePct companyPct:(double)companyPct {
    sbx::ContributionStep step(0, employeePct, companyPct);
    std::map<unsigned short, std::vector<sbx::ContributionStep> > uaSteps;
    std::pair<unsigned short, std::vector<sbx::ContributionStep> > uaStep(oid, std::vector<sbx::ContributionStep>{step});
    uaSteps.insert(uaStep);
    re.initUAContributionSteps(uaSteps);
}

-(void)addContributionStep:(long)index employeePct:(double)employeePct companyPct:(double)companyPct {
    sbx::ContributionStep step(index, employeePct, companyPct);
    ta.addContributionStep(step);
}

-(void)removeAllContributionSteps {
    ta.removeContributionSteps();
}

#pragma mark - value setters

-(void)setStringValue:(NSString*)value forPE:(unsigned short) peOid {
    ta.setValue(peOid, std::string([[value description] UTF8String]));
}

-(void)setLongValue:(NSNumber*)value forPE:(unsigned short) peOid {
    ta.setValue(peOid, [value longValue]);
}

-(void)setDoubleValue:(NSNumber*)value forPE:(unsigned short) peOid {
    ta.setValue(peOid, [value doubleValue]);
}

-(void)setBoolValue:(NSNumber*)value forPE:(unsigned short) peOid {
    ta.setValue(peOid, static_cast<bool>([value boolValue]));
}

-(void)unsetPEOid:(unsigned short) peOid {
    ta.remove(peOid);
}

#pragma mark - get options list

-(NSArray*)getAllowedValuesFor:(NSInteger)oid withType:(uint8_t)valueType {
    typedef enum _valueType{
        kValueTypeText          =0,
        kValueTypeInt           =1,
        kValueTypeDouble        =2,
        kValueTypeDate          =3,
        kValueTypeBool          =4,
        kValueTypeArray         =5,
        kValueTypeInput         =6,
        kValueTypeInvalid       =-1
    }ValueType;

    sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
    NSMutableArray *result;
    try {
        std::vector<std::shared_ptr<sbx::Constant>> list = re.getOptionsList(peOid);
        
        result = [NSMutableArray arrayWithCapacity:list.size()];
        
        for (std::shared_ptr<sbx::Constant> constant : list) {
            switch (valueType) {
                case kValueTypeText:
                    [result addObject:[NSString stringWithCString:constant->stringValue().c_str() encoding:NSUTF8StringEncoding]];
                    break;
                case kValueTypeInt:
                    [result addObject:[NSNumber numberWithLong:constant->longValue()]];
                    break;
                case kValueTypeDouble:
                    [result addObject:[NSNumber numberWithDouble:constant->doubleValue()]];
                    break;
                case kValueTypeBool:
                    [result addObject:[NSNumber numberWithBool:constant->boolValue()]];
                    break;
                default:
                    [result addObject:[NSString stringWithCString:constant->stringValue().c_str() encoding:NSUTF8StringEncoding]];
                    break;
            }
        }
    } catch (const std::exception &error) {
        NSLog(@"getAllowedValuesFor:%zd -> %s", oid, error.what());
    } catch (mup::ParserError error) {
        NSLog(@"getAllowedValuesFor:%zd -> mup::ParserError %s", oid, error.GetMsg().c_str());
    } catch (...) {
        NSLog(@"getAllowedValuesFor:%zd unknown exception", oid);
    }
    
    
    switch (valueType) {
        case kValueTypeText:
            result = [result sortedArrayUsingComparator:^(NSString* a, NSString* b) { return [a compare:b options:NSNumericSearch]; }];
            break;
        case kValueTypeInt:
            result = [result sortedArrayUsingComparator:^(NSNumber* a, NSNumber* b) { return [a compare:b]; }];
            break;
        case kValueTypeDouble:
            result = [result sortedArrayUsingComparator:^(NSNumber* a, NSNumber* b) { return [a compare:b]; }];
            break;
        default:
            break;
    }
    
    return result; //[result sortedArrayUsingComparator:^(NSString* a, NSString* b) { return [a compare:b options:NSNumericSearch]; }];
}

-(NSArray*)getAllowedValuesFor:(NSInteger)oid {
    sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
    NSMutableArray *result;
    try {
        std::vector<std::shared_ptr<sbx::Constant>> list = re.getOptionsList(peOid);
        
        result = [NSMutableArray arrayWithCapacity:list.size()];
        
        for (std::shared_ptr<sbx::Constant> constant : list) {
            [result addObject:[NSString stringWithCString:constant->stringValue().c_str() encoding:NSUTF8StringEncoding]];
        }
    } catch (const std::exception &error) {
        NSLog(@"getAllowedValuesFor:%zd -> %s", oid, error.what());
    } catch (mup::ParserError error) {
        NSLog(@"getAllowedValuesFor:%zd -> mup::ParserError %s", oid, error.GetMsg().c_str());
    } catch (...) {
        NSLog(@"getAllowedValuesFor:%zd unknown exception", oid);
    }
    
    return [result sortedArrayUsingComparator:^(NSString* a, NSString* b) { return [a compare:b options:NSNumericSearch]; }];
}

#pragma mark - get defaults

-(NSString*)getDefaultStringValueFor:(NSInteger)oid {
    sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
    
    NSString *result;
    try {
        std::shared_ptr<sbx::Constant> constant = re.getDefaultValue(peOid);
        if (constant != nullptr) {
            result = [NSString stringWithCString:constant->stringValue().c_str() encoding:NSUTF8StringEncoding];
        }
    } catch (const std::exception &error) {
        NSLog(@"getDefaultValueFor:%zd -> %s", oid, error.what());
        re.printVariablesInParser();
    } catch (mup::ParserError error) {
        NSLog(@"getDefaultValueFor:%zd -> mup::ParserError %s", oid, error.GetMsg().c_str());
        re.printVariablesInParser();
    } catch (...) {
        NSLog(@"getDefaultValueFor:%zd unknown exception", oid);
        re.printVariablesInParser();
    }
    
    return result;
}

-(NSNumber*)getDefaultBOOLValueFor:(NSInteger)oid {
    sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
    
    NSNumber *result;
    
    try {
        std::shared_ptr<sbx::Constant> constant = re.getDefaultValue(peOid);
        if (constant != nullptr) {
            result = [NSNumber numberWithBool:constant->boolValue()];
        }
    } catch (const std::exception &error) {
        NSLog(@"getDefaultValueFor:%zd -> %s", oid, error.what());
        re.printVariablesInParser();
    } catch (mup::ParserError error) {
        NSLog(@"getDefaultValueFor:%zd -> mup::ParserError %s", oid, error.GetMsg().c_str());
        re.printVariablesInParser();
    } catch (...) {
        NSLog(@"getDefaultValueFor:%zd unknown exception", oid);
        re.printVariablesInParser();
    }
    
    return result;
}

-(NSNumber*)getDefaultLongValueFor:(NSInteger)oid {
    sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
    
    NSNumber *result;
    
    try {
        std::shared_ptr<sbx::Constant> constant = re.getDefaultValue(peOid);
        if (constant != nullptr) {
            result = [NSNumber numberWithLong:constant->longValue()];
        }
    } catch (const std::exception &error) {
        NSLog(@"getDefaultValueFor:%zd -> %s", oid, error.what());
        re.printVariablesInParser();
    } catch (mup::ParserError error) {
        NSLog(@"getDefaultValueFor:%zd -> mup::ParserError %s", oid, error.GetMsg().c_str());
        re.printVariablesInParser();
    } catch (...) {
        NSLog(@"getDefaultValueFor:%zd unknown exception", oid);
        re.printVariablesInParser();
    }
    
    return result;
}

-(NSNumber*)getDefaultDoubleValueFor:(NSInteger)oid {
    sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
    
    NSNumber *result;
    
    try {
        std::shared_ptr<sbx::Constant> constant = re.getDefaultValue(peOid);
        if (constant != nullptr) {
            result = [NSNumber numberWithDouble:constant->doubleValue()];
        }
    } catch (const std::exception &error) {
        NSLog(@"getDefaultValueFor:%zd -> %s", oid, error.what());
        re.printVariablesInParser();
    } catch (mup::ParserError error) {
        NSLog(@"getDefaultValueFor:%zd -> mup::ParserError %s", oid, error.GetMsg().c_str());
        re.printVariablesInParser();
    } catch (...) {
        NSLog(@"getDefaultValueFor:%zd unknown exception", oid);
        re.printVariablesInParser();
    }
    
    return result;
}

#pragma mark - validation

-(NSArray*)mapValidationResults:(sbx::ValidationResults) results {
    NSMutableArray *allResults = [NSMutableArray array];

    bool ok = results.isAllOk();
    if (!ok) {
        for (auto result : results.getValidationResults()) {
            ValidationResult *r = [ValidationResult new];
            r.message = [NSString stringWithCString:result.second.getMessage().c_str() encoding:NSUTF8StringEncoding];
            r.validationCode = static_cast<uint16_t>(result.second.getValidationCode());
            r.oid = result.first;
            r.variable = [NSString stringWithCString:result.second.getVariableName().c_str() encoding:NSUTF8StringEncoding];
            [allResults addObject:r];
        }
    }
    
    return allResults;
}

-(NSArray*)validatePE:(unsigned short) peOid printDebug:(BOOL) printDebug {
    NSArray *allResults;
    try {
        if (printDebug) {
            re._printDebugAtValidation = true;
        }
        
        sbx::ValidationResults results = re.validate(ta, peOid);
        
        if (printDebug) {
            re._printDebugAtValidation = false;
        }
        
        if (printDebug) {
            std::cout << results;
            re.printConstantsInParser();
        }
        
        allResults = [self mapValidationResults:results];
        
        if (printDebug) {
            re.printVariablesInParser();
        }
    } catch (const std::exception &error) {
        NSLog(@"validatePE:%zd -> %s", peOid, error.what());
        re.printVariablesInParser();
    } catch (mup::ParserError error) {
        NSLog(@"validatePE:%zd -> mup::ParserError %s", peOid, error.GetMsg().c_str());
        re.printVariablesInParser();
    } catch (...) {
        NSLog(@"validatePE:%zd unknown exception", peOid);
        re.printVariablesInParser();
    }
    
    return allResults;
}

-(NSArray*)validatePEList:(NSArray*)peList printDebug:(BOOL) printDebug {
    NSArray *allResults;
    std::vector<unsigned short> peVector;
    try {
        if (printDebug) {
            re._printDebugAtValidation = true;
        }
        
        for (NSNumber *peOid in peList) {
            peVector.push_back([peOid shortValue]);
        }
        sbx::ValidationResults results = re.validate(ta, peVector);
        
        if (printDebug) {
            re._printDebugAtValidation = false;
        }
        
        if (printDebug) {
            std::cout << results;
            re.printConstantsInParser();
        }
        
        allResults = [self mapValidationResults:results];
        
        if (printDebug) {
            re.printVariablesInParser();
        }
    } catch (const std::exception &error) {
        NSLog(@"validatePEList:%@ -> %s", peList, error.what());
        re.printVariablesInParser();
    } catch (mup::ParserError error) {
        NSLog(@"validatePEList:%@ -> mup::ParserError %s", peList, error.GetMsg().c_str());
        re.printVariablesInParser();
    } catch (...) {
        NSLog(@"validatePEList:%@ unknown exception", peList);
        re.printVariablesInParser();
    }
    
    return allResults;
}

-(NSArray*)validateTAFull:(BOOL)full printDebug:(BOOL) printDebug {
    NSArray *allResults;
    try {
        if (printDebug) {
            re._printDebugAtValidation = true;
        }
        
        sbx::ValidationResults results = re.validate(ta, static_cast<bool>(full));
        
        if (printDebug) {
            re._printDebugAtValidation = false;
        }
        
        if (printDebug) {
            std::cout << results;
            re.printConstantsInParser();
        }
        
        allResults = [self mapValidationResults:results];
        
        if (printDebug) {
            re.printVariablesInParser();
        }
    } catch (const std::exception &error) {
        NSLog(@"validateTAFull:%d -> %s", full, error.what());
        re.printVariablesInParser();
    } catch (mup::ParserError error) {
        NSLog(@"validateTAFull:%d -> mup::ParserError %s", full, error.GetMsg().c_str());
        re.printVariablesInParser();
    } catch (...) {
        NSLog(@"validateTAFull:%d unknown exception", full);
        re.printVariablesInParser();
    }
    
    return allResults;
}

-(BOOL)isProductElementAllowed:(NSInteger)oid {
    return re.isProductElementAllowed(static_cast<unsigned short>(oid));
}


@end
