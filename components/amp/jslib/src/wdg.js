import * as WDG from 'WDG'

export function start(timeout) {
    if (!timeout) {
        throw new Error('params is invalid');
    }
    WDG.start(timeout);
}

export function feed() {
   WDG.feed();
}

export function stop() {
    WDG.stop();
}
