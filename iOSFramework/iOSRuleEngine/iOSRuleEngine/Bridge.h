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
@property (nonatomic) NSInteger validationCode;
@property (nonatomic, strong) NSString *message;
@end



@interface Bridge : NSObject
-(instancetype)initWithConstants:(NSString*)constantsFilePath ruleCatalog:(NSString*)ruleCatalogFilePath;
-(void)setSubKonceptOid:(NSInteger)subKonceptOid parameters:(NSDictionary*)parametersDictionary;

-(NSArray*)getAllowedValuesFor:(NSInteger)oid withType:(uint8_t)valueType;
-(NSArray*)getAllowedValuesFor:(NSInteger)oid;

-(NSString*)getDefaultStringValueFor:(NSInteger)oid;
-(NSNumber*)getDefaultBOOLValueFor:(NSInteger)oid;
-(NSNumber*)getDefaultLongValueFor:(NSInteger)oid;
-(NSNumber*)getDefaultDoubleValueFor:(NSInteger)oid;

-(void)createTA:(NSString*)CVR konceptOid:(unsigned short)konceptOid;
-(void)setValue:(id)value forPE:(unsigned short) peOid;
-(void)unsetPEOid:(unsigned short) peOid;

-(NSArray*)validatePE:(unsigned short) peOid;
@end
