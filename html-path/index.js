'use strict';

var path = require('path');
var Transform = require('readable-stream/transform');
var rs = require('replacestream');
var istextorbinary = require('istextorbinary');
var chalk = require('chalk');
const isRelativeUrl = require('is-relative-url');

// match url,link src='xxx/xxx.html?a' || href = 'xxx/xxx.html?a' || background:url('xxx/xxx.html?a');
var REG_HTML = /([\s]href=|[\s]src=)['"][a-zA-Z_\-\d\.\/\?&#=]+['"]|[\s:]url\(\s*['"]?[a-zA-Z_\-\d\.\/\?&#=]+['"]?\s*\)/g;
// match src="" || href = ""  || url()
var REG_SUB = /([\s]href=|[\s]src=)|['"]|[\s:]url\(\s*|\s*\)/g;
var MATCHED = 0;
var REPLACED = 0;

/**
 *
 * @param {String}  options.base        'root path'
 * @param {Boolean} options.log         'print log'
 * @param {String}  options.prePath     'cnd path'
 * @param {String}  options.ignore      'ignore url,do not replace.use regExp match，default "#ignore" '
 * @param {String}  options.ignoreHref  'ignore url,do not replace.use regExp match，default "#ignore" '*
 */
module.exports = function(options) {
  var base;
  options = options || {};

  options.ignore = options.ignore || '#ignore';
  options.ignoreHref = options.ignoreHref || true;
  options.ignoreCss = options.ignoreCss || true;
  options.ignoreJs = options.ignoreJs || true;


  if (options.hasOwnProperty('base')) {
    base = path.join(process.cwd(), options.base);
  } else {
    base = process.cwd();
  }
  var buildPathMode = options.mode || 'absolute';
  log('\nbase => ' + base);
  var isLog = options.log || false;

  function log(msg, ck) {
    if (isLog) {
      if (ck) {
        console.log(chalk[ck](msg))
      } else {
        console.log(msg)
      }
    }
  }

  return new Transform({
    objectMode: true,
    transform: function(file, enc, callback) {
      if (file.isNull()) {
        return callback(null, file);
      }

      function doReplace() {
        if (file.isStream()) {
          file.contents = file.contents.pipe(rs(search, replacement));
          return callback(null, file);
        }
        if (file.isBuffer()) {
          log('\nchecking file...' + file.history[0], 'green');
          var fileContent = file.contents.toString();
          var filePath = path.dirname(file.history[0]);
          log('filePath =>' + filePath)
          var extname = path.extname(file.history[0]);
          fileContent = fileContent.replace(REG_HTML, function(matchString) {
            var url = matchString.replace(REG_SUB, '');

            if(!isRelativeUrl(url)) {
              log('\n' + ' => ignore absolute path: ' + url, 'red');
              return matchString;
            }

            if(url.indexOf("//") != -1) {
              log('\n' + ' => ignore absolute path: ' + url, 'red');
              return matchString;
            }

            // ignore
            if (options.ignore && new RegExp(options.ignore, 'g').test(url)) {
              log('\n' + ' => ignore filepath: ' + url, 'red');
              return matchString;
            }

            if (options.ignoreHref && matchString.indexOf('href=') != -1) {
              log('\n' + ' => ignore href: ' + matchString, 'red');
              return matchString;
            }

            if (options.ignoreJs && url.endsWith('.js')) {
              log('\n' + ' => ignore js: ' + url, 'red');
              return matchString
            }

            if (options.ignoreCss && url.endsWith('.css')) {
              log('\n' + ' => ignore css: ' + url, 'red');
              return matchString
            }

            MATCHED++;
            log('\n' + matchString + ' => ' + url, 'blue');
            var absoluteUrl;
            if (url.indexOf('/') === 0) {
              absoluteUrl = path.join(base, url);
            } else {
              absoluteUrl = path.resolve(base, filePath, url);
            }



            log('absoluteUrl => ' + absoluteUrl);
            var relativeUrl = path.relative(filePath, absoluteUrl);
            log('ralvativeUrl => ' + relativeUrl);
            var absoluteUrlInHtml = absoluteUrl.replace(base, '');
            log('absoluteUrlInHtml => ' + absoluteUrlInHtml);
            var result;
            if (options.mode == 'relative') {
              result = matchString.split(url).join((options.prePath || '') + relativeUrl);
            } else {
              if(absoluteUrlInHtml.startsWith("/data")) {
                absoluteUrlInHtml = absoluteUrlInHtml.replace('/data', '');
              }
              result = matchString.split(url).join((options.prePath || '') + absoluteUrlInHtml);
            }
            log('result: ' + matchString + ' => ' + result);
            REPLACED++;
            return result.replace(/\\/g, '/');
          });
          file.contents = new Buffer(fileContent);
          return callback(null, file);
        }

        callback(null, file);
      }

      if (options && options.skipBinary) {
        istextorbinary.isText(file.path, file.contents, function(err, result) {
          if (err) {
            return callback(err, file);
          }

          if (!result) {
            callback(null, file);
          } else {
            doReplace();
          }
        });

        return;
      }

      doReplace();

      log('\nFOUND:' + MATCHED + ' REPLACED:' + REPLACED + '\n');
    }
  });
};