var event = require('events');
// console.log(event)

var uartInstance = new event.EventEmitter();

uartInstance.on('data', function(arg, arg2) {
    console.log('uart event: ', arg, arg2);
})

setInterval(function() { 
    uartInstance.emit('data', 'this is uart data', 'data 2'); 
}, 1000);