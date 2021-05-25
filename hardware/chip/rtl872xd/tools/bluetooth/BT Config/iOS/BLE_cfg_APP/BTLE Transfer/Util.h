//
//  Util.h
//  BTLE Transfer
//
//  Created by CN3_SD9 on 2016/8/10.
//  Copyright © 2016年 Apple. All rights reserved.
//

#ifndef Util_h
#define Util_h

@interface Util : NSObject

+ (BOOL) IsEnableWIFI;
+ (NSString *)fetchCurrBSSID;
+ (NSString *)fetchCurrSSID;
+ (NSString *)fetchCurrSSID_RAW;
+ (void)mac2str:(char *)mac :(char *)str;
+ (NSString *)standardFormateMAC:(NSString *)MAC;
@end

#endif /* Util_h */
