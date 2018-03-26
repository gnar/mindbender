

// Globale Variablen stehen im root-Table:

root.global_var = 4

// Syntaktischer Zucker:
global_var = 5 // "root." ist optional




// Lokale Variablen mit Scope:

root.x = 1 // (hier global)

func = function()
{
	sys.println(x)
	local x = 3
	{
		local x = 2
		sys.println(x)
	}
	sys.println(x)
};

func() // "1, 2, 3"


// (hash-)Table constructor:
tab = table [] // erzeugt leeren Table ('table' ist optional)
tab.x = 4 // erzeugt table-eintrag ("x"->4)

tab = [  // table constructor mit initialisierung
	abc = 4 
	def = 5

	ghi = function()
	{	
		sys.println("ghi():")
		sys.println(self)
		sys.println(self.abc) // liest member-variable aus, (wenn ghi als member-call aufgerufen wird)
		sys.println($abc)  // Syntaxzucker
	}
];

tab.ghi() // Fuehrt tab.ghi aus und setzt den self-Kontext auf tab

(tab.ghi)() // Normaler aufruf, aktueller self-Kontext (im Toplevel ist das root) wird kopiert


