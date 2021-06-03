
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

function onLaunch() {
    var myDate = new Date();
    var fs = require('fs');
    var path = './active_item_total.data';
    var data = fs.readSync(path);
    var total_price = JSON.parse(data);

    var shop_cart_path = './shopping_cart.data';
    var shop_cart_data = fs.readSync(shop_cart_path);
    var shop_cart_temp = JSON.parse(shop_cart_data);
    var sum = 0;
    var totalNum = 0;
    var price;
    var num;
	var str;

	shop_cart_temp.array.forEach( function(item){
		str = item.type;
		str = str.split("x");
		num = parseInt(str[1]);
		
		str = item.price;
		str = str.split("¥");
		price = parseFloat(str[1]);
		
		totalNum = totalNum + num;
		sum = sum + (num * price);
    })
	
	sum = sum.toFixed(2);
	var date_str =  (myDate.getMonth() + 1) + '.' + myDate.getDate();
    var time_str = myDate.getHours() + ':' + myDate.getMinutes();
    var order_list_path = './order_list.data';
    var order_list_data = fs.readSync(order_list_path);
    var order_list_temp = JSON.parse(order_list_data);
    order_list_temp.array.push({"date": date_str,"time": time_str,"number": totalNum + "件", "price": "¥" +sum,"index":order_list_temp.array.length}); 
	var order_list_str = JSON.stringify(order_list_temp);
	fs.writeSync(order_list_path, order_list_str);

    var shop_list_path = './shop_list_' + order_list_temp.array.length +'.data';
	fs.writeSync(shop_list_path, shop_cart_data);
	model.setData({
		TotalPrice: total_price,
		switch: true,
	});	
}

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
	onLaunch();

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});