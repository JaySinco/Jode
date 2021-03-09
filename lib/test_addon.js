import assert from 'assert';
import os from 'os';
import addon from '../bin/addon.node';

assert.strictEqual(addon.isFileHidden(`C:\\Users\\${os.userInfo().username}\\AppData`), true);
assert.strictEqual(addon.isFileHidden('C:\\Windows\\System32'), false);

console.log('All test passed.')
