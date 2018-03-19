
// Klasse Individuum
Individuum = [
	
	exestiere = function()
	{
		sys.println("Ich exestiere")
	}

]

// Klasse Mensch
Mensch = [

	parent = Individuum // Leite Mensch von Individuum ab

	new = function(name)
	{
		local obj = []
		obj.parent = Mensch
		if (name)
			obj.name = name
		else
			obj.name = "unbekannt"
		return(obj)
	}

	say = function()
	{
		sys.println("Hallo, ich heisse ", $name) // '$' Bedeutet, dass im this-Kontext gesucht wird
	}
]



// Mensch-Exemplar
fred = Mensch.new()
fred.name = "Fred"
fred.say()
fred.exestiere()

bob = Mensch.new("Bob")
bob.say()

