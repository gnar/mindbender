
t1 = function()
{
    local i
    for(i=100;i<130;i=i+1)
    {
	sys.println(i)
	yield()
    }
}

t2 = function()
{
    local i
    for(i=0;i<20;i=i+1)
    {
	sys.println(i)
	yield()
    }
}

sys.startthread(t1, null)
sys.startthread(t2, null)

