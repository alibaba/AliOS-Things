
 
var page1 = {
    currentValue: 33,
};

var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

array = [
		  { name: '原木纯品纸抽', type: 'x1', price: "¥6.00", index: 0 },
		  { name: '无标商品', type: 'x1', price: "¥15.88", index: 1 },
		  { name: '炫迈夹心口香糖', type: 'x1', price: "¥13.66", index: 2 },
		  { name: '无标商品', type: 'x1', price: "¥15.88", index: 3 },
		  { name: 'IoT设备1', type: 'x2', price: "¥101", index: 4 },
		  { name: 'IoT设备2', type: 'x1', price: "¥102", index: 5 },
		  { name: 'IoT设备3', type: 'x1', price: "¥102", index:6 },
		];


var sum;
var totalNum;
var price;
var num;
var str;

var itemNumStr;
var totalPriceStr;

			
function onItemEdit() {
    console.log('enter onLaunch----------------------------------------  ' + array.length);
	totalNum = 0;
	sum = 0;
	array.forEach( function(item){
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
	
     console.log('111---' +  totalNum)
     console.log('222---' +  sum)
	 
	 
	itemNumStr = totalNum.toString();
	itemNumStr = itemNumStr + '件';
	
	totalPriceStr = sum.toString();
	totalPriceStr = '¥' + totalPriceStr;
	
	
     console.log('---' +  itemNumStr)
     console.log('---' +  totalPriceStr)
	 var index = 1;
	 str = array[index].type;
	 str = str.split("x");
	 num = parseInt(str[1]);
	 
	model.setData({
		singleItemName: array[index].name,
		singleItemNum: array[index].type,
		singleItemPrice: array[index].price,
		selectItemNum: num,
		itemNum: itemNumStr,
		totalPrice: totalPriceStr
	});
}



Page({
  onShow: function() {
    console.log('enter page onShow');
	onItemEdit();

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});
