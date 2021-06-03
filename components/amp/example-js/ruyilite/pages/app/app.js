
var model = require('model');
if (!(model && model.setData)) {
    throw new Error("model: [failed] require(\'model\')");
}

function onLaunch() {
    console.log('=============enter onLaunch');
	model.setData({
		switch: true,
	});

}





