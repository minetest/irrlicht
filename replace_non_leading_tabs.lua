#!/bin/env luajit

-- SPDX-FileCopyrightText: 2024 DS
--
-- SPDX-License-Identifier: Apache-2.0

local function string_width(s)
	local w = 0
	for i = 1, #s do
		if s:sub(i,i) == "\t" then
			w = w + 4
		else
			w = w + 1 -- (just an estimate. works well for most of ascii)
		end
	end
	return w
end

local path = ...
assert(path, "No path given")

local file = assert(io.open(path, "r"))

local lines_new = {}
local in_multiline_comment = false

for line in file:lines() do
	local line_new = {}
	local line_new_columns = 0
	local function add_to_line_new(s)
		table.insert(line_new, s)
		line_new_columns = line_new_columns + string_width(s)
	end

	-- skip leading tabs
	local i = line:find("[^\t]") or #line + 1
	add_to_line_new(string.rep("\t", i-1))

	while i <= #line do
		if in_multiline_comment then
			local j = line:find("*/", i, true)
			if j then
				add_to_line_new(line:sub(i, j+1))
				in_multiline_comment = false
				i = j + 2
			else
				add_to_line_new(line:sub(i))
				i = #line + 1
			end
		else
			local j = line:find("\t", i, true) or #line + 1
			j = math.min(j, line:find("/*", i, true) or #line + 1)
			j = math.min(j, line:find("//", i, true) or #line + 1)
			if j ~= #line + 1 then
				add_to_line_new(line:sub(i, j-1))
				if line:sub(j, j) == "\t" then
					add_to_line_new(string.rep(" ", 4 - (line_new_columns % 4)))
					i = j + 1
				elseif line:sub(j, j+1) == "/*" then
					add_to_line_new("/*")
					in_multiline_comment = true
					i = j + 2
				else -- //
					add_to_line_new(line:sub(j))
					i = #line + 1
				end
			else
				add_to_line_new(line:sub(i))
				i = #line + 1
			end
		end
	end

	line_new = table.concat(line_new)
	table.insert(lines_new, line_new)
end

file:close()

-- new line at end of file
table.insert(lines_new, "")

lines_new = table.concat(lines_new, "\n")

file = assert(io.open(path, "w"))
assert(file:write(lines_new))
file:close()
