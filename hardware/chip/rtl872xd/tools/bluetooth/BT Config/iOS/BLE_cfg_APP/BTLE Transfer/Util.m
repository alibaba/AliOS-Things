//
//  Util.m
//  BTLE Transfer
//
//  Created by CN3_SD9 on 2016/8/10.
//  Copyright © 2016年 Apple. All rights reserved.
//

#import <ifaddrs.h> // For getifaddrs()
#import <Foundation/Foundation.h>
#import <SystemConfiguration/CaptiveNetwork.h>
#import <sys/socket.h>
#import <netinet/in.h>
#import <arpa/inet.h>
#import "Reachability.h"
#import "Util.h"

#pragma mark - Util implementation
@implementation Util
{

}

// is enable wifi
+ (BOOL) IsEnableWIFI
{
    return ([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] != NotReachable);
}

/*
+ (id)fetchSSIDInfo
{
    NSArray *ifs = (id)CFBridgingRelease(CNCopySupportedInterfaces());
    NSLog(@"%s: Supported interfaces: %@", __func__, ifs);
    id info = nil;
    for (NSString *ifnam in ifs) {
        info = (id)CFBridgingRelease(CNCopyCurrentNetworkInfo((CFStringRef)ifnam));
        NSLog(@"%s: %@ => %@", __func__, ifnam, info);
        if (info && [info count]) {
            break;
        }
    }
    return info;
}
*/

/* fetch BSSID info */
+(NSString *)fetchCurrBSSID
{
    NSArray *ifs = (id)CFBridgingRelease(CNCopySupportedInterfaces());
    NSDictionary *info = nil;
    for (NSString *ifnam in ifs) {
        info = (id)CFBridgingRelease(CNCopyCurrentNetworkInfo((__bridge CFStringRef)ifnam));
        if (info && [info count]) {
            break;
        }
#ifdef ARC
        [info release];
#endif
    }
#ifdef ARC
    [ifs release];
#endif
    
    NSString *auto_bssid = [info objectForKey:@"BSSID"];
    auto_bssid = [auto_bssid uppercaseString];
    //NSLog(@"Current BSSID: %@", auto_bssid);    
    return auto_bssid;
}

/* fetch SSID info */
+(NSString *)fetchCurrSSID
{
    NSArray *ifs = (id)CFBridgingRelease(CNCopySupportedInterfaces());
    NSDictionary *info = nil;
    for (NSString *ifnam in ifs) {
        info = (id)CFBridgingRelease(CNCopyCurrentNetworkInfo((__bridge CFStringRef)ifnam));
        if (info && [info count]) {
            break;
        }
#ifdef ARC
        [info release];
#endif
    }
#ifdef ARC
    [ifs release];
#endif
    
    NSString *auto_ssid = [info objectForKey:@"SSID"];
    //NSLog(@"Current SSID: %@", auto_ssid);
    
    return auto_ssid;
}

/* fetch SSID info */
+(NSString *)fetchCurrSSID_RAW
{
    NSArray *ifs = (id)CFBridgingRelease(CNCopySupportedInterfaces());
    NSDictionary *info = nil;
    for (NSString *ifnam in ifs) {
        info = (id)CFBridgingRelease(CNCopyCurrentNetworkInfo((__bridge CFStringRef)ifnam));
        if (info && [info count]) {
            break;
        }
#ifdef ARC
        [info release];
#endif
    }
#ifdef ARC
    [ifs release];
#endif
    
    NSString *str = [[NSString alloc]init];
    NSString *_ssid_data = @"";
    NSString *auto_ssid_data = [info objectForKey:@"SSIDDATA"];
    //NSLog(@"Current SSIDDATA: %@", auto_ssid_data);
    str = [NSString stringWithFormat:@"%@",auto_ssid_data];
    if(str.length>0){
        _ssid_data = [str stringByReplacingOccurrencesOfString:@"<" withString:@""];
        _ssid_data = [_ssid_data stringByReplacingOccurrencesOfString:@">" withString:@""];
        _ssid_data = [_ssid_data stringByReplacingOccurrencesOfString:@" " withString:@""];
        //NSLog(@"Current _ssid_data: %@", _ssid_data);
    }
    
    auto_ssid_data = _ssid_data;
    
    return auto_ssid_data;
}

/* fetch Local IP info */
- (unsigned int)getLocalIPAddress
{
    NSString        *address = @"error";
    struct ifaddrs  *interfaces = NULL;
    struct ifaddrs  *temp_addr = NULL;
    const char      *wifi_ip_char;
    unsigned int    wifi_ip = 0;
    int             success = 0;
    
    int count = 0;
    int bits = 0; //for sub_wifi_ip
    int bytes = 0; //for sub
    char sub_wifi_ip[3] = {0x30};//at most 3 byte of IP address format, e,g 192.
    unsigned char sub[4] = {0x0}; // four bytes for wifi_ip
    
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    
    if (success == 0) {
        // Loop through linked list of interfaces
        
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                // Check if interface is en0 which is the wifi connection on the iPhone
                // it may also be en1 on your ipad3.
                if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
                    // Get NSString from C String
                    address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                }
            }
            
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    
    wifi_ip_char = [address UTF8String];
    NSLog(@"address=%@, wifi_ip_char=%s", address, wifi_ip_char);
    if ([address isEqualToString:@"error"]) {
#define SC_ERR_LOCAL_IP     0
        return SC_ERR_LOCAL_IP;
    }
    while(1)
    {
        if(((wifi_ip_char[count]!='.')&&(bytes<3)) || ((bytes==3)&&(wifi_ip_char[count]!='\0')) )
        {
            sub_wifi_ip[bits] = wifi_ip_char[count];
            //NSLog(@"sub_wifi_ip[%d]=%02x", bits, wifi_ip_char[count]);
            bits++;
            count++;
            continue;
        }else{
            int i = 0;
            for (i=0; i<3; i++) {
                //NSLog(@"sub_wifi_ip[%d]=%02x", i, sub_wifi_ip[i]);
            }
            if (bits==1) {
                sub[bytes] = sub_wifi_ip[0]-0x30;
            }else if(bits==2){
                sub[bytes] = 10 * (sub_wifi_ip[0]-0x30) + (sub_wifi_ip[1]-0x30);
            }else if(bits==3){
                sub[bytes] = 100 * (sub_wifi_ip[0]-0x30) + 10 * (sub_wifi_ip[1]-0x30) + (sub_wifi_ip[2]-0x30);
            }
            //NSLog(@"sub[%d]=%d",bytes, sub[bytes]);
            bits=0;
            bytes++;
            count++;
            memset(sub_wifi_ip, 0x30, 3);
        }
        if(bytes==4)
            break;
    }
    
    wifi_ip = (sub[0]<<24) + (sub[1]<<16) + (sub[2]<<8) + sub[3];
    NSLog(@"wifi ip=%x",wifi_ip);
    return wifi_ip;
}

+(void)mac2str:(char *)mac :(char *)str
{
    int i,y=0;
    int c;
    char hex_to_char[16] = {'0','1','2','3','4','5','6','7','8','9',
                            'A','B','C','D','E','F'};
    
    for(i=0;i<6;i++){
        c = (mac[i] >> 4) &0x0f;
        str[y++] = hex_to_char[c];
        
        c = mac[i] & 0x0f;
        str[y++] = hex_to_char[c];
        
        str[y++] = ':';
    }
    
    str[y-1] = '\0';
}

+ (NSString *)standardFormateMAC:(NSString *)MAC {
    NSArray * subStr = [MAC componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@":-"]];
    NSMutableArray * subStr_M = [[NSMutableArray alloc] initWithCapacity:0];
    for (NSString * str in subStr) {
        if (1 == str.length) {
            NSString * tmpStr = [NSString stringWithFormat:@"0%@", str];
            [subStr_M addObject:tmpStr];
        } else {
            [subStr_M addObject:str];
        }
    }
    
    NSString * formateMAC = [subStr_M componentsJoinedByString:@":"];
    //[subStr_M release];
    return [formateMAC uppercaseString];
}

@end
