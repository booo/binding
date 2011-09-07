binding = require "../build/default/binding"
console.log binding

console.log "The function 'theFunction' returns: " + binding.theFunction()

console.log "The function 'anotherFunction' returns: " + binding.anotherFunction()

console.log "The result of add(1,1) is: " + binding.add 1,1

try
  binding.add()
catch e
  console.log "Exception : " + e

binding.functionWithCallback (data) ->
  console.log "Callback of 'functionWithCallback' called."
  console.log "data: " + data

binding.asyncFunction 10, (fac) ->
  console.log "10!: " + fac
