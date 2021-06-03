
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

function onLaunch() {
    console.log('=============enter onLaunch');
	model.setData({
		switch: true,
		itemName: "百事可乐500ml",
		itemNum: "4",
		
	});
}

function button_click_shopping_cart()
{
    var fs = require('fs');
	var path = './shopping_cart.data';
	var data = fs.readSync(path);
    userArray = JSON.parse(data);
	console.log('fs read: ' + data);
	console.log('fs userArray: ' + userArray);

}

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

