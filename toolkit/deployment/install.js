#!/usr/bin/env node
var temp_index = 'var http = require("http"); http.createServer(function(req, res) {res.writeHead(200); res.end("hello world"); }).listen(0); setInterval(function(){console.log(+new Date)},3)';
 




var appname = process.argv[2];
var apppath = process.argv[3];
var fs = require('fs'),
	path = require('path');

fs.writeFile( path.join(apppath,'index.js'), temp_index , function (err) {
  if (err) process.exit(1)
  else process.exit(0)
});
