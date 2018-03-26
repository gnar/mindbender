#!./cl2

print_if = function(cond, str)
{
	if (cond) sys.println(str);
};

print_if(1, "Hallo, Welt!");


hello = function()
{
	sys.println("Hello, World!");
}
hello();

// "Memberfunktionen"
object = 
[
	say = function()
	{
		sys.println("Hello, World")
		sys.println("My name is ", $name) // $name: Membervariable
	}

	name = "Bob"
];

object.say();	 // member-call (mit this = object)







