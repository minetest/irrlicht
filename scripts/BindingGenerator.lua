#!/usr/bin/lua
-- BindingGenerator.lua (c) hecks 2021
-- This script is a part of IrrlichtMT, released under the same license.

-- By default we assume you're running this from /scripts/
-- and that you have the necessary headers there (gitignored for your convenience)
local sourceTreePath = os.getenv( "IRRMTREPO" ) or "..";
-- Otherwise run this from wherever you want and set the above env variable.
local glHeaderPath = os.getenv( "GLHEADERPATH" ) or ".";
-- GL headers will be looked for in the current directory or GLHEADERPATH.
-- At the moment we require:
-- 		"glcorearb.h"
-- 		"gl2ext.h"
-- Files other than glcorearb.h are only parsed for vendor specific defines
-- and aliases. Otherwise we only use what exists in glcorearb.h, further
-- restricted to procedures that are either core or ARB.


-- Emulate a portion of the libraries that this was written against.
getmetatable( "" ).__index = string;
getmetatable( "" ).__len = string.len;
getmetatable( "" ).__call = string.format;
function string:Split( pat )
	local r = {};
	local pos = 1;
	local from, to;
	while pos and pos <= #self do
		from, to = self:find( pat, pos );
		if not from then
			break;
		end
		r[#r+1] = self:sub( pos, from - 1 );
		pos = to + 1;
	end
	r[#r+1] = self:sub( pos, #self );
	return r;
end
function string:TrimBothEnds()
	return self:gsub("^%s+",""):gsub("%s+$","");
end
local List;
List = function( t )
	return setmetatable( t or {}, {
		__index = {
			Add = function( t, v )
				t[#t+1] = v;
			end;
			AddFormat = function( t, str, ... )
				t:Add( str( ... ) );
			end;
			Where = function( t, f )
				local r = {};
				for i=1, #t do
					if f(t[i]) then r[#r+1] = t[i]; end
				end
				return List( r );
			end;
			Select = function( t, f )
				local r = {};
				for i=1, #t do
					r[#r+1] = f( t[i] );
				end
				return List( r );
			end;
			Join = function( t, n )
				local r = {};
				for i=1, #t do
					r[i] = t[i];
				end
				for i=1, #n do
					r[#r+1] = n[i];
				end
				return List( r );
			end;
			Concat = table.concat;
		};
	} );
end

-- GL and GLES alike
local vendorSuffixes = {
	"ARB", "EXT", "KHR", "OES",
	"NV", "AMD", "INTEL", "OVR", "QCOM", "IMG", "ANGLE", "APPLE", "MESA"
};
local vendorSuffixPattern = {};
for i=1, #vendorSuffixes do
	vendorSuffixPattern[i] = vendorSuffixes[i] .. "$";
end
-- Strip the uppercase extension vendor suffix from a name.
local function StripVendorSuffix( str )
	local n;
	for k=1, #vendorSuffixPattern do
		str, n = str:gsub( vendorSuffixPattern[k], "" );
		if n > 0 then
			return str, vendorSuffixes[k];
		end
	end
	return str;
end

-- Normalize the type of an argument or return, also stripping any suffix
-- and normalizing all whitespace regions to single spaces.
local function NormalizeType( str )
	local chunks = str:Split( "%s+" );
	for j=1, #chunks do
		chunks[j] = StripVendorSuffix( chunks[j] );
	end
	local T = table.concat(chunks, " " );
	return T:TrimBothEnds();
end

-- Normalize an argument, returning the normalized type and the name separately,
-- always sticking the * of a pointer to the type rather than the name.
-- We need this to generate a normalized arg list and function signature (below)
local function NormalizeArgument( str )
	local chunks = str:Split( "%s+" );
	for j=1, #chunks do
		chunks[j] = StripVendorSuffix( chunks[j] );
	end
	local last = chunks[#chunks];
	local name = last:match( "[%w_]+$" );
	chunks[#chunks] = #name ~= #last and last:sub( 1, #last-#name) or nil;
	local T = table.concat(chunks, " " ):TrimBothEnds();
	return T, name
end

-- Normalize an argument list so that two matching prototypes
-- will produce the same table if fed to this function.
local function NormalizeArgList( str )
	local args = str:Split( ",%s*" );
	local r = {};
	for i=1, #args do
		local T, name = NormalizeArgument( args[i] );
		r[i] = { T, name };
	end
	return r;
end

-- Normalize a function signature into a unique string for keying
-- in such a way that if two different GL procedures may be assigned
-- to the same function pointer, this will produce an identical string for both.
-- This makes it possible to detect function aliases that may work as a fallback.
-- You still have to check for the appropriate extension.
local function NormalizeFunctionSignature( T, str )
	local args = str:Split( ",%s*" );
	local r = {};
	for i=1, #args do
		r[i] = NormalizeArgument( args[i] );
	end
	return ("%s(%s)")( T, table.concat( r, ", " ) );
end

-- Mangle the PFN name so that we don't collide with a
-- typedef from any of the GL headers.
local pfnFormat = "PFNGL%sPROC_MT";

--( T, name, args )
local typedefFormat = "\ttypedef %s (APIENTRYP %s) (%s);"
-- Generate a PFN...GL style typedef for a procedure
--
local function GetProcedureTypedef( proc )
	local args = {};
	for i=1, #proc.args do
		args[i] = ("%s %s")( unpack( proc.args[i] ) )
	end
	return typedefFormat( proc.retType, pfnFormat( proc.name:upper() ), table.concat( args, ", " ) );
end

local procedures = List();
local nameset = {};
local definitions = List();

--[[
	Structured procedure representation:

	ProcSpec = {
		string name;		-- Normalized name as it appears in the GL spec
		string? vendor;		-- Uppercase vendor string (ARB, EXT, AMD, NV etc)
		string signature;
		string retType;
		args = { { type, name } };
	};
]]
-- Parse a whole header, extracting the data.
local function ParseHeader( path, into, apiRegex, defs, nameSet, noNewNames )
	defs:AddFormat( "\n// %s\n\n", path );
	local f = assert( io.open( path, "r" ), "Could not open " .. path );
	for line in f:lines() do
		-- Do not parse PFN typedefs; they're easily reconstructible.
		local T, rawName, args = line:match( apiRegex );
		if T then
			T = NormalizeType( T );
			-- Strip the 'gl' namespace prefix.
			local procName = rawName:sub(3,-1);
			local name, vendor = StripVendorSuffix( procName );
			if not (noNewNames and nameSet[name]) then
				nameSet[name] = true;
				into:Add{
					name = name;
					vendor = vendor;
					-- pfnType = pfnFormat( procName:upper() );
					signature = NormalizeFunctionSignature( T, args );
					retType = T;
					args = NormalizeArgList( args );
				};
			end
		elseif ( line:find( "#" ) and not line:find( "#include" ) )
			or ( line:find( "typedef" ) and not line:find( "%(" ) ) then
			-- Passthrough non-PFN typedefs and all preprocessing except #include
			defs:Add( line );
		end
	end
	f:close();
end

local funcRegex = "GLAPI%s+(.+)APIENTRY%s+(%w+)%s*%((.*)%)";
local funcRegexES = "GL_APICALL%s+(.+)GL_APIENTRY%s+(%w+)%s*%((.*)%)";
ParseHeader( glHeaderPath .. "/glcorearb.h", procedures, funcRegex, definitions, nameset );
-- ParseHeader( glHeaderPath .. "/glext.h", procedures, funcRegex, definitions, nameset, true );
ParseHeader( glHeaderPath .. "/gl2ext.h", procedures, funcRegexES, definitions, nameset, true );

local procTable = {};

local coreProcedures = procedures:Where( function(x) return not x.vendor; end );
local arbProcedures = procedures:Where( function(x) return x.vendor == "ARB"; end );

-- Only consider core and ARB functions.
local nameList = coreProcedures:Join( arbProcedures ):Select(
	function(p)
		return p.name;
	end );

local nameSet = {};
local uniqueNames = List();

for s, k in ipairs( nameList ) do
	if not nameSet[k] then
		nameSet[k] = true;
		uniqueNames:Add( k );
	end
end

for i=1, #procedures do
	local p = procedures[i];
	procTable[p.name] = procTable[p.name] or {};
	local key = p.vendor or "core";
	procTable[p.name][key] = p;
end

-- Sort procedures by procaddress priority.
-- Use the first function signature encountered,
-- and discard any signatures that do no match it.
-- For example, if a function exists in core,
-- try loading any of its aliases with the same signature as in core.
-- If a function does not exist in core but in ARB extensions, try that first
-- and compare all the signatures to the ARB version.
-- For sanity's sake, do not support mismatching function signatures,
-- and do not try to specialize the loader between platforms.
local priorityList = List{ "core" }:Join( vendorSuffixes );

local typedefs = List();
local pointers = List();
local loader = List();

for s, str in ipairs( uniqueNames ) do
	pointers:Add( ("\t%s %s = NULL;")( pfnFormat( str:upper() ), str ) );
	local typeDefGenerated = false;
	for i=1, #priorityList do
		local k = priorityList[i];
		local proc = procTable[str][k]
		if proc then
			if not typeDefGenerated then
				typedefs:Add( GetProcedureTypedef( proc ) );
				typeDefGenerated = true;
			end
			local vendor = k == "core" and "" or k;
			loader:AddFormat(
				'\tif (!%s) %s = (%s)cmgr->getProcAddress("%s");\n',
				str, str, pfnFormat( proc.name:upper() ), ("gl%s%s")(str,vendor)
			);
		end
	end
end

-- Write loader header
local f = io.open( sourceTreePath .. "/include/mt_opengl.h", "wb" );
f:write( "#pragma once\n" );
f:write( "#include \"irrTypes.h\"\n" );
f:write( "#include \"IContextManager.h\"\n" );
f:write( "#include <KHR/khrplatform.h>\n\n" );
f:write( "// This code was generated by scripts/BindingGenerator.lua\n" );
f:write( "// Do not modify or commit it, modify the generator instead.\n\n" );
f:write( [[
// This file contains substantial portions of OpenGL headers
// originally released under the MIT license:
// glcorearb.h gl2ext.h Copyright 2013-2020 The Khronos Group Inc.
]] );
f:write( definitions:Concat( "\n" ) );
f:write( "\n\n" );
-- The script will miss this particular typedef thinking it's a PFN,
-- so we have to paste it in manually. It's the only such type in OpenGL.
f:write( "typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam);" );
f:write( "\n\n" );
f:write( "class OpenGLProcedures {\n" );
f:write( "private:\n" );
f:write( typedefs:Concat( "\n" ) );
f:write( "\n\n" );
f:write( "\tvoid* GetProcAddress();" );
f:write( "\n\n" );
f:write( "public:\n" );
f:write( "\t// Call this once after creating the context.\n" );
f:write( "\tvoid LoadAllProcedures(irr::video::IContextManager *cmgr);" );
f:write( "\n\n" );
f:write( pointers:Concat( "\n" ) );
f:write( "\n\n" );
f:write( "};\n" );
f:write( "\n//Global GL procedures object.\n" );
f:write( [[
	#ifdef WIN32
		#ifdef MINETEST_CLIENT
			#define DLL_SHARED __declspec(dllimport)
		#else
			#define DLL_SHARED __declspec(dllexport)
		#endif
	#else
		// On Linux, this supposedly works out of the box.
		#define DLL_SHARED
		// And on OSX, everything is linked statically anyway.
	#endif
]] );
f:write( "DLL_SHARED extern OpenGLProcedures GL;\n" );
f:close();

-- Write loader implementation
f = io.open( sourceTreePath .. "/source/Irrlicht/mt_opengl_loader.cpp", "wb" );
f:write( "#include \"mt_opengl.h\"\n" );
f:write( "\n" );
f:write( "OpenGLProcedures GL = OpenGLProcedures();\n" );
f:write( "\n" );
f:write( "void OpenGLProcedures::LoadAllProcedures(irr::video::IContextManager *cmgr) {\n" );
f:write( loader:Concat() );
f:write( "}\n" );
f:close();
