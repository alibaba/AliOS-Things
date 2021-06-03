
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}


var itemNameStr;

function onItemInputMakesure() {
    console.log('=============enter onLaunch');

    var fs = require('fs');
	var path = './active_item_input.data';
	var data = fs.readSync(path);
    var str = JSON.parse(data);

	console.log('=============enter onLaunch' + str.Name);
	itemNameStr = '  录入成功         ' + str.Name;

	model.setData({
		switch: true,
		alert: itemNameStr,
	});
}

var ui = require('ui');
if (!(ui && ui.redirectTo)) {
    throw new Error("ui: [failed] require(\'ui\')");
}

var fs = require('fs');
var str;

var url;
setTimeout(
	function(){ 
		url = "pages/menu/menu";
		console.log('redirectTo page ' + url);
		ui.redirectTo(url);		
	}, 
2000);





Page({
  onShow: function() {
    console.log('enter page onShow');
	onItemInputMakesure();

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});