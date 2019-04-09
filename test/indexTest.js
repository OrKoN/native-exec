var child_process = require('child_process');
var assert = require('assert');
var exec = require('..');

describe('native-exec', function() {
  describe('validation', function() {
    it('should throw an error when there is no cmd', function() {
      assert.throws(
        () => exec(),
        Error
      );
    });
  });

  describe('01.sh', function() {
    it('should set environmental variables', function() {
      var result = child_process.spawnSync('./test/cases/01.sh').stdout.toString('utf-8');
      assert.ok(result.indexOf('fake=env\n') !== -1);
      assert.ok(result.indexOf('any=env\n') !== -1);
      assert.ok(result.indexOf('dißßerent=chäräcters\n') !== -1);
      assert.ok(result.indexOf('різні=символи\n') !== -1);
      assert.ok(result.indexOf('аЯкщо=так=ось\n') !== -1);
      assert.ok(result.indexOf('number=10\n') !== -1);
      assert.ok(result.indexOf('nil=null\n') !== -1);
      assert.ok(result.indexOf('undefined=undefined\n') !== -1);
      assert.ok(result.indexOf('etc=undefined\n') !== -1);
    });
  });

  describe('02.sh', function() {
    it('should work without env and args', function() {
      var result = child_process.spawnSync('./test/cases/02.sh').stdout.toString('utf-8');
      assert.ok(result.indexOf('fake=env\n') === -1);
    });
  });

  describe('03.sh', function() {
    it('should work without args', function() {
      var result = child_process.spawnSync('./test/cases/03.sh').stdout.toString('utf-8');
      assert.ok(result.indexOf('fake=env\n') !== -1);
    });
  });

  describe('04.sh', function() {
    it('should work with args', function() {
      var result = child_process.spawnSync('./test/cases/04.sh').stdout.toString('utf-8');
      assert.ok(result.startsWith('total'));
    });
  });

  describe('failure.sh', function() {
    it('should throw an exception in case of failure and preserve the environment', function() {
      var result = child_process.spawnSync('./test/cases/failure.sh').stdout.toString('utf-8');
      assert.ok(result.indexOf('err.code=ENOENT\n') !== -1);
      assert.ok(result.indexOf('TEST_ENV=before\n') !== -1);
    });
  });
});
