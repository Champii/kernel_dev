fs = require 'fs'
async = require 'async'

class Files

  rootDir: null
  opened: {}

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

  Open: (path, done) ->
    fs.open path, 'r', 666, (err, fd) =>
      if err
        return done err

      @opened[path] = fd
      console.log 'Opened : ', path
      done null, fd

  Close: (path, done) ->
    fs.close @opened[path], (err) =>
      delete @opened[path]
      done err

  Read: (path, length, position, doneRead) ->
    async.auto
      open: (done) =>
        if (!@opened[path]?)
          @Open path, done
        else
        done()
      read: ['open', (done, results) =>
        console.log path, length, position, @opened
        buffer = new Buffer(length)
        fs.read @opened[path], buffer, 0, length, position, (err, readLen, buffer) =>
          console.error err if err
          return done err if err
          done null, {readLen: readLen, buffer: buffer}]
      , (err, results) =>
        if err
          return doneRead err
        doneRead null, results.read.readLen, results.read.buffer

module.exports = Files
