CHIPNAME = "chip_template"

gincs = Split('''
    ''')

incs = Split('''
        ../include
        ./src
        ./chipset/%s/include
        ./chipset/%s/
    '''%(CHIPNAME, CHIPNAME))

src = Split('''
        ./src/core/km_to_irot.c
        ./src/core/std_se_adapter.c
        ./src/core/mtk_se_adapter.c
        ./src/log/chiplog.c
        ./chipset/%s/irot_impl/irot_hal.c
        ./chipset/%s/se_driver_impl/se_driver.c
    '''%(CHIPNAME, CHIPNAME))

comp = aos_component('se', src)
comp.add_global_includes(*gincs)
comp.add_includes(*incs)

