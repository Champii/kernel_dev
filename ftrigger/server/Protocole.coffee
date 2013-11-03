Struct = require('struct').Struct

class Protocole

  struct: null
  fields: null

  constructor: ->
    @struct = Struct()
              .word32Sle('code')
              .word32Sle('len')
              .word32Sle('off')
              .chars('args', 255)
    @struct.allocate()
    @fields = @struct.fields

module.exports = Protocole