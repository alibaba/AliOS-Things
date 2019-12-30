
var ds3119 = require("ds3119/src/index.js");
var servo = new ds3119('ds3119');

const SERVO_MID = 0;
const SERVO_MIN = -45;
const SERVO_MAX = 45;

servo.config(SERVO_MIN, SERVO_MID, SERVO_MAX);
servo.reset();
servo.left_turn(30); 
