# ESP_CRYPTOAUTH_UTILITY
# Description
 The python utility helps to configure and provision `ATECC608A` chip on `ESP32-WROOM-32SE` module.
    There are currently three types of ATECC608A which are [Trust & Go](https://www.microchip.com/wwwproducts/en/ATECC608A-TNGTLS), [TrustFlex](https://www.microchip.com/wwwproducts/en/ATECC608A-TFLXTLS) and [TrustCustom](https://www.microchip.com/wwwproducts/en/ATECC608A). `Trust & Go` and `TrustFlex` chips are preconfigured by the manufacturer (Microchip) so we only need to generate manifest file for those chips. `TrustCustom` type of chips are not configured, so for `TrustCustom` type of chips need to be first configured and then provisioned with a newly  generated device certificate and key pair. The script automatically detects which type of ATECC608A chip is integrated with `ESP32-WROOM-32SE` so it will proceed to next required steps on its own.

# Hardware Required
It requires [ESP32-WROOM-32SE](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32se_datasheet_en.pdf) which has Microchip's [ATECC608A](https://www.microchip.com/wwwproducts/en/ATECC608A) (Secure Element) integrated on the module.

> Note: It is recommended to change directory to `esp_cryptoauth_utility` to execute all following commands if not already done.

## Install python dependancies
To use the utility some python depencancies must be installed with following command(current directory should be `esp_cryptoauth_utility` for executing the command).

    pip install -r requirements.txt

## Step 1:- Generate Signer Certificate
Signer cert and key pair:
* In case of `TrustCustom` chips ,these certificate and key pair are used to sign the device cert which is going to be generted.

* In case of `Trust & Go` and `TrustFlex` devices the device certs are already signed by microchip signer cert, and the signer cert and key pair generated in this step are used to sign the manifest file.

By default the utility uses the `sample_signer_cert.pem` located in the `sample_certs` folder.if you want to keep using default certificats, then directly proceed to next step(Step 2).

Create a signer key and signer cert by executing following commands sequentially. The second command will ask some details about certificate such as `ORG, CN` which are needed to be filled by the user.

 `Important`: The signer cert `CN`_(Common Name)_ must end with `FFFF` as it is required by the `cert2certdef.py` (file by microchip) to create its definition properly. for e.g valid CN = `Sample Signer FFFF`( This is compulsory only in case of `TrustCustom` type of chips and not for the other two).

    openssl ecparam -out signerkey.pem -name prime256v1 -genkey

    openssl req -new -x509 -key signerkey.pem -out signercert.pem -days 365

## Step 2:- Provision the module/Generate manifest file

*   The tool will automatically detect the type of ATECC608A chip connected to ESP module and perform its intended task which are as follows.

    * For `TrustCustom` type of ATECC608A chip first configure ATECC608A chip with its default configuration options.The tool will create a device cert by generating a private key on slot 0 of the module, passing the CSR to host, sign the CSR with signer cert generated in step above. To set validity of device cert please refer [device_cert_validity](README.md#set-validity-of-device-cert-for-trustcustom). save the device cert on the ATECC chip as well as on the host machine as `device_cert.pem`,it also saves the cert definitions in `output_files` folder for future use.

    * For `Trust & Go` and `TrustFlex` type of ATECC608A devices this script will generate the manifest file with the name of chip serial number.The manifest file will be signed with the signer cert generated above. The generated manifest file should be registered with the cloud to register the device certificate.

The command is as follows,

```
python secure_cert_mfg.py --signer-cert signercert.pem --signer-cert-private-key signerkey.pem --port /UART/COM/PORT
```
> Note: The names `signercert.pem` and `signerkey.pem` denote the name of the signer cert and key files respectively, you can replace them with `relative/path/to/you/signer/cert` and `key` respectively. The `UART/COM/PORT` represents the host machine COM port to which your ESP32-WROOM-32SE is connected.Please refer [check serial port](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/establish-serial-connection.html#check-port-on-windows) for obtaining the serial port connected to ESP.

If you do not provide `signer-cert` and `signer_key` in above command, `sample_signer_cert.pem` stored at `sample_certs` will be used.

---
### Find type of ATECC608A chip connected to ESP32-WROOM32-SE.
The command is as follows,
    python secure_cert_mfg.py --port /serial/port --type
It will print the type of ATECC608A chip connected to ESP32-WROOM-32SE on console.

### Set validity of device cert for TrustCustom
The validity (in years) of device certificate generated for `TrustCustom` chips from the time of generation of cert can be set with `--valid-for-years` option.Please refer following example

    python secure_cert_mfg.py --port /serial/port --valid-for-years /Years
>Note: If `--valid-for-years` is not provided then default value for validity of certiticates will be used, which is 40 years.

### Using ATECC608A with ESP32-WROOM-32
This is not recommended(as it requires manual connections, and user may easily be confused), but surely possible.`ATECC608A` uses `I2C` interface for serial communication so you will have to make required `I2C` connections such as `SCK, SDA, GND, VDD` between `ESP` and `ATECC608A`.
* If you are using [ESP32-DevKitC](https://www.espressif.com/en/products/devkits/esp32-devkitc/resources) and connected ATECC608A to pins 21 (SDA), and 22(SCK) then you can directly use a prebuilt binary. The command is
```
python secure_cert_mfg.py --flash sample_bins/secure_cert_mfg_esp32.bin
```
