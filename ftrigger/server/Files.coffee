fs = require 'fs'

class Files

  rootDir: null

  constructor: (@rootDir) ->

  ReadFiles: (current, done) ->
    fs.readdir current, (err, files) =>
      if err
        return done err

      for file in files
        curDir = current + '/' + file
        sfile = fs.statSync curDir
        if sfile.isDirectory()
          done null, curDir, 3
          @ReadFiles curDir, done
        else
          done null, curDir, 2

module.exports = Files