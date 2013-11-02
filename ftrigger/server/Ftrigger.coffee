net = require 'net'

Files = require './Files'
Protocole = require './Protocole'

rootDir = process.argv[2]

class Ftrigger

  server: null
  proto: null
  files: null
  writeQueue: null
  actionArray:null

  constructor: ->
    @actionArray =
      1: @Mount
      4: @Read

    @proto = new Protocole()
    @files = new Files()
    @server = net.createServer (client) =>
      client.setNoDelay(true);
      console.log 'Connected'
      client.on 'data', (data) =>
        @ParseRequest data, client
      client.on 'end', ->
        console.log 'Disconnected'
      client.on 'error', (err) ->
        console.error 'ERROR : ', err

  SendFile: (client, file, type) ->
    realFile = file[rootDir.length ... file.length]
    @proto.fields.code = type
    @proto.fields.args = realFile
    buffer = new Buffer(@proto.struct.buffer())
    wrote = client.write buffer

  ParseRequest: (request, client) ->
    @proto.struct._setBuff request
    @actionArray[@proto.fields.code].apply @, [@proto.fields, client]

  Mount: (request, client) ->
    @files.WalkFiles rootDir, (err, file, type) =>
      return console.error if err

      @SendFile client, file, type

  Read: (request, client) ->
    @files.Read request.args

  Run: ->
    @server.listen '12345', () ->
      console.log 'Listening on 12345'


module.exports = Ftrigger
