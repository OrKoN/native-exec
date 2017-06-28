'use strict';

var addon = require('bindings')('addon');

module.exports = function(cmd, env) {
  addon.exec(cmd);
};