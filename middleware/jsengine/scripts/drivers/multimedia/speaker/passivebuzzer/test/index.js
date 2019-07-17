var PassiveBuzzer = require('passivebuzzer/src');
var handle = new PassiveBuzzer('buzzer');

setInterval(function() {
    handle.once()
}, 3000);