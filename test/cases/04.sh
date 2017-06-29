#!/usr/bin/env node

var exec = require('../..');

exec("ls", {
  fake: 'env',
}, ['-lsa']);