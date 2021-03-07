globalThis.require = require('module').createRequire(__rpath__ + '/');
globalThis.sleep = ms => new Promise(resolve => setTimeout(resolve, ms));
require('vm').runInThisContext(__code__, __filename__);
