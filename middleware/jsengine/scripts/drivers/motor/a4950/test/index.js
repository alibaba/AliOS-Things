var a4950 = require("a4950/src/index.js"); 
var left_motor = new a4950('a4950.left0', 'a4950.left1'); 
var right_motor = new a4950('a4950.right0', 'a4950.right1'); 
left_motor.stop(); 
right_motor.stop();
left_motor.forward(20); 
right_motor.forward(20); 