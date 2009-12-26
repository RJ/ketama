local ketama = require 'ketama'

local ket = ketama.roll('../ketama.servers')
--ket:print_continuum() --don't like!

local function compare(ket, a, b)
	--return the greater, or nil if they are equal
	local result = ket:compare(a,b)
	if result == 0 then return nil, 'equal'
	elseif result == 1 then return a
	else return b end
end

print("server for 'foo':", ket:get_server('foo'))
print("greater of 'foo' and 'bar':", compare(ket, 'foo', 'bar'))
print("hashi of 'foo':", ket:hashi('foo'))
print("md5 of 'foo':", ket:md5digest('foo'))

ket:smoke()
print("closed")
collectgarbage("collect")
print("kthxbye")
