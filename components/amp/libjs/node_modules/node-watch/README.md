# node-watch [![Status](https://travis-ci.org/yuanchuan/node-watch.svg?branch=master)](https://travis-ci.org/yuanchuan/node-watch "See test builds")

A wrapper and enhancements for [fs.watch](http://nodejs.org/api/fs.html#fs_fs_watch_filename_options_listener).

[![NPM](https://nodei.co/npm/node-watch.png?downloads=true&downloadRank=true&stars=true)](https://nodei.co/npm/node-watch.png/)


## Installation

```bash
npm install node-watch
```

## Example

```js
var watch = require('node-watch');

watch('file_or_dir', { recursive: true }, function(evt, name) {
  console.log('%s changed.', name);
});
```

Now it's fast to watch **deep** directories on macOS and Windows, since the `recursive` option is natively supported except on Linux.

```js
// watch the whole disk
watch('/', { recursive: true }, console.log);
```


## Why?

* Some editors will generate temporary files which will cause the callback function to be triggered multiple times.
* The callback function will only be triggered once on watching a single file.
* <del>Missing an option to watch a directory recursively.</del>
* Recursive watch is not supported on Linux or in older versions of nodejs.
* Keep it simple, stupid.


## Options

The usage and options of `node-watch` are compatible with [fs.watch](https://nodejs.org/dist/latest-v7.x/docs/api/fs.html#fs_fs_watch_filename_options_listener).
* `persistent: Boolean` (default **true**)
* `recursive: Boolean` (default **false**)
* `encoding: String` (default **'utf8'**)

**Extra options**

* `filter: RegExp | Function`

   Return that matches the filter expression.

    ```js
    // filter with regular expression
    watch('./', { filter: /\.json$/ });

    // filter with custom function
    watch('./', { filter: f => !/node_modules/.test(f) });
    ```
* `delay: Number` (in ms, default **200**)

   Delay time of the callback function.

   ```js
   // log after 5 seconds
   watch('./', { delay: 5000 }, console.log);
   ```

## Events

The events provided by the callback function is either `update` or `remove`, which is less confusing to `fs.watch`'s `rename` or `change`.

```js
watch('./', function(evt, name) {

  if (evt == 'update') {
    // on create or modify
  }

  if (evt == 'remove') {
    // on delete
  }

});
```


## Watcher object

The watch function returns a [fs.FSWatcher](https://nodejs.org/api/fs.html#fs_class_fs_fswatcher) like object as the same as `fs.watch` (>= v0.4.0).

#### Watcher events

```js
watcher.on('change', function(evt, name) {
  // callback
});

watcher.on('error', function(err) {
  // handle error
});

watcher.on('ready', function() {
  // the watcher is ready to respond to changes
});
```

#### Close

```js
// close
watcher.close();

// is closed?
watcher.isClosed()
```

#### List of methods

* `.on`
* `.once`
* `.emit`
* `.close`
* `.listeners`
* `.setMaxListeners`
* `.getMaxListeners`

##### Extra methods
* `.isClosed` detect if the watcher is closed


## Known issues

**Windows, node < v4.2.5**

  * Failed to detect `remove` event
  * Failed to get deleted filename or directory name

**MacOS, node 0.10.x**
  * Will emit double event if the directory name is of one single character.


## Misc

#### 1. Watch multiple files or directories in one place
```js
watch(['file1', 'file2'], console.log);
```

#### 2. Customize watch command line tool
```js
#!/usr/bin/env node

// https://github.com/nodejs/node-v0.x-archive/issues/3211
require('epipebomb')();

var watcher = require('node-watch')(
  process.argv[2] || './', { recursive: true }, console.log
);

process.on('SIGINT', watcher.close);
```
Monitoring chrome from disk:
```bash
$ watch / | grep -i chrome
```

#### 3. Got ENOSPC error?

If you get ENOSPC error, but you actually have free disk space - it means that your OS watcher limit is too low and you probably want to recursively watch a big tree of files.

Follow this description to increase the limit:
[https://confluence.jetbrains.com/display/IDEADEV/Inotify+Watches+Limit](https://confluence.jetbrains.com/display/IDEADEV/Inotify+Watches+Limit)


## Alternatives

* [chokidar](https://github.com/paulmillr/chokidar)
* [gaze](https://github.com/shama/gaze)
* [mikeal/watch](https://github.com/mikeal/watch)

## Contributors

Thanks goes to these wonderful people ([emoji key](https://github.com/kentcdodds/all-contributors#emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore -->
| [<img src="https://avatars1.githubusercontent.com/u/250426?v=4" width="100px;"/><br /><sub><b>Yuan Chuan</b></sub>](http://yuanchuan.name)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=yuanchuan "Code") [📖](https://github.com/yuanchuan/node-watch/commits?author=yuanchuan "Documentation") [⚠️](https://github.com/yuanchuan/node-watch/commits?author=yuanchuan "Tests") | [<img src="https://avatars2.githubusercontent.com/u/99367?v=4" width="100px;"/><br /><sub><b>Greg Thornton</b></sub>](http://xdissent.com)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=xdissent "Code") [🤔](#ideas-xdissent "Ideas, Planning, & Feedback") | [<img src="https://avatars1.githubusercontent.com/u/6019373?v=4" width="100px;"/><br /><sub><b>Amir Arad</b></sub>](https://github.com/amir-arad)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=amir-arad "Code") [📖](https://github.com/yuanchuan/node-watch/commits?author=amir-arad "Documentation") [⚠️](https://github.com/yuanchuan/node-watch/commits?author=amir-arad "Tests") | [<img src="https://avatars0.githubusercontent.com/u/693642?v=4" width="100px;"/><br /><sub><b>Gary Burgess</b></sub>](http://slipthrough.net)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=garyb "Code") | [<img src="https://avatars2.githubusercontent.com/u/557895?v=4" width="100px;"/><br /><sub><b>Peter deHaan</b></sub>](http://about.me/peterdehaan)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=pdehaan "Code") | [<img src="https://avatars2.githubusercontent.com/u/161968?v=4" width="100px;"/><br /><sub><b>kcliu</b></sub>](https://medium.com/@kcliu)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=kcliu "Code") | [<img src="https://avatars3.githubusercontent.com/u/309006?v=4" width="100px;"/><br /><sub><b>Hoovinator</b></sub>](https://github.com/crh3675)<br />[💬](#question-crh3675 "Answering Questions") |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| [<img src="https://avatars3.githubusercontent.com/u/142875?v=4" width="100px;"/><br /><sub><b>Steve Shreeve</b></sub>](https://github.com/shreeve)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=shreeve "Code") | [<img src="https://avatars2.githubusercontent.com/u/1456400?v=4" width="100px;"/><br /><sub><b>Blake Regalia</b></sub>](https://github.com/blake-regalia)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=blake-regalia "Code") | [<img src="https://avatars3.githubusercontent.com/u/1587937?v=4" width="100px;"/><br /><sub><b>Mike Collins</b></sub>](https://github.com/intervalia)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=intervalia "Code") | [<img src="https://avatars3.githubusercontent.com/u/156867?v=4" width="100px;"/><br /><sub><b>Timo Tijhof</b></sub>](https://timotijhof.net)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=Krinkle "Code") | [<img src="https://avatars3.githubusercontent.com/u/9532377?v=4" width="100px;"/><br /><sub><b>Bailey Herbert</b></sub>](https://bailey.sh/)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=baileyherbert "Code") | [<img src="https://avatars0.githubusercontent.com/u/1397032?v=4" width="100px;"/><br /><sub><b>Anthony Rey</b></sub>](https://github.com/Gnuk)<br />[💻](https://github.com/yuanchuan/node-watch/commits?author=Gnuk "Code") |
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/kentcdodds/all-contributors) specification. Contributions of any kind welcome!


## License
MIT

Copyright (c) 2012-2018 [yuanchuan](https://github.com/yuanchuan)
