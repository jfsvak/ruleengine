//
//  Bridge.h
//  REIntegration
//
//  Created by Anton Soloviev on 2015-05-14.
//  Copyright (c) 2015 Anton Soloviev. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Bridge : NSObject

-(instancetype)initWithConstants:(NSString*)constantsFilePath ruleCatalog:(NSString*)ruleCatalogFilePath;
-(void)setSubKonceptOid:(NSInteger)subKonceptOid parameters:(NSDictionary*)parametersDictionary;
-(NSArray*)getAllowedValuesFor:(NSInteger)oid;
-(id)getDefaultValueFor:(NSInteger)oid;

@end
