'use strict';

var addon = require('bindings')('addon');

module.exports = function(cmd, env, args) {
  if (!cmd) {
    throw new Error('Command is required');
  }
  const envArr = Object.keys(env || {}).map(key => {
    return `${key}=${env[key]}`;
  });
  addon.exec(cmd, args || [], envArr);
};