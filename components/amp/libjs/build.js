const watch = require('node-watch');
const minify = require('@node-minify/core');
const babelMinify = require('@node-minify/babel-minify');
const fse = require('fs-extra');

let watchFile = false;
process.argv.forEach((val, index) => {
    if(val === '--watch'){
        watchFile = true;
    }
});

fse.ensureDirSync('../build/bin/node_modules');

minify({
    compressor: babelMinify,
    input: 'lib/*.js',
    output: '../build/bin/node_modules/$1.js',
    options: {
    babelrc: '.babelrc'
    },
    callback: function(err, min) {
        console.log('minify updated');
    }
});

if(watchFile) {
    console.log('watching...');
    watch('./lib', { recursive: true }, function(evt, name) {
        console.log('%s changed.', name);
        minify({
            compressor: babelMinify,
            input: 'lib/*.js',
            output: '../build/bin/node_modules/$1.js',
            options: {
            babelrc: '.babelrc'
            },
            callback: function(err, min) {
                console.log('minify updated');
            }
        });
    });
}

