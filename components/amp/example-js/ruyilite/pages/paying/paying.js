
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

function onLaunch() {
    console.log('=============enter onLaunch');

    var path = './active_item_total.data';
    var data = fs.readSync(path);
    var temp = JSON.parse(data);
    console.log('111---' +  temp)

	model.setData({
		TotalPrice: temp,
		switch: true,
	});	
}

var url;
setTimeout(
	function(){ 
		url = "pages/pay_success/pay_success";
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