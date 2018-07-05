

foo = "foo bar stuff";
aStr = "ABCD";

aChar = aStr.charAt(0);
value = Integer.valueOf(aChar);

result = foo.length==13 && foo.indexOf("bar")==4 && 
		 foo.substring(8,13)=="stuff"  && foo.substr(-3) == "uff" && aChar == 'A' &&
         Integer.valueOf(aChar)==65;

console.log('string 测试 ' + (result ? '成功' : '失败'));

function toMethod(event)
{
    var i;
    var j = event.length;
    var ch;
    var ret = '';
    
    for(i=0; i<j; i++){
        
        ch = event.charAt(i);
        
        if( ch == '/' )
            ret = ret + '.';
        else
            ret = ret + ch;
    }
    
    return ret;
}

var events = [
    "thing/event/property/post"
];

result = ( 'thing.event.property.post' == toMethod(events[0])  )

console.log('string 测试 ' + (result ? '成功' : '失败'));

console.log( toMethod(events[0]) );



/*
// split 暂不支持
// test for string split
var b = "1,4,7";
var a = b.split(",");

result = a.length==3 && a[0]==1 && a[1]==4 && a[2]==7;
*/

