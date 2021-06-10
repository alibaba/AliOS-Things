
import * as lcd from 'lcd'

var msgbuf = 'this is amp lcd test'

lcd.open();

//var value = lcd.show(0, 0, 320, 240, buf);

var value = lcd.fill(0, 0, 320, 240, 0xffffffff);

console.log('lcd fill value is ' + value)

lcd.close();