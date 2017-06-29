'use strict';

var addon = require('bindings')('addon');
var path = require('path');
var fs = require('fs');

module.exports = function(cmd, env, args) {
  if (!cmd) {
    throw new Error('Command is required');
  }

  var envArr = Object.keys(env || {}).map(key => {
    return {
      key,
      value: env[key],
    };
  });

  addon.exec(cmd, envArr, args || []);
};