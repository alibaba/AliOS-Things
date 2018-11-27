Contents

mal
├── CMakeLists.txt
├── mal.c
├── mal.h
├── mal.mk
├── mdal_mal_export.h
├── mdal_mal_import.h 
└── ica
     ├── mdal_ica_at_client.c
     ├── mdal_ica_at_client.h
     ├── ica.mk
     └── test
         ├── mdal_ica_at_client_test.c
         └── mdal_ica_at_client_test.h 

Introduction
    MAL is short for MQTT Adapter Layer.

Features
     MAL(MQTT Adapter Layer) is a feature to use modem mqtt module to communicate with a mqtt server.

Dependencies

API
   MAL API details see mal/mdal_mal_export.h.
   int iotx_mdal_get_auth_username_passwd(char* out_username, char* out_password);

   Get auth username and password.

HAL
   MAL HAL details see mal/mdal_mal_import.h.
