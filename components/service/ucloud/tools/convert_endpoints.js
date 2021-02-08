#!/usr/bin/env node

'use strict';
const fs = require('fs');
const json = require('./endpoints.json');

const products = Object.keys(json.regional_endpoints);
const regions = json.regions;
const mapping = {};

const global_endpoints = json.global_endpoints;
const regional_endpoints = json.regional_endpoints;
const document_id = json.document_id;
const pattern = json.regional_endpoint_pattern;
const location_code_mapping = json.location_code_mapping;
const p_in_mapping = Object.keys(location_code_mapping);

const tocpp = {};
// assign global EP, as the lowest priority, can be overlopped by pattern and regional configuration
const products_with_global_endpoint = Object.keys(global_endpoints);
products_with_global_endpoint.forEach(function(p) {
  if (!products.includes(p)) {
    products.push(p);
  }

  if (!tocpp[p]) {
    tocpp[p] = {};
    tocpp[p].regional = {};
  }
  let ep = global_endpoints[p];
  regions.forEach(function(r) {
    tocpp[p].regional[r] = ep;
  });
});

// pattern
const products_with_pattern = Object.keys(pattern);
products_with_pattern.forEach(function (p) {
  if (!products.includes(p)) {
    products.push(p);
  }

  if (!tocpp[p]) {
    tocpp[p] = {};
    tocpp[p].regional = {};
  }
  let p_pattern = pattern[p];

  regions.forEach(function(r) {
    let pattern_ep = p_pattern.replace('[RegionId]', r);
    tocpp[p].regional[r] = pattern_ep;
  });

});

// regional has the highest priority
const products_with_regional = Object.keys(regional_endpoints);
products_with_regional.forEach(function(p) {
  if (!tocpp[p]) {
    tocpp[p] = {};
    tocpp[p].regional = {};
  }

  let ep_regional = regional_endpoints[p];
  Object.assign(tocpp[p].regional, ep_regional);
});

const aliyun_products = Object.keys(tocpp);
const mapped_products = Object.keys(location_code_mapping);

const mapped_tocpp = {};

aliyun_products.forEach(function(ap) {
  let found = false;
  for (var i = 0; i < mapped_products.length; i++) {
    let mp = mapped_products[i];
    if (ap === location_code_mapping[mp]) {
      mapped_tocpp[mp] = tocpp[ap];
      found = true;
      break
    }
  }
  if (!found) {
    mapped_tocpp[ap] = tocpp[ap];
  }
});

const total_products = Object.keys(mapped_tocpp);

const cpp_header = {};
cpp_header.products = total_products;
cpp_header.regions = regions;
cpp_header.endpoints = {};
total_products.forEach(function(p) {
  let regional = mapped_tocpp[p].regional;
  cpp_header.endpoints[p] = {}
  cpp_header.endpoints[p].regions = Object.keys(regional);
  cpp_header.endpoints[p].regional = regional;
});

var sorted_string = JSON.stringify(cpp_header, null, 2);

sorted_string = sorted_string.replace(/\"/g, "\\\"");
sorted_string = sorted_string.replace(/\n/g,"\"\n\"");

console.log("#include <string.h>");
console.log();
console.log("const std::string LOCAL_ENDPOINTS_CONFIG = \"" + sorted_string + '\";');
