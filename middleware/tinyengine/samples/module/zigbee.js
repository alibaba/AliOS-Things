Zigbee.on("message", function(addr, msg) {
  console.log("on_message: " + JSON.stringify(msg));
  setTimeout(function() {
    Zigbee.send(addr, JSON.stringify({ code: 200}), Zigbee.BZB_MESSAGE);
  }, 3000);
});

Zigbee.start({ port: 0x81, service: "dht11|brightness|screen", uartPort: 1, uartBaudrate: 115200 });

setInterval(function() {
  console.log("setInterval");
}, 15000);

Zigbee.notify();