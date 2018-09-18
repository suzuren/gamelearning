-- Markov Chain Program in Lua
print("Markov Chain Program in Lua")

local MAX_LINE = 2
local line_index = 1;
function allwords ()
    if line_index>=MAX_LINE then
		return nil
	end
	local line;
	if line_index<=MAX_LINE then
		line = io.read() -- current line
		line_index = line_index +1
	end	
	local pos = 1 -- current position in the line
	return function () -- iterator function
		while line do -- repeat while there are lines
			local s, e = string.find(line, "%w+", pos)
			if s then -- found a word?
				pos = e + 1 -- update next position
				return string.sub(line, s, e) -- return the word
			else
				if line_index <= MAX_LINE then
					line = io.read() -- word not found; try next line
					line_index = line_index +1
					pos = 1 -- restart from first position
				end
			end
		end
		return nil -- no more lines: end of traversal
	end
end


function prefix (w1, w2)
	return w1 .. ' ' .. w2
end

local statetab

function insert (index, value)
	if not statetab[index] then
		statetab[index] = {n=0}
	end
	table.insert(statetab[index], value)
end

local N = 2
local MAXGEN = 10


local NOWORD = "\n"
-- build table

print("build table - statetab")

statetab = {}
local w1, w2 = NOWORD, NOWORD

local words = allwords()
for w in words do
	print("word in allwords - ",w)
	insert(prefix(w1, w2), w)
	w1 = w2; w2 = w;
end
insert(prefix(w1, w2), NOWORD)


print("generate text")

-- generate text
w1 = NOWORD; w2 = NOWORD -- reinitialize
for i=1,MAXGEN do
	local list = statetab[prefix(w1, w2)]
	-- choose a random item from list
	local r = math.random(table.getn(list))
	local nextword = list[r]
	if nextword == NOWORD then return end
	io.write(nextword, " ")
	w1 = w2; w2 = nextword
end
