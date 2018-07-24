
gincs = Split('''
        ./../
        ./include
        ./platform/include
    ''')

incs = Split('''
        ../irot/include
        ./include
        ./platform/include
        ./src
        .
    ''')

src = Split('''
        ./src/util/util.c
        ./src/log/log.c
        ./src/alg/alg.c
        ./src/core/otp.c
        ./src/core/km_impl.c
        ./src/core/id2_client.c
        ./platform/aos/irot_pal.c
    ''')

comp = aos_component('id2', src)
comp.add_global_includes(*gincs)
comp.add_includes(*incs)
comp.add_comp_deps("utility/base64")
comp.add_comp_deps("security/alicrypto")

