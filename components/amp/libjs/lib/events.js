'use strict';

var spliceOne;

function EventEmitter() {
    EventEmitter.init.call(this);
}

EventEmitter.EventEmitter = EventEmitter;

EventEmitter.usingDomains = false;

EventEmitter.prototype._events = undefined;
EventEmitter.prototype._eventsCount = 0;
EventEmitter.prototype._maxListeners = undefined;

var defaultMaxListeners = 10;

Object.defineProperty(EventEmitter, 'defaultMaxListeners', {
    enumerable: true,
    get: function () {
        return defaultMaxListeners;
    },
    set: function (arg) {
        if (typeof arg !== 'number' || arg < 0 || Number.isNaN(arg)) {
            throw new Error('defaultMaxListeners:a non-negative number');
        }
        defaultMaxListeners = arg;
    }
});

EventEmitter.init = function () {

    if (this._events === undefined ||
        this._events === Object.getPrototypeOf(this)._events) {
        this._events = Object.create(null);
        this._eventsCount = 0;
    }

    this._maxListeners = this._maxListeners || undefined;
};

EventEmitter.prototype.setMaxListeners = function setMaxListeners(n) {
    if (typeof n !== 'number' || n < 0 || Number.isNaN(n)) {
        throw new Error('a non-negative number');
    }
    this._maxListeners = n;
    var that = this;
    return that;
};

function _getMaxListeners(that) {
    if (that._maxListeners === undefined)
        return EventEmitter.defaultMaxListeners;
    return that._maxListeners;
}

EventEmitter.prototype.getMaxListeners = function getMaxListeners() {
    return _getMaxListeners(this);
};

function longestSeqContainedIn(a, b) {
    for (var len = a.length; len >= 3; --len) {
        for (var i = 0; i < a.length - len; ++i) {
            for (var j = 0; j < b.length - len; ++j) {
                var matches = true;
                for (var k = 0; k < len; ++k) {
                    if (a[i + k] !== b[j + k]) {
                        matches = false;
                        break;
                    }
                }
                if (matches)
                    return [len, i, j];
            }
        }
    }

    return [0, 0, 0];
}

function enhanceStackTrace(err, own) {
    var sep = '\nEmitted \'error\' event at:\n';

    var errStack = err.stack.split('\n').slice(1);
    var ownStack = own.stack.split('\n').slice(1);

    var seq = longestSeqContainedIn(ownStack, errStack);
    if (seq.len > 0) {
        ownStack.splice(seq.off + 1, seq.len - 1,
            '    [... lines matching original stack trace ...]');
    }
    err.stack = err.stack + sep + ownStack.join('\n');
}

EventEmitter.prototype.emit = function emit(type) {
    var doError = (type === 'error');

    var events = this._events;
    if (events !== undefined)
        doError = (doError && events.error === undefined);
    else if (!doError)
        return false;

    var args = [];
    for (var i = 1, len = arguments.length; i < len; i++) {
        args.push(arguments[i]);
    }
    if (doError) {
        var er;
        if (args.length > 0)
            er = args[0];
        if (er instanceof Error) {
            throw er;
        }
        throw new Error('unhandled error');
    }

    var handler = events[type];

    if (handler === undefined)
        return false;

        if (typeof handler === 'function') {
        Function.prototype.apply.call(handler, this, args);
    } else {
        var len = handler.length;
        var listeners = arrayClone(handler, len);
        for (var i = 0; i < len; ++i)
            Function.prototype.apply.call(listeners[i], this, args);
    }

    return true;
};

function _addListener(target, type, listener, prepend) {
    var m;
    var events;
    var existing;

    if (typeof listener !== 'function') {
        throw new Error('addListener invalid arg type: Function');
    }

    events = target._events;
    if (events === undefined) {
        events = target._events = Object.create(null);
        target._eventsCount = 0;
    } else {
        if (events.newListener !== undefined) {
            target.emit('newListener', type,
                listener.listener ? listener.listener : listener);
            events = target._events;
        }
        existing = events[type];
    }

    if (existing === undefined) {
        existing = events[type] = listener;
        ++target._eventsCount;
    } else {
        if (typeof existing === 'function') {
            existing = events[type] =
                prepend ? [listener, existing] : [existing, listener];
        } else if (prepend) {
            existing.unshift(listener);
        } else {
            existing.push(listener);
        }
    }

    return target;
}

EventEmitter.prototype.addListener = function addListener(type, listener) {
    return _addListener(this, type, listener, false);
};

EventEmitter.prototype.on = EventEmitter.prototype.addListener;

EventEmitter.prototype.prependListener =
    function prependListener(type, listener) {
        return _addListener(this, type, listener, true);
    };

function onceWrapper() {
    if (!this.fired) {
        this.target.removeListener(this.type, this.wrapFn);
        this.fired = true;
        Function.prototype.apply.call(this.listener, this.target, arguments);
    }
}

function _onceWrap(target, type, listener) {
    var state = { fired: false, wrapFn: undefined, target: target, type: type, listener: listener };
    var wrapped = onceWrapper.bind(state);
    wrapped.listener = listener;
    state.wrapFn = wrapped;
    return wrapped;
}

EventEmitter.prototype.once = function once(type, listener) {
    if (typeof listener !== 'function') {
        throw new Error('listener invalid arg type');
    }
    this.on(type, _onceWrap(this, type, listener));
    var that = this;
    return that;
};

EventEmitter.prototype.prependOnceListener =
    function prependOnceListener(type, listener) {
        if (typeof listener !== 'function') {
            throw new Error('prependOnceListener invalid arg type');
        }
        this.prependListener(type, _onceWrap(this, type, listener));
        var that = this;
        return that;
    };

EventEmitter.prototype.removeListener =
    function removeListener(type, listener) {
        var list, events, position, i, originalListener;
        var that = this;

        if (typeof listener !== 'function') {
            throw new Error('removeListener invalid arg type');
        }

        events = this._events;
        if (events === undefined)
            return that;

        list = events[type];
        if (list === undefined)
            return that;

        if (list === listener || list.listener === listener) {
            if (--this._eventsCount === 0)
                this._events = Object.create(null);
            else {
                delete events[type];
                if (events.removeListener)
                    this.emit('removeListener', type, list.listener || listener);
            }
        } else if (typeof list !== 'function') {
            position = -1;

            for (i = list.length - 1; i >= 0; i--) {
                if (list[i] === listener || list[i].listener === listener) {
                    originalListener = list[i].listener;
                    position = i;
                    break;
                }
            }

            if (position < 0)
                return that;

            if (position === 0)
                list.shift();
            else {
                if (spliceOne === undefined)
                    spliceOne = require('internal/util').spliceOne;
                spliceOne(list, position);
            }

            if (list.length === 1)
                events[type] = list[0];

            if (events.removeListener !== undefined)
                this.emit('removeListener', type, originalListener || listener);
        }

        return that;
    };

EventEmitter.prototype.off = EventEmitter.prototype.removeListener;

EventEmitter.prototype.removeAllListeners =
    function removeAllListeners(type) {
        var listeners, events, i;
        var that = this;

        events = this._events;
        if (events === undefined)
            return that;

        if (events.removeListener === undefined) {
            if (arguments.length === 0) {
                this._events = Object.create(null);
                this._eventsCount = 0;
            } else if (events[type] !== undefined) {
                if (--this._eventsCount === 0)
                    this._events = Object.create(null);
                else
                    delete events[type];
            }
            return that;
        }

        if (arguments.length === 0) {
            var keys = Object.keys(events);
            var key;
            for (i = 0; i < keys.length; ++i) {
                key = keys[i];
                if (key === 'removeListener') continue;
                this.removeAllListeners(key);
            }
            this.removeAllListeners('removeListener');
            this._events = Object.create(null);
            this._eventsCount = 0;
            return that;
        }

        listeners = events[type];

        if (typeof listeners === 'function') {
            this.removeListener(type, listeners);
        } else if (listeners !== undefined) {
            for (i = listeners.length - 1; i >= 0; i--) {
                this.removeListener(type, listeners[i]);
            }
        }

        return that;
    };

function _listeners(target, type, unwrap) {
    var events = target._events;

    if (events === undefined)
        return [];

    var evlistener = events[type];
    if (evlistener === undefined)
        return [];

    if (typeof evlistener === 'function')
        return unwrap ? [evlistener.listener || evlistener] : [evlistener];

    return unwrap ?
        unwrapListeners(evlistener) : arrayClone(evlistener, evlistener.length);
}

EventEmitter.prototype.listeners = function listeners(type) {
    return _listeners(this, type, true);
};

EventEmitter.prototype.rawListeners = function rawListeners(type) {
    return _listeners(this, type, false);
};

EventEmitter.listenerCount = function (emitter, type) {
    if (typeof emitter.listenerCount === 'function') {
        return emitter.listenerCount(type);
    } else {
        return listenerCount.call(emitter, type);
    }
};

EventEmitter.prototype.listenerCount = listenerCount;
function listenerCount(type) {
    var events = this._events;

    if (events !== undefined) {
        var evlistener = events[type];

        if (typeof evlistener === 'function') {
            return 1;
        } else if (evlistener !== undefined) {
            return evlistener.length;
        }
    }

    return 0;
}

EventEmitter.prototype.eventNames = function eventNames() {
    return this._eventsCount > 0 ? Function.prototype.apply.call(this._events) : [];
};

function arrayClone(arr, n) {
    var copy = new Array(n);
    for (var i = 0; i < n; ++i)
        copy[i] = arr[i];
    return copy;
}

function unwrapListeners(arr) {
    var ret = new Array(arr.length);
    for (var i = 0; i < ret.length; ++i) {
        ret[i] = arr[i].listener || arr[i];
    }
    return ret;
}


module.exports = EventEmitter;
