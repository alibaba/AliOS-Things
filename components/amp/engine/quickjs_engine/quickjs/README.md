<h1>QuickJS Javascript Engine</h1>

<h2>News</h2>

<ul>
  <li>2020-04-12:
    <ul>
      <li>New release</li>
    </ul>
  </li>
  <li>2020-03-16:
    <ul>
      <li>New release</li>
    </ul>
  </li>
  <li>2020-01-19:
    <ul>
      <li>New release with
    experimental <a href="https://github.com/littledan/proposal-bigdecimal">BigDecimal</a>
    support and updated operator overloading.</li>
      <li><a href="https://bellard.org/quickjs/pi.html">Small Javascript programs</a> to compute
      one billion digits of pi.</li>
    </ul>
  </li>
</ul>

<h2>Introduction</h2>

QuickJS is a small and embeddable Javascript engine. It supports the
<a href="https://tc39.github.io/ecma262/">ES2020</a> specification
including modules, asynchronous generators, proxies and BigInt.
<p>
It optionally supports mathematical extensions such as big decimal
floating point numbers (BigDecimal), big binary floating point numbers
(BigFloat) and operator overloading.
<p>
Main Features:
<ul>

<li> Small and easily embeddable: just a few C files, no external
dependency, 210 KiB of x86 code for a simple <tt>hello world</tt>
program.</li>

<li> Fast interpreter with very low startup time: runs the 69000 tests
of the <a href="https://github.com/tc39/test262">ECMAScript Test
Suite</a> in about 95 seconds on a single core of a desktop PC. The
complete life cycle of a runtime instance completes in less than 300
microseconds.</li>

<li> Almost
  complete <a href="https://tc39.github.io/ecma262/">ES2020</a>
  support including modules, asynchronous generators and full Annex B
  support (legacy web compatibility).</li>

<li>Passes nearly 100% of the ECMAScript Test Suite tests when selecting the ES2020 features.</li>

<li> Can compile Javascript sources to executables with no external dependency.</li>

<li> Garbage collection using reference counting (to reduce memory usage
  and have deterministic behavior) with cycle removal.</li>

<li> Mathematical extensions: BigDecimal, BigFloat, operator overloading, bigint mode, math mode.</li>

<li> Command line interpreter with contextual colorization implemented in Javascript.</li>

<li> Small built-in standard library with C library wrappers.</li>

</ul>

<h2><a href="https://bellard.org/quickjs/bench.html">Benchmark</a></h2>

<h2>Online Demo</h2>

An online demonstration of the QuickJS engine with its mathematical
extensions is available
at <a href="http://numcalc.com">numcalc.com</a>. It was compiled from
C to WASM/asm.js with Emscripten.
<p>
<tt>qjs</tt> and <tt>qjscalc</tt> can be run in <a href="https://bellard.org/jslinux/vm.html?url=buildroot-x86.cfg">JSLinux</a>.

<h2>Documentation</h2>

QuickJS documentation: <a href="https://bellard.org/quickjs/quickjs.html">HTML version</a>,
<a href="https://bellard.org/quickjs/quickjs.pdf">PDF version</a>.
<p>
Specification of the JS Bignum Extensions: <a href="https://bellard.org/quickjs/jsbignum.html">HTML
version</a>, <a href="https://bellard.org/quickjs/jsbignum.pdf">PDF version</a>.

<h2>Download</h2>

<ul>
  <li>
    QuickJS source code: <a href="https://bellard.org/quickjs/quickjs-2020-04-12.tar.xz">quickjs-2020-04-12.tar.xz</a></li>
  <li>
    QuickJS extras (contain the unicode files needed to rebuild the unicode tables and the bench-v8 benchmark): <a href="https://bellard.org/quickjs/quickjs-extras-2020-04-12.tar.xz">quickjs-extras-2020-04-12.tar.xz</a>
  </li>
  <li>
    Unofficial <a href="https://github.com/horhof/quickjs">git mirror</a>.
  </li>
  <li>
    Binary releases for Linux are available in <a href="https://github.com/GoogleChromeLabs/jsvu">jsvu</a> and <a href="https://bellard.org/quickjs/binary_releases">here</a>.
  </li>
  <li>
    Typescript compiler compiled with QuickJS: <a href="https://bellard.org/quickjs/quickjs-typescript-linux-x86.tar.xz">quickjs-typescript-linux-x86.tar.xz</a>
  </li>
  <li>
    Babel compiler compiled with QuickJS: <a href="https://bellard.org/quickjs/quickjs-babel-linux-x86.tar.xz">quickjs-babel-linux-x86.tar.xz</a>
  </li>
</ul>

<h2>Sub-projects</h2>

QuickJS embeds the following C libraries which can be used in other
projects:
<ul>
  <li><b>libregexp</b>: small and fast regexp library fully compliant with the Javascript ES 2019 specification.</li>
  <li><b>libunicode</b>: small unicode library supporting case
conversion, unicode normalization, unicode script queries, unicode
general category queries and all unicode binary properties.</li>
  <li><b>libbf</b>: small library implementing arbitrary precision
    IEEE 754 floating point operations and transcendental functions with
    exact rounding. It is maintained as a <a href="https://bellard.org/libbf">separate project</a>.
  </li>
</ul>

<h2>Links</h2>

<ul>
  <li><a href="https://www.freelists.org/list/quickjs-devel">QuickJS Development</a> mailing list</li>
</ul>

<h2>Licensing</h2>

QuickJS is released under
the <a href="https://opensource.org/licenses/MIT">MIT license</a>.
<p>
Unless otherwise specified, the QuickJS sources are copyright Fabrice
Bellard and Charlie Gordon.

<hr>
Fabrice Bellard - <a href="https://bellard.org">https://bellard.org/</a>
