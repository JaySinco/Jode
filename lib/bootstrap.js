global.require = require('module').createRequire(global.__rpath__ + '/');
global.sleep = ms => new Promise(resolve => setTimeout(resolve, ms));
require('vm').runInThisContext(global.__code__, global.__filename__);
