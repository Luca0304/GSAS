const gulp = require('gulp');
const debug = require('gulp-debug');
const moment = require('moment');

const rev = require('gulp-rev');
const revFormat = require('gulp-rev-format');

const plumber = require('gulp-plumber');　 

const fingerprint = require('gulp-fingerprint');
const htmlPath = require('./html-path');
const { task } = require('gulp');

const basePath = "/data"
const destPath = "/data"

const urlPrefix = process.env.URL_PREFIX

const targetImage = [`${basePath}/**/*.jpg`, `${basePath}/**/*.gif`, `${basePath}/**/*.png`, `${basePath}/**/*.jpeg`, `${basePath}/**/*.svg`, `!${basePath}/**/*.cache.*`]
const targetSource = [`${basePath}/**/*.html`, `${basePath}/**/*.css`, `${basePath}/**/*.php`, `!${basePath}/**/*jquery*`, `!${basePath}/**/*.js`,]

const lastYear = moment().subtract(1, 'year').toDate()

gulp.task('since', (done) => {
  return gulp.src(targetImage, { nocase: true, since: lastYear })
    .pipe(plumber())
    .on('error', function(err) {
      console.log(err.message);
    })
    .pipe(debug())
})

gulp.task('test', (done) => {
  console.log("hello, world");
  console.log(process.env.URL_PREFIX)
  done()
})


gulp.task('rev', () => {
  return gulp.src(targetImage, { nocase: true, since: lastYear })
    .pipe(rev())
    .pipe(debug())
    .pipe(revFormat({
      suffix: '.cache'
    }))
    .pipe(gulp.dest(destPath))
    .pipe(rev.manifest())
    .pipe(gulp.dest(destPath))
})

gulp.task('absolute', () => {
  return gulp.src(targetSource)
    .pipe(htmlPath({
      base: basePath,
      mode: 'absolute',
      log: true,
      ignore:'#ignore',
      ignoreJs: true,
      ignoreCss: true,
      ignoreHref: true
    }))
    .pipe(gulp.dest(destPath));
});

gulp.task('fingerprint', () => {

  var options = {
    prefix: urlPrefix,
    verbose: true
  };

  return gulp.src(targetSource, { nocase: true })
    .pipe(fingerprint(require(`${destPath}/rev-manifest.json`), options))
    .pipe(gulp.dest(destPath));
});

gulp.task('auto-format', (done) => {
  var cdnify = require('gulp-cdnify');

  return gulp.src(targetSource, { nocase: true })
    .pipe(cdnify({
      rewriter: function(path, process) {
        if(!path) {
          return path;
        } else if (!path.startsWith('//')) {
          return path;
        } else if (/(png|jpg|gif|jpeg|svg)$/i.test(path)) {
          path += (path.split('?')[1] ? '&':'?') + 'auto=format'
          return path
        } else {
          return path;
        }
    }
  }))
    .pipe(gulp.dest(destPath))
});

gulp.task('default', gulp.series(
  'rev'
))

gulp.task('cdnify', gulp.series(
  'absolute',
  'fingerprint',
  'auto-format'
))
