var LCD1602 = require('lcd1602/src');
var handle = new LCD1602('lcd1602');

var cval0 = 65;
var cval1 = 97;
var i = 0;
handle.init_lcd();
for(i=0;i<39;i++){
    cval0 = cval0 + 1;
    handle.show_char(i,0,cval0);
    cval1 = cval1 + 1;
    handle.show_char(i,1,cval1);
}
 
