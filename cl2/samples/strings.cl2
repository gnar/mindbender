

tmp = "Hallo"

sys.println(tmp)
sys.println(tmp.concat(" Welt!"))
sys.println(tmp)

local pos, char
foreach (pos, char in tmp)
{
	sys.println("tmp[", pos, "] = ", char)
}


