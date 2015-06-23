//
//  Bridge.h
//  REIntegration
//
//  Created by Anton Soloviev on 2015-05-14.
//  Copyright (c) 2015 Anton Soloviev. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface ValidationResult : NSObject
@property (nonatomic) NSInteger oid;
@property (nonatomic) unsigned short validationCode;
@property (nonatomic, strong) NSString *message;
@property (nonatomic, strong) NSString *variable;
@end



@interface Bridge : NSObject
-(instancetype)initWithConstants:(NSString*)constantsFilePath ruleCatalog:(NSString*)ruleCatalogFilePath;
-(void)setKonceptOid:(NSInteger)konceptOid numOfEmpl:(NSInteger)numOfEmpl numOfRiskClass:(NSInteger)numOfRiscClassC
                                                                                parameters:(NSDictionary*)parametersDictionary;

-(void)createTA:(NSString*)CVR konceptOid:(unsigned short)konceptOid;
-(void)setTAUnionAgreementRelation:(uint8_t)relation;
-(void)setTAUnionAgreement:(uint8_t)uaOid;
-(void)unsetTAUnionAgreement;

-(BOOL)isProductElementAllowed:(NSInteger)oid;

-(NSArray*)getAllowedValuesFor:(NSInteger)oid withType:(uint8_t)valueType;
-(NSArray*)getAllowedValuesFor:(NSInteger)oid;

-(NSString*)getDefaultStringValueFor:(NSInteger)oid;
-(NSNumber*)getDefaultBOOLValueFor:(NSInteger)oid;
-(NSNumber*)getDefaultLongValueFor:(NSInteger)oid;
-(NSNumber*)getDefaultDoubleValueFor:(NSInteger)oid;

-(void)setStringValue:(NSString*)value forPE:(unsigned short) peOid;
-(void)setLongValue:(NSNumber*)value forPE:(unsigned short) peOid;
-(void)setDoubleValue:(NSNumber*)value forPE:(unsigned short) peOid;
-(void)setBoolValue:(NSNumber*)value forPE:(unsigned short) peOid;

-(void)initUAContribution:(unsigned short)oid employeePct:(double)employeePct companyPct:(double)companyPct;
-(void)addContributionStep:(long)index employeePct:(double)employeePct companyPct:(double)companyPct;
-(void)removeAllContributionSteps;

-(void)unsetPEOid:(unsigned short) peOid;

/**
 *  Validate single product element
 *  @param peOid      product element OID
 *  @param printDebug print debug info from c++ code
 *  @return array of ValidationResult objects, could contain results for other elements, could contain more than one result per product element
 */
-(NSArray*)validatePE:(unsigned short) peOid printDebug:(BOOL) printDebug;

/**
 *  Validate list of product elements
 *  @param peList     array of NSNumber objects with OIDs to validate
 *  @param printDebug print debug info from c++ code
 *  @return array of ValidationResult objects, could contain results for other elements, could contain more than one result per product element
 */
-(NSArray*)validatePEList:(NSArray*)peList printDebug:(BOOL) printDebug;

/**
 *  Validate current TA
 *  @param full       full (YES), or partial (NO) validation
 *  @param printDebug print debug info from c++ code
 *  @return array of ValidationResult objects, could contain more than one result per product element
 */
-(NSArray*)validateTAFull:(BOOL)full printDebug:(BOOL) printDebug;

@end
