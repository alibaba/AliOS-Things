#all targets defined here will be built
buildtargets = {
    'mk3060-helloworld': {
        'buildargs': 'helloworld@mk3060',
        'bincopy': ["out/helloworld@mk3060/binary/helloworld@mk3060.bin", "helloworld@mk3060-general.bin"]
    },
    'mk3060-mqttapp': {
        'buildargs': 'mqttapp@mk3060',
        'bincopy': ["out/mqttapp@mk3060/binary/mqttapp@mk3060.bin", "mqttapp@mk3060-general.bin"],
        'keyinfo':{
            'keyfile': "example/mqttapp/mqtt_example.c",
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
            'keyfile': "example/linkkitapp/linkkit_example_solo.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a12iufuXqUA',
                'PRODUCT_SECRET': 'tbZ7PUfV9VY2rwCl',
                'DEVICE_NAME': 'linkkit_mk3060_test01',
                'DEVICE_SECRET': 'bvachR4vkApeZFrcQeZfkMTqc5oaBlJ1'
            }
        },
    },
    #'mk3080-helloworld': {
    #    'buildargs': 'helloworld@mk3080',
    #    'bincopy': ["out/helloworld@mk3080/binary/helloworld@mk3080.ota.bin", "helloworld@mk3080-general.bin"]
    #},
    #'mk3080-mqttapp': {
    #    'buildargs': 'mqttapp@mk3080',
    #    'bincopy': ["out/mqttapp@mk3080/binary/mqttapp@mk3080.ota.bin", "mqttapp@mk3080-general.bin"],
    #    'keyinfo':{
    #        'keyfile': "example/mqttapp/mqtt_example.c",
    #        'keyvalue': {
    #            'PRODUCT_KEY': 'a16GcXEm4uG',
    #            'PRODUCT_SECRET': '7YgRyCVZNqAgq5x2',
    #            'DEVICE_NAME': 'mqtt_mk3080_test01',
    #            'DEVICE_SECRET': 'd9XY9WsMpPlsKSzobR8DYLjEh2RMjPKW'
    #        }
    #    }
    #},
    #'mk3080-linkkitapp': {
    #    'buildargs': 'linkkitapp@mk3080 pvtest=y',
    #    'bincopy': ["out/linkkitapp@mk3080/binary/linkkitapp@mk3080.ota.bin", "linkkitapp@mk3080-general.bin"],
    #    'keyinfo':{
    #        'keyfile': "example/linkkitapp/linkkit_example_solo.c",
    #        'keyvalue': {
    #            'PRODUCT_KEY': 'a12iufuXqUA',
    #            'PRODUCT_SECRET': 'tbZ7PUfV9VY2rwCl',
    #            'DEVICE_NAME': 'linkkit_mk3080_test01',
    #            'DEVICE_SECRET': 'MP9HhSK9PGXFYc1CQBca250HJAm3YNzN'
    #        }
    #    },
    #},
    'esp32-helloworld': {
        'buildargs': 'helloworld@esp32devkitc',
        'bincopy': ["out/helloworld@esp32devkitc/binary/helloworld@esp32devkitc.bin", "helloworld@esp32-general.bin"]
    },
    'esp32-mqttapp': {
        'buildargs': 'mqttapp@esp32devkitc',
        'bincopy': ["out/mqttapp@esp32devkitc/binary/mqttapp@esp32devkitc.bin", "mqttapp@esp32-general.bin"],
        'keyinfo':{
            'keyfile': "example/mqttapp/mqtt_example.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a1VmpWRfdCb',
                'PRODUCT_SECRET': 'iAOf2332l2haSn9i',
                'DEVICE_NAME': 'mqtt_esp32_test01',
                'DEVICE_SECRET': 'eQucRNkFJ4SxkfVja1o70K1Oyp2xC2mP'
            }
        }
    },
    'esp32-linkkitapp': {
        'buildargs': 'linkkitapp@esp32devkitc pvtest=y',
        'bincopy': ["out/linkkitapp@esp32devkitc/binary/linkkitapp@esp32devkitc.bin", "linkkitapp@esp32-general.bin"],
        'keyinfo':{
            'keyfile': "example/linkkitapp/linkkit_example_solo.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a1VmpWRfdCb',
                'PRODUCT_SECRET': 'iAOf2332l2haSn9i',
                'DEVICE_NAME': 'linkkit_esp32_test01',
                'DEVICE_SECRET': '9TZvVGnjgfrnB5lYjIraHT8DKiY99im5'
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
            'keyfile': "example/mqttapp/mqtt_example.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a1VmpWRfdCb',
                'PRODUCT_SECRET': 'iAOf2332l2haSn9i',
                'DEVICE_NAME': 'mqtt_esp8266_test01',
                'DEVICE_SECRET': 's5DxqBDJhhHVgdBW4KFNmO2zq17wO6O9'
            }
        }
    },
    'esp8266-linkkitapp': {
        'buildargs': 'linkkitapp@esp8266 pvtest=y',
        'bincopy': ["out/linkkitapp@esp8266/binary/linkkitapp@esp8266.bin", "linkkitapp@esp8266-general.bin"],
        'keyinfo':{
            'keyfile': "example/linkkitapp/linkkit_example_solo.c",
            'keyvalue': {
                'PRODUCT_KEY': 'a1VmpWRfdCb',
                'PRODUCT_SECRET': 'iAOf2332l2haSn9i',
                'DEVICE_NAME': 'linkkit_esp8266_test01',
                'DEVICE_SECRET': 'GeWPjX7IkZ0Nb4Nh58VeIATKq9KHAuQC'
            }
        },
    },
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
            'linkkit_connect': {'script':'linkkit_connect_test.py', 'firmware_prefix':'linkkitapp@', 'firmware_suffix': '-general.bin'},
            'mqtt_connect': {'script':'mqtt_connect_test.py', 'firmware_prefix':'mqttapp@', 'firmware_suffix': '-general.bin'},
            'coap_connect': {'script':'coap_connect_test.py', 'firmware_prefix':'coapapp@', 'firmware_suffix': '-general.bin'},
            },
        'wifissid': 'aos_test_01',
        'wifipass': 'Alios@Embedded',
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
       #['basic', 'helloworld'],
       #['link', 'linkkit_connect'],
       #['link', 'mqtt_connect'],
       ],
   'esp32': [
       ['basic', 'helloworld'],
       ['link', 'linkkit_connect'],
       ['link', 'mqtt_connect'],
       ],
   'esp8266': [
       ['basic', 'helloworld'],
       #['link', 'linkkit_connect'],
       #['link', 'mqtt_connect'],
       ],
    'sv6266': [
       ['basic', 'helloworld'],
       ],
    'rda5981': [
       #['basic', 'helloworld'],
       ],
}

