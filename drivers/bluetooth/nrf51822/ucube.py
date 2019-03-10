src =Split(''' 
    nrf51822.c
''')
component =aos_component('hci_h4_nrf51822', src)



global_macros =Split(''' 
    HCI_H4_NRF51822
''')
for i in global_macros:
    component.add_global_macros(i)



