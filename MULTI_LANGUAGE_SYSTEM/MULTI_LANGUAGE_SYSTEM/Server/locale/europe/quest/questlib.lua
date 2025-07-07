--[[ 1. ]]
-- Search
dofile(get_locale_base_path() .. "/quest/GFquestlib.lua")

-- Add above
function file_exists(filename)
	local file = io.open(filename, "r")
	if (file) then
		file:close()
		return true
	end
	return false
end

-- Multi Locale Lib
if file_exists(get_locale_base_path() .. "/quest/multiLocale.lua.lua") then
	dofile(get_locale_base_path() .. "/quest/multiLocale.lua.lua")
end