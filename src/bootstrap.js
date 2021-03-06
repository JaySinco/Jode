globalThis.require = require('module').createRequire(__rpath__ + '/');
require('vm').runInThisContext(__code__, __filename__);
