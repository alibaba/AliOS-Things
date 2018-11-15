Contents

mdal/
├── README.md
├── mal
│   ├── CMakeLists.txt
│   ├── mal.c
│   ├── mal.h
│   ├── mal.mk
│   ├── mdal_mal_export.h
│   ├── mdal_mal_import.h 
│   └── ica
│        ├── mdal_ica_at_client.c
│        ├── mdal_ica_at_client.h
│        ├── ica.mk
│        └── test
│             ├── mdal_ica_at_client_test.c
│             └── mdal_ica_at_client_test.h 
└── sal
    ├── sal_import.h
    ├── sal.mk
    ├── include
    │     ├── sal_arch.h
    │     ├── sal_def.h
    │     ├── sal_err.h
    │     ├── sal_ipaddr.h
    │     ├── sal_sockets.h
    │     └── internal
    │          ├── sal_arch_internal.h
    │          ├── sal_pcb.h
    │          ├── sal_sockets_internal.h
    │          └── sal_util.h
    └── src
         ├── ip4_addr.c
         ├── sal_arch_aos.c
         ├── sal_arch.c
         ├── sal_err.c
         ├── sal_sockets.c
         └── sal_util.c

Introduction
    MDAL(Modem Adapter Layer) contains two part, SAL and MAL.

Features
    SAL(Socket Adapter Layer) is a feature to use modem network module to communicate with a network server.
    MAL(MQTT Adapter Layer) is a feature to use modem mqtt module to communicate with a mqtt server.

Dependencies

API
   MAL API details see mal/mdal_mal_export.h.
   int iotx_mdal_get_auth_username_passwd(char* out_username, char* out_password);

   Get auth username and password.

HAL
   MAL HAL details see mal/mdal_mal_import.h.
