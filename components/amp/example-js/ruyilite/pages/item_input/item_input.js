
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

var ui = require('ui');
if (!(ui && ui.redirectTo)) {
    throw new Error("ui: [failed] require(\'ui\')");
}

function onItemInput() {
	console.log('=============enter onLaunch');
    var fs = require('fs');
	var path = './active_item_input.data';
	var data = fs.readSync(path);
    var str = JSON.parse(data);
	
	model.setData({
		singleItemNo: str.Number,
		singleItemName: str.Name,
		singleItemPrice: str.Price,
		singleItemNum: str.amount
	});

}

var url;

function onItemPriceEditClick() 
{	
	console.log('=============enter onItemPriceEditClick');
	url = "pages/item_input_price_edit/item_input_price_edit";
	console.log('redirectTo page ' + url);
	ui.redirectTo(url);
}

function onItemNumEditClick() {
	
		url = "pages/item_input_num_edit/item_input_num_edit";
		console.log('redirectTo page ' + url);
		ui.redirectTo(url);
}

function onItemMakesureClick() {
	
		url = "pages/item_input_makesure/item_input_makesure";
		console.log('redirectTo page ' + url);
		ui.redirectTo(url);
}

Page({
  onShow: function() {
    console.log('enter page onShow');
	onItemInput();

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});

