
var fs = require('fs');

var url;
var curstatus = {
		 collect_logo: "yes",
};

function button_click_collect(arg)
{
    var path = './ruyilite.data';   
    curstatus.collect_logo = "yes";

    var str = JSON.stringify(curstatus);
    fs.writeSync(path, str);

	url = "pages/logo/logo";
	console.log('redirectTo page ' + url);
	ui.redirectTo(url);
}


function button_click_stock(arg)
{
	var path = './ruyilite.data';   
    curstatus.collect_logo = "no";

    var str = JSON.stringify(curstatus);
    fs.writeSync(path, str);

	url = "pages/logo/logo";
	console.log('redirectTo page ' + url);
	ui.redirectTo(url);
}

function button_click_order(arg)
{
	url = "pages/order_list/order_list";
	console.log('redirectTo page ' + url);
	ui.redirectTo(url);
}
Page({
  onShow: function() {
    console.log('enter page onShow');

  },

  onExit: function() {
    console.log('enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});