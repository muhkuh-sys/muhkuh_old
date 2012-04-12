-----------------------------------------------------------------------------
--   Copyright (C) 2008 by Christoph Thelen                                --
--   doc_bacardi@users.sourceforge.net                                     --
--                                                                         --
--   This program is free software; you can redistribute it and/or modify  --
--   it under the terms of the GNU General Public License as published by  --
--   the Free Software Foundation; either version 2 of the License, or     --
--   (at your option) any later version.                                   --
--                                                                         --
--   This program is distributed in the hope that it will be useful,       --
--   but WITHOUT ANY WARRANTY; without even the implied warranty of        --
--   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         --
--   GNU General Public License for more details.                          --
--                                                                         --
--   You should have received a copy of the GNU General Public License     --
--   along with this program; if not, write to the                         --
--   Free Software Foundation, Inc.,                                       --
--   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             --
-----------------------------------------------------------------------------

module("mmio10", package.seeall)

XM0_IO0            = 0x00
XM0_IO1            = 0x01
XM0_IO2            = 0x02
XM0_IO3            = 0x03
XM0_IO4            = 0x04
XM0_IO5            = 0x05
XM0_RX             = 0x06
GPIO0              = 0x07
GPIO1              = 0x08
GPIO2              = 0x09
GPIO3              = 0x0a
GPIO4              = 0x0b
GPIO5              = 0x0c
GPIO6              = 0x0d
GPIO7              = 0x0e
PHY0_LED0          = 0x0f
PHY0_LED1          = 0x10
PHY0_LED2          = 0x11
PHY0_LED3          = 0x12
SPI0_CS1N          = 0x13
SPI0_CS2N          = 0x14
SPI1_CLK           = 0x15
SPI1_CS0N          = 0x16
SPI1_CS1N          = 0x17
SPI1_CS2N          = 0x18
SPI1_MISO          = 0x19
SPI1_MOSI          = 0x1a
I2C_SCL_MMIO       = 0x1b
I2C_SDA_MMIO       = 0x1c
UART0_CTS          = 0x1d
UART0_RTS          = 0x1e
UART0_RXD          = 0x1f
UART0_TXD          = 0x20
UART1_CTS          = 0x21
UART1_RTS          = 0x22
UART1_RXD          = 0x23
UART1_TXD          = 0x24
PWM_FAILURE_N      = 0x25
POS_ENC0_A         = 0x26
POS_ENC0_B         = 0x27
POS_ENC0_N         = 0x28
POS_ENC1_A         = 0x29
POS_ENC1_B         = 0x2a
POS_ENC1_N         = 0x2b
POS_MP0            = 0x2c
POS_MP1            = 0x2d
IO_LINK0_IN        = 0x2e
IO_LINK0_OUT       = 0x2f
IO_LINK0_OE        = 0x30
IO_LINK1_IN        = 0x31
IO_LINK1_OUT       = 0x32
IO_LINK1_OE        = 0x33
IO_LINK2_IN        = 0x34
IO_LINK2_OUT       = 0x35
IO_LINK2_OE        = 0x36
IO_LINK3_IN        = 0x37
IO_LINK3_OUT       = 0x38
IO_LINK3_OE        = 0x39
Res_0              = 0x3a
Res_1              = 0x3b
Res_2              = 0x3c
Res_3              = 0x3d
Res_4              = 0x3e
PIO_MODE           = 0x3f
INPUT              = 0xff

local pin_names =
{
	[XM0_IO0]		= "XM0_IO0",
	[XM0_IO1]		= "XM0_IO1",
	[XM0_IO2]		= "XM0_IO2",
	[XM0_IO3]		= "XM0_IO3",
	[XM0_IO4]		= "XM0_IO4",
	[XM0_IO5]		= "XM0_IO5",
	[XM0_RX]		= "XM0_RX",
	[GPIO0]		    = "GPIO0",
	[GPIO1]		    = "GPIO1",
	[GPIO2]		    = "GPIO2",
	[GPIO3]		    = "GPIO3",
	[GPIO4]		    = "GPIO4",
	[GPIO5]		    = "GPIO5",
	[GPIO6]		    = "GPIO6",
	[GPIO7]		    = "GPIO7",
	[PHY0_LED0]		= "PHY0_LED0",
	[PHY0_LED1]		= "PHY0_LED1",
	[PHY0_LED2]		= "PHY0_LED2",
	[PHY0_LED3]	    = "PHY0_LED3",
	[SPI0_CS1N]		= "SPI0_CS1N",
	[SPI0_CS2N]		= "SPI0_CS2N",
	[SPI1_CLK]		= "SPI1_CLK",
	[SPI1_CS0N]		= "SPI1_CS0N",
	[SPI1_CS1N]		= "SPI1_CS1N",
	[SPI1_CS2N]		= "SPI1_CS2N",
	[SPI1_MISO]		= "SPI1_MISO",
	[SPI1_MOSI]		= "SPI1_MOSI",
	[I2C_SCL_MMIO]	= "I2C_SCL_MMIO",
	[I2C_SDA_MMIO]	= "I2C_SDA_MMIO",
	[UART0_CTS]		= "UART0_CTS",
	[UART0_RTS]		= "UART0_RTS",
	[UART0_RXD]		= "UART0_RXD",
	[UART0_TXD]		= "UART0_TXD",
	[UART1_CTS]		= "UART1_CTS",
	[UART1_RTS]		= "UART1_RTS",
	[UART1_RXD]		= "UART1_RXD",
	[UART1_TXD]		= "UART1_TXD",
	[PWM_FAILURE_N]	= "PWM_FAILURE_N",
	[POS_ENC0_A]	= "POS_ENC0_A",
	[POS_ENC0_B]	= "POS_ENC0_B",
	[POS_ENC0_N]	= "POS_ENC0_N",
	[POS_ENC1_A]	= "POS_ENC1_A",
	[POS_ENC1_B]	= "POS_ENC1_B",
	[POS_ENC1_N]	= "POS_ENC1_N",
	[POS_MP0]		= "POS_MP0",
	[POS_MP1]		= "POS_MP1",
	[IO_LINK0_IN]	= "IO_LINK0_IN",
	[IO_LINK0_OUT]	= "IO_LINK0_OUT",
	[IO_LINK0_OE]	= "IO_LINK0_OE",
	[IO_LINK1_IN]	= "IO_LINK1_IN",
	[IO_LINK1_OUT]	= "IO_LINK1_OUT",
	[IO_LINK1_OE]	= "IO_LINK1_OE",
	[IO_LINK2_IN]	= "IO_LINK2_IN",
	[IO_LINK2_OUT]	= "IO_LINK2_OUT",
	[IO_LINK2_OE]	= "IO_LINK2_OE",
	[IO_LINK3_IN]	= "IO_LINK3_IN",
	[IO_LINK3_OUT]	= "IO_LINK3_OUT",
	[IO_LINK3_OE]	= "IO_LINK3_OE",
	[Res_0]		    = "Res_0",
	[Res_1]		    = "Res_1",
	[Res_2]		    = "Res_2",
	[Res_3]		    = "Res_3",
	[Res_4]		    = "Res_4",
	[PIO_MODE]		= "PIO_MODE",
	[INPUT]			= "INPUT"
}

---------------------------------------
-- init
--   Init a variable with a valid mmio table.
--
-- Parameters:
--   none
--
-- Return:
--   mmio table
--
function init()
	local t

	-- create an empty array
	t = {}
	-- init all mmio entries with INPUT
	for i=0,23 do
		t[i] = { func=INPUT, fInvertIn=false, fInvertOut=false }
	end

	-- return the array
	return t
end


---------------------------------------
-- show
--   Prints the contents of a mmio table.
--
-- Parameters:
--   t: mmio table
--
-- Return:
--   nothing
--
-- Notes:
--   The 't' parameter must be an initialized mmio table (see function 'init')
--
function show(t)
	local name
	local func


	for i=0,23 do
		func = t[i].func
		name = pin_names[func]
		if not name then
			name = "??? unknown setting: " .. func
		end
		print(string.format("%02d",i) .. " = " .. name .. ", InvIn=" .. tostring(t[i].fInvertIn) .. ", InvOut=" .. tostring(t[i].fInvertOut))
	end
end


---------------------------------------
-- setConfig
--   Write a mmio table to a netX device.
--
-- Parameters:
--   plugin: romloader plugin to use for netX communication
--   t : mmio table, write the values from this table to the netX
--
-- Return:
--   nothing
--
-- Notes:
--   1) The 'plugin' parameter must be a romloader plugin. The connection
--      must be open.
--   2) The 't' parameter must be an initialized mmio table (see function 'init')
--
function setConfig(plugin, t)
	local key
	local val


	for i=0,23 do
		-- unlock access key (that's at address 0x101C0070)
		key = plugin:read_data32(0x101C0070)
		plugin:write_data32(0x101C0070, key)
		-- set the config for the pin
		val = t[i].func + (t[i].fInvertOut and 0x0100 or 0) + (t[i].fInvertIn and 0x0200 or 0)
--		print("Write " .. string.format("0x%08X", val) .. " to pin " .. i)
		plugin:write_data32(0x101C0A00 + i*4, val)
	end
end


---------------------------------------
-- getConfig
--   Read the mmio table from a netX device.
--
-- Parameters:
--   plugin: romloader plugin to use for netX communication
--   t : mmio table, write the mmio table from the netX to this table
--
-- Return:
--   nothing
--
-- Notes:
--   1) The 'plugin' parameter must be a romloader plugin. The connection
--      must be open.
--   2) The 't' parameter must be an initialized mmio table (see function 'init')
--
function getConfig(plugin, t)
	local val


	for i=0,23 do
		val = plugin:read_data32(0x101C0A00 + i*4)
		t[i].func = bit.band(val, 0xff)
		t[i].fInvertOut = (bit.band(val,0x0100) ~= 0)
		t[i].fInvertIn = (bit.band(val,0x0200) ~= 0)
	end
end

