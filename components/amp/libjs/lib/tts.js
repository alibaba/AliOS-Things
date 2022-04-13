
'use strict';

class TTSSystem {
    play(content, encode) {
        if (!content || !encode) {
            throw new Error('invalid params');
        }
        return __native.TTS.play(content, encode);
    };

    stop() {
        return __native.TTS.stop();
    };

    pitch(type) {
        if (!type) {
            throw new Error('invalid params');
        }
        return __native.TTS.pitch(type);
    };

    speed(speed) {
        if (!speed) {
            throw new Error('invalid params');
        }
        return __native.TTS.speed(speed);
    };

    volume(volume) {
        if (!volume) {
            throw new Error('invalid params');
        }
        return __native.TTS.volume(volume);
    };
}

function create() {
    return new TTSSystem();
}

module.exports = {
    create,
}