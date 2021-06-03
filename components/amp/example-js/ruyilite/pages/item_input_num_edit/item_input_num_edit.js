
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

var itemNameStr;
var itemName;

function onItemInputNumEdit() {
	console.log('=============enter onLaunch');

    var fs = require('fs');
	var path = './active_item_input.data';
	var data = fs.readSync(path);
    var str = JSON.parse(data);

	itemName = str.Name,
	console.log('=============enter onLaunch' + str.Name);
	itemNameStr = itemName + '(个)';

	model.setData({
		switch: true,
		itemName: itemNameStr,
		itemNum: str.amount,
	});

}

var url;
setTimeout(
	function(){ 
		url = "pages/item_input_num_makesure/item_input_num_makesure";
		console.log('redirectTo page ' + url);
		ui.redirectTo(url);
	}, 
2000);




Page({
  onShow: function() {
    console.log('enter page onShow');
	onItemInputNumEdit();

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});