fs = require 'fs'

class Files

  rootDir: null

  constructor: (@rootDir) ->

  WalkFiles: (current, done) ->
    fs.readdir current, (err, files) =>
      console.error err if err
      return err if err

      for file in files
        tmp = (file) =>
          curDir = current + '/' + file
          fs.stat curDir, (err, sfile) =>
            console.error err if err
            return err if err

            if sfile.isDirectory()
              done null, curDir, 3
              @WalkFiles curDir, done
            else
              done null, curDir, 2
        tmp file

  Read: (path) ->
    # fs.open path, 'r', 0666, (err, fd) =>
    #   return err if err
    #   fs.read()


module.exports = Files
