/*
 
 File: ViewController.m
 
 Abstract: View Controller to select whether the App runs in Central or
 Peripheral Mode
 
 
 Version: 1.0

 Created by CN3_SD9 on 2016/9/28.
 Copyright © 2016年 Realtek. All rights reserved.
 
 */

#import <NetworkExtension/NetworkExtension.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <CFNetwork/CFNetwork.h>
#import <QuartzCore/QuartzCore.h>
#import "ViewController.h"
#import "TransferService.h"
#import "ClientListCell.h"
#import "bleconfigLibrary.h"
#import "Util.h"

@interface ViewController () <CBCentralManagerDelegate, CBPeripheralDelegate>{
    bleconfigLibrary *handleRequest;
    NSString *lastDevUUID;
    BOOL isCheckDevicePosition;
    int stateNum;
    int ss_section[22];
    struct rtk_btconfig_ss_result ss_result_2G;
    struct rtk_btconfig_ss_result ss_result_5G;
}

@property (strong, nonatomic) CBPeripheral          *discoveredPeripheral;

@end

#define MAC_ADDR_LEN                (6)
#define MAX_INFO_LEN                (64)
#define MAX_BUF_SIZE                (512)

#define tag_table_confirm   2
#define tag_table_ap_list  3
#define tag_table_connect   4

#define tag_timer           10101
#define tag_cfg_confirm     10601
#define tag_cfg_connect     10602
#define tag_cfg_alertView_AP 10603

#define tag_bt_check 10604
#define tag_scan_ap 10605
#define tag_connect_ap 10606
#define tag_scan_bt 10607
#define tag_scan_qr 10608
#define tag_connect_homeap 10609

#define RSSI_Level_1    62
#define RSSI_Level_2    52
#define RSSI_Level_3    35

#define RSSI_POSITION_CLOSE  65//RSSI 65 = 65 - 100 -dBm = -35 dBm
#define RSSI_POSITION_FAR    45//RSSI 45 = 45 - 100 -dBm = -55 dBm

#define READ_GROUP_UNIT     3

#define DEVICE_NAME @"WiFi Client"

#define PRODUCT_NAME1 @"realtek_rpt"
#define PRODUCT_NAME2 @"Ameba"

int g_rssi_high = RSSI_POSITION_CLOSE;
int g_rssi_low = RSSI_POSITION_FAR;

UITextField *g_testField_pin = nil;
NSString *homeAP_BSSID = nil;
NSString *homeAP_SSID = nil;
NSString *homeAP_password = nil;
NSString *defaultPIN = @"12345678";
NSString *PIN_cfg = nil;
NSString *PIN_backdoor = @"00000000";

NSTimer         *waitTimer;
UIAlertView     *connect_alert;
UIAlertView     *waitingAlert;
UIAlertController *alertController_pwd;

UIProgressView  *cfgProgressView;
UITableView     *tableDeviceList;

NSUserDefaults  *BTinfo_DataFile = nil;
NSUserDefaults  *APInfo_DataFile = nil;      //SSID1,password1;SSID,password2;...
NSUserDefaults  *WIFIInfo_DataFile = nil;

NSTimeInterval configTimerStart = 0;
NSTimeInterval configTimerPause = 0;
NSTimeInterval configTimerResume = 0;

int discovery_dev_uuid_index = 0;

NSString *filter_BT_device = DEVICE_NAME;
NSString *discovery_dev_uuid[8] = {0};
NSString *dev_uuid_cfg[8] = {0};
NSString *dev_bt_name[8] = {0};
NSString *selectDevice = @"";

BOOL isShowConnectList = NO;
BOOL can_toConnect    = NO;
BOOL isConnected      = NO;
BOOL can_toSiteSurvey = NO;
BOOL isConfiguring    = NO;
BOOL isWiFiConnecting = NO;
BOOL isHomeAP_exist = NO;
BOOL isHomeAP_check = YES;
BOOL isWiFiList_ready = NO;

struct dev_info new_dev;
struct rtk_btconfig_bss_info targetAp;

Byte product_type = 0;

UIImageView *targetAP_RSSI_Info;
UILabel *targetAP_SSID_Info;
BOOL show_ap_info = YES;
UIAlertController *alert_APInfo;

NSString *imgName_rssi = @"";
NSMutableArray *AP_Profile_queue;
NSString *Confirm_BSSID = @"";
NSString *Confirm_SSID = @"";
BOOL isSecured = NO;
BOOL show_tip = NO;
BOOL show_tip_2 = NO;
BOOL ble_on = NO;

UIImageView *imgViewQRCode;
UILabel *QRCode_tip;
UIAlertController *alert_QRCode;
UIAlertController *alertController_CurrQRcode;

BOOL wlan2GEnabled = NO;
BOOL wlan5GEnabled = NO;

unsigned char HomeAP_Security = 0;
unsigned char HomeAP_Band = 0;

@implementation ViewController
@synthesize discover_table, myConnectTable, dev_array,label_btDevice,img_bt_device_state,btn_return,btn_search,btn_qrcode,btn_setting;
@synthesize target_SSID,target_BSSID,target_security,target_RSSI;

@synthesize centralManager;

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    handleRequest = [bleconfigLibrary alloc];

    [self init_Data];
    [self init_UI];
}

- (void)viewWillDisappear:(BOOL)animated
{
    // Don't keep it going while we're not showing.
    NSLog(@"Scanning stopped");
    [self BT_StopScan];
    
    [super viewWillDisappear:animated];
}

-(void) init_BT
{
    // Start up the CBCentralManager
    //centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil options:@{CBCentralManagerOptionShowPowerAlertKey:@NO}];
    self.discoveredPeripheral = nil;
}

-(void) init_Data
{
    lastDevUUID = @"";
    
    product_type = 0;
    
    /* init scan result list */
    m_devlist = [[NSMutableArray alloc] initWithObjects:self, nil];
    m_APlist = [[NSMutableArray alloc] initWithObjects:self, nil];
    AP_Profile_queue = [[NSMutableArray alloc] init];
    
    APInfo_DataFile = [NSUserDefaults standardUserDefaults];
    BTinfo_DataFile = [NSUserDefaults standardUserDefaults];
    WIFIInfo_DataFile = [NSUserDefaults standardUserDefaults];
    
    [m_APlist removeAllObjects];
    [m_devlist removeAllObjects];
    [AP_Profile_queue removeAllObjects];
    
    isCheckDevicePosition = NO;
    
    tableDeviceList.tag         = tag_table_confirm;
    self.discover_table.tag     = tag_table_ap_list;
    
    self.discover_table.delegate = self;
    self.discover_table.dataSource = self;

    show_ap_info = YES;
    
    AP_Profile_queue = [[NSMutableArray alloc] init];
    if([WIFIInfo_DataFile stringForKey:@"AP1"] != nil){
        [AP_Profile_queue addObject:[WIFIInfo_DataFile stringForKey:@"AP1"]];
    }
    if([WIFIInfo_DataFile stringForKey:@"AP2"] != nil){
        [AP_Profile_queue addObject:[WIFIInfo_DataFile stringForKey:@"AP2"]];
    }
    if([WIFIInfo_DataFile stringForKey:@"AP3"] != nil){
        [AP_Profile_queue addObject:[WIFIInfo_DataFile stringForKey:@"AP3"]];
    }

}

-(void) init_UI
{
    [[UIApplication sharedApplication] setStatusBarStyle:YES animated:YES];
    
    [self reset_target_info];
    
    if ([waitTimer isValid]) {
        [waitTimer invalidate];
        waitTimer = nil;
    }

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)reset_target_info
{
    target_SSID.text = @"";
    target_BSSID.text = @"";
    target_security.image = [UIImage imageNamed:@""];
    target_RSSI.image = [UIImage imageNamed:@""];
    //[self setBTDeviceState:@"disconnect.png"];
}

-(void) show_dev_info: (struct dev_info *)dev
{
    NSLog(@"======Dump dev_info======");
    NSLog(@"MAC: %02x:%02x:%02x:%02x:%02x:%02x", dev->mac[0], dev->mac[1],dev->mac[2],dev->mac[3],dev->mac[4],dev->mac[5]);
    NSLog(@"Status: %d", dev->status);
    NSLog(@"Device type: %d", dev->dev_type);
    NSLog(@"IP:%x", dev->ip);
    NSLog(@"Name:%@", [NSString stringWithCString:(const char *)(dev->extra_info) encoding:NSUTF8StringEncoding]);
    NSLog(@"Require_PIN:%@", (dev->require_pin==1)?@"Yes":@"No");
    NSLog(@"=========================");
}

#pragma mark - Central Methods
- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    switch (central.state){
        case CBCentralManagerStatePoweredOn:
            NSLog(@"CBCentralManagerStatePoweredOn");
            ble_on = YES;
            [self scan_BT_peripheral];
            break;
        case CBCentralManagerStatePoweredOff:
            NSLog(@"CBCentralManagerStatePoweredOff");
            ble_on = NO;
            [self Show_BT_tip];
            break;
        case CBCentralManagerStateResetting:
            NSLog(@"CBCentralManagerStateResetting");
            break;
        case CBCentralManagerStateUnsupported:
            NSLog(@"CBCentralManagerStateUnsupported");
            break;
        case CBCentralManagerStateUnauthorized:
            NSLog(@"CBCentralManagerStateUnauthorized");
            break;
        case CBCentralManagerStateUnknown:
            NSLog(@"CBCentralManagerStateUnknown");
            break;
    }
    
    if (central.state != CBCentralManagerStatePoweredOn) {
        // In a real app, you'd deal with all the states correctly
        return;
    }
}

/** Scan for peripherals - specifically for our service's 128bit CBUUID
 */
- (void)scan_BT_peripheral
{
    [self.centralManager scanForPeripheralsWithServices:nil
                                                options:@{ CBCentralManagerScanOptionAllowDuplicatesKey : @YES }];
    NSLog(@"Scanning started");
}

- (void)disconnectBT
{
    
    if(self.discoveredPeripheral == nil){
        NSLog(@"self.centralManager = NULL");
        return;
    }
    
    if(self.discoveredPeripheral.state == CBPeripheralStateConnected){
        [self.centralManager cancelPeripheralConnection:self.discoveredPeripheral];
        NSLog(@"BT Disconnect!!!");
    }

    [self init_Data];
    
}

/** This callback comes whenever a peripheral that is advertising the TRANSFER_SERVICE_UUID is discovered.
 *  We check the RSSI, to make sure it's close enough that we're interested in it, and if it is,
 *  we start the connection process
 */
- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    BOOL isMyDevice = NO;
    BOOL ret =NO;
    BOOL isNewDevice = YES;
    BOOL isCfgDevice = NO;
    int i = 0;

    NSString *uuid = [peripheral.identifier UUIDString];
    //NSLog(@"\n===================================\nTarget service discovered! RSSI.integerValue : %ld", (long)RSSI.integerValue);
    //NSLog(@"peripheral:%@\n%@\n",peripheral.services,peripheral);
    //NSLog(@"advertisementData\n%@\n",advertisementData);
    //NSLog(@"name:%@(%@) ID:%@ state:%ld",peripheral.name,RSSI,uuid,(long)peripheral.state);
    
    NSString *deviceName = peripheral.name;

    if( deviceName == nil)
        return;
    
    if( deviceName.length == 0)
        return;
    
    if( [deviceName rangeOfString:filter_BT_device].location != NSNotFound )
        isMyDevice = YES;
    if( [deviceName rangeOfString:PRODUCT_NAME1].location != NSNotFound )
        isMyDevice = YES;
    if( [deviceName rangeOfString:PRODUCT_NAME2].location != NSNotFound )
        isMyDevice = YES;
    
    if(isMyDevice==NO)
        return;
    
    if(isShowConnectList==NO){
    isShowConnectList = YES;
    [self stopWaiting];
    configTimerPause = NSDate.date.timeIntervalSince1970;

    NSString *title = @"";
    NSString *devName = @"";
    NSString *devUUID = @"";
    
    [self setToConnectionState];

    devUUID = [NSString stringWithFormat:@"%@", uuid];

    lastDevUUID = devUUID;
    [self setPreviousBTProfile:devUUID];
    devUUID = [devUUID substringToIndex:8];
    devName = [NSString stringWithFormat:@"%@", deviceName];//struct dev_info dev;
    
    [self setLabelText:devName];
    NSLog(@"<APP> to connect!");
    
    configTimerResume = NSDate.date.timeIntervalSince1970;
    
    can_toConnect = YES;
    [connect_alert dismissWithClickedButtonIndex:1 animated:YES];
    title= [NSString stringWithFormat:@"Connecting to %@", deviceName];
    [self startWaiting:@"":title:30:tag_connect_ap];
 
    }
 
    if(can_toConnect==YES){
        //check that it is vaild
        isCfgDevice = [uuid isEqualToString:lastDevUUID];
        if(isCfgDevice==NO)
            return;
    }else{
        return;
    }
    
    // Ok, it's in range - have we already seen it?
    if (self.discoveredPeripheral != peripheral && peripheral.state == CBPeripheralStateDisconnected) {
        
        selectDevice = deviceName;
        
        NSLog(@"toConnect %@ uuid:%@",deviceName,uuid);
        // Save a local copy of the peripheral, so CoreBluetooth doesn't get rid of it
        self.discoveredPeripheral = peripheral;
        
        // And connect
        [self.centralManager connectPeripheral:peripheral options:nil];

        isHomeAP_check = YES;
        isWiFiList_ready = NO;
    }

}
/** If the connection fails for whatever reason, we need to deal with it. */
- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"Failed to connect to %@. (%@)\n", peripheral, [error localizedDescription]);
    can_toSiteSurvey = NO;
    isConnected      = NO;
}

/** We've connected to the peripheral, now we need to discover the services and characteristics to find the 'transfer' characteristic.*/
- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    NSLog(@"Peripheral %@ Connected\n",peripheral.name);
    
    [self setLabelText:peripheral.name];
    
    isConnected      = YES;
    can_toSiteSurvey = YES;
    
    // Make sure we get the discovery callbacks
    peripheral.delegate = self;
    // Search only for services that match our UUID
    [peripheral discoverServices:@[[CBUUID UUIDWithString:BTCONF_SERVICE_UUID]]];

    [self reset_target_info];
    [m_APlist removeAllObjects];
    [discover_table reloadData];

    stateNum = STATE_CAPABILITY;
    
    [self stopWaiting];

    btn_search.hidden = YES;
    btn_qrcode.hidden = YES;
    btn_setting.hidden = YES;
    btn_return.hidden = NO;
    
    show_tip = YES;
    
    [self startWaiting:@"":@"Scanning WiFi AP":30:tag_scan_ap];
}

/** The Transfer Service was discovered
 */
//scan services after connected
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
    if (error) {
        NSLog(@"Error discovering services: %@\n",[error localizedDescription]);
        return;
    }
    
    NSLog(@"Target %u service discovered:%@\n",(unsigned int)(peripheral.services.count),peripheral.services);
    
    // Loop through the newly filled peripheral.services array, just in case there's more than one.
    for (CBService *service in peripheral.services) {
        NSLog(@"Service found with UUID: %@\n",service.UUID);
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:BTCONF_BLECONFIG_UUID]] forService:service];
    }
}

/** The Transfer characteristic was discovered.
 *  Once this has been found, we want to subscribe to it, which lets the peripheral know we want the data it contains
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{

    if (error) {
        NSLog(@"1Error discovering characteristics: %@\n",[error localizedDescription]);
        return;
    }
    
    NSLog(@"===================================Discover chr start");
    // Again, we loop through the array, just in case.
    for (CBCharacteristic *chr in service.characteristics) {

        if(service.UUID == NULL || chr.UUID == NULL)
            continue;

        NSLog(@"Service UUID %@ read characteristics %@", service.UUID,chr.UUID.UUIDString);
        
        if([chr.UUID.UUIDString isEqualToString:BTCONF_BLECONFIG_UUID]){
            [peripheral readValueForCharacteristic:chr];
        }

    }
    NSLog(@"===================================Discover chr end");
}


/** This callback lets us know more data has arrived via notification on the characteristic
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    BOOL ss_isOver = NO;
    int i=0;
    int len=0;
    int ss_sectionNum=0;
    int ss_max_section = 0;
    uint8_t val[MAX_BUF_SIZE] = {0};


    NSData *valData = [NSData dataWithBytes:(void*)&val length:sizeof(val)];
    
    if (error) {
        NSLog(@"2Error(%@) discovering characteristics: %@\n",characteristic.UUID.UUIDString,[error localizedDescription]);
        return;
    }
    
    const uint8_t *bytes = (const uint8_t*)[characteristic.value bytes];
    NSInteger totalData = [characteristic.value length] / sizeof(uint8_t);
    //NSLog(@" >>>>>>>> characteristic(%ld) \n",[characteristic.value length]);
  
    if(totalData==0 || totalData==1 ){
        len =[handleRequest gen_cmd_capability:val];
        valData = [NSData dataWithBytes:(const void *)val length:len];
        [peripheral writeValue:valData forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
        [peripheral readValueForCharacteristic:characteristic];
        stateNum = STATE_CAPABILITY;
    }else{ //recv length > 1
        
        i = 0 ;

        if(stateNum==STATE_CAPABILITY){// get capability
            if( !([handleRequest is_cmd_capability:bytes]) ){
                len =[handleRequest gen_cmd_capability:val];
                valData = [NSData dataWithBytes:(const void *)val length:len];
                [peripheral writeValue:valData forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
                [peripheral readValueForCharacteristic:characteristic];
            }else{// it is capability cmd
                
                wlan2GEnabled = [handleRequest is_cmd_support_2G:bytes];
                wlan5GEnabled = [handleRequest is_cmd_support_5G:bytes];

                //NSLog(@"wlan2GEnabled: %d",wlan2GEnabled);
                //NSLog(@"wlan5GEnabled: %d",wlan5GEnabled);
                
                if(wlan2GEnabled == YES && wlan5GEnabled == NO){
                    NSLog(@"Support 2.4G Capability Only");
                    stateNum = STATE_SCAN_2G;
                    memset(&ss_result_2G,0,sizeof(ss_result_2G));
                    
                    len =[handleRequest gen_cmd_sitesurvey_2G:val];

                    valData = [NSData dataWithBytes:(const void *)val length:len];
                    [peripheral writeValue:valData forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
                    
                    memset(ss_section,0,sizeof(ss_section));
                    [peripheral readValueForCharacteristic:characteristic];
                }
                else if(wlan5GEnabled == YES && wlan2GEnabled == YES){
                    NSLog(@"Support Dual Band Capability");
                    stateNum = STATE_SCAN_2G;
                    memset(&ss_result_2G,0,sizeof(ss_result_2G));
                    
                    len =[handleRequest gen_cmd_sitesurvey_2G:val];
                    
                    valData = [NSData dataWithBytes:(const void *)val length:len];
                    [peripheral writeValue:valData forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
                    
                    memset(ss_section,0,sizeof(ss_section));
                    [peripheral readValueForCharacteristic:characteristic];
      
                }
                else if(wlan5GEnabled == YES && wlan2GEnabled == NO){
                    NSLog(@"Support 5G Capability Only");
                    stateNum = STATE_SCAN_5G;
                    memset(&ss_result_5G,0,sizeof(ss_result_5G));
                    
                    len =[handleRequest gen_cmd_sitesurvey_5G:val];
                    valData = [NSData dataWithBytes:(const void *)val length:len];
                    [peripheral writeValue:valData forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
                    
                    memset(ss_section,0,sizeof(ss_section));
                    [peripheral readValueForCharacteristic:characteristic];
                }
                
                product_type = [handleRequest get_product_type:bytes];
                //NSLog(@"product_type:%d",product_type);
                
            }
        }else if(stateNum == STATE_SCAN_5G){//site survey 5G
            
            NSString *homeAP_BSSID = [Util fetchCurrBSSID];
            homeAP_BSSID = [Util standardFormateMAC:homeAP_BSSID];
            
            if([handleRequest is_cmd_ss_5G:bytes]){
                
                ss_max_section = RTK_BTCONFIG_MAX_BSS_NUM/READ_GROUP_UNIT;
                if(RTK_BTCONFIG_MAX_BSS_NUM%READ_GROUP_UNIT!=0)ss_max_section++;
                
                ss_sectionNum = bytes[6]/READ_GROUP_UNIT;
                if(bytes[6]%READ_GROUP_UNIT)ss_sectionNum++;
                ss_section[ss_sectionNum-1] = 1;
                //check it if finish
                ss_isOver = YES;
                for(i=0;i<ss_max_section;i++){
                    if(ss_section[i]==0)
                        ss_isOver=NO;
                }
                if( ss_isOver==NO && ss_sectionNum>0 && ss_sectionNum<=ss_max_section ){
                    [peripheral readValueForCharacteristic:characteristic];

                    [handleRequest setSiteSurveyResult :bytes:&(ss_result_5G):ss_sectionNum:READ_GROUP_UNIT];
                    
                    for(int i=0;i<3;i++){
                        
                        char bssid_tmp[32] = {0};
                        [Util mac2str: (char *)ss_result_5G.bss_info[(ss_sectionNum-1)*3+i].bdBssId :bssid_tmp];
                        NSString* scan_BSSID = [NSString stringWithCString:bssid_tmp encoding:NSASCIIStringEncoding];
                        
                        if([scan_BSSID isEqualToString:homeAP_BSSID]){
                            
                            if(isHomeAP_check == YES){
                                isHomeAP_exist = YES;
                                isHomeAP_check = NO;
                                discover_table.hidden = YES;
                                [self stopWaiting];
                                memset(&targetAp,0,sizeof(targetAp));
                                memcpy(&targetAp,&ss_result_5G.bss_info[(ss_sectionNum-1)*3+i],sizeof(targetAp));
                                [self alertView_checkPassword_HomeAP :ss_result_5G.bss_info[(ss_sectionNum-1)*3+i].authAlg];
                            }
                        }
                    }

                    
                }else if(ss_isOver==YES){
                    
                    isWiFiList_ready = YES;
                    
                    discover_table.hidden = NO;
                    
                    char ssid[32] = {0};
                    int macTotalNum = 0;

                    for(i=0;i<RTK_BTCONFIG_MAX_BSS_NUM;i++){
                        memcpy(ssid,ss_result_5G.bss_info[i].bdSsIdBuf,sizeof(ssid));
                        
                        macTotalNum = ss_result_5G.bss_info[i].bdBssId[0]+
                        ss_result_5G.bss_info[i].bdBssId[1]+
                        ss_result_5G.bss_info[i].bdBssId[2]+
                        ss_result_5G.bss_info[i].bdBssId[3]+
                        ss_result_5G.bss_info[i].bdBssId[4]+
                        ss_result_5G.bss_info[i].bdBssId[5];
                        
                        if(ss_result_5G.bss_info[i].rssi>0 && macTotalNum!=0 && ss_result_5G.bss_info[i].ChannelNumber>0){
                            
                            struct rtk_btconfig_bss_info bssInfo_test;
                            int j;
                            BOOL found = false;
                            NSValue *dev_val;
                            
                            for(j=0;j<m_APlist.count;j++){
                                dev_val = [m_APlist objectAtIndex:j];
                                [dev_val getValue:&bssInfo_test];
                                if((bssInfo_test.bdBssId[0] == ss_result_5G.bss_info[i].bdBssId[0])&&(bssInfo_test.bdBssId[1] == ss_result_5G.bss_info[i].bdBssId[1])&&(bssInfo_test.bdBssId[2] == ss_result_5G.bss_info[i].bdBssId[2])&&(bssInfo_test.bdBssId[3] == ss_result_5G.bss_info[i].bdBssId[3])&&(bssInfo_test.bdBssId[4] == ss_result_5G.bss_info[i].bdBssId[4])&&(bssInfo_test.bdBssId[5] == ss_result_5G.bss_info[i].bdBssId[5])){
                                    
                                    found = true;
                                    
                                    // update TTL and RSSI for existed AP
                                    bssInfo_test.TTL = 5;
                                    bssInfo_test.rssi = ss_result_5G.bss_info[i].rssi;

                                    [m_APlist replaceObjectAtIndex:j withObject:[NSValue valueWithBytes:&bssInfo_test objCType:@encode(struct rtk_btconfig_bss_info)]];
                                }
                            }
                            
                            if(found == false){
                                ss_result_5G.bss_info[i].TTL = 5;
                                [m_APlist addObject:[NSValue valueWithBytes:&ss_result_5G.bss_info[i] objCType:@encode(struct rtk_btconfig_bss_info)]];
                            }
                        }
                    }
                    
                    struct rtk_btconfig_bss_info bssInfo_test;
                    int j;
                    NSValue *dev_val;
                    
                    for(j=0;j<m_APlist.count;j++){
                        dev_val = [m_APlist objectAtIndex:j];
                        [dev_val getValue:&bssInfo_test];
                        bssInfo_test.TTL--;
                        [m_APlist replaceObjectAtIndex:j withObject:[NSValue valueWithBytes:&bssInfo_test objCType:@encode(struct rtk_btconfig_bss_info)]];
                        
                        if(bssInfo_test.TTL == 0){
                            [m_APlist removeObjectAtIndex:j];
                        }
                    }
                    
                    [self sortAP_byRSSI];
                    
                    NSValue *dev_val_tmp;
                    struct rtk_btconfig_bss_info bssInfo_test_tmp;
                    char ssid_tmp[32] = {0};
                    
                    //check dulicated SSID AP (remove low RSSI repeated SSID)
                    for(j=0;j<m_APlist.count;j++){
                        dev_val = [m_APlist objectAtIndex:j];
                        [dev_val getValue:&bssInfo_test];
                        memcpy(ssid,bssInfo_test.bdSsIdBuf,sizeof(ssid));
                        if(strlen(ssid) != 0){ //hidden SSID
                            for(i=j+1;i<m_APlist.count;i++){
                                dev_val_tmp = [m_APlist objectAtIndex:i];
                                [dev_val_tmp getValue:&bssInfo_test_tmp];
                                memcpy(ssid_tmp,bssInfo_test_tmp.bdSsIdBuf,sizeof(ssid_tmp));
                                if(strcmp(ssid, ssid_tmp) == 0){
                                    [m_APlist removeObjectAtIndex:i];
                                }
                            }
                        }
                    }

                    for(j=0;j<m_APlist.count;j++){
                        dev_val = [m_APlist objectAtIndex:j];
                        [dev_val getValue:&bssInfo_test];
                        memcpy(ssid,bssInfo_test.bdSsIdBuf,sizeof(ssid));
                    }

                    if(isHomeAP_exist==NO){
                        discover_table.hidden = NO;
                        [discover_table reloadData];
                        
                        if(show_tip_2){
                            [self Show_tip_msg:@"Keep Searching and updating WiFi AP"];
                            show_tip_2 = NO;
                        }
                        
                        if(show_tip){
                            [self Show_tip_msg:@"Please choose target AP to connect"];
                            show_tip = NO;
                            show_tip_2 = YES;
                        }
                    }
                    else{
                        discover_table.hidden = YES;
                        [discover_table reloadData];
                    }
                    
                    [self stopWaiting];
                    
                    [peripheral readValueForCharacteristic:characteristic];
                    
                    stateNum = STATE_CAPABILITY;
                }
            }
            
        }else if(stateNum == STATE_SCAN_2G){//site survey 2.4G
            
            NSString *homeAP_BSSID = [Util fetchCurrBSSID];
            homeAP_BSSID = [Util standardFormateMAC:homeAP_BSSID];

            ss_max_section = RTK_BTCONFIG_MAX_BSS_NUM/READ_GROUP_UNIT;
            if(RTK_BTCONFIG_MAX_BSS_NUM%READ_GROUP_UNIT!=0)ss_max_section++;
            
            ss_sectionNum = bytes[6]/READ_GROUP_UNIT;
            if(bytes[6]%READ_GROUP_UNIT)ss_sectionNum++;
            
            ss_section[ss_sectionNum-1] = 1;
            //check it if finish
            ss_isOver = YES;
            for(i=0;i<ss_max_section;i++){
                if(ss_section[i]==0)
                    ss_isOver=NO;
            }
            if( ss_isOver==NO && ss_sectionNum>0 && ss_sectionNum<=ss_max_section ){
                [peripheral readValueForCharacteristic:characteristic];
                
                [handleRequest setSiteSurveyResult :bytes:&(ss_result_2G):ss_sectionNum:READ_GROUP_UNIT];
                
                for(int i=0;i<3;i++){
                    char bssid_tmp[32] = {0};
                    [Util mac2str: (char *)ss_result_2G.bss_info[(ss_sectionNum-1)*3+i].bdBssId :bssid_tmp];
                    NSString* scan_BSSID = [NSString stringWithCString:bssid_tmp encoding:NSASCIIStringEncoding];
                    
                    if([scan_BSSID isEqualToString:homeAP_BSSID]){
                        
                        if(isHomeAP_check == YES){
                            isHomeAP_exist = YES;
                            isHomeAP_check = NO;
                            discover_table.hidden = YES;
                            [self stopWaiting];
                            memset(&targetAp,0,sizeof(targetAp));
                            memcpy(&targetAp,&ss_result_2G.bss_info[(ss_sectionNum-1)*3+i],sizeof(targetAp));
                            [self alertView_checkPassword_HomeAP :ss_result_2G.bss_info[(ss_sectionNum-1)*3+i].authAlg];
                        }
                    }
                }
            }
            
            if(ss_isOver==YES){

                isWiFiList_ready = YES;
                
                discover_table.hidden = NO;
                
                char ssid[32] = {0};
                int macTotalNum = 0;

                for(i=0;i<RTK_BTCONFIG_MAX_BSS_NUM;i++){
                    memcpy(ssid,ss_result_2G.bss_info[i].bdSsIdBuf,sizeof(ssid));
                   
                    macTotalNum = ss_result_2G.bss_info[i].bdBssId[0]+
                    ss_result_2G.bss_info[i].bdBssId[1]+
                    ss_result_2G.bss_info[i].bdBssId[2]+
                    ss_result_2G.bss_info[i].bdBssId[3]+
                    ss_result_2G.bss_info[i].bdBssId[4]+
                    ss_result_2G.bss_info[i].bdBssId[5];
                    
                    if(ss_result_2G.bss_info[i].rssi>0 && macTotalNum!=0 && ss_result_2G.bss_info[i].ChannelNumber>0){
                        
                        struct rtk_btconfig_bss_info bssInfo_test;
                        int j;
                        BOOL found = false;
                        NSValue *dev_val;
                        
                        for(j=0;j<m_APlist.count;j++){
                            dev_val = [m_APlist objectAtIndex:j];
                            [dev_val getValue:&bssInfo_test];
                            if((bssInfo_test.bdBssId[0] == ss_result_2G.bss_info[i].bdBssId[0])&&(bssInfo_test.bdBssId[1] == ss_result_2G.bss_info[i].bdBssId[1])&&(bssInfo_test.bdBssId[2] == ss_result_2G.bss_info[i].bdBssId[2])&&(bssInfo_test.bdBssId[3] == ss_result_2G.bss_info[i].bdBssId[3])&&(bssInfo_test.bdBssId[4] == ss_result_2G.bss_info[i].bdBssId[4])&&(bssInfo_test.bdBssId[5] == ss_result_2G.bss_info[i].bdBssId[5])){
                                
                                found = true;
                                
                                // update TTL and RSSI for existed AP
                                bssInfo_test.TTL = 5;
                                bssInfo_test.rssi = ss_result_2G.bss_info[i].rssi;
                                
                                [m_APlist replaceObjectAtIndex:j withObject:[NSValue valueWithBytes:&bssInfo_test objCType:@encode(struct rtk_btconfig_bss_info)]];
                            }
                        }
                        
                        if(found == false){
                            ss_result_2G.bss_info[i].TTL = 5;
                            [m_APlist addObject:[NSValue valueWithBytes:&ss_result_2G.bss_info[i] objCType:@encode(struct rtk_btconfig_bss_info)]];
                        }
                    }
                }
                
                struct rtk_btconfig_bss_info bssInfo_test;
                int j;
                NSValue *dev_val;
                
                for(j=0;j<m_APlist.count;j++){
                    dev_val = [m_APlist objectAtIndex:j];
                    [dev_val getValue:&bssInfo_test];
                    bssInfo_test.TTL--;
                    [m_APlist replaceObjectAtIndex:j withObject:[NSValue valueWithBytes:&bssInfo_test objCType:@encode(struct rtk_btconfig_bss_info)]];
                    
                    if(bssInfo_test.TTL == 0){
                        [m_APlist removeObjectAtIndex:j];
                    }
                }
                
                [self sortAP_byRSSI];
                
                NSValue *dev_val_tmp;
                struct rtk_btconfig_bss_info bssInfo_test_tmp;
                char ssid_tmp[32] = {0};
                
                //check dulicated SSID AP (remove low RSSI repeated SSID)
                for(j=0;j<m_APlist.count;j++){
                    dev_val = [m_APlist objectAtIndex:j];
                    [dev_val getValue:&bssInfo_test];
                    memcpy(ssid,bssInfo_test.bdSsIdBuf,sizeof(ssid));
                    if(strlen(ssid) != 0){ //hidden SSID
                        for(i=j+1;i<m_APlist.count;i++){
                            dev_val_tmp = [m_APlist objectAtIndex:i];
                            [dev_val_tmp getValue:&bssInfo_test_tmp];
                            memcpy(ssid_tmp,bssInfo_test_tmp.bdSsIdBuf,sizeof(ssid_tmp));
                            if(strcmp(ssid, ssid_tmp) == 0){
                                [m_APlist removeObjectAtIndex:i];
                            }
                        }
                    }
                }
                
                for(j=0;j<m_APlist.count;j++){
                    dev_val = [m_APlist objectAtIndex:j];
                    [dev_val getValue:&bssInfo_test];
                    memcpy(ssid,bssInfo_test.bdSsIdBuf,sizeof(ssid));
                    
                }

                if(isHomeAP_exist==NO){
                    discover_table.hidden = NO;
                    [discover_table reloadData];
                    
                    if(show_tip_2){
                        [self Show_tip_msg:@"Keep Searching and updating WiFi AP"];
                        show_tip_2 = NO;
                    }
                    
                    if(show_tip){
                        [self Show_tip_msg:@"Please choose target AP to connect"];
                        show_tip = NO;
                        show_tip_2 = YES;
                    }
                }
                else{
                    discover_table.hidden = YES;
                    [discover_table reloadData];
                }
                
                [self stopWaiting];

                if(wlan5GEnabled == NO){
                    stateNum = STATE_CAPABILITY;
                }else{
                    stateNum = STATE_SCAN_25G;
                    memset(&ss_result_2G,0,sizeof(ss_result_2G));
                    
                    len =[handleRequest gen_cmd_sitesurvey_5G:val];
                    valData = [NSData dataWithBytes:(const void *)val length:len];
                    [peripheral writeValue:valData forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
                    
                    memset(ss_section,0,sizeof(ss_section));
                    [peripheral readValueForCharacteristic:characteristic];
                }
            //}
            }
        }else if(stateNum == STATE_SCAN_25G){
          
            NSString *homeAP_BSSID = [Util fetchCurrBSSID];
            homeAP_BSSID = [Util standardFormateMAC:homeAP_BSSID];

            ss_max_section = RTK_BTCONFIG_MAX_BSS_NUM/READ_GROUP_UNIT;
            if(RTK_BTCONFIG_MAX_BSS_NUM%READ_GROUP_UNIT!=0)ss_max_section++;
            
            ss_sectionNum = bytes[6]/READ_GROUP_UNIT;
            if(bytes[6]%READ_GROUP_UNIT)ss_sectionNum++;

            ss_section[ss_sectionNum-1] = 1;
            //check it if finish
            ss_isOver = YES;
            for(i=0;i<ss_max_section;i++){
                if(ss_section[i]==0)
                    ss_isOver=NO;
            }
            if( ss_isOver==NO && ss_sectionNum>0 && ss_sectionNum<=ss_max_section ){
                [peripheral readValueForCharacteristic:characteristic];
                
                [handleRequest setSiteSurveyResult :bytes:&(ss_result_2G):ss_sectionNum:READ_GROUP_UNIT];
                
                for(int i=0;i<3;i++){
                 
                    char bssid_tmp[32] = {0};
                    [Util mac2str: (char *)ss_result_2G.bss_info[(ss_sectionNum-1)*3+i].bdBssId :bssid_tmp];
                    NSString* scan_BSSID = [NSString stringWithCString:bssid_tmp encoding:NSASCIIStringEncoding];

                    if([scan_BSSID isEqualToString:homeAP_BSSID]){

                        if(isHomeAP_check == YES){
                            isHomeAP_exist = YES;
                            isHomeAP_check = NO;
                            discover_table.hidden = YES;
                            [self stopWaiting];
                            memset(&targetAp,0,sizeof(targetAp));
                            memcpy(&targetAp,&ss_result_2G.bss_info[(ss_sectionNum-1)*3+i],sizeof(targetAp));
                            [self alertView_checkPassword_HomeAP :ss_result_2G.bss_info[(ss_sectionNum-1)*3+i].authAlg];
                        }
                    }
                }
            }
            
            if(ss_isOver==YES){

                isWiFiList_ready = YES;
                
                discover_table.hidden = NO;
                
                char ssid[32] = {0};
                int macTotalNum = 0;

                for(i=0;i<RTK_BTCONFIG_MAX_BSS_NUM;i++){
                    memcpy(ssid,ss_result_2G.bss_info[i].bdSsIdBuf,sizeof(ssid));
                    
                    macTotalNum = ss_result_2G.bss_info[i].bdBssId[0]+
                    ss_result_2G.bss_info[i].bdBssId[1]+
                    ss_result_2G.bss_info[i].bdBssId[2]+
                    ss_result_2G.bss_info[i].bdBssId[3]+
                    ss_result_2G.bss_info[i].bdBssId[4]+
                    ss_result_2G.bss_info[i].bdBssId[5];
                    
                    if(ss_result_2G.bss_info[i].rssi>0 && macTotalNum!=0 && ss_result_2G.bss_info[i].ChannelNumber>0){
                        
                        struct rtk_btconfig_bss_info bssInfo_test;
                        int j;
                        BOOL found = false;
                        NSValue *dev_val;
                        
                        for(j=0;j<m_APlist.count;j++){
                            dev_val = [m_APlist objectAtIndex:j];
                            [dev_val getValue:&bssInfo_test];
                            if((bssInfo_test.bdBssId[0] == ss_result_2G.bss_info[i].bdBssId[0])&&(bssInfo_test.bdBssId[1] == ss_result_2G.bss_info[i].bdBssId[1])&&(bssInfo_test.bdBssId[2] == ss_result_2G.bss_info[i].bdBssId[2])&&(bssInfo_test.bdBssId[3] == ss_result_2G.bss_info[i].bdBssId[3])&&(bssInfo_test.bdBssId[4] == ss_result_2G.bss_info[i].bdBssId[4])&&(bssInfo_test.bdBssId[5] == ss_result_2G.bss_info[i].bdBssId[5])){
                                
                                found = true;
                                
                                // update TTL and RSSI for existed AP
                                bssInfo_test.TTL = 5;
                                bssInfo_test.rssi = ss_result_2G.bss_info[i].rssi;
                                
                                [m_APlist replaceObjectAtIndex:j withObject:[NSValue valueWithBytes:&bssInfo_test objCType:@encode(struct rtk_btconfig_bss_info)]];
                            }
                        }
                        
                        if(found == false){
                            ss_result_2G.bss_info[i].TTL = 5;
                            [m_APlist addObject:[NSValue valueWithBytes:&ss_result_2G.bss_info[i] objCType:@encode(struct rtk_btconfig_bss_info)]];
                        }
                    }
                }
                
                struct rtk_btconfig_bss_info bssInfo_test;
                int j;
                NSValue *dev_val;
                
                for(j=0;j<m_APlist.count;j++){
                    dev_val = [m_APlist objectAtIndex:j];
                    [dev_val getValue:&bssInfo_test];
                    bssInfo_test.TTL--;
                    [m_APlist replaceObjectAtIndex:j withObject:[NSValue valueWithBytes:&bssInfo_test objCType:@encode(struct rtk_btconfig_bss_info)]];
                    
                    if(bssInfo_test.TTL == 0){
                        [m_APlist removeObjectAtIndex:j];
                    }
                }
                
                [self sortAP_byRSSI];
                
                NSValue *dev_val_tmp;
                struct rtk_btconfig_bss_info bssInfo_test_tmp;
                char ssid_tmp[32] = {0};
                
                //check dulicated SSID AP (remove low RSSI repeated SSID)
                for(j=0;j<m_APlist.count;j++){
                    dev_val = [m_APlist objectAtIndex:j];
                    [dev_val getValue:&bssInfo_test];
                    memcpy(ssid,bssInfo_test.bdSsIdBuf,sizeof(ssid));
                    if(strlen(ssid) != 0){ //hidden SSID
                        for(i=j+1;i<m_APlist.count;i++){
                            dev_val_tmp = [m_APlist objectAtIndex:i];
                            [dev_val_tmp getValue:&bssInfo_test_tmp];
                            memcpy(ssid_tmp,bssInfo_test_tmp.bdSsIdBuf,sizeof(ssid_tmp));
                            if(strcmp(ssid, ssid_tmp) == 0){
                                [m_APlist removeObjectAtIndex:i];
                            }
                        }
                    }
                }
                
                for(j=0;j<m_APlist.count;j++){
                    dev_val = [m_APlist objectAtIndex:j];
                    [dev_val getValue:&bssInfo_test];
                    memcpy(ssid,bssInfo_test.bdSsIdBuf,sizeof(ssid));
                }

                if(isHomeAP_exist==NO){
                    discover_table.hidden = NO;
                    [discover_table reloadData];
                }
                else{
                    discover_table.hidden = YES;
                    [discover_table reloadData];
                }
                
                [self stopWaiting];
                
                [peripheral readValueForCharacteristic:characteristic];
                
                stateNum = STATE_CAPABILITY;
                
            }
           
        }else if(stateNum == STATE_CONNECTION){
            if( !([handleRequest is_cmd_connection:bytes]) ){

                NSString *m_password = homeAP_password;
                const char *str_password = [m_password cStringUsingEncoding:NSUTF8StringEncoding];
                len = [handleRequest gen_cmd_connection_request:val :targetAp.ChannelNumber :targetAp.authAlg :targetAp.bdSsIdBuf :targetAp.bdBssId :(uint8_t*)str_password :(int)m_password.length];

                valData = [NSData dataWithBytes:(const void *)val length:len];
                [peripheral writeValue:valData forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];

                sleep(5);

                [peripheral readValueForCharacteristic:characteristic];

            }else{

                //NEXT STEP: GET STATUS
                NSDictionary * wrapper = [NSDictionary dictionaryWithObjectsAndKeys:peripheral,@"obj1",characteristic,@"obj2", nil];
                [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(btcfg_cmd_getStatus:) userInfo:wrapper repeats:NO];

                stateNum = STATE_CONNECTION_STATUS;
            }
            
        }else if(stateNum == STATE_CONNECTION_STATUS){
            
            // prevent keyboard pop-up
            [[UIApplication sharedApplication] sendAction:@selector(resignFirstResponder) to:nil from:nil forEvent:nil];

            if( !([handleRequest is_cmd_status:bytes]) ){

                if([handleRequest getDevice_connectedStatus:bytes]==STATE_WRONG_PASSWORD){
                    if(isWiFiConnecting){
                        isWiFiConnecting = NO;
                        [self stopWaiting];

                        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Warning" message:@"The password is wrong!" preferredStyle:UIAlertControllerStyleAlert];
                        
                        UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action){

                            [alertController_pwd dismissViewControllerAnimated:YES completion:nil];

                            stateNum = STATE_CAPABILITY;
                            discover_table.hidden = NO;
                            show_ap_info = YES;
                            show_tip = YES;
                            isHomeAP_exist = NO;

                            if(isWiFiList_ready==NO){
                                [self startWaiting:@"":@"Scanning WiFi AP":30:tag_scan_ap];
                            }

                        }];
                        
                        [alert addAction:okAction];
                        [self presentViewController:alert animated:YES completion:nil];
                    }

                }
                else if([handleRequest getDevice_connectedStatus:bytes]==STATE_IDLE){
                    if(isWiFiConnecting){
                        isWiFiConnecting = NO;
                        [self stopWaiting];

                        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Warning" message:@"Connection Fail!" preferredStyle:UIAlertControllerStyleAlert];
                        
                        UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action){

                            [alertController_pwd dismissViewControllerAnimated:YES completion:nil];

                            stateNum = STATE_CAPABILITY;
                            discover_table.hidden = NO;
                            show_ap_info = YES;
                            show_tip = YES;
                            isHomeAP_exist = NO;
                            
                            if(isWiFiList_ready==NO){
                                [self startWaiting:@"":@"Scanning WiFi AP":30:tag_scan_ap];
                            }

                        }];
                        
                        [alert addAction:okAction];
                        [self presentViewController:alert animated:YES completion:nil];
                    }
                }
                
                NSDictionary * wrapper = [NSDictionary dictionaryWithObjectsAndKeys:peripheral,@"obj1",characteristic,@"obj2", nil];
                [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(btcfg_cmd_getStatus:) userInfo:wrapper repeats:NO];
                
            }else{

                isWiFiConnecting = NO;
                //configuration success
                NSLog(@"configuration success:%ld",(long)totalData);

                discover_table.hidden = YES;

                if(isConfiguring){
                    memset(&new_dev,0,sizeof(new_dev));

                    if([handleRequest dump_device_information:bytes :&new_dev]){
                        NSLog(@"got Device.");
                        [m_devlist addObject:[NSValue valueWithBytes:&new_dev objCType:@encode(struct dev_info)]];
                    }
                    
                    isConfiguring = NO;
                }
                
                isCheckDevicePosition = YES;
                
                stateNum = STATE_CFG_IDLE;
                [peripheral readValueForCharacteristic:characteristic];
            }

        }else{
            Confirm_BSSID = @"";
            Confirm_BSSID = [handleRequest getDevice_connectedBSSID:bytes];

            if(show_ap_info){
                [self Show_APInfo];
                show_ap_info = NO;
            }

            NSDictionary * wrapper = [NSDictionary dictionaryWithObjectsAndKeys:peripheral,@"obj1",characteristic,@"obj2", nil];
            [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(btcfg_cmd_getStatus:) userInfo:wrapper repeats:NO];
            
            if([handleRequest getDevice_connectedStatus:bytes]==STATE_CONNECTED){
                [self setBTDeviceState:nil];
            }else{
                [self setBTDeviceState:@"disconnect.png"];
            }
            
            if(isCheckDevicePosition){
                
                uint8_t rssiValue = 0;
                BOOL isEncrypt = NO;
                
                [self stopWaiting];
                isCheckDevicePosition = NO;
                
                isEncrypt = [handleRequest isDevice_connectedEncrypt:bytes];
                rssiValue = [handleRequest getDevice_connectedRSSI:bytes];
                if(rssiValue>=RSSI_Level_1){
                    imgName_rssi = @"strength_grade4.png";
                }else if( rssiValue<RSSI_Level_1 && rssiValue>=RSSI_Level_2){
                    imgName_rssi = @"strength_grade3.png";
                }else if( rssiValue<RSSI_Level_2 && rssiValue>=RSSI_Level_3){
                    imgName_rssi = @"strength_grade2.png";
                }else{
                    imgName_rssi = @"strength_grade1.png";
                }
                
                target_SSID.text = [handleRequest getDevice_connectedSSID:bytes];
                target_BSSID.text = [handleRequest getDevice_connectedBSSID:bytes];
                target_security.image = [UIImage imageNamed:isEncrypt ? @"encrypt_lock.png":@"encrypt_open.png"];
                target_RSSI.image = [UIImage imageNamed:imgName_rssi];

                if(product_type==1){//Ameba
                    [self alertView_showPosition_dev1:[handleRequest getDevice_connectedRSSI:bytes]];
                }else{
                    [self alertView_showPosition:[handleRequest getDevice_connectedRSSI:bytes]];
                }
                
            }

            uint8_t rssiValue = [handleRequest getDevice_connectedRSSI:bytes];
            if(rssiValue>=RSSI_Level_1){
                imgName_rssi = @"strength_grade4.png";
            }else if( rssiValue<RSSI_Level_1 && rssiValue>=RSSI_Level_2){
                imgName_rssi = @"strength_grade3.png";
            }else if( rssiValue<RSSI_Level_2 && rssiValue>=RSSI_Level_3){
                imgName_rssi = @"strength_grade2.png";
            }else{
                imgName_rssi = @"strength_grade1.png";
            }

            if([handleRequest getDevice_connectedStatus:bytes]!=STATE_CONNECTED){
                NSLog(@"configuration success:    disconnect!!");
                targetAP_RSSI_Info.image = [UIImage imageNamed:@"disconnect.png"];
            }
            else{
                NSLog(@"configuration success:   connected!!");
                targetAP_RSSI_Info.image = [UIImage imageNamed:imgName_rssi];
            }
            
            targetAP_SSID_Info.text = [handleRequest getDevice_connectedSSID:bytes];
            
            NSString *complete = [NSString stringWithFormat:@"%@%@", @"Name:  ",targetAP_SSID_Info.text];
            targetAP_SSID_Info.text = complete;

            [alert_APInfo.view addSubview:targetAP_RSSI_Info];
            [alert_APInfo.view addSubview:targetAP_SSID_Info];
            
        }
    }

}

- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (error) {
        NSLog(@"3Error(%@) discovering characteristics: %@\n",characteristic.UUID.UUIDString,[error localizedDescription]);
        return;
    }
}

/** The peripheral letting us know whether our subscribe/unsubscribe happened or not
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    
    NSLog(@">>>>Notification began on %@", characteristic);
    if (error) {
        NSLog(@"Error changing notification state: %@\n",[error localizedDescription]);
    }
    
    // Notification has started
    if (characteristic.isNotifying) {
        NSLog(@"Notification began on %@", characteristic);
    }
    
    // Notification has stopped
    else {
        // so disconnect from the peripheral
        NSLog(@"Notification stopped on %@.  Disconnecting", characteristic);
        [self.centralManager cancelPeripheralConnection:peripheral];
    }
        
    NSString *stringFromData = [[NSString alloc] initWithData:characteristic.value encoding:NSUTF8StringEncoding];
    NSLog(@"\n>>>>>>>>>>>>Received:%@",stringFromData);
}

/** Once the disconnection happens, we need to clean up our local copy of the peripheral
 */
- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"Peripheral Disconnected to %@. (%@)\n", peripheral, [error localizedDescription]);

    [self setBTDeviceState:nil];
    
    stateNum = STATE_CFG_IDLE;
    
    [m_APlist removeAllObjects];
    [discover_table reloadData];
    
    isConfiguring = YES;
    [m_devlist removeAllObjects];

    isConnected = NO;
    
    self.discoveredPeripheral = nil;

    [alertController_pwd dismissViewControllerAnimated:YES completion:nil];
    [alert_APInfo dismissViewControllerAnimated:YES completion:nil];
    [self stopWaiting];

    btn_search.hidden = NO;
    btn_qrcode.hidden = NO;
    btn_setting.hidden = NO;
    btn_return.hidden = YES;

}

- (IBAction)version_btnClick:(id)sender
{
    NSString *version = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"];
    NSString *verMsg = [NSString stringWithFormat:@"Version : %@", version];
    
    NSLog(@"version_btnClick:%@",verMsg);
    
    //show about
    UIAlertView *aboutShow = [[UIAlertView alloc]
                              initWithTitle:NSLocalizedString(@"About WiFiConfig", @"AlertView")
                              message:NSLocalizedString(verMsg, @"AlertView")
                              delegate:self
                              cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                              otherButtonTitles:nil];
    [aboutShow show];
}


- (IBAction)Info_btnClick:(id)sender
{

    int rssi_dbm_high = g_rssi_high - 100;
    int rssi_dbm_low = g_rssi_low - 100;
    
    NSString *msg = @"";
    NSString *rssi_high = [NSString stringWithFormat:@"default RSSI(high) %d dBm", rssi_dbm_high];
    NSString *rssi_low = [NSString stringWithFormat: @"default RSSI(low) %d dBm", rssi_dbm_low];

    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"RSSI Configuration\n &\n Clear Connection Record" message:nil preferredStyle:UIAlertControllerStyleAlert];
    [alert addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"RSSI(high)";
        textField.placeholder = rssi_high;
        textField.clearButtonMode = UITextFieldViewModeWhileEditing;
    }];
    [alert addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"RSSI(low)";
        textField.placeholder = rssi_low;
        textField.clearButtonMode = UITextFieldViewModeWhileEditing;
    }];
    
    
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action){
        
        NSString *h = [alert.textFields[0] text];
        NSString *l = [alert.textFields[1] text];
        
        if([h intValue]<0)
            g_rssi_high = [h intValue] + 100;
        if([l intValue]<0)
            g_rssi_low  = [l intValue] + 100;
    }];
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleDefault handler:nil];
    
    UIAlertAction *ResetAction = [UIAlertAction actionWithTitle:@"Clear" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action){
        
        [BTinfo_DataFile removeObjectForKey:@"BTUUID"];
        [APInfo_DataFile removeObjectForKey:@"APINFO"];
        [WIFIInfo_DataFile removeObjectForKey:@"AP1"];
        [WIFIInfo_DataFile removeObjectForKey:@"AP2"];
        [WIFIInfo_DataFile removeObjectForKey:@"AP3"];
        
        [AP_Profile_queue removeAllObjects];
        
    }];

    [alert addAction:okAction];
    [alert addAction:cancelAction];
    [alert addAction:ResetAction];
    
    [self presentViewController:alert animated:YES completion:nil];

}


//test
+ (void)clear_action
{
    [BTinfo_DataFile removeObjectForKey:@"BTUUID"];
    [APInfo_DataFile removeObjectForKey:@"APINFO"];
    [WIFIInfo_DataFile removeObjectForKey:@"AP1"];
    [WIFIInfo_DataFile removeObjectForKey:@"AP2"];
    [WIFIInfo_DataFile removeObjectForKey:@"AP3"];
    
    [AP_Profile_queue removeAllObjects];
}

+ (void)rssi_setting:(int)d_rssi_low :(int)d_rssi_high;
{
    g_rssi_high = d_rssi_high;
    g_rssi_low = d_rssi_low;
}

- (IBAction)Btn_sitsurvey:(id)sender //uesless now
{

    if(can_toSiteSurvey && isConnected){
        stateNum=STATE_CAPABILITY;
        [m_APlist removeAllObjects];
        [discover_table reloadData];
        [self startWaiting:@"":@"Scanning WiFi AP":30:tag_scan_ap];
        
    }else{
        UIAlertView *aboutShow = [[UIAlertView alloc]
                                  initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                  message:NSLocalizedString(@"Please scan BT device to connect it.", @"AlertView")
                                  delegate:self
                                  cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                  otherButtonTitles:nil];
        [aboutShow show];
    }
}


- (IBAction)Btn_GoSetting:(id)sender
{
    [self init_BT];
    
    memset(discovery_dev_uuid,0,sizeof(discovery_dev_uuid));
    memset(dev_uuid_cfg,0,sizeof(dev_uuid_cfg));
    product_type = 0;
    discovery_dev_uuid_index = 0;
    can_toConnect=NO;
    isConnected      = NO;
    can_toSiteSurvey = NO;
    isShowConnectList = NO;

    stateNum = STATE_CFG_IDLE;
    
    [m_APlist removeAllObjects];
    [discover_table reloadData];
    
    isConfiguring = YES;
    [m_devlist removeAllObjects];

    if(ble_on){
        [self disconnectBT];
        [self scan_BT_peripheral];
        NSLog(@"Configing");
        [self startWaiting:@"":@"Scan Bluetooth Device":0:tag_scan_bt];
    }

    isHomeAP_check = YES;
    isWiFiList_ready = NO;

}

- (IBAction)Btn_GoQRcode:(id)sender
{
    stateNum = STATE_CFG_IDLE;
    
    [m_APlist removeAllObjects];
    [discover_table reloadData];
    
    isConfiguring = YES;

    NSString *homeAP_SSID = @"";
    NSString *homeAP_SSIDraw = @"";
    
    if (floor(NSFoundationVersionNumber) > NSFoundationVersionNumber_iOS_9_0) {
       //ios9 up supports NEHotspotHelper
        
        homeAP_SSID = [Util fetchCurrSSID];
        homeAP_SSIDraw = [Util fetchCurrSSID_RAW];
        
    }else{
        homeAP_SSID = [Util fetchCurrSSID];
        homeAP_SSIDraw = [Util fetchCurrSSID_RAW];
    }
    
    if(homeAP_SSID != nil){
        [self alertView_checkCurrPassword:homeAP_SSID:homeAP_SSIDraw];
    }
    else{
        alert_QRCode = [UIAlertController alertControllerWithTitle:@""
                                                           message:@"The device is not connected to WiFi"
                                                           preferredStyle:UIAlertControllerStyleAlert];
        
        UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:nil];
        [alert_QRCode addAction:okAction];
        [self presentViewController:alert_QRCode animated:YES completion:nil];
    }

    
}

- (IBAction)Btn_Return:(id)sender
{

    [imgViewQRCode removeFromSuperview];
    imgViewQRCode = nil;
    [QRCode_tip removeFromSuperview];
    QRCode_tip = nil;
    
    btn_search.hidden = NO;
    btn_qrcode.hidden = NO;
    btn_setting.hidden = NO;
    btn_return.hidden = YES;
    
    [self disconnectBT];

}

-(void) alertView_checkCurrPassword:(NSString *)targetAP_SSID:(NSString *)targetAP_SSIDRAW
{
    BOOL isSecurity = YES;
    static BOOL isCheck = NO;
    static NSData* qr_data;
    NSString *msg = [NSString stringWithFormat:@"Enter the password for AP \"%@\" ", targetAP_SSID];

    alertController_CurrQRcode = [UIAlertController alertControllerWithTitle:msg
                                                              message:nil
                                                              preferredStyle:UIAlertControllerStyleAlert];
    if(isSecurity){
        [alertController_CurrQRcode addTextFieldWithConfigurationHandler:^(UITextField *textField) {
            NSString *storedPassword = [self getWiFipassword:targetAP_SSID];
            textField.placeholder = @"Password";
            
            if(storedPassword.length>0){
                NSLog(@"storedPassword:%@",storedPassword);
                textField.text = storedPassword;
            }
            textField.secureTextEntry = YES;
        }];
    }
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Back" style:UIAlertActionStyleDefault handler:nil];
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"Continue" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        
        btn_qrcode.hidden = YES;
        btn_search.hidden = YES;

        if(isSecurity){
            UITextField *password = alertController_CurrQRcode.textFields.firstObject;
            homeAP_password = password.text;
            NSLog(@"password:%@",homeAP_password);
            [self setWiFipassword:targetAP_SSID :homeAP_password];
            
            if(homeAP_password.length==0){
                isCheck = YES;
            }else if(homeAP_password.length<8){
                isCheck = NO;
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is invalid!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:nil];
                aboutShow.tag = tag_scan_qr;
                [aboutShow show];
            }
            else{
                isCheck = YES;
            }
        }

        if(isCheck){ //generate QRcode
            const char *str1 = [@"WIFI:S:" UTF8String];
            const char *str2 = [@";T:WPA;P:" UTF8String];
            const char *str3 = [@";;" UTF8String];

            const char *tmp = [targetAP_SSIDRAW UTF8String];
            const char *tmp_pwd = [homeAP_password UTF8String];
            
            unsigned char SSIDRaw[33] = {0x0};
            unsigned char buffer[128];
            memset(buffer, '\0', 128);
            
            // string to hex data
            for(int i=0;i<(strlen(tmp)/2);i++){
                SSIDRaw[i] = (tmp[2*i] <= '9'? tmp[2*i] - '0': tmp[2*i] - 'a' + 10) << 4 | (tmp[2*i+1] <= '9'? tmp[2*i+1] - '0': tmp[2*i+1] - 'a' + 10);
            }

            if(homeAP_password.length == 0){ //OPEN
                memcpy(buffer, str1, 7);
                memcpy(buffer+7, SSIDRaw, strlen(SSIDRaw));
                memcpy(buffer+7+strlen(SSIDRaw), str3, 2);
            }else{
                memcpy(buffer, str1, 7);
                memcpy(buffer+7, SSIDRaw, strlen(SSIDRaw));
                memcpy(buffer+7+strlen(SSIDRaw), str2, 9);
                memcpy(buffer+7+strlen(SSIDRaw)+9, tmp_pwd, strlen(tmp_pwd));
                memcpy(buffer+7+strlen(SSIDRaw)+9+strlen(tmp_pwd), str3, 2);
            }

            qr_data =[NSData dataWithBytes:buffer length:strlen(buffer)];
            
            int maxScreen_width = [[UIScreen mainScreen] bounds].size.width;
            int maxScreen_height = [[UIScreen mainScreen] bounds].size.height;

            imgViewQRCode = [[UIImageView alloc] initWithFrame:CGRectMake(10, (maxScreen_height - maxScreen_width)/1.5, maxScreen_width - 20, maxScreen_width - 20)];
            
            imgViewQRCode.backgroundColor = [UIColor blueColor];
            
            imgViewQRCode.contentMode = UIViewContentModeScaleAspectFit;
            // Generation of QR code image
            CIFilter *qrCodeFilter = [CIFilter filterWithName:@"CIQRCodeGenerator"];
            [qrCodeFilter setValue:qr_data forKey:@"inputMessage"];
            [qrCodeFilter setValue:@"L" forKey:@"inputCorrectionLevel"]; //default of L,M,Q & H modes
            
            CIImage *qrCodeImage = qrCodeFilter.outputImage;
            
            float scaleX = imgViewQRCode.frame.size.width / qrCodeImage.extent.size.width;
            float scaleY = imgViewQRCode.frame.size.height / qrCodeImage.extent.size.height;
            
            qrCodeImage = [qrCodeImage imageByApplyingTransform:CGAffineTransformMakeScale(scaleX, scaleY)];
           
            UIImage *qrCodeImageByTransform = [UIImage imageWithCIImage:qrCodeImage];
            imgViewQRCode.image = qrCodeImageByTransform;
            [self.view addSubview:imgViewQRCode];
            
            QRCode_tip = [[UILabel alloc]initWithFrame:CGRectMake( 0, 100, maxScreen_width, 50)];
            QRCode_tip.text = @"Please capture the QR Code with camera for WiFi login";
            QRCode_tip.textAlignment = NSTextAlignmentCenter;
            QRCode_tip.lineBreakMode = NSLineBreakByWordWrapping;
            QRCode_tip.numberOfLines = 0;
            [self.view addSubview:QRCode_tip];
            
            btn_search.hidden = YES;
            btn_qrcode.hidden = YES;
            
            btn_setting.hidden = YES;
            btn_return.hidden = NO;
   
        }
 
    }];
    
    [alertController_CurrQRcode addAction:cancelAction];
    [alertController_CurrQRcode addAction:okAction];
    
    [self presentViewController:alertController_CurrQRcode animated:YES completion:nil];
}

-(void) alertView_checkPassword:(unsigned char)isSecurity
{

    NSString *targetAP_SSID = @"";
    Confirm_SSID = @"";
    
    NSString *msg = @"";
    
    int len = (int)strlen(targetAp.bdSsIdBuf);
    targetAP_SSID = [[NSString alloc]initWithBytes:targetAp.bdSsIdBuf length:len encoding:NSUTF8StringEncoding];
    
    Confirm_SSID = targetAP_SSID;

    if(isSecurity == 0){
        isSecured = NO;
        msg = [NSString stringWithFormat:@"The target AP \"%@\" has no security. Suggest choosing an secured AP.",Confirm_SSID];
    }
    else{
        isSecured = YES;
        msg = [NSString stringWithFormat:@"Enter the password for AP \"%@\" ",Confirm_SSID];
    }

    alertController_pwd = [UIAlertController alertControllerWithTitle:@""//@"Configure New Device"
                                                                             message:msg
                                                                      preferredStyle:UIAlertControllerStyleAlert];
    if(isSecurity != 0){
        [alertController_pwd addTextFieldWithConfigurationHandler:^(UITextField *textField) {
            NSString *storedPassword = [self getWiFipassword:targetAP_SSID];
            
            textField.placeholder = @"Password";
            
            if(storedPassword.length>0){
                NSLog(@"storedPassword:%@",storedPassword);
                textField.text = storedPassword;
            }
            textField.secureTextEntry = YES;
        }];
    }
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil];
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"Next" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {

        int nextStep = NO;
        
        if(isSecurity == 1){ //WPA2
            UITextField *password = alertController_pwd.textFields.firstObject;
            homeAP_password = password.text;
            //NSLog(@"password:%@",homeAP_password);
            
            if(homeAP_password.length==0){
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is empty!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                          otherButtonTitles:nil];
                [aboutShow show];
            }else if(homeAP_password.length<8){
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is invalid!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                          otherButtonTitles:nil];
                [aboutShow show];
            }else{
                nextStep = YES;
            }
            
        }else if(isSecurity == 2){ //WEP
            NSArray *pwd_len = @[@5, @10, @13, @26];
            UITextField *password = alertController_pwd.textFields.firstObject;
            homeAP_password = password.text;

            if(homeAP_password.length==0){
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is empty!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                          otherButtonTitles:nil];
                [aboutShow show];
            }else if(![pwd_len containsObject:@(homeAP_password.length)]){
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is invalid!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                          otherButtonTitles:nil];
                [aboutShow show];
            }else{
                nextStep = YES;
            }
        }
        else{
            nextStep = YES;
        }
        
        if(nextStep){
            stateNum = STATE_CONNECTION;
            NSString *context = [NSString stringWithFormat:@"Connecting to %@",targetAP_SSID];
            isWiFiConnecting = YES;
            [self startWaiting_ConnectAP:@"":context:60:tag_connect_ap];
        }
        
    }];
    
    [alertController_pwd addAction:cancelAction];
    [alertController_pwd addAction:okAction];

    [self presentViewController:alertController_pwd animated:YES completion:nil];
}

-(void) alertView_checkPassword_HomeAP:(unsigned char)isSecurity
{
    HomeAP_Security = isSecurity;
    NSString *targetAP_SSID = @"";
    Confirm_SSID = @"";
    
    NSString *msg = @"";
    
    int len = (int)strlen(targetAp.bdSsIdBuf);
    targetAP_SSID = [[NSString alloc]initWithBytes:targetAp.bdSsIdBuf length:len encoding:NSUTF8StringEncoding];
    
    Confirm_SSID = targetAP_SSID;

    if(isSecurity == 0){
        isSecured = NO;
        msg = [NSString stringWithFormat:@"The target AP \"%@\" has no security. Suggest choosing an secured AP.",Confirm_SSID];
    }
    else{
        isSecured = YES;
        msg = [NSString stringWithFormat:@"Enter the password for AP \"%@\" ",Confirm_SSID];
    }
    
    alertController_pwd = [UIAlertController alertControllerWithTitle:@""//@"Configure New Device"
                                                              message:msg
                                                       preferredStyle:UIAlertControllerStyleAlert];
    if(isSecurity != 0){
        [alertController_pwd addTextFieldWithConfigurationHandler:^(UITextField *textField) {
            NSString *storedPassword = [self getWiFipassword:targetAP_SSID];
            
            textField.placeholder = @"Password";
            
            if(storedPassword.length>0){
                NSLog(@"storedPassword:%@",storedPassword);
                textField.text = storedPassword;
            }
            
            textField.secureTextEntry = YES;
        }];
    }
    
    UIAlertAction *selectAction = [UIAlertAction actionWithTitle:@"Select AP" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action){
        stateNum = STATE_CAPABILITY;
        discover_table.hidden = NO;
        show_ap_info = YES;
        show_tip = YES;
        isHomeAP_exist = NO;

        if(isWiFiList_ready==NO){
            [self startWaiting:@"":@"Scanning WiFi AP":30:tag_scan_ap];
        }
    }];
    
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"Next" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        
        int nextStep = NO;
        
        if(isSecurity == 1){ //WPA2
            UITextField *password = alertController_pwd.textFields.firstObject;
            homeAP_password = password.text;
            NSLog(@"password:%@",homeAP_password);
            
            if(homeAP_password.length==0){
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is empty!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                          otherButtonTitles:nil];
                
                aboutShow.tag = tag_connect_homeap;
                [aboutShow show];
            }else if(homeAP_password.length<8){
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is invalid!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                          otherButtonTitles:nil];
                
                aboutShow.tag = tag_connect_homeap;
                [aboutShow show];
            }else{
                nextStep = YES;
            }
            
        }else if(isSecurity == 2){ //WEP
            NSArray *pwd_len = @[@5, @10, @13, @26];
            UITextField *password = alertController_pwd.textFields.firstObject;
            homeAP_password = password.text;
            NSLog(@"password:%@",homeAP_password);
            
            if(homeAP_password.length==0){
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is empty!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                          otherButtonTitles:nil];
                
                aboutShow.tag = tag_connect_homeap;
                [aboutShow show];
            }else if(![pwd_len containsObject:@(homeAP_password.length)]){
                UIAlertView *aboutShow = [[UIAlertView alloc]
                                          initWithTitle:NSLocalizedString(@"Warning", @"AlertView")
                                          message:NSLocalizedString(@"The password is invalid!", @"AlertView")
                                          delegate:self
                                          cancelButtonTitle:NSLocalizedString(@"OK", @"AlertView")
                                          otherButtonTitles:nil];
                
                aboutShow.tag = tag_connect_homeap;
                [aboutShow show];
            }else{
                nextStep = YES;
            }
        }else{
            nextStep = YES;
        }
        
        if(nextStep){
            stateNum = STATE_CONNECTION;
            NSString *context = [NSString stringWithFormat:@"Connecting to %@",targetAP_SSID];
            isWiFiConnecting = YES;
            [self startWaiting_ConnectAP:@"":context:60:tag_connect_ap];
        }
  
    }];
    
    [alertController_pwd addAction:selectAction];
    [alertController_pwd addAction:okAction];
    
    [self presentViewController:alertController_pwd animated:YES completion:nil];
}

-(void) alertView_showPosition:(unsigned char)RSSI
{
    NSString *targetAP_SSID = @"";
    NSString *title = @"";
    NSString *msg = @"";
    NSString *btn1_msg = @"";
    NSString *btn2_msg = @"";
    
    NSLog(@"check RSSI:%d",RSSI);

    targetAP_SSID = [[NSString alloc]initWithBytes:targetAp.bdSsIdBuf length:sizeof(targetAp.bdSsIdBuf) encoding:NSUTF8StringEncoding];
    
    if(RSSI>g_rssi_high){
        title = [NSString stringWithFormat:@"%@'s Position Too Close From AP!", selectDevice];
        //title = @"WiFi Client's Position Too Close From AP!";
        msg = @"Suggest to adjust position far away from AP!";
        btn1_msg = @"Adjusted";
        btn2_msg = @"NO,thanks!";
    }else if(RSSI<g_rssi_low){
        title = [NSString stringWithFormat:@"%@'s Position Too Far From AP!", selectDevice];
        //title = @"WiFi Client's Position Too Far From AP!";
        msg = @"Suggest to adjust position close to AP!";
        btn1_msg = @"Adjusted";
        btn2_msg = @"NO,thanks!";
    }else{//position is fine
        title = [NSString stringWithFormat:@"%@'s position is fine.", selectDevice];
        //title = @"WiFi Client's Position is Fine!";
        msg = [NSString stringWithFormat:@"%@'s position is fine.", selectDevice];
        btn1_msg = @"To do double check!";
        btn2_msg = @"OK";
    }
    
    
    UIAlertController *alertController_position = [UIAlertController alertControllerWithTitle:title
                                                                                 message:msg
                                                                          preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:btn1_msg style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        
        isCheckDevicePosition = YES;
    }];
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:btn2_msg style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        isCheckDevicePosition = NO;
    }];
    
    [alertController_position addAction:cancelAction];
    [alertController_position addAction:okAction];
    
    [self presentViewController:alertController_position animated:YES completion:nil];
}

-(void) alertView_showPosition_dev1:(unsigned char)RSSI
{
    NSString *targetAP_SSID = @"";
    NSString *title = @"";
    NSString *msg = @"";
    NSString *btn1_msg = @"";
    NSString *btn2_msg = @"";
    
    NSLog(@"RSSI:%d",RSSI);

    targetAP_SSID = [[NSString alloc]initWithBytes:targetAp.bdSsIdBuf length:sizeof(targetAp.bdSsIdBuf) encoding:NSUTF8StringEncoding];
    
    if(RSSI<g_rssi_low){
        title = [NSString stringWithFormat:@"%@'s Position Too Far From AP!", selectDevice];
        //title = @"WiFi Client's Position Too Far From AP!";
        msg = @"Suggest to adjust position close to AP!";
        btn1_msg = @"Adjusted";
        btn2_msg = @"NO,thanks!";
        
        UIAlertController *alertController_position = [UIAlertController alertControllerWithTitle:title
                                                                                     message:msg
                                                                              preferredStyle:UIAlertControllerStyleAlert];
        
        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:btn1_msg style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
            
            isCheckDevicePosition = YES;
        }];
        UIAlertAction *okAction = [UIAlertAction actionWithTitle:btn2_msg style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
            isCheckDevicePosition = NO;
        }];
        
        [alertController_position addAction:cancelAction];
        [alertController_position addAction:okAction];
        
        [self presentViewController:alertController_position animated:YES completion:nil];
    }
    
    
    
}

- (void)startWaiting:(NSString *) wait_title :(NSString *) wait_msg :(float)timeout :(int)alert_tag
{
    
    //  Purchasing Spinner.
        waitingAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(wait_title,@"")
                                                  message:wait_msg
                                                 delegate:self
                                        cancelButtonTitle:@"Cancel"
                                        otherButtonTitles:nil];

        waitingAlert.tag = alert_tag;
        
        UIActivityIndicatorView *actview = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        actview.color=[UIColor blackColor];
        [actview startAnimating];
        
        [waitingAlert setValue:actview forKey:@"accessoryView"];
        
        [waitingAlert show];
        
        if (timeout>0) {
            waitTimer = [NSTimer scheduledTimerWithTimeInterval: timeout
                                                         target: self
                                                       selector:@selector(stopWaiting)
                                                       userInfo: nil repeats:NO];
        }
}

- (void)startWaiting_ConnectAP:(NSString *) wait_title :(NSString *) wait_msg :(float)timeout :(int)alert_tag
{
    
    //  Purchasing Spinner.
    waitingAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(wait_title,@"")
                                              message:wait_msg
                                             delegate:self
                                    cancelButtonTitle:nil
                                    otherButtonTitles:nil];
    
    waitingAlert.tag = alert_tag;
    
    UIActivityIndicatorView *actview = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    actview.color=[UIColor blackColor];
    [actview startAnimating];
    
    [waitingAlert setValue:actview forKey:@"accessoryView"];
    
    [waitingAlert show];
    
    if (timeout>0) {
        waitTimer = [NSTimer scheduledTimerWithTimeInterval: timeout
                                                     target: self
                                                   selector:@selector(stopWaiting)
                                                   userInfo: nil repeats:NO];

    }
}


-(void)stopWaiting
{
    if(waitTimer){
        [waitTimer invalidate];
        waitTimer = nil;
    }
    
    if (waitingAlert) {
        [waitingAlert dismissWithClickedButtonIndex:0 animated:YES];
        waitingAlert = nil;
    }
}

-(void)showConfigList
{

    UIAlertView* confirm_alert = [[UIAlertView alloc] initWithTitle:@"Configured Device"
                                                            message:@"The device list is below."
                                                           delegate:self
                                                  cancelButtonTitle:@"Confirm"
                                                  otherButtonTitles: nil];
    
    confirm_alert.tag = tag_cfg_confirm;
    int table_height = 210;
    NSInteger focusValue = 1;//[confirm_list count]%2==0 ? ((NSInteger)[confirm_list count]/2) : ((NSInteger)[confirm_list count]/2+1);
    
    NSIndexPath *focusIndex = [NSIndexPath indexPathForRow:focusValue inSection:0];
    
    UITableView* myView = [[UITableView alloc] initWithFrame:CGRectMake(10, 45, 264, table_height) style:UITableViewStyleGrouped];
    myView.tag = tag_table_confirm;
    
    [myView selectRowAtIndexPath:focusIndex
                        animated:NO
                  scrollPosition:UITableViewScrollPositionNone];
    [myView setAllowsSelection:NO];
    myView.delegate = self;
    myView.dataSource = self;
    myView.backgroundColor = [UIColor clearColor];
    [confirm_alert setValue:myView forKey:@"accessoryView"];
    [confirm_alert show];
    
}


-(void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{

    if(alertView.tag == tag_cfg_connect){
        
        if(buttonIndex == 0) {//Cancel
            isConfiguring = NO;
            cfgProgressView.progress = 0;
            
        }else{
            NSLog(@"dismiss tag_cfg_connect");
        }
        
    }else if(alertView.tag == tag_cfg_confirm){
        NSLog(@"<APP> Confirm!");
       
        
        [discover_table reloadData];
    }else if(alertView.tag == tag_timer){
        
        if(buttonIndex == 0) {//Cancel
            //NSLog(@"<APP> wait alert show: Cancel");
            //[self stopWaiting];NSLog(@"line:%d",__LINE__);
        }
        
    }else if(alertView.tag == tag_cfg_alertView_AP){
        
    }
}

//---------------------------------  UI table controllers  -------------------------------------
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(nonnull NSIndexPath *)indexPath
{
    if((tableView.tag == tag_table_connect)){
        return 60;
    }
    
    return 44;
}
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    int index = (int)indexPath.row;
    struct dev_info dev;
    struct rtk_btconfig_bss_info bssInfo;
    NSValue *dev_val;
    static NSString *simpleTableIdentifier = @"Cell";
    NSString *devName = @"";
    NSString *devUUID = @"";
    
    switch ([tableView tag]) {
        case tag_table_connect:{
            UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
            BOOL isPreUUID =NO;
            NSString *preUUID = @"";
            
            if (cell == nil) {
                cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
            }
            //get the element(rowIndex) device
            dev_val = [m_devlist objectAtIndex:index];
            devUUID = [NSString stringWithFormat:@"%@", dev_val];
            
            
            preUUID = [self getPreviousBTProfile];
            
            if(preUUID.length>0){
                if([preUUID isEqualToString:devUUID]){
                    isPreUUID = YES;
                }
            }
            devUUID = [devUUID substringToIndex:8];
            
            if(isPreUUID){
                devName = [NSString stringWithFormat:@"%@\nPrevious Saved Profile", dev_bt_name[index]];
                cell.textLabel.textColor = [UIColor blueColor];
            }else{
                devName = [NSString stringWithFormat:@"%@", dev_bt_name[index]];
            }
            
            cell.textLabel.numberOfLines = 3;
            cell.textLabel.text = devName;
            cell.textLabel.font = [cell.textLabel.font fontWithSize:12];
            cell.selectionStyle = UITableViewCellSelectionStyleNone;

            return cell;
            break;
        }
        case tag_table_confirm:{
            UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
            
            if (cell == nil) {
                cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
            }
            //get the element(rowIndex) device
            dev_val = [m_devlist objectAtIndex:index];
            [dev_val getValue:&dev];
            devName = [NSString stringWithCString:(const char *)(dev.extra_info) encoding:NSUTF8StringEncoding];
            if(devName.length==0){
                char tmp[32] = {0};
                sprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x", dev.mac[0], dev.mac[1],dev.mac[2],dev.mac[3],dev.mac[4],dev.mac[5]);
                cell.textLabel.text = [NSString stringWithUTF8String:(const char *)tmp];
            }else{
                cell.textLabel.text = devName;
            }
            cell.textLabel.font = [cell.textLabel.font fontWithSize:14];
            cell.selectionStyle = UITableViewCellSelectionStyleNone;

            return cell;
            break;
        }
        case tag_table_ap_list:{
            BOOL highlight = false;
            NSString *imaName_en = @"";
            NSString *imgName_rssi = @"";
            static NSString *cell_apIdentifier = @"DiscoverCell";
            char c_bssid[32] = {0};
            ClientListCell *cell_ap = (ClientListCell *)[tableView dequeueReusableCellWithIdentifier:cell_apIdentifier];
            
            if(cell_ap==nil){
                cell_ap = [[ClientListCell alloc]initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cell_apIdentifier];
            }
            dev_val = [m_APlist objectAtIndex:index];
            [dev_val getValue:&bssInfo];
            
            NSString *ssid = [[NSString alloc]initWithBytes:bssInfo.bdSsIdBuf length:sizeof(bssInfo.bdSsIdBuf) encoding:NSUTF8StringEncoding];
            
            [Util mac2str: (char *)bssInfo.bdBssId :c_bssid];
            NSString *bssid = [NSString stringWithCString:c_bssid encoding:NSASCIIStringEncoding];
            
            imaName_en = bssInfo.authAlg==0 ? @"encrypt_open.png":@"encrypt_lock.png";
            
            if(bssInfo.rssi>=RSSI_Level_1){
                imgName_rssi = @"strength_grade4.png";
            }else if( bssInfo.rssi<RSSI_Level_1 && bssInfo.rssi>=RSSI_Level_2){
                imgName_rssi = @"strength_grade3.png";
            }else if( bssInfo.rssi<RSSI_Level_2 && bssInfo.rssi>=RSSI_Level_3){
                imgName_rssi = @"strength_grade2.png";
            }else{
                imgName_rssi = @"strength_grade1.png";
            }

            int j;
            int queue_count = (int)AP_Profile_queue.count;
            if(AP_Profile_queue.count > 0){
                for(j=queue_count;j>0;j--){
                    if([bssid isEqualToString:[AP_Profile_queue objectAtIndex:j-1]]){
                        highlight = true;
                    }
                }
            }

            
            [cell_ap setContent:ssid mac:bssid :imaName_en :imgName_rssi type:0 flag:highlight];

            return cell_ap;

            break;
        }
            
        default:
            NSLog(@"ERROR TABLE TAG");
            break;
    }
    
    return nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    int index = (int)indexPath.row;
    NSValue *dev_val;
    struct rtk_btconfig_bss_info bssInfo;

    if(tableView.tag == tag_table_connect){  //useless now
        
        NSString *title = @"";
        NSString *devName = @"";
        NSString *devUUID = @"";
        
        [self setToConnectionState];
        
        dev_val = [m_devlist objectAtIndex:index];
        devUUID = [NSString stringWithFormat:@"%@", dev_val];
        NSLog(@"<APP> to connect! %@", devUUID);
        
        lastDevUUID = devUUID;
        [self setPreviousBTProfile:devUUID];
        devUUID = [devUUID substringToIndex:8];
        devName = [NSString stringWithFormat:@"%@", dev_bt_name[index]];//struct dev_info dev;
        NSLog(@"<APP> to connect! %@", devName);
        
        [self setLabelText:devName];
        NSLog(@"<APP> to connect!");
        
        configTimerResume = NSDate.date.timeIntervalSince1970;
        
        can_toConnect = YES;
        [connect_alert dismissWithClickedButtonIndex:1 animated:YES];
        title= [NSString stringWithFormat:@"Connecting to %@", dev_bt_name[index]];
        [self startWaiting:@"":title:30:tag_connect_ap];
        
    }else if( tableView.tag == tag_table_ap_list){
        dev_val = [m_APlist objectAtIndex:index];
        [dev_val getValue:&bssInfo];
        
        char ssid[32] = {0};
        memcpy(ssid,bssInfo.bdSsIdBuf,sizeof(ssid));
        memset(&targetAp,0,sizeof(targetAp));
        memcpy(&targetAp,&bssInfo,sizeof(targetAp));

        [self alertView_checkPassword:bssInfo.authAlg];
    }
    
    return;
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    if(tableView.tag == tag_table_ap_list){
        return 1;
    }else{
        return 1;
    }
    
    return 1;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    //return YES;
    return YES;
}
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{

    if(tableView.tag == tag_table_ap_list){
        if(m_APlist!=nil)
            return [m_APlist count];
    }else{
        if(m_devlist!=nil)
            return [m_devlist count];
    }
    
    return 1;
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    if(isConfiguring){
        
    }else{
        //[discover_table reloadData];
    }
    [discover_table reloadData];
    //NSLog(@"scan done");
}
//---------------------------------  UI table controllers  -------------------------------------

-(void)cfgConnectSwitched:(id) sender
{
    int rowIndex = (int)[sender tag];
    UISwitch* switchControl = sender;
    NSLog( @"switch(%d) is %@", rowIndex, switchControl.on ? @"ON" : @"OFF" );
}

-(void)cfgConfirmSwitched:(id) sender
{
    int rowIndex = (int)[sender tag];
    UISwitch* switchControl = sender;
    NSLog( @"switch(%d) is %@", rowIndex, switchControl.on ? @"ON" : @"OFF" );
    
}

-(void) isUsePinSwitched:(id) sender
{
    UISwitch* switchControl = sender;
    NSLog( @"PIN Code : %@",switchControl.on ? @"ON" : @"OFF");
    if(switchControl.on){
        g_testField_pin.placeholder = @"PIN Code";
        [g_testField_pin setBackgroundColor:[UIColor whiteColor]];
        g_testField_pin.userInteractionEnabled = true;
        g_testField_pin.enabled = YES;
    }else{
        g_testField_pin.placeholder = @"";
        [g_testField_pin setBackgroundColor:[UIColor darkGrayColor]];
        g_testField_pin.userInteractionEnabled = false;
        g_testField_pin.enabled = NO;
    }
}

-(NSString*)getPreviousBTProfile
{
    NSString *uuid = @"";
    
    uuid = [BTinfo_DataFile objectForKey:@"BTUUID"];
    return uuid;
}
-(void)setPreviousBTProfile : (NSString *)bt_uuid
{
    [BTinfo_DataFile setValue:bt_uuid forKey:@"BTUUID"];
    [BTinfo_DataFile synchronize];
}
-(NSString*)getWiFipassword : (NSString *)APname
{
    NSString *passwordAllData = @"";
    NSString *password = @"";

    passwordAllData = [APInfo_DataFile stringForKey:@"APINFO"];
    
    NSArray *APData = [passwordAllData componentsSeparatedByString:@";"];
    NSArray *APItemInfo = nil;

    for (int i = 0; i < [APData count]; i++) {
        NSLog(@"<APP> get AP-%d:%@\n", i+1, [APData objectAtIndex:i]);
        
        APItemInfo = [ [APData objectAtIndex:i] componentsSeparatedByString:@","];
        if( [APname isEqualToString: [APItemInfo objectAtIndex:0] ] )
        {
            password = [APItemInfo objectAtIndex:1];
            return password;
        }
    }

    return password;
}

-(Boolean)setWiFipassword : (NSString *)APname : (NSString *)password
{
    Boolean ret = false;
    Boolean isNewAP = true;
    //check previous data
    NSString *preWiFiAllData = @"";
    NSString *storeWiFiData = @"";
    NSString *storeWiFiAllData = @"";

    preWiFiAllData = [APInfo_DataFile stringForKey:@"APINFO"];
    
    NSArray *APData = [preWiFiAllData componentsSeparatedByString:@";"];
    NSArray *APItemInfo = nil;
    int APNum = (int)[APData count]-1;
    for (int i = 0; i < APNum; i++)
    {

        APItemInfo = [ [APData objectAtIndex:i] componentsSeparatedByString:@","];
        if( [APname isEqualToString: [APItemInfo objectAtIndex:0] ] )
        {
            isNewAP = false;
            break;
        }
        
    }
    
    if (isNewAP) {//new
        
        if(preWiFiAllData==nil)
            preWiFiAllData = @"";

        storeWiFiAllData = [NSString stringWithFormat:@"%@%@,%@;", preWiFiAllData, APname, password];
    }else{//update
        for (int i = 0; i < APNum; i++)
        {
            APItemInfo = [ [APData objectAtIndex:i] componentsSeparatedByString:@","];
            if( [APname isEqualToString: [APItemInfo objectAtIndex:0] ] )
            {
                storeWiFiData = [NSString stringWithFormat:@"%@,%@;", [APItemInfo objectAtIndex:0], password];
            }else{
                storeWiFiData = [NSString stringWithFormat:@"%@,%@;", [APItemInfo objectAtIndex:0], [APItemInfo objectAtIndex:1]];
            }
            storeWiFiAllData = [storeWiFiAllData stringByAppendingString:storeWiFiData];
        }
    }
    
    NSLog(@"storeWiFiAllData:%@",storeWiFiAllData);
    [APInfo_DataFile setValue:storeWiFiAllData forKey:@"APINFO"];
    [APInfo_DataFile synchronize];
    
    ret = true;
    
    return ret;
}

-(void)reloadConnectTableData
{
    [myConnectTable reloadData];
}

-(void)BT_StopScan
{
    [self.centralManager stopScan];
    NSLog(@"Scanning stopped");
}

-(void)resetLabelText
{
    label_btDevice.text = @"BT Device";
    label_btDevice.textColor = [UIColor blackColor];

    target_SSID.text = @"";
    target_BSSID.text = @"";
    target_security.image = nil;
    target_RSSI.image = nil;
}

-(void)setLabelText:(NSString*) message
{
    label_btDevice.text = message;
    label_btDevice.textColor = [UIColor blueColor];
    
}
-(void)setBTDeviceState:(NSString*) img_file
{
    img_bt_device_state.image = [UIImage imageNamed:img_file];
}

-(void)setToConnectionState
{
    [m_APlist removeAllObjects];
    [discover_table reloadData];
}

-(void)btcfg_cmd_getStatus:(NSTimer*)t
{
    int len = 0;
    NSDictionary *wrapper = (NSDictionary*)[t userInfo];
    CBPeripheral *peripheral = [wrapper objectForKey:@"obj1"];
    CBCharacteristic *characteristic = [wrapper objectForKey:@"obj2"];
    
    NSData *valData = nil;
    uint8_t val[MAX_BUF_SIZE] = {0};
    
    len = [handleRequest gen_cmd_connection_status:val];
    valData = [NSData dataWithBytes:(const void *)val length:5];
    
    [peripheral writeValue:valData forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
    //NSLog(@"%d UUID:%@ writeValue",__LINE__,characteristic.UUID.UUIDString);
    [peripheral readValueForCharacteristic:characteristic];
}

-(void)sortAP_byRSSI
{

    int i = 0,j;
    int AP_count = (int)m_APlist.count;

    struct rtk_btconfig_bss_info AP_info[RTK_BTCONFIG_MAX_BSS_NUM*5] = {0};//2G+5G

    struct rtk_btconfig_bss_info tmp = {0};

    NSValue *dev_val;
    
    for(i=0;i<AP_count;i++){
   
        dev_val = [m_APlist objectAtIndex:i];
    
        [dev_val getValue:&tmp];

        memcpy(&AP_info[i],&tmp,sizeof(struct rtk_btconfig_bss_info));
    }

    //sort
    memset(&tmp,0,sizeof(struct rtk_btconfig_bss_info));
    
    char c_bssid[32] = {0};
    
    int counter = 0;
    int queue_count = (int)AP_Profile_queue.count;

    for(j=queue_count;j>0;j--){
        for(i=0;i<AP_count-1;i++){
            [Util mac2str: (char *)AP_info[i].bdBssId :c_bssid];
            NSString *bssid = [NSString stringWithCString:c_bssid encoding:NSASCIIStringEncoding];
            if([bssid isEqualToString:[AP_Profile_queue objectAtIndex:j-1]]){
                memcpy(&tmp,&AP_info[i],sizeof(struct rtk_btconfig_bss_info));
                memcpy(&AP_info[i],&AP_info[counter],sizeof(struct rtk_btconfig_bss_info));
                memcpy(&AP_info[counter],&tmp,sizeof(struct rtk_btconfig_bss_info));
                counter ++;
            }
        }
    }

    for(i=counter;i<AP_count-1;i++){
        for(j=counter;j<AP_count-1;j++){
            if(AP_info[j].rssi<AP_info[j+1].rssi){//swap
                memcpy(&tmp,&AP_info[j],sizeof(struct rtk_btconfig_bss_info));
                memcpy(&AP_info[j],&AP_info[j+1],sizeof(struct rtk_btconfig_bss_info));
                memcpy(&AP_info[j+1],&tmp,sizeof(struct rtk_btconfig_bss_info));
            }
        }
    }
   
    [m_APlist removeAllObjects];
    
    for(i=0;i<AP_count;i++){
        [m_APlist addObject:[NSValue valueWithBytes:&AP_info[i] objCType:@encode(struct rtk_btconfig_bss_info)]];
    }
}


- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{

    //  the user clicked one of the OK/Cancel buttons
    if(alertView.tag == tag_scan_ap){     // check alert by tag (scanning wifi ap)
        if (buttonIndex == [alertView cancelButtonIndex])
        {
            btn_search.hidden = NO;
            btn_qrcode.hidden = NO;
            btn_setting.hidden = NO;
            btn_return.hidden = YES;            
            [self disconnectBT];
        }

    }
    else if(alertView.tag == tag_bt_check){    // check alert by tag (check BT device)
        btn_search.hidden = NO;
        btn_qrcode.hidden = NO;
        btn_setting.hidden = NO;
        btn_return.hidden = YES;       
    }
    else if(alertView.tag == tag_scan_qr){    // check alert by tag
        // prevent keyboard pop-up
        [[UIApplication sharedApplication] sendAction:@selector(resignFirstResponder) to:nil from:nil forEvent:nil];
        
        btn_search.hidden = NO;
        btn_qrcode.hidden = NO;
        btn_setting.hidden = NO;
        btn_return.hidden = YES;
    }
    else if(alertView.tag == tag_connect_homeap){    // check alert by tag
        [self alertView_checkPassword_HomeAP:HomeAP_Security];
    }

}

-(void)Show_APInfo
{
    // prevent keyboard pop-up
    [[UIApplication sharedApplication] sendAction:@selector(resignFirstResponder) to:nil from:nil forEvent:nil];

    alert_APInfo = [UIAlertController alertControllerWithTitle:@"Configure Success" message:@"\n\n\n\n\n\n\n\n" preferredStyle:UIAlertControllerStyleAlert];

    NSLayoutConstraint *height= [NSLayoutConstraint  constraintWithItem:alert_APInfo.view
                                                     attribute:NSLayoutAttributeHeight
                                                     relatedBy:NSLayoutRelationEqual
                                                     toItem:nil
                                                     attribute:NSLayoutAttributeNotAnAttribute
                                                     multiplier:1.0
                                                     constant:250
                                                     ];

    [alert_APInfo.view addConstraint:height];

    targetAP_SSID_Info = [[UILabel alloc]initWithFrame:CGRectMake( 10, 80, 250, 30)];
    targetAP_SSID_Info.text = @"";
    targetAP_SSID_Info.textAlignment = NSTextAlignmentCenter;
    
    NSString *complete = [NSString stringWithFormat:@"%@%@", @"Name:  ",targetAP_SSID_Info.text];
    targetAP_SSID_Info.text = complete;

    UILabel *RSSI_msg = [[UILabel alloc]initWithFrame:CGRectMake( 80, 150, 70, 30)];
    RSSI_msg.text = @"Quality:";
    RSSI_msg.textAlignment = NSTextAlignmentCenter;

    targetAP_RSSI_Info = [[UIImageView alloc] initWithFrame: CGRectMake(80 + 70, 150, 30, 30)];
    UIImage *dyImage = [UIImage imageNamed:imgName_rssi];
    targetAP_RSSI_Info.image = dyImage;

    UIAlertAction *Action = [UIAlertAction actionWithTitle:@"Try other AP" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action){
        stateNum = STATE_CAPABILITY;
        discover_table.hidden = NO;
        show_ap_info = YES;
        show_tip = YES;
        isHomeAP_exist = NO;

        if(isWiFiList_ready==NO){
            [self startWaiting:@"":@"Scanning WiFi AP":30:tag_scan_ap];
        }
        
    }];
    
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"Confirm" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action){
        
        if(isSecured){
            [self setWiFipassword:Confirm_SSID :homeAP_password];
            
            if([AP_Profile_queue containsObject:Confirm_BSSID]){
                [AP_Profile_queue removeObjectAtIndex:[AP_Profile_queue indexOfObject:Confirm_BSSID]];
            }
            
            [AP_Profile_queue addObject:Confirm_BSSID];
            if(AP_Profile_queue.count > 3){
                [AP_Profile_queue removeObjectAtIndex:0];
            }
            
            if(AP_Profile_queue.count > 0){
                [WIFIInfo_DataFile setValue:[AP_Profile_queue objectAtIndex:0] forKey:@"AP1"];
                [AP_Profile_queue removeObjectAtIndex:0];
            }
            if(AP_Profile_queue.count > 0){
                [WIFIInfo_DataFile setValue:[AP_Profile_queue objectAtIndex:0] forKey:@"AP2"];
                [AP_Profile_queue removeObjectAtIndex:0];
            }
            if(AP_Profile_queue.count > 0){
                [WIFIInfo_DataFile setValue:[AP_Profile_queue objectAtIndex:0] forKey:@"AP3"];
                [AP_Profile_queue removeObjectAtIndex:0];
            }
            
            [WIFIInfo_DataFile synchronize];
        }

        btn_search.hidden = NO;
        btn_qrcode.hidden = NO;
        btn_setting.hidden = NO;
        btn_return.hidden = YES;
        
        [self disconnectBT];
        
    }];
    
    [alert_APInfo addAction:Action];
    [alert_APInfo addAction:okAction];

    [alert_APInfo.view addSubview:targetAP_RSSI_Info];

    [alert_APInfo.view addSubview:targetAP_SSID_Info];
    
    [alert_APInfo.view addSubview:RSSI_msg];
    
    //[self dismissViewControllerAnimated:YES completion:nil];

    [self presentViewController:alert_APInfo animated:YES completion:nil];

}

- (void)Show_BT_tip
{
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"" message:@"Turn On Bluetooth to Allow \"WiFiConfig\" to Connect to Accessories" preferredStyle:(UIAlertControllerStyleAlert)];

    UIAlertAction *Action_setting = [UIAlertAction actionWithTitle:@"Settings" style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
        [[UIApplication sharedApplication]openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
    }];

    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:(UIAlertActionStyleDefault) handler:nil];
    
    [alertController addAction:Action_setting];
    [alertController addAction:okAction];
    
    [[UIApplication sharedApplication].keyWindow.rootViewController presentViewController:alertController animated:YES completion:nil];
    
}


-(void)Show_tip_msg:(NSString *)message
{
    int maxScreen_height = [[UIScreen mainScreen] bounds].size.height;
    int maxScreen_width = [[UIScreen mainScreen] bounds].size.width;
    
    UIWindow * window = [UIApplication sharedApplication].keyWindow;
    UIView *showview = [[UIView alloc]init];
    showview.backgroundColor = [UIColor blackColor];
    showview.frame = CGRectMake(1, 1, 1, 1);
    showview.alpha = 1.0f;
    showview.layer.cornerRadius = 5.0f;
    showview.layer.masksToBounds = YES;
    [window addSubview:showview];
    
    UILabel *label = [[UILabel alloc]init];
    CGSize LabelSize = [message sizeWithFont:[UIFont systemFontOfSize:17] constrainedToSize:CGSizeMake(290, 9000)];

    label.frame = CGRectMake(10, 5, LabelSize.width, LabelSize.height);
    label.text = message;
    label.textColor = [UIColor whiteColor];
    label.textAlignment = 1;
    label.backgroundColor = [UIColor clearColor];
    label.font = [UIFont boldSystemFontOfSize:15];
    [showview addSubview:label];
    showview.frame = CGRectMake((maxScreen_width - LabelSize.width - 20)/2, maxScreen_height - 100, LabelSize.width+20, LabelSize.height+10);
    [UIView animateWithDuration:3 animations:^{
        showview.alpha = 0;
    } completion:^(BOOL finished) {
        [showview removeFromSuperview];
    }];
}


- (IBAction)Btn_QRcode:(id)sender {
}
- (IBAction)btn:(id)sender {
}
@end
