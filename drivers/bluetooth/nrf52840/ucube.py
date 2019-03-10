src =Split(''' 
    nrf52840.c
''')
component =aos_component('hci_h4_nrf52840', src)



global_macros =Split(''' 
    HCI_H4_NRF52840
''')
for i in global_macros:
    component.add_global_macros(i)



