var child_process = require('child_process');
var assert = require('assert');

describe('native-exec', function() {
  describe('01.sh', function() {
    it('should set environmental variables', function() {
      var result = child_process.spawnSync('./test/cases/01.sh').stdout.toString('utf-8');
      assert.strictEqual(
        result,
        'fake=env\n' + 
        'any=env\n' + 
        'dißßerent=chäräcters\n' + 
        'різні=символи\n' + 
        'аЯкщо=так=ось\n' + 
        'number=10\n' + 
        'boolean=1\n' + 
        'nil=null\n' + 
        'undefined=undefined\n' + 
        'etc=undefined\n'
      );
    });
  });
});