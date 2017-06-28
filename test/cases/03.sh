#!/usr/bin/env node

var exec = require('../..');

exec("env", {
  fake: 'env',
}, []);