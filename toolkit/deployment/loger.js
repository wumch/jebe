#!/usr/bin/env node



var appname = process.argv[2];


var fs = require('fs');

var stream =  fs.createWriteStream('./'+appname+'.log', {'flags':'a'})
process.stdin.pipe(stream)
process.stdin.resume();