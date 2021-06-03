

var ui = require('ui');
if (!(ui && ui.redirectTo)) {
    throw new Error("ui: [failed] require(\'ui\')");
}

var fs = require('fs');
var str;

var url;



Page({
  onShow: function() {
    console.log('----------------------------------- enter page onShow');
       var path = './ruyilite.data';
    console.log('-------------------------------- 2enter page onShow');
        var data = fs.readSync(path);
    console.log('-------------------------------- 34enter page onShow');
        console.log('fs read: ' + data);
    console.log('-------------------------------- 4enter page onShow');
        str = JSON.parse(data);
    console.log('-------------------------------- 5enter page onShow');
		
    console.log('----------------------------111---- enter page onShow');
		setTimeout(
			function(){ 
				
				if (str.collect_logo === "yes") 
				{
					url = "pages/item_auto_add/item_auto_add";
					console.log('redirectTo page ' + url);
					ui.redirectTo(url);
				}
				else if (str.collect_logo === "no")
				{
					url = "pages/item_input/item_input";
					console.log('redirectTo page ' + url);
					ui.redirectTo(url);		
				}
			}, 
		2000);
 
    console.log('--------------------------222------ enter page onExit');
  },

  onExit: function() {
    console.log('-------------------------------- 1enter page onExit');

  },

  onUpdate: function() {
    console.log('enter page onUpdate');
  }

});