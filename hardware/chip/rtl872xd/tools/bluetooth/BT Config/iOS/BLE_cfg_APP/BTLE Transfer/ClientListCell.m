//
//  ClientListCell.m
//  SimpleConfig
//
//  Created by Realsil on 14/11/17.
//  Copyright (c) 2014年 Realtek. All rights reserved.
//

#import "ClientListCell.h"

@implementation ClientListCell
@synthesize cell_dev_name, cell_dev_mac, cell_security, cell_rssi;

- (id)initWithContent: (NSString *)name mac:(NSString *)mac :(NSString *)img_security :(NSString *)img_rssi type:(unsigned int)type
{
    cell_dev_name.text = name;
    cell_dev_mac.text = mac;
    cell_security.image = [UIImage imageNamed:img_security];
    cell_rssi.image = [UIImage imageNamed:img_rssi];
    return [super initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"DiscoverCell"];
}

- (void)dealloc {
#ifdef ARC
    [cell_dev_name release];
    [cell_dev_mac release];
    [cell_security release];
    [cell_rssi release];
    [super dealloc];
#endif
}

- (void)setContent: (NSString *)name mac:(NSString *)mac :(NSString *)img_security :(NSString *)img_rssi type:(unsigned int)type flag:(BOOL)flag
{

    cell_dev_name.text = name;
    cell_dev_mac.text = mac;
    cell_security.image = [UIImage imageNamed:img_security];
    cell_rssi.image = [UIImage imageNamed:img_rssi];
    
    if(flag){
        cell_dev_name.textColor = [UIColor blueColor];
    }
    else{
        cell_dev_name.textColor = [UIColor blackColor];
    }
    
    return;
}

@end
