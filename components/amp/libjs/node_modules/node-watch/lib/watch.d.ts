import { FSWatcher } from 'fs';

/**
 * Watch for changes on `filename`, where filename is either a file or a directory. The second argument is optional.
 *
 * - If `options` is provided as a string, it specifies the encoding. Otherwise `options` should be passed as an object.
 *
 * - The listener callback gets two arguments `(eventType, filename)`. `eventType` is either `update` or `remove`, and
 * `filename` is the name of the file which triggered the event.
 *
 * @param {Filename} filename File or directory to watch.
 * @param {WatchOptions|string} options
 * @param {Function} listener
 */
declare function watch(filename: Filename, listener : (eventType: 'update' | 'remove', filename: string) => any) : ImprovedFSWatcher;
declare function watch(filename: Filename, options : string, listener : (eventType: 'update' | 'remove', filename: string) => any) : ImprovedFSWatcher;
declare function watch(filename: Filename, options : WatchOptions, listener : (eventType: 'update' | 'remove', filename: string) => any) : ImprovedFSWatcher;

interface FilenameArray extends Array<FilenameArray | string> {}

type Filename = string | FilenameArray;

type WatchOptions = {
    /**
     * Indicates whether the process should continue to run as long as files are being watched.
     * @default true
     */
    persistent ?: boolean;

    /**
     * Indicates whether all subdirectories should be watched, or only the current directory. This applies when a
     * directory is specified, and only on supported platforms.
     *
     * @default false
     */
    recursive ?: boolean;

    /**
     * Specifies the character encoding to be used for the filename passed to the listener.
     * @default 'utf8'
     */
    encoding ?: string;

    /**
     * Only files which pass this filter (when it returns `true`) will be sent to the listener.
     */
    filter ?: RegExp | ((file: string) => boolean);

    /**
     * Delay time of the callback function.
     * @default 200
     */
    delay ?: number;
};

declare interface ImprovedFSWatcher extends FSWatcher {
    /**
     * Returns `true` if the watcher has been closed.
     */
    isClosed(): boolean;
}

export default watch;
