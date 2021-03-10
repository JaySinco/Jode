import assert from 'assert';
import os from 'os';
import {shell} from '~/bin/addon.node';

assert.strictEqual(shell.isFileHidden(`C:\\Users\\${os.userInfo().username}\\AppData`), true);
assert.strictEqual(shell.isFileHidden('C:\\Windows\\System32'), false);

console.log('[PASSED] all tests.')
