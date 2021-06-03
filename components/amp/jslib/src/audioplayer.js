'use strict';
import * as AUDIOPLAYER from 'AUDIOPLAYER'
import * as events from 'events'

export class AudioPlayer extends events.EventEmitter {
    constructor(){
        super();
        this._onState();
    }

    play(source, callback) {
        if (!source || !callback) {
            throw new Error('invalid params');
        }
        return AUDIOPLAYER.play(source, callback);
    };

    pause() {
        return AUDIOPLAYER.pause();
    };

    resume() {
        return AUDIOPLAYER.resume();
    };

    stop() {
        return AUDIOPLAYER.stop();
    };

    seekto(seconds) {
        if (seconds < 0) {
            throw new Error('invalid params');
        }
        return AUDIOPLAYER.seekto(seconds);
    };

    getPosition() {
        return AUDIOPLAYER.getPosition();
    };

    getDuration() {
        return AUDIOPLAYER.getDuration();
    };

    getState() {
        return AUDIOPLAYER.getState();
    };

    _onState() {
        AUDIOPLAYER.onState(function(state) {
            this.emit('stateChange', state);
        }.bind(this));
    };

    listPlay(sourcelist, callback) {
        if (!sourcelist || !callback) {
            throw new Error('invalid params');
        }
        return AUDIOPLAYER.listPlay(sourcelist, callback);
    };

    listPlayStop() {
        return AUDIOPLAYER.listPlayStop();
    };

    setVolume(volume) {
        if (volume < 0) {
            throw new Error('invalid params');
        }
        return AUDIOPLAYER.setVolume(volume);
    };

    getVolume() {
        return AUDIOPLAYER.getVolume();
    };
}