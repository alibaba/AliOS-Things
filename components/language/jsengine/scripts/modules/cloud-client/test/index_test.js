var cloudClient = require("../src/index.js");
var i;

console.log("=================================");
console.log("        cloud client       ");
console.log("=================================");

//
function lightOn() {
  console.log("lightOn");
  cloudClient.postProperty({ LightSwitch: 1 });
}

//
function lightOff() {
  console.log("lightOff");
  cloudClient.postProperty({ LightSwitch: 0 });
}

//
cloudClient.start(
  //config
  {
    productKey: "please_input_your_key",
    deviceName: "please_input_device_name",
    deviceSecret: "please_input_device_secret"
  },

  //cb
  function (err, data) {
    console.log("onStart");
    if (err) {
      return;
    }
    //
    cloudClient.onPropertySet(function (msg) {
      if (msg.method === "thing.service.property.set") {
        if (msg.params.LightSwitch === 1) {
          lightOn();
        } else {
          lightOff();
        }
      }
    });
  }
);

setInterval(function () {
  if (i)
    lightOn();
  else
    lightOff();

  i = (i + 1) % 2;

}, 4000);
