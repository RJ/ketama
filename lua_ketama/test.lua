--[[
Copyright (c) 2009, Phoenix Sol - http://github.com/phoenixsol

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
]]--

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
