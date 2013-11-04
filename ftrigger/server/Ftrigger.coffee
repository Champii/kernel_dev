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
      7: @Write
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

  SendFileName: (client, file, size, type) ->
    realFile = file[rootDir.length ... file.length]
    @proto.fields.code = type
    @proto.fields.size = size
    @proto.fields.args = realFile
    buffer = new Buffer(@proto.struct.buffer())
    wrote = client.sockCtrl.write buffer

  ParseRequest: (request, client) ->
    @proto.struct._setBuff request
    if @actionArray[@proto.fields.code]?
      @actionArray[@proto.fields.code].apply @, [@proto.fields, client]

  Mount: (request, client) ->
    @files.WalkFiles rootDir, (err, file, size, type) =>
      return console.error if err

      @SendFileName client, file, size, type

  Open: (request, client) ->
    @files.Open rootDir + request.args, 0, (err, fd) =>
      return @SendError client, err if err
      @proto.fields.code = 4
      buffer = new Buffer(@proto.struct.buffer())
      client.sockData.write buffer

  Close: (request, client) ->
    @files.Close rootDir + request.args, (err) =>
      return @SendError client, err if err
      @proto.fields.code = 8
      buffer = new Buffer(@proto.struct.buffer())
      client.sockData.write buffer

  Read: (request, client) ->
    @files.Read rootDir + request.args, request.len, request.off, (err, readLen, chunk) =>
      return @SendError client, err if err
      @proto.fields.code = 6
      @proto.fields.len = readLen
      @proto.fields.args = chunk
      buffer = new Buffer(@proto.struct.buffer())
      client.sockData.write buffer

  Write: (request, client) ->
    @files.Write rootDir + request.path, request.args, request.len, request.off, (err, writeLen) =>
      return @SendError client, err if err
      @proto.fields.code = 7
      @proto.fields.len = writeLen
      buffer = new Buffer(@proto.struct.buffer())
      client.sockData.write buffer

  SendError: (client, err) ->
    console.error err
    @proto.fields.code = 9
    @proto.fields.args = err
    buffer = new Buffer(@proto.struct.buffer())
    client.sockCtrl.write buffer

  Run: ->
    @server.listen '12345', () ->
      console.log 'Listening on 12345'


module.exports = Ftrigger
