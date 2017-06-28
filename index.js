'use strict';

var addon = require('bindings')('addon');

module.exports = function(cmd, args, env) {
  const envArr = Object.keys(env).map(key => {
    return `${key}=${env[key]}`;
  });
  addon.exec(cmd, args, envArr);
};