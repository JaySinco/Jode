import assert from 'assert';
import {shell} from '../bin/addon';

const text_w = '剪贴板功能测试';
const ok_w = shell.writeClipboard(text_w);
assert.strictEqual(ok_w, true);
const [ok_r, text_r] = shell.readClipboard();
assert.strictEqual(ok_r, true);
assert.strictEqual(text_r, text_w);

console.log('[PASSED] all tests.')
