src = Split('''
    wsf.c 
    wsf_list.c 
    wsf_network.c 
    wsf_client.c 
    wsf_msg.c
	wsf_ssl_cert.c 
    wsf_config.c 
    wsf_msg_queue.c 
    wsf_worker.c
    connectivity_manager.c
''')

component = aos_component('wsf', src)

component.add_macro('DEBUG')