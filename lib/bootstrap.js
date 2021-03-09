global.require = require('module').createRequire(global.__rpath + '/');
global.sleep = ms => new Promise(resolve => setTimeout(resolve, ms));
require('vm').runInThisContext(global.__code, global.__filename);
