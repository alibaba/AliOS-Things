
var page1 = {
    currentValue: 33,
};

var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

var sum;
var totalNum;
var price;
var num;
var str;

var itemNumStr;
var totalPriceStr;

function onLaunch() {
	totalNum = 0;
	sum = 0;
    var fs = require('fs');
    var order_item_list_path = './order_item_list_path.data';
    var data = fs.readSync(order_item_list_path);
    var temp = JSON.parse(data);
    console.log('enter onLaunch----------------------------------------  ' + temp);

    var data = fs.readSync(temp);
    var temp = JSON.parse(data);
    console.log('enter onLaunch----------------------------------------  ' + temp.array.lengh);

	temp.array.forEach( function(item){
		str = item.type;
		str = str.split("x");
        console.log("1111")
        console.log(str[1])
		num = parseInt(str[1]);
        console.log("2222")
        console.log(num)
		
		str = item.price;
		str = str.split("¥");
		price = parseFloat(str[1]);
        console.log(item.type + '---' +  str[1])
		
		totalNum = totalNum + num;
		sum = sum + (num * price);
    })
	
	itemNumStr = totalNum.toString();
	itemNumStr = itemNumStr + '件';
	sum = sum.toFixed(2);
	totalPriceStr = sum.toString();
	totalPriceStr = '¥' + totalPriceStr;
	
    console.log('enter page model------------------------------------------');
	model.setData({
		itemNum: itemNumStr,
		totalPrice: totalPriceStr,
		sites: temp.array
	});
    console.log('enter page setData------------------------------------------');
}


Page({
  onShow: function() {
    console.log('enter page onShow------------------------------------------');
	onLaunch();
	
	setTimeout(
		function(){ 
			var url;
			console.log('enter page setTimeout------------------------------------------');
			var ui = require('ui');
			if (!(ui && ui.redirectTo)) {
				throw new Error("ui: [failed] require(\'ui\')");
			}
			 
			url = "pages/order_list/order_list";
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