//
//  Bridge.m
//  REIntegration
//
//  Created by Anton Soloviev on 2015-05-14.
//  Copyright (c) 2015 Anton Soloviev. All rights reserved.
//

#import "Bridge.h"

#include "RuleEngine_sbx.h"
#include "Constant_sbx.h"
#include <iostream>
#include <fstream>
#include "KonceptInfo_sbx.h"
#include "Constant_sbx.h"

@implementation Bridge {
    sbx::RuleEngine *re;
}

-(instancetype)initWithRulesFilePath:(NSString*) rulesFilePath {
    self = [super init];
    if (self) {
        re = new sbx::RuleEngine();
//        NSString *rulesFilePath = [NSString stringWithFormat:@"%@", [[NSBundle mainBundle] pathForResource:@"basedata-ruleconstants" ofType:@"json"]];
        std::string json = get_file_contents(rulesFilePath.UTF8String);
        re->initConstants(json);
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
    
    re->initContext(ki);
}

-(void)dealloc {
    delete re;
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
    NSMutableArray *result;
    
    try {
        sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
        std::vector<std::shared_ptr<sbx::Constant>> list = re->getOptionsList(peOid);
        
        result = [NSMutableArray arrayWithCapacity:list.size()];
        
        for (std::shared_ptr<sbx::Constant> constant : list) {
            [result addObject:[NSString stringWithCString:constant->stringValue().c_str() encoding:NSUTF8StringEncoding]];
        }
    } catch (std::domain_error error) {
        NSLog(@"No options list for oid %zd", oid);
    }
    
	return result;
}

-(id)getDefaultValueFor:(NSInteger)oid {
    NSString *result = @"";
    
    try {
        sbx::ProductElementOid peOid = (sbx::ProductElementOid)oid;
        std::shared_ptr<sbx::Constant> constant = re->getDefaultValue(peOid);
        
        result = [NSString stringWithCString:constant->stringValue().c_str() encoding:NSUTF8StringEncoding];

    } catch (std::domain_error error) {
        NSLog(@"No default value for oid %zd", oid);
    }
    
    return result;
}

@end