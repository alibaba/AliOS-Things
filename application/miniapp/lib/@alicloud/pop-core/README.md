# @alicloud/pop-core

The core SDK of POP API.

[![NPM version][npm-image]][npm-url]
[![build status][travis-image]][travis-url]
[![codecov][cov-image]][cov-url]

[npm-image]: https://img.shields.io/npm/v/@alicloud/pop-core.svg?style=flat-square
[npm-url]: https://npmjs.org/package/@alicloud/pop-core
[travis-image]: https://img.shields.io/travis/aliyun/openapi-core-nodejs-sdk/master.svg?style=flat-square
[travis-url]: https://travis-ci.org/aliyun/openapi-core-nodejs-sdk
[cov-image]: https://codecov.io/gh/aliyun/openapi-core-nodejs-sdk/branch/master/graph/badge.svg
[cov-url]: https://codecov.io/gh/aliyun/openapi-core-nodejs-sdk

## Installation

Install it and write into package.json dependences.

```sh
$ npm install @alicloud/pop-core -S
```

## Prerequisite

Node.js >= 8.x

### Notes

You must know your `AK`(`accessKeyId/accessKeySecret`), and the cloud product's `endpoint` and `apiVersion`.

For example, The ECS OpenAPI(https://help.aliyun.com/document_detail/25490.html), the API version is `2014-05-26`.

And the endpoint list can be found at [here](https://help.aliyun.com/document_detail/25489.html), the center endpoint is ecs.aliyuncs.com. Add http protocol `http` or `https`, should be `http://ecs.aliyuncs.com/`.


## Online Demo

**[API Explorer](https://api.aliyun.com)** provides the ability to call the cloud product OpenAPI online, and dynamically generate SDK Example code and quick retrieval interface, which can significantly reduce the difficulty of using the cloud API. **It is highly recommended**.

<a href="https://api.aliyun.com" target="api_explorer">
  <img src="https://img.alicdn.com/tfs/TB12GX6zW6qK1RjSZFmXXX0PFXa-744-122.png" width="180" />
</a>


## Usage

The RPC style client:

```js
var RPCClient = require('@alicloud/pop-core').RPCClient;

var client = new RPCClient({
  accessKeyId: '<accessKeyId>',
  accessKeySecret: '<accessKeySecret>',
  endpoint: '<endpoint>',
  apiVersion: '<apiVersion>'
});

// => returns Promise
client.request(action, params);
// co/yield, async/await

// options
client.request(action, params, {
  timeout: 3000, // default 3000 ms
  formatAction: true, // default true, format the action to Action
  formatParams: true, // default true, format the parameter name to first letter upper case
  method: 'GET', // set the http method, default is GET
  headers: {}, // set the http request headers
});
```

The ROA style client:

```js
var ROAClient = require('@alicloud/pop-core').ROAClient;

var client = new ROAClient({
  accessKeyId: '<accessKeyId>',
  accessKeySecret: '<secretAccessKey>',
  endpoint: '<endpoint>',
  apiVersion: '<apiVersion>'
});

// => returns Promise
// request(HTTPMethod, uriPath, queries, body, headers, options);
// options => {timeout}
client.request('GET', '/regions');
// co/yield, async/await
```

### Custom opts

We offer two ways to customize request opts.

One way is passing opts through the Client constructor. You should treat opts passed through the constructor as default custom opts, because all requests will use this opts.

```js
var client = new RPCClient({
  accessKeyId: '<accessKeyId>',
  accessKeySecret: '<accessKeySecret>',
  endpoint: '<endpoint>',
  apiVersion: '<apiVersion>',
  opts: {
    timeout: 3000
  }
});
```

Another way is passing opts through the function's parameter. You should use this way when you want to just pass opts in specific functions.

```js
client.request(action, params, {
  timeout: 3000
});
```

When both ways are used, opts will be merged. But for the opt with the same key, the opts provided by the function parameter overrides the opts provided by the constructor.

### Http Proxy Support

```js
var tunnel = require('tunnel-agent');

var RPCClient = require('@alicloud/pop-core').RPCClient;

var client = new RPCClient({
  accessKeyId: '<accessKeyId>',
  accessKeySecret: '<accessKeySecret>',
  endpoint: '<endpoint>',
  apiVersion: '<apiVersion>'
});


client.request(action, params, {
  agent: tunnel.httpOverHttp({
    proxy: {
      host: 'host',
      port: port
    }
  });
});
```

## License
The MIT License
