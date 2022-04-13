'use strict';

class AudioSystem {
    play(source) {
        if (!source) {
            throw new Error('invalid params');
        }
        return __native.Audio.play(source);
    };

    pause() {
        return __native.Audio.pause();
    };

    resume() {
        return __native.Audio.resume();
    };

    stop() {
        return __native.Audio.stop();
    };

    volume(device, volume) {
        if (!device || !volume) {
            throw new Error('invalid params');
        }
        return __native.Audio.volume(device, volume);
    };

    setGain(gain) {
        if (!gain) {
            throw new Error('invalid params');
        }
        return __native.Audio.setGain(gain);
    };

    getGain() {
        return __native.Audio.getGain();
    };

    mute(state) {
        if (!state) {
            throw new Error('invalid params');
        }
        return __native.Audio.mute(state);
    };
}

function create() {
    return new AudioSystem();
}

module.exports = {
    create,
}