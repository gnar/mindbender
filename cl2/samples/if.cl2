#!./cl2

sys.println("5<1 ? ", 5<1); // null ist "Unwahr"
sys.println("5>1 ? ", 5>1); // Alles ausser null ist "Wahr" (also auch 0)

if ("abc" == "abc")
{
	sys.println("Ok: abc == abc")
} else {
	sys.println("Verdammt")
}

sys.println("not null: ", not null)
sys.println("null and not null: ", null and not null)
sys.println("not null or 2: ", not null or 2)


f = function()
{
	sys.println("(Seiteneffekt!)")
	// return null; ist implizit
}

if (1 or f())
{
	sys.println("Keine Short-Circuit-Evaluation")
}

