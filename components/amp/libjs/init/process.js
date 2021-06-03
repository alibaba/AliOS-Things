(function () {
    globalThis = new Function('return this;')();
    globalThis.process = system;
})();