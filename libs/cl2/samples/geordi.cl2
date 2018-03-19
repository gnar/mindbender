#!/home/gunnar/src/cl2/bin/cl2

Geordi = 
[
	babble = function()
	{
		local B = array
		[
			array ["field "], array ["tachyon "], array ["baryon "], array ["lepton "], array ["e-m "],
			array ["phase "], array ["pulse "], array ["sub-space "], array ["spectral "], array ["antimatter "],
			array ["plasma "], array ["bandwidth "], array ["particle "]
		]

		local C = array 
		[
			array["dispersion "], array ["induction "], array ["frequency "], array ["resonance "]
		]

		local D = array 
		[
			array["conduit "], array["discriminator "], array["modulator "], 
			array["transducer "], array["wave-guide "], array["coils "], 
			array["matrix "], array["sensors "]
		]

		local E = array [
			array ["one "],
			array ["two "],
			array ["three "],
			array ["four "],
			array ["five "]
		]

		local A = array [
			array ["perform a level ", E, "diagnostic on "],
			array ["run a level ", E, "diagnostic on "],
			array ["reroute the ", B, C, D, "to "],
			array ["redirect the ", B, C, D, "to "],
			array ["divert the ", B, C, D, "to bypass "],
			array ["amplify "], array ["modify "], array ["polarize "], array ["reconfigure "],
			array ["extend "], array ["rebuild "], array ["vary "], array ["analyze "],
			array ["adjust "], array ["recalibrate "]
		]

		local W = array ["Captain, we need to ", A, "the ", B, C, D, "!"] 

		return (W)
	}

	get_string = function(choice)
	{
		local is_string = function(o)
		{
			return (o.length != null) // hack
		}

		local choose_item = function(arr)
		{
			return(arr[math.random(arr.n)])
		}

		if (is_string(choice)) return(choice)

		// sonst ist es ein array
		local result = "", item
		foreach(item in choice)
		{
			if (is_string(item))
			{
				result = result.concat(item)
			} else {
				result = result.concat($get_string(choose_item(item)))
			}
		}
		
		return(result)
	}

	Go = function()
	{
		return($get_string($babble()))
	}
]

sys.println(Geordi.Go())
