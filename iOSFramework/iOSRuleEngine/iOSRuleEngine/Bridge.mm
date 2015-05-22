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

-(instancetype)initWithConstants:(NSString*)constantsFilePath ruleCatalog:(NSString*)ruleCatalogFilePath  {
    self = [super init];
    if (self) {
        re = sbx::RuleEngine();
        re.initConstants(get_file_contents(constantsFilePath.UTF8String));
        re.parseRuleCatalogueJSON(get_file_contents(ruleCatalogFilePath.UTF8String));
    }
    return self;
}

-(void)setSubKonceptOid:(NSInteger)subKonceptOid parameters:(NSDictionary*)parametersDictionary {
    std::map<unsigned short, std::string> parameters;
    
    sbx::KonceptInfo ki = sbx::KonceptInfo(static_cast<unsigned short>(subKonceptOid));
    
    if (parametersDictionary) {
        for (NSNumber *oid in parametersDictionary) {
            if ([parametersDictionary[oid] isKindOfClass:[NSString class]]) {
                ki.addParameterValue(static_cast<unsigned short>([oid shortValue]), std::string([parametersDictionary[oid] UTF8String]));
            } else if([oid isEqual:@14]) {
                ki.addParameterValue(14, [parametersDictionary[oid] boolValue]);
            } else {
                NSAssert(YES, @"Can't add parameter to koncept info: %@", parametersDictionary);
            }
        }
    }
    
    re.initContext(ki);
}

-(void)createTA:(NSString*)CVR konceptOid:(unsigned short)konceptOid {
    ta = sbx::TA([CVR UTF8String], konceptOid);
}

-(void)setValue:(id)value forPE:(unsigned short) peOid {
    ta.setValue(peOid, std::string([[value description] UTF8String]));
}

-(void)unsetPEOid:(unsigned short) peOid {
    auto values = ta.getValues();
    values.erase(peOid);
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

-(NSArray*)getAllowedValuesFor:(NSInteger)oid {
    sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
    NSMutableArray *result;
    try {
        std::vector<std::shared_ptr<sbx::Constant>> list = re.getOptionsList(peOid);
        
        result = [NSMutableArray arrayWithCapacity:list.size()];
        
        for (std::shared_ptr<sbx::Constant> constant : list) {
            [result addObject:[NSString stringWithCString:constant->stringValue().c_str() encoding:NSUTF8StringEncoding]];
        }
    } catch (std::exception error) {
        NSLog(@"getAllowedValuesFor:%zd -> %s", oid, error.std::exception::what());
    } catch (mup::ParserError error) {
        NSLog(@"getAllowedValuesFor:%zd -> %s", oid, error.GetMsg().c_str());
    } catch (...) {
        NSLog(@"getAllowedValuesFor:%zd unknown exception", oid);
    }
    
	return result;
}

-(id)getDefaultValueFor:(NSInteger)oid {
    sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
    
    NSString *result;
    try {
        std::shared_ptr<sbx::Constant> constant = re.getDefaultValue(peOid);
        result = [NSString stringWithCString:constant->stringValue().c_str() encoding:NSUTF8StringEncoding];
    } catch (std::exception error) {
        NSLog(@"getDefaultValueFor:%zd -> %s", oid, error.std::exception::what());
    } catch (mup::ParserError error) {
        NSLog(@"getDefaultValueFor:%zd -> %s", oid, error.GetMsg().c_str());
    } catch (...) {
        NSLog(@"getDefaultValueFor:%zd unknown exception", oid);
    }
    
    return result;
}

-(NSArray*)validatePE:(unsigned short) peOid {
    NSMutableArray *allResults = [NSMutableArray array];

    try {
        sbx::ValidationResults results = re.validate(ta, peOid);
        bool ok = results.isAllOk();
        if (!ok) {
            for (auto result : results.getValidationResults()) {
                ValidationResult *r = [ValidationResult new];
                r.message = [NSString stringWithCString:result.second.getMessage().c_str() encoding:NSUTF8StringEncoding];
                r.validationCode = static_cast<uint16_t>(result.second.getValidationCode());
                r.oid = result.first;
                [allResults addObject:r];
            }
        }
    } catch (std::exception error) {
        NSLog(@"getDefaultValueFor:%zd -> %s", peOid, error.std::exception::what());
    } catch (mup::ParserError error) {
        NSLog(@"getDefaultValueFor:%zd -> %s", peOid, error.GetMsg().c_str());
    } catch (...) {
        NSLog(@"getDefaultValueFor:%zd unknown exception", peOid);
    }
    
    return allResults;
}

@end
