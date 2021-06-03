
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

var itemNameStr;
var itemName;

function onLaunch() {
	console.log('=============enter onLaunch');

	itemName = "百事可乐500ml",
	itemNameStr = itemName + '(个)';

	model.setData({
		switch: true,
		itemName: itemNameStr,
		itemNum: "4",
		
	});

}


Page({
  onShow: function() {
    console.log('enter page onShow');
	onLaunch();
	
	setTimeout(
		function(){ 
			var url;
			url = "pages/menu/menu";
			console.log('redirectTo page ' + url);
			ui.redirectTo(url);
		}, 
	3000);

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});