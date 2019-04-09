#!/usr/bin/env node

var exec = require('../..');

process.env.TEST_ENV = 'before';

try {
    exec("non-existing-program", {TEST_ENV: 'after'}, []);
} catch (err) {
    if (err instanceof Error) {
        console.log('err.code=' + err.code);
    }
}

console.log('TEST_ENV=' + process.env.TEST_ENV);
