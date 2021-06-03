
var sum;
var totalNum;
var price;
var num;
var str;
var strnum;

var itemNumStr;
var totalPriceStr;
var temp_array = {
	"array":[
		{"date":"10.22","time":"9:22","number":"1件","price":"¥6.00","index":0},
		{"date":"10.22","time":"10:25","number":"1件","price":"¥15.88","index":1},
		{"date":"10.22","time":"10:26","number":"1件","price":"¥13.66","index":2},
		{"date":"10.22","time":"10:32","number":"1件","price":"¥15.88","index":3},
		{"date":"10.22","time":"11:35","number":"1件","price":"¥101","index":4},
		{"date":"10.22","time":"11:47","number":"1件","price":"¥102","index":5},
		{"date":"1.5","time":"15:16","number":"65件","price":"¥1358.10","index":6},
		{"date":"1.5","time":"16:50","number":"67件","price":"¥1389.70","index":7}
		]
};

function onLaunch() {
	totalNum = 0;
	sum = 0;
	
    console.log('111111111111111111111111 enter page onShow');
	var model = require('model');
	if (!(model && model.setData)) {
		throw new Error("model: [failed] require(\'model\')");
	}

    console.log('2222222222222222 enter page onShow');
    var fs = require('fs');
    var path = './order_list.data';
    var data = fs.readSync(path);
    temp_array = JSON.parse(data);

    console.log('enter onLaunch----------------------------------------');

	temp_array.array.forEach( function(item){		
		strnum = item.number;
		strnum = strnum.split("件");
		num = parseInt(strnum[0]);
		str = item.price;
		str = str.split("¥");
		price = parseFloat(str[1]);
		
		totalNum = totalNum + num;
		sum = sum + price;
    })

    console.log('333333333333333333333 enter page onShow');
	var total_num = temp_array.array.length;
	itemNumStr = total_num.toString() + '笔';
	
    console.log('444444444444444444444444 enter page onShow');
	sum = sum.toFixed(2);
	totalPriceStr = sum.toString();
	totalPriceStr = '¥' + totalPriceStr;

    console.log('itemNumStr --------------------------------------- ' + itemNumStr);
    console.log('totalPriceStr --------------------------------------- ' + totalPriceStr);
	model.setData({
		itemNum: itemNumStr,
		totalPrice: totalPriceStr,
		sites: temp_array.array
	});
    console.log('5555555555555555555 enter page onShow');
}

function on_order_list_click(id, index) {
	var path = './order_item_list_path.data';	
    index = index + 1;
	var curItem = './shop_list_' + index + '.data';
	var str = JSON.stringify(curItem);
	fs.writeSync(path, str);
	
	url = "pages/order_item_list/order_item_list";
	console.log('redirectTo page ' + url);
	ui.redirectTo(url);
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