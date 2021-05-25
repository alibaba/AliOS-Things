//
//  ModalDialogViewController.m
//  MyConfig
//
//  Created by CN3_SD9 on 2019/1/11.
//  Copyright © 2019年 Apple. All rights reserved.
//

#import <CoreBluetooth/CoreBluetooth.h>
#import "ModalDialogViewController.h"
#import "ViewController.h"

@interface ModalDialogViewController ()
@end

@implementation ModalDialogViewController

/*
#define RSSI_POSITION_CLOSE  65//RSSI 65 = 65 - 100 -dBm = -35 dBm
#define RSSI_POSITION_FAR    45//RSSI 45 = 45 - 100 -dBm = -55 dBm
*/
int _rssi_high = 65;
int _rssi_low  = 45;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender{
    NSLog(@"Current Segue: %@", segue.identifier);
   
}

- (IBAction)btn_clear_record:(id)sender
{

    NSString *Msg = [NSString stringWithFormat:@"Are you sure to clear all cached information?"];

    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Warning" message:Msg preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action){

    [ViewController clear_action];
        
    }];
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleDefault handler:nil];
    
    [alert addAction:cancelAction];
    [alert addAction:okAction];
    
    [self presentViewController:alert animated:YES completion:nil];
}

- (IBAction)btn_rssi_setting:(id)sender
{

    int rssi_dbm_high = _rssi_high - 100;
    int rssi_dbm_low = _rssi_low - 100;
    
    NSString *msg = @"";
    NSString *rssi_high = [NSString stringWithFormat:@"default RSSI(high) %d dBm", rssi_dbm_high];
    NSString *rssi_low = [NSString stringWithFormat: @"default RSSI(low)  %d dBm", rssi_dbm_low];
    
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"RSSI Configuration" message:nil preferredStyle:UIAlertControllerStyleAlert];
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
        NSLog(@"1:%@",[alert.textFields[0] text]);
        NSLog(@"2:%@",[alert.textFields[1] text]);
        //NSLog(@"3:%@",[alert.textFields[2] text]);
        
        NSString *h = [alert.textFields[0] text];
        NSString *l = [alert.textFields[1] text];
        
        //filter_BT_device = [alert.textFields[0] text];
        if([h intValue]<0)
            _rssi_high = [h intValue] + 100;
        if([l intValue]<0)
            _rssi_low  = [l intValue] + 100;
        
        [ViewController rssi_setting:_rssi_high:_rssi_low];
        
    }];
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleDefault handler:nil];
        
    [alert addAction:cancelAction];
    [alert addAction:okAction];
    
    [self presentViewController:alert animated:YES completion:nil];
}


/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
