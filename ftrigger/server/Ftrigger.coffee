net = require 'net'

Files = require './Files'
Protocole = require './Protocole'

rootDir = process.argv[2]

class Ftrigger

  server: null
  proto: null
  files: null
  actionArray:null

  constructor: ->
    @actionArray =
      1: @Mount

    @proto = new Protocole()
    @files = new Files()
    @server = net.createServer (client) =>
      console.log 'Connected'
      client.on 'data', (data) =>
        @ParseRequest data, client
      client.on 'end', ->
        console.log 'Disconnected'

  SendFile: (client, file, type) ->
    realFile = file[rootDir.length ... file.length]
    console.log realFile
    @proto.fields.code = type
    @proto.fields.args = realFile
    client.write @proto.struct.buffer()

  ParseRequest: (request, client) ->
    @proto.struct._setBuff request
    @actionArray[@proto.fields.code].apply @, [client]

  Mount: (client) ->
    @files.ReadFiles rootDir, (err, file, type) =>
      return console.error if err

      @SendFile client, file, type

  Run: ->
    @server.listen '12345', () ->
      console.log 'Listening on 12345'


module.exports = Ftrigger