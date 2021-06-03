import * as BT_HOST from 'BT_HOST'
import * as events from 'events'

class bt_host extends events.EventEmitter{
	constructor(options) {
		super();
        if (!options) {
            throw new Error('options is invalid');
        }

		if (this.inited == true) {
			throw new Error('bt_host already inited');
		}
        this.options = {
            deviceName: options.deviceName,
            conn_num_max: options.conn_num_max,
        };

		this.success = options.success|| function(){};
		this.failed = options.failed|| function(){};

		this._init();
	}

	_init() {
		console.log('bt_host native init')
		let result = BT_HOST.init(this.options);
		if (result == 0) {
			this.inited = true;
			console.log('bt host inited');
			this.success();
		} else {
			console.log('bt host init failed');
			this.failed();
		}
	}

	start_adv(params) {
		console.log('bt host start adv')
		if (this.inited == false) {
			throw new Error('bt host not initialed');
		}
	
		if (this.adv_flag == true) {
			this.stop_adv()
		}

		let result = BT_HOST.start_adv(params, function(conn_handle, connect){
			console.log('connect callback'/*+conn_handle+connect*/);
			if (connect) {
				this.adv_flag = false
				this.emit('connect', conn_handle);
			} else {
				this.emit('disconnect', conn_handle);
			}
    }.bind(this));
		if (result == 0) {
			this.adv_flag = true;
			if (params.success) {
				params.success();
			}
		} else {
			if (params.failed) {
				params.failed();
			}
		}
	}

	stop_adv(params) {
		if (this.inited == false) {
			throw new Error('bt host not initialed');
		}

		if (this.adv_flag == false) {
			return;
		}

		let result = BT_HOST.stop_adv();
		if (result == 0) {
			if (params.success) {
				params.success();
			}
			this.adv_flag = false;
		} else {
			if (params.failed) {
				params.failed();
			}
		}
	}

	start_scan(params) {
		console.log('bt host start scan')
		if (this.inited == false) {
			throw new Error('bt host not initialed');
		}
	
		if (this.scan_flag == true) {
			stop_scan()
		}

		let result = BT_HOST.start_scan(params, function(addr, addr_type, adv_type, adv_data, rssi){
			console.log('scan result callback'+' addr:'+addr+' data:'+adv_data);
        }.bind(this));
		if (result == 0) {
			this.scan_flag = true;
			if (params.success) {
				params.success();
			}
		} else {
			if (params.failed) {
				params.failed();
			}
		}
	}

	stop_scan(params) {
		if (this.inited == false) {
			throw new Error('bt host not initialed');
		}

		if (this.scan_flag == false) {
			return;
		}

		let result = BT_HOST.stop_scan();
		if (result == 0) {
			this.scan_flag = false;
			if (params.success) {
				params.success();
			}
		} else {
			if (params.failed) {
				params.failed();
			}
		}
	}

	add_service(params) {
		if (this.inited == false) {
			throw new Error('bt host not initialed');
		}
		console.log('srvc_cfg: ' + params.service);
		let result = BT_HOST.add_service(params.service, function(len, data){
			console.log('add_service: len: ' + len + ', data: ' + data);
			this.emit('onCharWrite', data);
		}.bind(this));
		console.log('add_service result: ' + result);
		if (result == 0) {
			if (params.success) {
				params.success();
			}
		} else {
			if (params.failed) {
				params.failed();
			}
		}
	}

	update_char(params) {
		if (this.inited == false) {
			throw new Error('bt host not initialed');
		}
		let result = BT_HOST.update_chars(params.arg);
		if (result == 0) {
			if (params.success) {
				params.success();
			}
		} else {
			if (params.failed) {
				params.failed();
			}
		}
	}
}

export function open(options) {
    return new bt_host(options);
}
