binding = require "../build/default/binding"
console.log binding

console.log "The function 'theFunction' returns: " + binding.theFunction()

console.log "The function 'anotherFunction' returns: " + binding.anotherFunction()
