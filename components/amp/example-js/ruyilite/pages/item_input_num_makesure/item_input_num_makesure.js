
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

var itemNameStr;
var itemName;

var new_num = "31";

function ItemInputNumMakesure() {
	console.log('=============enter onLaunch');

    var fs = require('fs');
	var path = './active_item_input.data';
	var data = fs.readSync(path);
    var str = JSON.parse(data);

	itemName = str.Name,
	itemNameStr = itemName + '(个)';

    str.amount = new_num;

	var str_write = JSON.stringify(str);
	fs.writeSync(path, str_write);

	model.setData({
		switch: true,
		itemName: itemNameStr,
		itemNum: str.amount,
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
	ItemInputNumMakesure();

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});