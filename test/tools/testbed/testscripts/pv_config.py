#all targets defined here will be built
buildtargets = {
    #'mk3060-coapapp': {
    #    'buildargs': 'coapapp@mk3060',
    #    'bincopy': ["out/coapapp@mk3060/binary/coapapp@mk3060.bin", "coapapp@mk3060-general.bin"],
    #    'keyinfo':{
    #        'keyfile': "app/example/coapapp/coap_example.c",
    #        'keyvalue': {
    #            'IOTX_PRODUCT_KEY': 'a16GcXEm4uG',
    #            'IOTX_DEVICE_NAME': 'coap_mk3060_test01',
    #            'IOTX_DEVICE_SECRET': 'u4OcpastxpK91HMN06XrhrZYpNeE3ysq',
    #            'IOTX_DEVICE_ID': 'coap_mk3060_test01'
    #        }
    #    }
    #},
    'mk3060-helloworld': {
        'buildargs': 'helloworld@mk3060',
        'bincopy': ["out/helloworld@mk3060/binary/helloworld@mk3060.bin", "helloworld@mk3060-general.bin"]
    },
    'mk3060-mqttapp': {
        'buildargs': 'mqttapp@mk3060',
        'bincopy': ["out/mqttapp@mk3060/binary/mqttapp@mk3060.bin", "mqttapp@mk3060-general.bin"],
        'keyinfo':{
            'keyfile': "app/example/mqttapp/mqtt_example.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a16GcXEm4uG',
                'PRODUCT_SECRET': '7YgRyCVZNqAgq5x2',
                'DEVICE_NAME': 'mqtt_mk3060_test01',
                'DEVICE_SECRET': 'wJBI866M9aA22oY97eQnY3fI022rSjOs'
            }
        }
    },
    'mk3060-linkkitapp': {
        'buildargs': 'linkkitapp@mk3060 pvtest=y',
        'bincopy': ["out/linkkitapp@mk3060/binary/linkkitapp@mk3060.bin", "linkkitapp@mk3060-general.bin"],
        'keyinfo':{
            'keyfile': "app/example/linkkitapp/linkkit_example_solo.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a12iufuXqUA',
                'PRODUCT_SECRET': 'tbZ7PUfV9VY2rwCl',
                'DEVICE_NAME': 'linkkit_mk3060_test01',
                'DEVICE_SECRET': 'bvachR4vkApeZFrcQeZfkMTqc5oaBlJ1'
            }
        },
    },
    'mk3080-helloworld': {
        'buildargs': 'helloworld@mk3080',
        'bincopy': ["out/helloworld@mk3080/binary/helloworld@mk3080.ota.bin", "helloworld@mk3080-general.bin"]
    },
    'mk3080-mqttapp': {
        'buildargs': 'mqttapp@mk3080',
        'bincopy': ["out/mqttapp@mk3080/binary/mqttapp@mk3080.ota.bin", "mqttapp@mk3080-general.bin"],
        'keyinfo':{
            'keyfile': "app/example/mqttapp/mqtt_example.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a16GcXEm4uG',
                'PRODUCT_SECRET': '7YgRyCVZNqAgq5x2',
                'DEVICE_NAME': 'mqtt_mk3080_test01',
                'DEVICE_SECRET': 'd9XY9WsMpPlsKSzobR8DYLjEh2RMjPKW'
            }
        }
    },
    'mk3080-linkkitapp': {
        'buildargs': 'linkkitapp@mk3080 pvtest=y',
        'bincopy': ["out/linkkitapp@mk3080/binary/linkkitapp@mk3080.ota.bin", "linkkitapp@mk3080-general.bin"],
        'keyinfo':{
            'keyfile': "app/example/linkkitapp/linkkit_example_solo.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a12iufuXqUA',
                'PRODUCT_SECRET': 'tbZ7PUfV9VY2rwCl',
                'DEVICE_NAME': 'linkkit_mk3080_test01',
                'DEVICE_SECRET': 'MP9HhSK9PGXFYc1CQBca250HJAm3YNzN'
            }
        },
    },
    'esp8266-helloworld': {
        'buildargs': 'helloworld@esp8266',
        'bincopy': ["out/helloworld@esp8266/binary/helloworld@esp8266.bin", "helloworld@esp8266-general.bin"],
    },
    'esp8266-mqttapp': {
        'buildargs': 'mqttapp@esp8266',
        'bincopy': ["out/mqttapp@esp8266/binary/mqttapp@esp8266.bin", "mqttapp@esp8266-general.bin"],
        'keyinfo':{
            'keyfile': "app/example/mqttapp/mqtt_example.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a16GcXEm4uG',
                'PRODUCT_SECRET': '7YgRyCVZNqAgq5x2',
                'DEVICE_NAME': 'mqtt_esp8266_test01',
                'DEVICE_SECRET': 'i3aO87beMtgxIhNKu7BooBXrEjTe4FDH'
            }
        }
    },
    #'esp8266-linkkitapp': {
    #    'buildargs': 'linkkitapp@esp8266 pvtest=y',
    #    'bincopy': ["out/linkkitapp@esp8266/binary/linkkitapp@esp8266.bin", "linkkitapp@esp8266-general.bin"],
    #    'keyinfo':{
    #        'keyfile': "app/example/linkkitapp/linkkit_example_solo.c",
    #        'keyvalue': {
    #            'PRODUCT_KEY': 'a12iufuXqUA',
    #            'PRODUCT_SECRET': 'tbZ7PUfV9VY2rwCl',
    #            'DEVICE_NAME': 'linkkit_esp8266_test01',
    #            'DEVICE_SECRET': 'rJrxKebZODECZUNNNwYXonojdhTOL7sU'
    #        }
    #    },
    #},
    'sv6266-helloworld': {
        'buildargs': 'helloworld@sv6266_evb',
        'bincopy': ["out/helloworld@sv6266_evb/binary/helloworld@sv6266_evb.bin", "helloworld@sv6266-general.bin"]
    },
    'rda5981-helloworld': {
        'buildargs': 'helloworld@uno-91h',
        'bincopy': ["out/helloworld@uno-91h/binary/helloworld@uno-91h.bin", "helloworld@rda5981-general.bin"]
    },
}

testsuits = {
    'basic': {
        'tests': {
            'helloworld': {'script': 'helloworld_test.py', 'firmware_prefix':'helloworld@', 'firmware_suffix': '-general.bin'},
            'kernel': {'script': 'kernel_test.py', 'firmware_prefix':'kernel@', 'firmware_suffix': '-general.bin'},
            }
        },
    'benchmark': {
        'tests': {
            'realtime': {'script': 'benchmark.py', 'firmware_prefix':'benchmark.realtime_test@', 'firmware_suffix': '-general.bin'},
            'realtimesysstats': {'script': 'benchmark.py', 'firmware_prefix':'benchmark.realtime_test@', 'firmware_suffix': '-sysstats-general.bin'},
            'pwr': {'script': 'benchmark.py', 'firmware_prefix':'benchmark.pwr_test@', 'firmware_suffix': '-general.bin'},
            }
        },
    'link': {
        'tests':{
            'linkkit2pps': {'script':'linkkit_test.py', 'firmware_prefix':'linkkitapp@', 'firmware_suffix': '-general.bin', 'args':'--testname=short2pps','dependencies':['device.jar']},
            'alink5pps': {'script':'alink_testrun.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin', 'args':'--testname=short5pps'},
            'mesh5pps': {'script':'alink_mesh_testrun.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin', 'args':'--testname=short5pps'},
            'linkkit_connect': {'script':'linkkit_connect_test.py', 'firmware_prefix':'linkkitapp@', 'firmware_suffix': '-general.bin'},
            'mqtt_connect': {'script':'mqtt_connect_test.py', 'firmware_prefix':'mqttapp@', 'firmware_suffix': '-general.bin'},
            'coap_connect': {'script':'coap_connect_test.py', 'firmware_prefix':'coapapp@', 'firmware_suffix': '-general.bin'},
            },
        'wifissid': 'aos_test_01',
        'wifipass': 'Alios@Embedded',
        },
    'mesh': {
        'tests': {
            'line_topology': {'script':'line_topology_v4_test.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin'},
            'tree_topology': {'script': 'tree_topology_v4_test.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin'},
            'multicast': {'script': 'multicast_v4_test.py', 'firmware_prefix':'alinkapp@', 'firmware_suffix': '-general.bin'},
            },
        'wifissid': 'alibaba-test-1227981',
        'wifipass': 'Alios@Things',
        },
    'net': {
        'tests': {
            'domain': {'script': 'network_v4_test.py', 'firmware_prefix':'networkapp@', 'firmware_suffix': '-general.bin'},
            },
        'wifissid': 'alibaba-test-1227982',
        'wifipass': 'Alios@Things',
        },
    'auth': {
        'tests': {
            'meshauth': {'script': 'auth_v4_test.py', 'firmware_prefix':'meshapp@', 'firmware_suffix': '-meshauth.bin'},
            }
        },
    'ut': {
        'tests': {
            'ut': {'script': 'ut_test.sh', 'firmware_prefix': 'yts@', 'firmware_suffix':'.elf'},
            }
        },

    'activation': {
        'tests': {
            'acapp': {'script': 'acapp_test.py', 'firmware_prefix': 'acapp@', 'firmware_suffix':'-general.bin'},
            }
        },
    'autoywss': {
        'tests': {
            'linkkitapp': {'script':'autoywss.py', 'firmware_prefix':'linkkitapp@', 'firmware_suffix': '-autoywss.bin', 'dependencies':['autoywss.cfg']},
            },
        },
    'autozconfig': {
        'tests': {
            'linkkitapp': {'script':'autoywss_zconfig.py', 'firmware_prefix':'linkkitapp@', 'firmware_suffix': '-autoywss.bin'},
            },
        'wifissid': 'Yuemewifi-3766',
        'wifipass': 'aos12345',
        },
}

models = {
   'mk3060': [
       ['basic', 'helloworld'],
       ['link', 'linkkit_connect'],
       ['link', 'mqtt_connect'],
       #['link', 'coap_connect'],
       ],
    'mk3080': [
       ['basic', 'helloworld'],
       ['link', 'linkkit_connect'],
       ['link', 'mqtt_connect'],
       ],
   'esp8266': [
       ['basic', 'helloworld'],
       #['link', 'linkkit_connect'],
       ['link', 'mqtt_connect'],
       ],
    'sv6266': [
       ['basic', 'helloworld'],
       ],
    'rda5981': [
       ['basic', 'helloworld'],
       ],
}

