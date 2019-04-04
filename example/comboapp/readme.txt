
1.modify device&product info in combo_net_devinfo.h 

2.make:
    For platform using vendor's ble stack, please add veriable btstack as follows, take bk7231u as example:
    aos make clean
    aos make comboapp@bk7231udevkitc btstack=vendor

