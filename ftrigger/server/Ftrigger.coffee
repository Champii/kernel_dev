net = require 'net'

Files = require './Files'
Protocole = require './Protocole'


rootDir = process.argv[2]

clients = []

class Client

  sockCtrl: null
  sockData: null

  constructor: ->



class Ftrigger

  server: null
  proto: null
  files: null
  writeQueue: null
  actionArray:null

  constructor: ->
    @actionArray =
      1: @Mount
      4: @Open
      5: @Read
      8: @Close

    @proto = new Protocole()
    @files = new Files()
    @server = net.createServer (stream) =>
      if !clients[stream.remoteAddress]?
        console.log 'Connected', stream.remoteAddress
        client = new Client()
        client.sockCtrl = stream
        clients[stream.remoteAddress] = client
      else
        clients[stream.remoteAddress].sockData = stream
        console.log 'Connected ', stream.remoteAddress, ' -> Data socket'

      stream.setNoDelay(true);
      stream.on 'data', (data) =>
        @ParseRequest data, client
      stream.on 'end', ->
        console.log 'Disconnected'
      stream.on 'error', (err) ->
        console.error 'ERROR : ', err

  SendFileName: (client, file, type) ->
    realFile = file[rootDir.length ... file.length]
    @proto.fields.code = type
    @proto.fields.args = realFile
    buffer = new Buffer(@proto.struct.buffer())
    wrote = client.sockCtrl.write buffer

  ParseRequest: (request, client) ->
    @proto.struct._setBuff request
    @actionArray[@proto.fields.code].apply @, [@proto.fields, client]

  Mount: (request, client) ->
    @files.WalkFiles rootDir, (err, file, type) =>
      return console.error if err

      @SendFileName client, file, type

  Open: (request, client) ->
    @files.Open rootDir + request.args, (err, fd) =>
      if err
        console.error err

  Close: (request, client) ->
    @files.Close rootDir + request.args, (err) =>
      console.error err if err

  Read: (request, client) ->
    @files.Read rootDir + request.args, request.len, request.off, (err, readLen, chunk) =>
      @proto.fields.code = 6
      @proto.fields.len = readLen
      @proto.fields.args = chunk
      buffer = new Buffer(@proto.struct.buffer())
      client.sockData.write buffer

  Run: ->
    @server.listen '12345', () ->
      console.log 'Listening on 12345'


module.exports = Ftrigger
