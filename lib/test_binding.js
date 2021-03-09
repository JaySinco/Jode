import addon from '~/bin/binding.node';
log.debug('hello debug');
log.info(`hello world: ${addon.getAllocationSize('build.sh')}`);
