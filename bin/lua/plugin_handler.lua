---------------------------------------------------------------------------
-- Copyright (C) 2012 Hilscher Gesellschaft für Systemautomation mbH
--
-- Description:
-- - Scan for V1 and V2 plugins
-- - Remove some unwanted plugin references from the list
-- - Instantiate plugin references, add V2 function name aliases to V1 plugins
--
--  Changes:
--    Date        Author  Description
--  29 mar  12    SL      rewrote OpenOCD filtering, including blank FTDI
--  27 mar  12    SL      skip V1 UART references if romloader_uart v2 is loaded
--                        added ethernet
--  23 mar  12    SL      renamed netx56->51/52 in JTAGkey filter list
--  13 mar  12    SL      filter only if >1 OpenOCD plugin found
--   7 mar  12    SL      filtering for OpenOCD plugins
--  16 feb  12    SL      removed wrapping of v2 plugins
--                        simpler wrapping of v1 plugins
--   3 nov  11    SL      added wrap_v1_plugin_instance, cleanup
--  13 sept 11    SL      Created
---------------------------------------------------------------------------

module("plugin_handler", package.seeall)

---------------------------------------------------------------------------
-- SVN Keywords
--
SVN_DATE   ="$Date: $"
SVN_VERSION="$Revision: $"
-- $Author: $
---------------------------------------------------------------------------


-- ================================================================
--                     enumerate plugins/interfaces
-- ================================================================

-- Plugin Filtering:
-- USB: 
-- For the moment, we want to use the V1 plugin for netX500/100 and
-- the V2 plugin for netX 10/51/52.
-- The V1 plugin only recognizes netX 500/100.
-- The V2 plugin will recognize netX 500 and netX 10/51/52.

-- While scanning the "old style" plugins, we store the names 
-- and match against them when scanning the new style plugins.
-- old: romloader_usb_0000bbaa
-- new: romloader_usb_bb_aa
-- bb = Bus number (libusb_get_bus_number)
-- aa = Device address (libusb_get_device_address)

-- UART:
-- If both the V1 and V2 plugin is loaded 
-- we ignore the V1 plugin references.
-- (may happen due to problems with .cfg files)

-- JTAG:
-- The JTAG configurations using the JTAGkey or a blank FTDI chip
-- can only detect the CPU type but not the netX type. 
-- If several plugin references containing "JTAGkey" or "blank FTDI" 
-- are found, we connect using the first of these references, detect
-- the actual chip type and keep only the reference which has the
-- correct chip type in its name.
-- ** This assumes that only one netX is connected via a JTAGkey or
-- ** blank FTDI chip.


function ScanPlugins(strPattern)
	local atPluginRefs = {}
	local atPluginRefsV1OpenOCD = {}
	
	-- get V1 plugins
	print("Scanning for V1 plugins")
	muhkuh.ScanPlugins(strPattern)
	for pr in muhkuh.GetNextPlugin do
		print(pr:GetTyp(), pr:GetName())
		if pr:IsValid()==false then -- ugly, but... 
			break
		end
		
		-- collect OpenOCD configs in separate list for filtering
		if pr:GetTyp()=="romloader_openocd" then
			table.insert(atPluginRefsV1OpenOCD, pr)
			
		-- skip V1 UART references if romloader_uart v2 is loaded
		elseif pr:GetTyp()=="romloader_uart" then
			if romloader_uart == nil then
				table.insert(atPluginRefs, pr)
			else
				-- print("Skipping old-style COM plugin")
			end
			
		else
			table.insert(atPluginRefs, pr)
		end
	end
	
	-- Look at the names of the JTAG plugin references.
	-- Gather those containing "JTAGkey" or "blank FTDI" in separate lists.
	-- Any others (NXHX 500 RE etc.) are accepted into the result list without checking.
	
	local atJTAGKeyRefs = {}
	local atBlankFtdiRefs = {}
	local strPluginName
	
	local atPluginRefsToFilter = {}
	for i, pr in ipairs(atPluginRefsV1OpenOCD) do
		local strPluginName = pr:GetName()
		strPluginName = strPluginName:lower()
		if strPluginName:find("jtagkey") then
			table.insert(atJTAGKeyRefs, pr)
		elseif strPluginName:find("blank ftdi") then
			table.insert(atBlankFtdiRefs, pr)
		else
			table.insert(atPluginRefs, pr)
		end
	end
	
	checkJtagRefs(atJTAGKeyRefs, atPluginRefs)
	checkJtagRefs(atBlankFtdiRefs, atPluginRefs)
	
	
	-- Scan for V2 plugins
	-- skip plugin references whose name does not match the pattern 
	-- AND devices already recognized by "old" USB plugin
		
	if __MUHKUH_PLUGINS then
		print("Scanning for V2 plugins")
		local atPluginRefsV2 = {}
		for i, pp in ipairs(__MUHKUH_PLUGINS) do
			local iDetected = pp:DetectInterfaces(atPluginRefsV2)
			print(string.format("Found %d interfaces with plugin_provider %s", iDetected, pp:GetID()))
		end
		
		-- collect a list of V1 plugin names
		local afV1PluginNames = {}
		for i, pr in ipairs(atPluginRefs) do
			afV1PluginNames[pr:GetName()] = true
		end
		
		for i, pr in ipairs(atPluginRefsV2) do
			print(pr:GetTyp(), pr:GetName())
			local strV2Name = pr:GetName()
			local strV1Name = strV2Name:gsub("romloader_usb_(%x%x)_(%x%x)", "romloader_usb_0000%1%2")
			if afV1PluginNames[strV1Name] then
				print(strV2Name, "-->", strV1Name, "removed (already found by V1 plugin)")
			elseif strV2Name:match(strPattern) then
				print(strV2Name)
				table.insert(atPluginRefs, pr)
			else
				print("skipped: ", strV2Name, "(name does not match pattern)")
			end
		end
	end
	
	return atPluginRefs
end

-- Check plugin references in atJTAGRefs for the correct chip type and
-- Insert the accepted plugin refs into atPluginRefs.
-- Filtering is only done if there are more than one config
-- to choose from AND exactly one matching config is found.

function checkJtagRefs(atJTAGRefs, atPluginRefs)
	local fFiltered = false
	local tSelectedPluginRefs = {}
	
	if #atJTAGRefs > 1 then
		
		local iChiptype = nil
		local strChiptypeName = nil
		local aChipTypeFlags = {}
		local pr = atJTAGRefs[1]
		local plugin = getOldPluginInstance(pr)
		
		plugin:connect()
		iChiptype = plugin:get_chiptyp()
		strChiptypeName = plugin:get_chiptyp_name(iChiptype)
		plugin:disconnect()
		plugin:delete()
		plugin = nil
		
		if iChiptype ~= 0 and strChiptypeName then
			print("detected chip type: ", strChiptypeName)
			for strChiptype in strChiptypeName:gmatch("%d+") do
				if strChiptype == "51" or strChiptype == "52" then
					strChiptype = "56"
				end
				aChipTypeFlags[strChiptype] = true
			end
			
			for i, pr in ipairs(atJTAGRefs) do
				local strPluginName = pr:GetName()
				for strPluginChiptype in strPluginName:gmatch("%d+") do
					--print("strPluginChiptype:", strPluginChiptype)
					if strPluginChiptype == "51" or strPluginChiptype == "52" then
						strPluginChiptype = "56"
					end
					
					if aChipTypeFlags[strPluginChiptype] then
						print("accepted: ", strPluginName)
						table.insert(tSelectedPluginRefs, pr)
						break
					end
				end
			end
			
			if #tSelectedPluginRefs == 1 then 
				fFiltered = true
			end
		end
	end
	
	if fFiltered then
		for i, pr in ipairs(tSelectedPluginRefs) do
			table.insert(atPluginRefs, pr)
		end
	else
		for i, pr in ipairs(atJTAGRefs) do
			table.insert(atPluginRefs, pr)
		end
	end
end



-- ================================================================
--               instantiate plugins
-- ================================================================

-- instantiate "old" plugin references
function getOldPluginInstance(plugin)
	local env = _G
	local fullname = plugin:GetLuaCreateFn()
	print("plugin create function name:", fullname)
	for w in string.gmatch(fullname, "[^\.]+") do
		env = env[w]
		if env==nil then
			break
		end
	end

	if env~=nil and type(env)=="function" then
		plugin_instance = env(plugin:GetHandle())
	else
		error("plugin '"..plugin:GetName().."' specified an invalid contructor: '"..fullname.."'")
	end

	return plugin_instance
end


-- set V2 function names in V1 plugin 
-- (this is possible with V1 plugins, but not with V2 plugins)
-- not used
function wrap_v1_plugin_instance(tPlugin)
	tPlugin.Connect        = tPlugin.connect
	tPlugin.Disconnect     = tPlugin.disconnect
	tPlugin.IsConnected    = tPlugin.is_connected
	tPlugin.GetName        = tPlugin.get_name
	tPlugin.GetTyp         = tPlugin.get_typ
	tPlugin.GetChiptyp     = tPlugin.get_chiptyp
	tPlugin.GetChiptypName = tPlugin.get_chiptyp_name
	tPlugin.GetRomcode     = tPlugin.get_romcode
	tPlugin.GetRomcodeName = tPlugin.get_romcode_name
end


function isV2PluginRef(pr)
	local mt = getmetatable(pr)
	return mt and 
		(mt[".type"] =="romloader_usb_reference" 
		or mt[".type"] =="romloader_uart_reference"
		or mt[".type"] =="romloader_eth_reference")
end

-- instantiates the plugin reference
-- and wraps V2 plugins with V1 and V2 function names
-- (V1 plugins are not wrapped)
function getPluginInstance(pr)
	if isV2PluginRef(pr) then
		local pl = pr:Create()
		if pl then
			return pl
		else
			return nil, "plugin:Create() (V1) failed"
		end
	else
		local pl = getOldPluginInstance(pr)
		if pl then
			wrap_v1_plugin_instance(pl)
			return pl
		else
			return nil, "plugin:Create() (V2) failed"
		end
	end
end



-- ================================================================
-- Return plugin instance with the given name.
-- Used by command line scripts, e.g. bootwizard command line flasher
-- Note: there is no guarantee that the same port will always
-- get the same plugin name.
-- ================================================================
function getPluginByName(strName)
	local atPluginRefs = ScanPlugins(".*")
	local atPluginRefs_filtered = {}
	for i, ref in ipairs(atPluginRefs) do
		if ref:GetName() == strName then
			table.insert(atPluginRefs_filtered, ref)
		end
	end
	
	if #atPluginRefs_filtered == 0 then
		return nil, "Plugin not found."
	elseif #atPluginRefs_filtered>1 then
		return nil, "More than one plugin was found."
	else
		return getPluginInstance(atPluginRefs_filtered[1])
	end
end



-- ================================================================
--        get/closeCommonPlugin - replacement for tester.lua
-- ================================================================

-- from tester.getCommonPlugin
function open_plugin(plugin_instance)
	print("trying to connect")
	local fOk, strError = pcall(plugin_instance.connect, plugin_instance)
	if fOk then
		print("connected")
		m_commonPlugin = plugin_instance
	else
		if plugin_instance.delete then
			plugin_instance:delete()
		end
		print(strError)
		print("could not connect")
	end
	return fOk, strError
end


-- from tester.closeCommonPlugin
function closeCommonPlugin()
	if m_commonPlugin then
		m_commonPlugin:disconnect()
		if m_commonPlugin.delete then
			m_commonPlugin:delete()
		end
		m_commonPlugin = nil
	end
end
