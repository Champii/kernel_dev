fs = require 'fs'

class Files

  rootDir: null

  constructor: (@rootDir) ->

  ReadFiles: (current, done) ->
    files = fs.readdirSync current

    for file in files
      curDir = current + '/' + file
      sfile = fs.statSync curDir
      if sfile.isDirectory()
        done null, curDir, 3
        @ReadFiles curDir, done
      else
        done null, curDir, 2

module.exports = Files