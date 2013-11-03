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

  Open: (path, mode, done) ->
    if !mode
      mode = 'r+'

    fs.open path, mode, 666, (err, fd) =>
      if err
        return done err

      @opened[path] = {fd: fd, mode: mode}
      # console.log 'Opened : ', path
      done null, fd
  Close: (path, done) ->
    fs.close @opened[path].fd, (err) =>
      # console.log 'Closed : ', path
      delete @opened[path]
      done err

  Read: (path, length, position, doneRead) ->
    async.auto
      open: (done) =>
        if !@opened[path]?
          @Open path, 'r', done
        else
          done()
      read: ['open', (done, results) =>
        # console.log path, length, position, @opened
        buffer = new Buffer(length)
        fs.read @opened[path].fd, buffer, 0, length, position, (err, readLen, buffer) =>
          return done err if err
          done null, {readLen: readLen, buffer: buffer}]
      , (err, results) =>
        return doneRead err if err
        doneRead null, results.read.readLen, results.read.buffer

  Write: (path, data, length, position, doneWrite) ->
    async.auto
      open: (done) =>
        if !@opened[path]?
          @Open path, 'w', done
        else
          done()
      write: ['open', (done, results) =>
        buffer = new Buffer(data)
        fs.write @opened[path].fd, buffer, 0, length, position, (err, writeLen) =>
          return done err if err
          done null, writeLen]
    , (err, results) =>
      return doneWrite err if err
      doneWrite null, results.write


module.exports = Files
