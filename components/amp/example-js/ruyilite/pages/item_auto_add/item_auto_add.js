
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

var active_name = "无标商品";
var active_price = "¥15.80";
var active_type = "x2";


var fs = require('fs');
var path = './shopping_cart.data';
var data = fs.readSync(path);
var temp;
var last_name;
var last_price;


function onItemAutoAdd() 
{
    console.log('=============enter item auto add onLaunch');
    temp = JSON.parse(data);

    last_name = temp.array[temp.array.length - 1].name;
    last_price = temp.array[temp.array.length - 1].price;

    temp.array.push({"name": active_name,"type": active_type,"price": active_price, "index":temp.array.length}); 
	var str = JSON.stringify(temp);
	fs.writeSync(path, str);

    var sum = 0;
    var totalNum = 0;
    var price;
    var num;
	
	temp.array.forEach( function(item){

		str = item.type;
		str = str.split("x");
        console.log(str[1])
		num = parseInt(str[1]);
        console.log(num)
		
		str = item.price;
		str = str.split("¥");
		price = parseFloat(str[1]);
		
		totalNum = totalNum + num;
		sum = sum + (num * price);
    })
	
    console.log('111---' +  totalNum)
    console.log('222---' +  sum)
	 
	 
	itemNumStr = totalNum.toString();
	itemNumStr = itemNumStr + '件';
	sum = sum.toFixed(2);
	totalPriceStr = sum.toString();
	totalPriceStr = '¥' + totalPriceStr;

    path = './active_item_total.data';
	str = JSON.stringify(totalPriceStr);
	fs.writeSync(path, str);

	model.setData({
		switch: true,
		LastName: last_name,
		LastPrice: last_price,
		ActiveName: active_name,
		ActivePrice: active_price,
		TotalNum: itemNumStr,
		TotalPrice: totalPriceStr,
	});
}


Page({
  onShow: function() {
    console.log('enter page onShow');
	onItemAutoAdd();

	setTimeout(
		function(){ 
			var url;
			url = "pages/pay_start/pay_start";
			console.log('redirectTo page ' + url);
			ui.redirectTo(url);
		}, 
	2000);
  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});



