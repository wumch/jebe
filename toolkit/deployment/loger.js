#!/usr/bin/env node



var appname = process.argv[2];
var logfile = process.argv.length >= 4 ? process.argv[3] : ('./' + appname + '.log');

var fs = require('fs');

var stream =  fs.createWriteStream(logfile, {'flags':'a'})
process.stdin.pipe(stream)
process.stdin.resume();
