Overview
********

This example demonstrates how to leverage the Alibaba ble mesh
profile APIs to support provisioning with Tmall Genie (as provisioner)
over PB-GATT bearer and join the bluetooth low energy mesh network.

Alibaba ble mesh profile APIs are encapsulated based on ported Zephyr
bluetooth mesh stack. If vendor has their own ble mesh stack, just need to
reimplement the related APIs based on their own mesh stack instead.

The ble mesh profile APIs could be found in the directory:
/framework/bluetooth/mesh_profile/mesh_profile.h

Prerequisites:
*************

Apply for the triple-elements: <product id, device name, device key, product key>
from Alibaba first. Among of these triple-elemetns, device name will be used to
construct the uuid as mac address. And other elements will be used to calcualte
the sha256 of static oob information.

