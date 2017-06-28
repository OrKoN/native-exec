#!/usr/bin/env node

var exec = require('../..');

exec("env", {
  fake: 'env',
  any: 'env',
  dißßerent: 'chäräcters',
  різні: "символи",
  аЯкщо: "так=ось",
  number: 10,
  boolean: 1,
  nil: null,
  undefined: undefined,
  etc: undefined,
}, []);