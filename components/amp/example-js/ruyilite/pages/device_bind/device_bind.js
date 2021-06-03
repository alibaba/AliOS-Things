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

var ui = require('ui');
if (!(ui && ui.redirectTo)) {
    throw new Error("ui: [failed] require(\'ui\')");
}

var url;
setTimeout(
	function(){ 
		url = "pages/shop/shop";
		console.log('redirectTo page ' + url);
		ui.redirectTo(url);
	}, 
2000);



