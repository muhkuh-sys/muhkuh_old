module("plugin_handler", package.seeall)

require("romloader_usb")
require("romloader_uart")
require("romloader_eth")

---------------------------------------------------------------------------
-- Copyright (C) 2012 Hilscher Gesellschaft für Systemautomation mbH
--
-- Description:
--   plugin_handler.lua: low-level plugin handling 
-- 
-- - Scan for V1 and V2 plugins
-- - Remove some unwanted plugin references from the list
-- - Instantiate plugin references, add V2 function name aliases to V1 plugins
--
--  Changes:
--    Date        Author  Description
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
	
	-- If more than one JTAG plugin has been found, filter them.
	local iChiptype = nil
	local strChiptypeName = nil

	if #atPluginRefsV1OpenOCD == 1 then
		table.insert(atPluginRefs, atPluginRefsV1OpenOCD[1])
	elseif #atPluginRefsV1OpenOCD > 1 then
		for i, pr in ipairs(atPluginRefsV1OpenOCD) do
				if iChiptype == nil then
					local plugin = getOldPluginInstance(pr)
					plugin:connect()
					iChiptype = plugin:get_chiptyp()
					strChiptypeName = plugin:get_chiptyp_name(iChiptype)
					plugin:disconnect()
					plugin:delete()
					plugin = nil
				end
				
				if iChiptype and openocd_is_plugin_compatible(pr, iChiptype, strChiptypeName) then
					table.insert(atPluginRefs, pr)
				else
					print("skipped: ", pr:GetName(), "(openOCD config does not match detected chiptype)")   
				end
		end
	end

	
	if __MUHKUH_PLUGINS then
		print("Scanning for V2 plugins")
		local atPluginRefsV2 = {}
		for i, pp in ipairs(__MUHKUH_PLUGINS) do
			local iDetected = pp:DetectInterfaces(atPluginRefsV2)
			print(string.format("Found %d interfaces with plugin_provider %s", iDetected, pp:GetID()))
		end
		
		-- filter out plugins whose name does not match the pattern 
		-- AND devices already recognized by "old" USB plugin
		
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


-- from Muhkuh V1 romloader.cpp SVN >=1444
ROMLOADER_CHIPTYP_UNKNOWN			= 0
ROMLOADER_CHIPTYP_NETX500			= 1
ROMLOADER_CHIPTYP_NETX100			= 2
ROMLOADER_CHIPTYP_NETX50			= 3
ROMLOADER_CHIPTYP_NETX5				= 4
ROMLOADER_CHIPTYP_NETX10			= 5
ROMLOADER_CHIPTYP_NETX56			= 6


openocd_filter_patterns = {
{strPattern = "netx100/500.*jtagkey.*",      aiChiptypes = {ROMLOADER_CHIPTYP_NETX500, ROMLOADER_CHIPTYP_NETX100}},
{strPattern = "netx50.+jtagkey.*",           aiChiptypes = {ROMLOADER_CHIPTYP_NETX50}},
{strPattern = "netx10.+jtagkey.*",           aiChiptypes = {ROMLOADER_CHIPTYP_NETX10}},
{strPattern = "netx51/52.+jtagkey.*",        aiChiptypes = {ROMLOADER_CHIPTYP_NETX56}},

{strPattern = "NXHX 100/500 blank FTDI",      aiChiptypes = {ROMLOADER_CHIPTYP_NETX500, ROMLOADER_CHIPTYP_NETX100}},
{strPattern = "NXHX 50 blank FTDI",           aiChiptypes = {ROMLOADER_CHIPTYP_NETX50}},
{strPattern = "NXHX 10 blank FTDI",           aiChiptypes = {ROMLOADER_CHIPTYP_NETX10}},
}

-- Checks if an openOCD plugin reference matches iChipType.
-- pr: V1 plugin reference
-- iChiptype, strChiptypeName: detected chip type
-- 
-- Limitation: This will not work correctly when more than one OpenOCD devices
-- are connected to the PC.

function openocd_is_plugin_compatible(pr, iChiptype, strChiptypeName)
	local strName = pr:GetName()
	strName = strName:lower()
	print("openocd_is_plugin_compatible", strName, iChiptype, strChiptypeName)
	local fNameFound = false
	for _, tEntry in ipairs(openocd_filter_patterns) do
		if strName:match(tEntry.strPattern) then
			print(strName, tEntry.strPattern)
			for _, iType in ipairs(tEntry.aiChiptypes) do
				if iType == iChiptype then
					return true
				end
			end
			return false
		end
	
	end
	return true
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
		plugin_instance:delete()
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
			m_commonPlugin.delete()
		end
		--m_commonPlugin:delete()
		--deletePlugin(m_commonPlugin)
		m_commonPlugin = nil
	end
end
