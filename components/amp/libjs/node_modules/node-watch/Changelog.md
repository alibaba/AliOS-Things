# Changelog

## 0.6.3

* Types: Allow watching multiple files.

## 0.6.2

* Detect temporary editor files more wisely in order to avoid side effects on Windows.

## 0.6.1

* Add TypeScript support.
* Fix race condition of `fs.exists` and `fs.stat`.
* Prevent redundant events on Windows when creating file/directory.


## 0.6.0
Special thanks to [Timo Tijhof](https://github.com/Krinkle)

* Drop support for node < 6.0
* Add `ready` event for the watcher.
* Lots of bug fixed.

## 0.5.9
* Fix function detection.
* Emit `close` event after calling `.close()`.
* Don't emit any events after close.
* Change default `delay` to 200ms.

## 0.5.8
* Fix async function detection.

## 0.5.7
* Add `delay` option and set default to 100ms.

## 0.5.6
* Fix recursive watch with filter option.

## 0.5.5
* Remove duplicate events from a composed watcher.

## 0.5.4
* Accept Buffer filename.
* Add support for `encoding` option.

## 0.5.3
* The `filter` option can be of either Function or RegExp type.

## 0.5.0
* The `recursive` option is default to be `false`.
* The callback function will always provide an event name.

## 0.4.0
* Returns a [fs.FSWatcher](https://nodejs.org/api/fs.html#fs_class_fs_fswatcher) like object.
