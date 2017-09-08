#include "menu.h"

#include <stdio.h>
#include <string.h>
#include "../../3rdparty/openbsd/string.h"
#include "../../fpga/fpgadevice.h"
#include "../../fpga/fpgacommand.h"
#include "../osd/osd.h"

// Constants
const char* menu::config_tos_mem[] = { "512 kB", "1 MB", "2 MB", "4 MB", "8 MB", "14 MB", "--", "--" };
const char* menu::config_tos_wrprot[] = { "none", "A:", "B:", "A: and B:" };
const char* menu::config_tos_usb[] = { "none", "control", "debug", "serial", "parallel", "midi" };

const char* menu::config_memory_chip_msg[] = { "512kb", "1mb", "1.5mb", "2mb" };
const char* menu::config_memory_slow_msg[] = { "none", "512kb", "1mb", "1.5mb" };
const char* menu::config_memory_fast_msg[] = { "none", "2mb", "4mb","24mb","24mb" };

const char* menu::config_filter_msg[] = { "none", "HORIZONTAL", "VERTICAL", "H+V" };
const char* menu::config_scanlines_msg[] = { "off", "dim", "black" };
const char* menu::config_ar_msg[] = { "4:3", "16:9" };
const char* menu::config_blank_msg[] = { "Blank", "Blank+" };
const char* menu::config_dither_msg[] = { "off", "SPT", "RND", "S+R" };
const char* menu::config_cpu_msg[] = { "68000", "68010", "-----","68020" };
const char* menu::config_hdf_msg[] = { "Disabled", "Hardfile (disk img)", "MMC/SD card", "MMC/SD partition 1", "MMC/SD partition 2", "MMC/SD partition 3", "MMC/SD partition 4" };;
const char* menu::config_chipset_msg[] = { "OCS-A500", "OCS-A1000", "ECS", "---", "---", "---", "AGA", "---" };
const char* menu::config_turbo_msg[] = { "none", "CHIPRAM", "KICK", "BOTH" };
const char* menu::config_autofire_msg[] = { "        AUTOFIRE OFF", "        AUTOFIRE FAST", "        AUTOFIRE MEDIUM", "        AUTOFIRE SLOW" };
const char* menu::config_cd32pad_msg[] = { "OFF", "ON" };
const char* menu::config_button_turbo_msg[] = { "OFF", "FAST", "MEDIUM", "SLOW" };
const char* menu::config_button_turbo_choice_msg[] = { "A only", "B only", "A & B" };
const char* menu::joy_button_map[] = { "RIGHT", "LEFT", "DOWN", "UP", "BUTTON 1", "BUTTON 2", "BUTTON 3", "BUTTON 4", "KBD TOGGLE", "BUTTON OSD" };

const char* menu::helptexts[] =
	{
		0,
		"                                Welcome to MiSTer!  Use the cursor keys to navigate the menus.  Use space bar or enter to select an item.  Press Esc or F12 to exit the menus.  Joystick emulation on the numeric keypad can be toggled with the numlock or scrlock key, while pressing Ctrl-Alt-0 (numeric keypad) toggles autofire mode.",
		"                                Minimig can emulate an A600/A1200 IDE harddisk interface.  The emulation can make use of Minimig-style hardfiles (complete disk images) or UAE-style hardfiles (filesystem images with no partition table).",
		"                                Minimig's processor core can emulate a 68000 or 68020 processor (though the 68020 mode is still experimental.)  If you're running software built for 68000, there's no advantage to using the 68020 mode, since the 68000 emulation runs just as fast.",
		"                                Minimig can make use of up to 2 megabytes of Chip RAM, up to 1.5 megabytes of Slow RAM (A500 Trapdoor RAM), and up to 24 megabytes of true Fast RAM.  To use the HRTmon feature you will need a file on the SD card named hrtmon.rom. HRTMon is not compatible with Fast RAM.",
		"                                Minimig's video features include a blur filter, to simulate the poorer picture quality on older monitors, and also scanline generation to simulate the appearance of a screen with low vertical resolution.",
		0
	};

menu::menu()
{
}

menu::~menu()
{
}

void menu::handleUI()
{
	uint8_t menu = 0;

	// Menu key press closes the menu
	if (menu)
		menustate = MENU_NONE1;

	switch (menustate)
	{
		case MENU_NONE2:
			menuNone2();
			break;
		case MENU_NONE1:
			menuNone1();
			break;
		case MENU_ARCHIE_MAIN1:
			menuArchieMain1();
			break;
		case MENU_ARCHIE_MAIN2:
			menuArchieMain2();
			break;
		case MENU_ARCHIE_MAIN_FILE_SELECTED:
			menuArchieMainFileSelected();
			break;
	};
}

/*
 * Displays file selection menu
 */
bool menu::selectFile(const char* extension, ScanningOptions options, bool changeDir)
{
	bool result = false;

	/*
	AdjustDirectory(SelectedPath);

	if (strncmp(pFileExt, fs_pFileExt, 12) != 0 || !strlen(SelectedPath)) // check desired file extension
	{ // if different from the current one go to the root directory and init entry buffer
		SelectedPath[0] = 0;

		if(((user_io_core_type() == CORE_TYPE_8BIT) || (user_io_core_type() == CORE_TYPE_MINIMIG2)) && changeDir)
		{
			strcpy(SelectedPath, (user_io_core_type() == CORE_TYPE_MINIMIG2) ? "Amiga" : user_io_get_core_name());
			ScanDirectory(SelectedPath, SCAN_INIT, pFileExt, Options);
			if (!nDirEntries)
			{
				SelectedPath[0] = 0;
				ScanDirectory(SelectedPath, SCAN_INIT, pFileExt, Options);
			}
		}
		else
		{
			ScanDirectory(SelectedPath, SCAN_INIT, pFileExt, Options);
		}
	}

	strlcpy(fileExtension, pFileExt, sizeof(fileExtension));
	this->fileExtensionLen = strlen(fs_pFileExt);
	this->options = options;
	*/

	return result;
}


// Helper methods
void menu::menuNone1()
{
	osd& osd = osd::instance();

	helptext = helptexts[HELPTEXT_NONE];
	menumask = 0;
	menustate = MENU_NONE2;

	osd.hide();
}

void menu::menuNone2()
{
	uint8_t menu = 0;

	FPGADevice& fpga = FPGADevice::instance();
	FPGACommand& command = *fpga.command;
	osd& osd = osd::instance();

	CoreType coreType = command.getCoreType();

	if (menu)
	{
		uint8_t keymodifier = 0; //= get_key_mod();
		if (keymodifier)// & (LALT | RALT)) // Alt+Menu
		{
			//SelectFile("RBF", 0, MENU_FIRMWARE_CORE_FILE_SELECTED, MENU_NONE1, 0);
		}
		else
		{
			switch (coreType)
			{
				case CoreType::CORE_TYPE_MINIMIG2:
					menustate = MENU_MAIN1;
					break;
				case CoreType::CORE_TYPE_MIST:
					menustate = MENU_MIST_MAIN1;
					break;
				case CoreType::CORE_TYPE_ARCHIE:
					menustate = MENU_ARCHIE_MAIN1;
					break;
				case CoreType::CORE_TYPE_MENU:
					// OsdSetSize(16);
					// OsdCoreNameSet("");
					// SelectFile("RBF", 0, MENU_FIRMWARE_CORE_FILE_SELECTED, MENU_FIRMWARE1, 0);
					break;
				default:
					menustate = MENU_8BIT_MAIN1;
					break;
			};
		}

		menusub = 0;
		osd.clear();
		osd.showHighres();
	}
}

void menu::menuArchieMain1()
{
	osd& osd = osd::instance();

	menumask = 0x3F;
	osd.setTitle("ARCHIE", false);

	/*
	strcpy(s, " Floppy 0: ");
	strcat(s, archie_get_floppy_name(0));
	OsdWrite(0, s, menusub == 0, 0);

	strcpy(s, " Floppy 1: ");
	strcat(s, archie_get_floppy_name(1));
	OsdWrite(1, s, menusub == 1, 0);

	strcpy(s, " OS ROM: ");
	strcat(s, archie_get_rom_name());
	OsdWrite(2, s, menusub == 2, 0);

	OsdWrite(3, "", 0, 0);

	// the following is exactly like the atatri st core
	OsdWrite(4, " Firmware & Core           \x16", menusub == 3, 0);
	OsdWrite(5, " Save config                ", menusub == 4, 0);
	OsdWrite(6, "", 0, 0);
	OsdWrite(7, STD_EXIT, menusub == 5, 0);
	*/

	menustate = MENU_ARCHIE_MAIN2;
	parentstate = MENU_ARCHIE_MAIN1;
}

void menu::menuArchieMain2()
{
	/*
	if (select)
	{
		switch (menusub)
		{
		case 0:  // Floppy 0
		case 1:  // Floppy 1
			if (archie_floppy_is_inserted(menusub))
			{
				archie_set_floppy(menusub, NULL);
				menustate = MENU_ARCHIE_MAIN1;
			}
			else
				SelectFile("ADF", SCAN_DIR, MENU_ARCHIE_MAIN_FILE_SELECTED, MENU_ARCHIE_MAIN1, 1);
			break;

		case 2:  // Load ROM
			SelectFile("ROM", 0, MENU_ARCHIE_MAIN_FILE_SELECTED, MENU_ARCHIE_MAIN1, 1);
			break;

		case 3:  // Firmware submenu
			menustate = MENU_FIRMWARE1;
			menusub = 1;
			break;

		case 4:  // Save config
			menustate = MENU_NONE1;
			archie_save_config();
			break;

		case 5:  // Exit
			menustate = MENU_NONE1;
			break;
		}
	}
	*/
}

void menu::menuArchieMainFileSelected()
{
	// Attach disk image to correspondent Archie device (Floppy0/1 or ROM)
	switch (menusub)
	{
		case 0:
			// archie_set_floppy(0, SelectedPath);
			break;
		case 1:
			// archie_set_floppy(1, SelectedPath);
			break;
		case 2:
			// archie_set_rom(SelectedPath);
			break;
		default:
			break;
	}

	// Return back to root Archie menu
	menustate = MENU_ARCHIE_MAIN1;
}

bool menu::changeCurrentFolder(const char *folderpath)
{
	bool result = false;

	return result;
}

