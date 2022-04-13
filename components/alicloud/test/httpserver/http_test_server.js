'use strict';
const express = require('express');
const bodyParser = require('body-parser');
const app = express();

app.listen(8021);

app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.text({ type: 'text/html' }))
app.use(bodyParser.raw({ type: 'application/vnd.custom-type' }))
app.use(bodyParser.json({ type: 'application/*+json' }))

app.get('*', function (req, res) {
  console.log('GET req.params: ', req.params);
  console.log('GET req.query:  ', req.query);
  if (req.params[0] === '/readTimeoutTest') {
    setTimeout(function() {
      res.send("something");
    }, Number(req.query.timeout) + 100);
  } else {
    res.send(JSON.stringify(req.query));
  }
});

app.post('*', function (req, res) {
  console.log('POST req.params: ', req.params);
  console.log('POST req.query:  ', req.query);
  console.log('POST req.body:', req.body);
  res.send('POST: ' + req.body);
});


app.put('*', function (req, res) {
  console.log('PUT req.params: ', req.params);
  console.log('PUT req.query:  ', req.query);
  console.log('PUT req.body:   ', req.body);
  res.send('PUT: ' + req.body);
});

app.delete('*', function(req, res) {
  console.log('DELETE req.params: ', req.params);
  console.log('DELETE req.query:  ', req.query);
  console.log('DELETE req.body:   ', req.body);
  res.send("DELETE: " + req.body);
});
