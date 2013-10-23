net = require 'net'
Struct = require('struct').Struct

Prot = Struct()
        .word32Sle('i')
        .chars('c', 1)

Prot.allocate()

server = net.createServer (c) ->
  console.log 'Connected'

  c.on 'data', (data) ->
    Prot._setBuff data
    prot = Prot.fields
    console.log prot.i, prot.c

    prot.i = 2;
    prot.c = 'b'
    c.write Prot.buffer()

  c.on 'end', ->
    console.log 'Disconnected'

server.listen '12345', () ->
  console.log 'Listening on 12345'
