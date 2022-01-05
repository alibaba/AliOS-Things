import * as events from 'events'
import * as AUDIORECORDER from 'AUDIORECORDER';

class AUDIO_RECORDER extends events.EventEmitter {
    constructor(){
        super();
    }

    record(samplerate, channels, sbits, frames, sink) {
        if (!sink) {
            throw new Error('invalid params');
        }
        return AUDIORECORDER.record(samplerate, channels, sbits, frames, sink);
    };

    pause() {
        return AUDIORECORDER.pause();
    };

    resume() {
        return AUDIORECORDER.resume();
    };

    stop() {
        return AUDIORECORDER.stop();
    };

    getPosition() {
        return AUDIORECORDER.getPosition();
    };

    getState() {
        return AUDIORECORDER.getState();
    };
}

export function open() {
    return new AUDIO_RECORDER();
}

