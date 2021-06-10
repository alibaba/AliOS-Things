'use strict';

const EventEmitter = require('events');

module.exports = new class AudioPlayer extends EventEmitter {
    constructor(){
        super();
        this._onState();
    }

    play(source, callback) {
        if (!source || !callback) {
            throw new Error('invalid params');
        }
        return __native.audioplayer.play(source, callback);
    };

    pause() {
        return __native.audioplayer.pause();
    };

    resume() {
        return __native.audioplayer.resume();
    };

    stop() {
        return __native.audioplayer.stop();
    };

    seekto(seconds) {
        if (seconds < 0) {
            throw new Error('invalid params');
        }
        return __native.audioplayer.seekto(seconds);
    };

    getPosition() {
        return __native.audioplayer.getPosition();
    };

    getDuration() {
        return __native.audioplayer.getDuration();
    };

    getState() {
        return __native.audioplayer.getState();
    };

    _onState() {
        __native.audioplayer.onState(function(state) {
            this.emit('stateChange', state);
        }.bind(this));
    };

    listPlay(sourcelist, callback) {
        if (!sourcelist || !callback) {
            throw new Error('invalid params');
        }
        return __native.audioplayer.listPlay(sourcelist, callback);
    };

    listPlayStop() {
        return __native.audioplayer.listPlayStop();
    };

    setVolume(volume) {
        if (volume < 0) {
            throw new Error('invalid params');
        }
        return __native.audioplayer.setVolume(volume);
    };

    getVolume() {
        return __native.audioplayer.getVolume();
    };
}