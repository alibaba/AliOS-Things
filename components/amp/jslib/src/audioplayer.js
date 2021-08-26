import * as AUDIOPLAYER from 'AUDIOPLAYER';
import * as events from 'events'

export default class audioplayer extends events.EventEmitter {
    constructor(){
        super();
        this._onState();
    }

    static play(source, callback) {
        if (!source || !callback) {
            throw new Error('invalid params');
        }
        console.log('play ' + source);
        return AUDIOPLAYER.play(source, callback);
    };

    static pause() {
        return AUDIOPLAYER.pause();
    };

    static resume() {
        return AUDIOPLAYER.resume();
    };

    static stop() {
        return AUDIOPLAYER.stop();
    };

    static seekto(seconds) {
        if (seconds < 0) {
            throw new Error('invalid params');
        }
        return AUDIOPLAYER.seekto(seconds);
    };

    static getPosition() {
        return AUDIOPLAYER.getPosition();
    };

    static getDuration() {
        return AUDIOPLAYER.getDuration();
    };

    static getState() {
        return AUDIOPLAYER.getState();
    };

    static _onState() {
        AUDIOPLAYER.onState(function(state) {
            this.emit('stateChange', state);
        }.bind(this));
    };

    static listPlay(sourcelist, callback) {
        if (!sourcelist || !callback) {
            throw new Error('invalid params');
        }
        return AUDIOPLAYER.listPlay(sourcelist, callback);
    };

    static listPlayStop() {
        return AUDIOPLAYER.listPlayStop();
    };

    static setVolume(volume) {
        if (volume < 0) {
            throw new Error('invalid params');
        }
        return AUDIOPLAYER.setVolume(volume);
    };

    static getVolume() {
        return AUDIOPLAYER.getVolume();
    };
}