import * as bt_host from 'bt_host' 



function onConnect() {
    // onConnect
	console.log('bt_host onConnect');
}

function onDisconnect() {
    // onDisconnect
	console.log('bt_host onDisconnect');
	app_start_adv();
}

function app_add_service() {

	let service = {
			service: "{\
	        \"s_uuid\":\"0x1A1A\",\
	        \"chars_list\":[\
	            {\
	                \"char_uuid\":\"0x1B1B\",\
	                \"char_permit\":\"RW\",\
	                \"char_descr\":{\
	                    \"descr_type\":\"CCC\",\
	                    \"descr_uuid\":\"0x1C1C\"\
	                }\
	            },\
	            {\
	                \"char_uuid\":\"0x1D1D\",\
	                \"char_permit\":\"R\"\
	            }\
	        ]\
	    }"
	}
	bt_host_instant.add_service(service);
			
}

function app_start_adv() {
	bt_host_instant.start_adv({
		type: 0,
		adv_data: '020106',
		scan_rsp_data: '',
		interval_min: 160,
		interval_max: 320,
		channel_map: 7,
		success: function() {
			console.log('bt_host start adv success');
			setTimeout( function() {
				app_stop_adv();
			}, 30000)
		},
		failed: function() {
			console.log('bt_host start adv failed');
		},
	});
}

function app_stop_adv() {
	bt_host_instant.stop_adv({
		success: function() {
			console.log('bt_host stop adv success');
			setTimeout( function() {
				app_start_scan();
			}, 2000)
		},
		failed: function() {
			console.log('bt_host stop adv failed');
		},
	});
}

function app_start_scan() {
	bt_host_instant.start_scan({
		type: 0,
		interval: 200,
		window: 50,
		success: function() {
			console.log('bt_host start scan success');
			setTimeout( function() {
				app_stop_scan();
			}, 10000)
		},
		failed: function() {
			console.log('bt_host stop scan failed');
		},
	});
}

function app_stop_scan() {
	bt_host_instant.stop_scan({
		success: function() {
			console.log('bt_host stop scan success');
			setTimeout( function() {
				app_start_adv();
			}, 2000)
		},
		failed: function() {
			console.log('bt_host stop scan failed');
		},
	});
}

// create bt host
var bt_host_instant = bt_host.open({
  deviceName: 'ble_test',
  conn_num_max: 2,
  success: function() {
    console.log('bt_host init success');
	setTimeout(() => {
		app_add_service()
		app_start_adv();
		}, 1000)
//    app_start_adv();
  },
  fail: function() {
    console.log('bt_host init failed');
  }
});

bt_host_instant.on('connect', function() {
    onConnect();
});

bt_host_instant.on('disconnect', function() {
  	onDisconnect();
});

bt_host_instant.on('onCharWrite', function() {
    console.log('app: onCharWrite');
});


