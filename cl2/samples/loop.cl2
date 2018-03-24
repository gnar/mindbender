
//
// C-Schleifen
//

local i;
for (i=0; i<10; i=i+1)
{
	sys.println("Loop: ", i)
}

//
// Foreach-Schleifen
//
local key, value
arr = array [0, 5, 2, 3, 6]

foreach (key, value in arr)
{
	sys.println("Foreach: ", key, "->", value)
}

tab = table [ def = 5, abc = 7 ] // tables sind unsortiert, 'table'-Keyword optional
foreach (key, value in tab)
{
	sys.println("Foreach2: ", key, "->", value)
}

foreach (key in array [4,5,6])
{
	sys.println("Foreach3: ", key)
}

