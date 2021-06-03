
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

var ui = require('ui');
if (!(ui && ui.redirectTo)) {
    throw new Error("ui: [failed] require(\'ui\')");
}

var itemNameStr;
var itemName;

var new_price = "20.00";

function onLaunch() {
	console.log('=============enter onLaunch');

    var fs = require('fs');
	var path = './active_item_input.data';
	var data = fs.readSync(path);
    var str = JSON.parse(data);

    str.Price = new_price;

	var str_write = JSON.stringify(str);
	fs.writeSync(path, str_write);


	model.setData({
		switch: true,
		itemName: str.Name,
		itemNum: str.Price ,
	});

}

var url;
setTimeout(
	function(){ 
		url = "pages/item_input/item_input";
		console.log('redirectTo page ' + url);
		ui.redirectTo(url);
	}, 
2000);
Page({
  onShow: function() {
    console.log('enter page onShow');
	onLaunch();

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});