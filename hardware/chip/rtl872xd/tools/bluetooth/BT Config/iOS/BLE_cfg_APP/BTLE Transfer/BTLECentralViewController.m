/*
 
 File: LECentralViewController.m
 
 Abstract: Interface to use a CBCentralManager to scan for, and receive
 data from, a version of the app in Peripheral Mode
 
 Version: 1.0
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by
 Apple Inc. ("Apple") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Apple software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc.
 may be used to endorse or promote products derived from the Apple
 Software without specific prior written permission from Apple.  Except
 as expressly stated in this notice, no other rights or licenses, express
 or implied, are granted by Apple herein, including but not limited to
 any patent rights that may be infringed by your derivative works or by
 other works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2012 Apple Inc. All Rights Reserved.
 
 */

#import "BTLECentralViewController.h"
#import <CoreBluetooth/CoreBluetooth.h>

#import "TransferService.h"

@interface BTLECentralViewController () <CBCentralManagerDelegate, CBPeripheralDelegate>

@property (strong, nonatomic) CBCentralManager      *centralManager;
@property (strong, nonatomic) CBPeripheral          *discoveredPeripheral;
@property (strong, nonatomic) NSMutableData         *data;

@end



@implementation BTLECentralViewController



#pragma mark - View Lifecycle


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Start up the CBCentralManager
    _centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    
    // And somewhere to store the incoming data
    _data = [[NSMutableData alloc] init];
}

- (void)viewWillDisappear:(BOOL)animated
{
    // Don't keep it going while we're not showing.
    [self.centralManager stopScan];
    NSLog(@"Scanning stopped");
    
    [super viewWillDisappear:animated];
}



#pragma mark - Central Methods

/** centralManagerDidUpdateState is a required protocol method.
 *  Usually, you'd check for other states to make sure the current device supports LE, is powered on, etc.
 *  In this instance, we're just using it to wait for CBCentralManagerStatePoweredOn, which indicates
 *  the Central is ready to be used.
 */
- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    
    switch (central.state){
        case CBCentralManagerStatePoweredOn:
            NSLog(@"CBCentralManagerStatePoweredOn");
            // ... so start scanning
            [self scan];
            break;
        case CBCentralManagerStatePoweredOff:
            NSLog(@"CBCentralManagerStatePoweredOff");
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
    
    // The state must be CBCentralManagerStatePoweredOn...

    
    
}


/** Scan for peripherals - specifically for our service's 128bit CBUUID
 */
- (void)scan
{
    //[self.centralManager scanForPeripheralsWithServices:@[[CBUUID UUIDWithString:BTCONF_SERVICE_UUID]]
    //                                            options:@{ CBCentralManagerScanOptionAllowDuplicatesKey : @YES }];
    
    //NSArray *services = [NSArray arrayWithObjects:[CBUUID UUIDWithString:BTCONF_SERVICE_UUID], nil];
    [self.centralManager scanForPeripheralsWithServices:nil
                                                 options:@{ CBCentralManagerScanOptionAllowDuplicatesKey : @YES }];
    NSLog(@"Scanning started");
}


/** This callback comes whenever a peripheral that is advertising the TRANSFER_SERVICE_UUID is discovered.
 *  We check the RSSI, to make sure it's close enough that we're interested in it, and if it is, 
 *  we start the connection process
 */
- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    //NSLog(@"Target service discovered! RSSI.integerValue : %ld", (long)RSSI.integerValue);
    //NSLog(@"peripheral:%@\n%@\n",peripheral.services,peripheral);
    //NSLog(@"advertisementData\n%@\n",advertisementData);
    //NSLog(@"name:%@(%@) ID:%@ state:%ld",peripheral.name,RSSI,[peripheral.identifier UUIDString],(long)peripheral.state);
    
    //characteristic.UUID.UUIDString
    
#if 0
    NSString *newLog = [NSString stringWithFormat:@"Name: %@ ID:%@ RSSI:%@",peripheral.name,[peripheral.identifier UUIDString],RSSI];
    NSString *fullLog = [logTextView.text stringByAppendingString:newLog];
    [logTextView setText:fullLog];
#endif
    
    // Reject any where the value is above reasonable range
//    if (RSSI.integerValue > -15) {
//        return;
//    }
//        
//    // Reject if the signal strength is too low to be close enough (Close is around -22dB)
//    if (RSSI.integerValue < -35) {
//        return;
//    }
    NSString *deviceName = peripheral.name;
    if( ![deviceName isEqualToString:@"realtek_repeater" ] ) return;
    
#if 1
    // Ok, it's in range - have we already seen it?
    if (self.discoveredPeripheral != peripheral && peripheral.state == CBPeripheralStateDisconnected) {
        
        // Save a local copy of the peripheral, so CoreBluetooth doesn't get rid of it
        self.discoveredPeripheral = peripheral;
        
        // And connect
        //NSLog(@"Connecting to peripheral %@", peripheral);
        NSLog(@"Connecting to peripheral %@", peripheral);
        [self.centralManager connectPeripheral:peripheral options:nil];
    }
#endif
}


/** If the connection fails for whatever reason, we need to deal with it.
 */
- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSString *newLog = [NSString stringWithFormat:@"Failed to connect to %@. (%@)\n", peripheral, [error localizedDescription]];
    NSLog(@"%@", newLog);
    
    [self cleanup];
}


/** We've connected to the peripheral, now we need to discover the services and characteristics to find the 'transfer' characteristic.
 */
- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    NSString *newLog = [NSString stringWithFormat:@"Peripheral %@ Connected\n",peripheral.name];
    NSLog(@"%@", newLog);
    
    // Stop scanning
    [self.centralManager stopScan];
    NSLog(@"Scanning stopped");
    
    // Clear the data that we may already have
    [self.data setLength:0];

    // Make sure we get the discovery callbacks
    peripheral.delegate = self;
    
    // Search only for services that match our UUID
    //[peripheral discoverServices:@[[CBUUID UUIDWithString:BTCONF_SERVICE_UUID]]];
    [peripheral discoverServices:nil];
}


/** The Transfer Service was discovered
 */
//scan services after connected
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
    if (error) {
        NSString *newLog = [NSString stringWithFormat:@"Error discovering services: %@\n",[error localizedDescription]];
        NSLog(@"%@", newLog);
        
        [self cleanup];
        return;
    }
    
    // Discover the characteristic we want...
    NSString *newLog = [NSString stringWithFormat:@"Target %u service discovered:%@\n",(unsigned int)(peripheral.services.count),peripheral.services];
    NSLog(@"%@", newLog);
    
    //NSString *fullLog = [logTextView.text stringByAppendingString:newLog];
    //[logTextView setText:fullLog];
    
    // Loop through the newly filled peripheral.services array, just in case there's more than one.
    for (CBService *service in peripheral.services) {
        NSLog(@"Service found with UUID: %@\n",service.UUID);
        //[peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:BTCONF_SSID_PSK_CHR_UUID]] forService:service];
        //[peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:BTCONF_WIFI_STATE_CHR_UUID]] forService:service];
        //[peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"180E"]] forService:service];
        //[peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"1811"]] forService:service];
    }
}


/** The Transfer characteristic was discovered.
 *  Once this has been found, we want to subscribe to it, which lets the peripheral know we want the data it contains
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{
    // Deal with errors (if any)
    if (error) {
        NSString *newLog = [NSString stringWithFormat:@"1Error discovering characteristics: %@\n",[error localizedDescription]];
        NSLog(@"%@", newLog);
        
        [self cleanup];
        //return;
    }
    
    NSLog(@"===================================");
    
    // Discover the characteristic we want...
    //NSString *newLog = [NSString stringWithFormat:@"Target characteristic discovered!\n"];
    //NSLog(@"%@", newLog);
    //NSString *fullLog = [logTextView.text stringByAppendingString:newLog];
    //[logTextView setText:fullLog];
    
    // Again, we loop through the array, just in case.
    for (CBCharacteristic *chr in service.characteristics) {
#if 0
        // And check if it's the right one
        if ([chr.UUID isEqual:[CBUUID UUIDWithString:TRANSFER_CHARACTERISTIC_UUID]]) {
     
            // If it is, subscribe to it
            [peripheral setNotifyValue:YES forCharacteristic:chr];
        }
#else
        if(service.UUID == NULL || chr.UUID == NULL)return;
        
        NSLog(@"Service UUID %@ read characteristics %@", service.UUID,chr.UUID.UUIDString);
        
        if([chr.UUID.UUIDString isEqualToString:@"2A17"] ){
            //[peripheral setNotifyValue:YES forCharacteristic:chr];
            [peripheral readValueForCharacteristic:chr];
        }else{
            [peripheral readValueForCharacteristic:chr];
        }
        
        
#endif
    }
    
    // Once this is complete, we just need to wait for the data to come in.
}

/** This callback lets us know more data has arrived via notification on the characteristic
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (error) {
        NSString *newLog = [NSString stringWithFormat:@"2Error(%@) discovering characteristics: %@\n",characteristic.UUID.UUIDString,[error localizedDescription]];
        NSLog(@"%@", newLog);
        return;
    }
#if 0
    int i=0;
    NSRange search = {0,0};
    NSString *stringFromData = [[NSString alloc] initWithData:characteristic.value encoding:NSUTF8StringEncoding];
    NSString *wifiProfile = @"RTK8881A_92E_5G_Audio;123454321";

    NSLog(@"\nReceived:%@",stringFromData);
    
    // Have we got everything we need?
    
    search = [stringFromData rangeOfString:@"rtk_btconfig_ssid_psw"];
    if(search.location==0){
        // Write response SSID/PSK
        if ([characteristic.UUID.UUIDString isEqualToString:BTCONF_SSID_PSK_CHR_UUID ]) {
    
            wifiProfile = [NSString stringWithFormat:@"%@;%d",wifiProfile,i];
            NSLog(@"%d Writing(CHR:%@) value :%@", __LINE__,BTCONF_SSID_PSK_CHR_UUID,wifiProfile);
            [peripheral writeValue: [wifiProfile dataUsingEncoding:NSUTF8StringEncoding] forCharacteristic:characteristic
                              type:CBCharacteristicWriteWithResponse];
            /*
            while(i++<5){
                sleep(1);
                wifiProfile = [NSString stringWithFormat:@"%@;%d",wifiProfile,i];
                NSLog(@"%d Writing(CHR:%@) value :%@", __LINE__,BTCONF_SSID_PSK_CHR_UUID,wifiProfile);
                [peripheral writeValue: [wifiProfile dataUsingEncoding:NSUTF8StringEncoding] forCharacteristic:characteristic
                                  type:CBCharacteristicWriteWithResponse];
            }
             */
            
        }
    }
#endif

    
}

- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (error) {
        NSString *newLog = [NSString stringWithFormat:@"3Error(%@) discovering characteristics: %@\n",characteristic.UUID.UUIDString,[error localizedDescription]];
        NSLog(@"%@", newLog);
        return;
    }
}


/** The peripheral letting us know whether our subscribe/unsubscribe happened or not
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    
    NSLog(@">>>>Notification began on %@", characteristic);
    if (error) {
        NSString *newLog = [NSString stringWithFormat:@"Error changing notification state: %@\n",[error localizedDescription]];
        NSLog(@"%@", newLog);
    }
    
    // Exit if it's not the transfer characteristic
//    if (![characteristic.UUID isEqual:[CBUUID UUIDWithString:BTCONF_READ_SSID_CHARACTERISTIC_UUID]]) {
//        return;
//    }
    
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
    NSString *newLog = [NSString stringWithFormat:@"Peripheral Disconnected\n"];
    NSLog(@"%@", newLog);
    
    self.discoveredPeripheral = nil;
    
    // We're disconnected, so start scanning again
    [self scan];
}


/** Call this when things either go wrong, or you're done with the connection.
 *  This cancels any subscriptions if there are any, or straight disconnects if not.
 *  (didUpdateNotificationStateForCharacteristic will cancel the connection if a subscription is involved)
 */
- (void)cleanup
{
    // Don't do anything if we're not connected
    //if (!self.discoveredPeripheral.isConnected) {
    //    return;
    //}
}


@end
