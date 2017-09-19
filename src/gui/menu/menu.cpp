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
		case MENU_8BIT_MAIN1:
			menu8bitMain1();
			break;
		case MENU_8BIT_MAIN2:
			menu8bitMain2();
			break;
		case MENU_8BIT_MAIN_FILE_SELECTED:
			menu8bitMainFileSelected();
			break;
		case MENU_8BIT_MAIN_IMAGE_SELECTED:
			menu8bitMainImageSelected();
			break;
		case MENU_8BIT_SYSTEM1:
			menu8bitSystem1();
			break;
		case MENU_8BIT_SYSTEM2:
			menu8bitSystem2();
			break;
		case MENU_8BIT_ABOUT1:
			menu8bitAbout1();
			break;
		case MENU_8BIT_ABOUT2:
			menu8bitAbout2();
			break;
		case MENU_MIST_MAIN1:
			menuMistMain1();
			break;
		case MENU_MIST_MAIN2:
			menuMistMain2();
			break;
		case MENU_MIST_MAIN_FILE_SELECTED:
			menuMistMainFileSelected();
			break;
		case MENU_MIST_STORAGE1:
			menuMistStorage1();
			break;
		case MENU_MIST_STORAGE2:
			menuMistStorage2();
			break;
		case MENU_MIST_STORAGE_FILE_SELECTED:
			menuMistStorageFileSelected();
			break;
		case MENU_MIST_SYSTEM1:
			menuMistSystem1();
			break;
		case MENU_MIST_SYSTEM2:
			menuMistSystem2();
			break;
		case MENU_MIST_SYSTEM_FILE_SELECTED:
			menuMistSystemFileSelected();
			break;
		case MENU_MIST_VIDEO1:
			menuMistVideo1();
			break;
		case MENU_MIST_VIDEO2:
			menuMistVideo2();
			break;
		case MENU_MIST_VIDEO_ADJUST1:
			menuMistVideoAdjust1();
			break;
		case MENU_MIST_VIDEO_ADJUST2:
			menuMistVideoAdjust2();
			break;
		// Minimig
		case MENU_MAIN1:
			menuMain1();
			break;
		case MENU_FILE_SELECTED:
			menuFileSelected();
			break;
		case MENU_MAIN2:
			menuMain2();
			break;
		case MENU_LOADCONFIG_1:
			menuLoadConfig1();
			break;
		case MENU_LOADCONFIG_2:
			menuLoadConfig2();
			break;
		case MENU_FILE_SELECT1:
			menuFileSelect1();
			break;
		case MENU_FILE_SELECT2:
			menuFileSelect2();
			break;
		case MENU_RESET1:
			menuReset1();
			break;
		case MENU_RESET2:
			menuReset2();
			break;
		case MENU_SAVECONFIG_1:
			menuSaveConfig1();
			break;
		case MENU_SAVECONFIG_2:
			menuSaveConfig2();
			break;
		case MENU_SETTINGS_CHIPSET1:
			menuSettingsChipset1();
			break;
		case MENU_SETTINGS_CHIPSET2:
			menuSettingsChipset2();
			break;
		case MENU_SETTINGS_MEMORY1:
			menuSettingsMemory1();
			break;
		case MENU_SETTINGS_MEMORY2:
			menuSettingsMemory2();
			break;
		case MENU_SETTINGS_HARDFILE1:
			menuSettingsHardfile1();
			break;
		case MENU_SETTINGS_HARDFILE2:
			menuSettingsHardfile2();
			break;
		case MENU_HARDFILE_SELECTED:
			menuHardfileSelected();
			break;
		case MENU_HARDFILE_EXIT:
			menuHardfileExit();
			break;
		case MENU_HARDFILE_CHANGED1:
			menuHardfileChanged1();
			break;
		case MENU_HARDFILE_CHANGED2:
			menuHardfileChanged2();
			break;
		case MENU_SYNTHRDB1:
			menuSynthRDB1();
			break;
		case MENU_SYNTHRDB2_1:
			menuSynthRDB2_1();
			break;
		case MENU_SYNTHRDB2:
			menuSynthRDB2();
			break;
		case MENU_SETTINGS_VIDEO1:
			menuSettingsVideo1();
			break;
		case MENU_SETTINGS_VIDEO2:
			menuSettingsVideo2();
			break;
		case MENU_ROMFILE_SELECTED:
			menuRomfileSelected();
			break;
		case MENU_ROMFILE_SELECTED1:
			menuRomfileSelected1();
			break;
		case MENU_ROMFILE_SELECTED2:
			menuRomfileSelected2();
			break;
		case MENU_FIRMWARE1:
			menuFirmware1();
			break;
		case MENU_FIRMWARE2:
			menuFirmware2();
			break;
		case MENU_FIRMWARE_CORE_FILE_SELECTED:
			menuFirmwareCoreFileSelected();
			break;
		case MENU_STORAGE:
			menuStorage();
			break;
		case MENU_KBDMAP:
			menuKbdmap();
			break;
		case MENU_KBDMAP1:
			menuKbdmap1();
			break;

		case MENU_JOYDIGMAP:
			menuJoydigmap();
			break;
		case MENU_JOYDIGMAP1:
			menuJoydigmap1();
			break;

		case MENU_ERROR:
			menuError();
			break;
		case MENU_INFO:
			menuInfo();
			break;
	};
}

/*
 * Displays file selection menu
 */
bool menu::selectFile(const char* extension)
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

void menu::menu8bitMain1()
{
	int entry;
	int selentry;

	/*
	int old_osd_size = OsdGetSize();
	while (1)
	{
		joy_bcount = 0;
		selentry = 0;
		entry = 0;
		menumask = 0;
		p = user_io_get_core_name();
		if (!p[0]) OsdSetTitle("8BIT", OSD_ARROW_RIGHT);
		else      OsdSetTitle(p, OSD_ARROW_RIGHT);

		if (!p[0]) OsdCoreNameSet("8BIT");
		else      OsdCoreNameSet(p);

		// check if there's a file type supported
		p = user_io_8bit_get_string(1);
		if (p && strlen(p)) {
			entry++;
			selentry++;
			menumask = 1;
			strcpy(s, " Load *.");
			strcat(s, GetExt(p));
			OsdWrite(0, s, menusub == 0, 0);
		}

		// add options as requested by core
		i = 2;
		do
		{
			char* pos;
			unsigned long status = user_io_8bit_set_status(0, 0);  // 0,0 gets status

			p = user_io_8bit_get_string(i);
			//printf("Option %d: %s\n", i-1, p);

			// check for 'F'ile or 'S'D image strings
			if (p && ((p[0] == 'F') || (p[0] == 'S')))
			{
				substrcpy(s, p, 2);
				if (strlen(s))
				{
					strcpy(s, " ");
					substrcpy(s + 1, p, 2);
					strcat(s, " *.");
				}
				else
				{
					if (p[0] == 'F') strcpy(s, " Load *.");
					else            strcpy(s, " Mount *.");
				}
				pos = s + strlen(s);
				substrcpy(pos, p, 1);
				strcpy(pos, GetExt(pos));
				OsdWrite(entry, s, menusub == selentry, 0);

				// add bit in menu mask
				menumask = (menumask << 1) | 1;
				entry++;
				selentry++;
			}

			// check for 'T'oggle strings
			if (p && (p[0] == 'T')) {

				s[0] = ' ';
				substrcpy(s + 1, p, 1);
				OsdWrite(entry, s, menusub == selentry, 0);

				// add bit in menu mask
				menumask = (menumask << 1) | 1;
				entry++;
				selentry++;
			}

			// check for 'O'ption strings
			if (p && (p[0] == 'O'))
			{
				//option handled by ARM
				if (p[1] == 'X') p++;

				unsigned long x = getStatus(p, status);

				// get currently active option
				substrcpy(s, p, 2 + x);
				char l = strlen(s);
				if (!l)
				{
					// option's index is outside of available values.
					// reset to 0.
					x = 0;
					user_io_8bit_set_status(setStatus(p, status, x), 0xffffffff);
					substrcpy(s, p, 2 + x);
					l = strlen(s);
				}

				s[0] = ' ';
				substrcpy(s + 1, p, 1);

				char *end = s + strlen(s) - 1;
				while ((end > s+1) && (*end == ' ')) end--;
				*(end + 1) = 0;

				strcat(s, ":");
				l = 28 - l - strlen(s);
				while (l--)
					strcat(s, " ");

				substrcpy(s + strlen(s), p, 2 + x);

				OsdWrite(entry, s, menusub == selentry, 0);

				// add bit in menu mask
				menumask = (menumask << 1) | 1;
				entry++;
				selentry++;
			}

			// delimiter
			if (p && (p[0] == '-'))
			{
				OsdWrite(entry, "", 0, 0);
				entry++;
			}

			// check for 'V'ersion strings
			if (p && (p[0] == 'V'))
			{
				// get version string
				strcpy(s, OsdCoreName());
				strcat(s, " ");
				substrcpy(s + strlen(s), p, 1);
				OsdCoreNameSet(s);
			}

			if (p && (p[0] == 'J'))
			{
				// joystick button names.
				for (int n = 0; n < 12; n++)
				{
					substrcpy(joy_bnames[n], p, n+1);
					//printf("joy_bname = %s\n", joy_bnames[n]);
					if(!joy_bnames[n][0]) break;
					joy_bcount++;
				}

				//printf("joy_bcount = %d\n", joy_bcount);
			}

			i++;
		}
		while (p);

		OsdSetSize(entry > 7 ? 16 : 8);
		if (old_osd_size == OsdGetSize()) break;
		old_osd_size = OsdGetSize();
	}

	// exit row
	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == selentry, 0);
	menusub_last = selentry;
	menumask = (menumask << 1) | 1;

	for (; entry<OsdGetSize() - 1; entry++)
		OsdWrite(entry, "", 0, 0);

	menustate = MENU_8BIT_MAIN2;
	parentstate = MENU_8BIT_MAIN1;

	// set helptext with core display on top of basic info
	siprintf(helptext_custom, HELPTEXT_SPACER);
	strcat(helptext_custom, OsdCoreName());
	strcat(helptext_custom, helptexts[HELPTEXT_MAIN]);
	helptext = helptext_custom;
	*/
}

void menu::menu8bitMain2()
{
	/*
	// menu key closes menu
	if (menu)
	{
		menustate = MENU_NONE1;
	}
	if (select)
	{
		if (menusub == menusub_last)
		{
			menustate = MENU_NONE1;
		}
		else
		{
			char fs_present;
			p = user_io_8bit_get_string(1);
			fs_present = p && strlen(p);

			int entry = 0;
			int i = 1;
			while (1)
			{
				p = user_io_8bit_get_string(i++);
				if (!p || p[0] == '-') continue;
				if (entry == menusub) break;
				entry++;
			}

			// entry 0 = file selector
			if(!menusub && fs_present)
			{
				// use a local copy of "p" since SelectFile will destroy the buffer behind it
				static char ext[13];
				strncpy(ext, p, 13);
				while (strlen(ext) < 3) strcat(ext, " ");
				SelectFile(ext, SCAN_DIR, MENU_8BIT_MAIN_FILE_SELECTED, MENU_8BIT_MAIN1, 1);
			}
			else if ((p[0] == 'F') || (p[0] == 'S'))
			{
				drive_num = 0;
				if (p[1] >= '0' && p[1] <= '3') drive_num = p[1] - '0';
				static char ext[13];
				substrcpy(ext, p, 1);
				while (strlen(ext) < 3) strcat(ext, " ");
				SelectFile(ext, SCAN_DIR | ((p[0] == 'S') ? SCAN_UMOUNT : 0),
					(p[0] == 'F') ? MENU_8BIT_MAIN_FILE_SELECTED : MENU_8BIT_MAIN_IMAGE_SELECTED,
					MENU_8BIT_MAIN1, 1);
			}
			else if (p[0] == 'O')
			{
				int byarm = 0;
				if (p[1] == 'X')
				{
					byarm = 1;
					p++;
				}

				unsigned long status = user_io_8bit_set_status(0, 0);  // 0,0 gets status
				unsigned long x = getStatus(p, status) + 1;

				if (byarm && is_x86_core())
				{
					if (p[1] == '2') x86_set_fdd_boot(!(x&1));
				}
				// check if next value available
				substrcpy(s, p, 2 + x);
				if (!strlen(s)) x = 0;

				user_io_8bit_set_status(setStatus(p, status, x), 0xffffffff);

				menustate = MENU_8BIT_MAIN1;
			}
			else if (p[0] == 'T')
			{
				// determine which status bit is affected
				unsigned long mask = 1 << getIdx(p);
				if (mask == 1 && is_x86_core())
				{
					x86_init();
					menustate = MENU_NONE1;
				}
				else
				{
					unsigned long status = user_io_8bit_set_status(0, 0);

					user_io_8bit_set_status(status ^ mask, mask);
					user_io_8bit_set_status(status, mask);
					menustate = MENU_8BIT_MAIN1;
				}

			}
		}
	}
	else if (right)
	{
		menustate = MENU_8BIT_SYSTEM1;
		menusub = 0;
	}
	*/
}

void menu::menu8bitMainFileSelected()
{
	//iprintf("File selected: %s\n", SelectedPath);
	//user_io_file_tx(SelectedPath, user_io_ext_idx(SelectedPath, fs_pFileExt) << 6 | (menusub + 1));

	menustate = MENU_NONE1;
}

void menu::menu8bitMainImageSelected()
{
	/*
	iprintf("Image selected: %s\n", SelectedPath);
	if (is_x86_core())
	{
		x86_set_image(drive_num, SelectedPath);
	}
	else
	{
		user_io_set_index(user_io_ext_idx(SelectedPath, fs_pFileExt) << 6 | (menusub + 1));
		user_io_file_mount(drive_num, SelectedPath);
	}

	menustate = SelectedPath[0] ? MENU_NONE1 : MENU_8BIT_MAIN1;
	*/
}

void menu::menu8bitSystem1()
{
	/*
	helptext = helptexts[HELPTEXT_MAIN];
	int m = 0;
	if (user_io_core_type() == CORE_TYPE_MINIMIG2)
		m = 1;
	menumask = m ? 0x7b : 0xfb;

	OsdSetTitle("System", OSD_ARROW_LEFT);
	menustate = MENU_8BIT_SYSTEM2;
	parentstate = MENU_8BIT_SYSTEM1;

	if (OsdIsBig)
		OsdWrite(0, "", 0, 0);
	OsdWrite(OsdIsBig ? 1 : 0, " Firmware & Core           \x16", menusub == 0, 0);

	if (OsdIsBig)
		OsdWrite(2, "", 0, 0);
	OsdWrite(OsdIsBig ? 3 : 1, " Define joystick buttons", menusub == 1, 0);
	OsdWrite(OsdIsBig ? 4 : 2, "", 0, 0);

	if (OsdIsBig)
		OsdWrite(5, "", 0, 0);
	OsdWrite(OsdIsBig ? 6 : 3, m ? " Reset" : " Reset settings", menusub == 3, 0);
	if (m)
		OsdWrite(OsdIsBig ? 7 : 4, "", 0, 0);
	else
		OsdWrite(OsdIsBig ? 7 : 4, " Save settings", menusub == 4, 0); // Minimig saves settings elsewhere

	if (OsdIsBig)
		OsdWrite(8, "", 0, 0);
	OsdWrite(OsdIsBig ? 9 : 5, " Cold reset", menusub == (5 - m), 0);

	if (OsdIsBig)
		OsdWrite(10, "", 0, 0);
	OsdWrite(OsdIsBig ? 11 : 6, " About", menusub == (6 - m), 0);

	if (OsdIsBig)
		for (int i = 12; i < OsdGetSize() - 1; i++)
			OsdWrite(i, "", 0, 0);

	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == (7 - m), 0);
	*/
}

void menu::menu8bitSystem2()
{
	/*
	int m = 0;
	if (user_io_core_type() == CORE_TYPE_MINIMIG2)
		m = 1;

	// menu key closes menu
	if (menu)
		menustate = MENU_NONE1;
	if (select)
	{
		switch (menusub)
		{
			case 0:
				// Firmware submenu
				menustate = MENU_FIRMWARE1;
				menusub = 0;
				break;
			case 1:
				if (is_minimig())
				{
					joy_bcount = 7;
					strcpy(joy_bnames[0], "Red/Fire");
					strcpy(joy_bnames[1], "Blue");
					strcpy(joy_bnames[2], "Yellow");
					strcpy(joy_bnames[3], "Green");
					strcpy(joy_bnames[4], "Right Trigger");
					strcpy(joy_bnames[5], "Left Trigger");
					strcpy(joy_bnames[6], "Pause");
				}
				start_map_setting(joy_bcount ? joy_bcount+5 : 9);
				menustate = MENU_JOYDIGMAP;
				menusub = 0;
				break;
			case 2:
				break;
			case 3:
				menustate = MENU_RESET1;
				menusub = 1;
				break;
			case 4:
				if (m)
				{
					reboot(1);
				}
				else
				{
					// Save settings
					char *filename = user_io_create_config_name();
					unsigned long status = user_io_8bit_set_status(0, 0);
					iprintf("Saving config to %s\n", filename);
					FileSaveConfig(filename, &status, 4);
					if (is_x86_core()) x86_config_save();
					menustate = MENU_8BIT_MAIN1;
					menusub = 0;
				}
				break;
			case 5:
				if (m)
				{
					menustate = MENU_8BIT_ABOUT1;
					menusub = 0;
				}
				else
				{
					reboot(1);
				}
				break;
			case 6:
				if (m) {
					menustate = MENU_NONE1;
					menusub = 0;
				}
				else {
					// About logo
					menustate = MENU_8BIT_ABOUT1;
					menusub = 0;
				}
				break;
			case 7:
				// Exit
				menustate = MENU_NONE1;
				menusub = 0;
				break;
		}
	}
	else
	{
		if (left)
		{
			// go back to core requesting this menu
			switch (user_io_core_type())
			{
				case CORE_TYPE_MINIMIG2:
					menusub = 0;
					menustate = MENU_MAIN1;
					break;
				case CORE_TYPE_MIST:
					menusub = 5;
					menustate = MENU_MIST_MAIN1;
					break;
				case CORE_TYPE_ARCHIE:
					menusub = 3;
					menustate = MENU_ARCHIE_MAIN1;
					break;
				case CORE_TYPE_8BIT:
					menusub = 0;
					menustate = MENU_8BIT_MAIN1;
					break;
			}
		}
	}
	*/
}

void menu::menu8bitAbout1()
{
	/*
 	umask = 0;
	helptext = helptexts[HELPTEXT_NONE];
	OsdSetTitle("About", 0);
	menustate = MENU_8BIT_ABOUT2;
	parentstate = MENU_8BIT_ABOUT1;
	for (int i = 5; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
	OsdDrawLogo(0, 0, 1);
	OsdDrawLogo(1, 1, 1);
	OsdDrawLogo(2, 2, 1);
	OsdDrawLogo(3, 3, 1);
	OsdDrawLogo(4, 4, 1);
	OsdDrawLogo(5, 5, 1);
	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == 0, 0);
	StarsInit();
	ScrollReset();
	*/
}

void menu::menu8bitAbout2()
{
	/*
	tarsUpdate();
	OsdDrawLogo(0, 0, 1);
	OsdDrawLogo(1, 1, 1);
	OsdDrawLogo(2, 2, 1);
	OsdDrawLogo(3, 3, 1);
	OsdDrawLogo(4, 4, 1);
	OsdDrawLogo(5, 5, 1);
	ScrollText(OsdIsBig ? 13 : 6, "                                 MiSTer by Sorgelig, based on MiST by Till Harbaum and Minimig by Dennis van Weeren and other projects. MiSTer hardware and software is distributed under the terms of the GNU General Public License version 3. MiSTer FPGA cores are the work of their respective authors under individual licensing.", 0, 0, 0, 0);

	if (menu | select | left)
	{
		menustate = MENU_8BIT_SYSTEM1;
		menusub = 6-m;
	}
	*/
}

void menu::menuMistMain1()
{
	/*
	OsdSetSize(8);
	menumask = 0xff;
	OsdSetTitle("Mist", 0);

	// most important: main page has setup for floppy A: and screen
	strcpy(s, " A: ");
	strcat(s, tos_get_disk_name(0));
	if (tos_system_ctrl() & TOS_CONTROL_FDC_WR_PROT_A) strcat(s, " \x17");
	OsdWrite(0, s, menusub == 0, 0);

	strcpy(s, " Screen: ");
	if (tos_system_ctrl() & TOS_CONTROL_VIDEO_COLOR) strcat(s, "Color");
	else                                          strcat(s, "Mono");
	OsdWrite(1, s, menusub == 1, 0);

	// everything else is in submenus
	OsdWrite(2, " Storage                   \x16", menusub == 2, 0);
	OsdWrite(3, " System                    \x16", menusub == 3, 0);
	OsdWrite(4, " Audio / Video             \x16", menusub == 4, 0);
	OsdWrite(5, " Firmware & Core           \x16", menusub == 5, 0);

	OsdWrite(6, " Save config                ", menusub == 6, 0);

	OsdWrite(7, STD_EXIT, menusub == 7, 0);
	*/

	menustate = MENU_MIST_MAIN2;
	parentstate = MENU_MIST_MAIN1;
}

void menu::menuMistMain2()
{
	/*
	// menu key closes menu
	if (menu)
		menustate = MENU_NONE1;
	if (select)
	{
		switch (menusub)
		{
		case 0:
			if (tos_disk_is_inserted(0))
			{
				tos_insert_disk(0, NULL);
				menustate = MENU_MIST_MAIN1;
			}
			else
				SelectFile("ST ", SCAN_DIR, MENU_MIST_MAIN_FILE_SELECTED, MENU_MIST_MAIN1, 0);
			break;

		case 1:
			tos_update_sysctrl(tos_system_ctrl() ^ TOS_CONTROL_VIDEO_COLOR);
			menustate = MENU_MIST_MAIN1;
			break;

		case 2:  // Storage submenu
			menustate = MENU_MIST_STORAGE1;
			menusub = 0;
			break;

		case 3:  // System submenu
			menustate = MENU_MIST_SYSTEM1;
			menusub = 0;
			break;

		case 4:  // Video submenu
			menustate = MENU_MIST_VIDEO1;
			menusub = 0;
			break;

		case 5:  // Firmware submenu
			menustate = MENU_FIRMWARE1;
			menusub = 0;
			break;

		case 6:  // Save config
			menustate = MENU_NONE1;
			tos_config_save();
			break;

		case 7:  // Exit
			menustate = MENU_NONE1;
			break;
		}
	}
	*/
}

void menu::menuMistMainFileSelected()
{
	//tos_insert_disk(0, SelectedPath);
	menustate = MENU_MIST_MAIN1;
}

void menu::menuMistStorage1()
{
	/*
	menumask = tos_get_direct_hdd() ? 0x3f : 0x7f;
	OsdSetTitle("Storage", 0);
	// entries for both floppies
	for (i = 0; i<2; i++) {
		strcpy(s, " A: ");
		strcat(s, tos_get_disk_name(i));
		s[1] = 'A' + i;
		if (tos_system_ctrl() & (TOS_CONTROL_FDC_WR_PROT_A << i))
			strcat(s, " \x17");
		OsdWrite(i, s, menusub == i, 0);
	}
	strcpy(s, " Write protect: ");
	strcat(s, config_tos_wrprot[(tos_system_ctrl() >> 6) & 3]);
	OsdWrite(2, s, menusub == 2, 0);
	OsdWrite(3, "", 0, 0);
	strcpy(s, " ACSI0 direct SD: ");
	strcat(s, tos_get_direct_hdd() ? "on" : "off");
	OsdWrite(4, s, menusub == 3, 0);
	for (i = 0; i<2; i++) {
		strcpy(s, " ACSI0: ");
		s[5] = '0' + i;

		strcat(s, tos_get_disk_name(2 + i));
		OsdWrite(5 + i, s, ((i == 1) || !tos_get_direct_hdd()) ? (menusub == (!tos_get_direct_hdd() ? 4 : 3) + i) : 0,
			(i == 0) && tos_get_direct_hdd());
	}
	OsdWrite(7, STD_EXIT, !tos_get_direct_hdd() ? (menusub == 6) : (menusub == 5), 0);
	*/

	parentstate = menustate;
	menustate = MENU_MIST_STORAGE2;
}

void menu::menuMistStorage2()
{
	int menu = 0;

	if (menu)
	{
		menustate = MENU_MIST_MAIN1;
		menusub = 2;
	}

	/*
	if (select)
	{
		if (menusub <= 1)
		{
			if (tos_disk_is_inserted(menusub))
			{
				tos_insert_disk(menusub, NULL);
				menustate = MENU_MIST_STORAGE1;
			}
			else
				SelectFile("ST ", SCAN_DIR, MENU_MIST_STORAGE_FILE_SELECTED, MENU_MIST_STORAGE1, 0);
		}
		else if (menusub == 2)
		{
			// remove current write protect bits and increase by one
			tos_update_sysctrl((tos_system_ctrl() & ~(TOS_CONTROL_FDC_WR_PROT_A | TOS_CONTROL_FDC_WR_PROT_B))
				| (((((tos_system_ctrl() >> 6) & 3) + 1) & 3) << 6));
			menustate = MENU_MIST_STORAGE1;

		}
		else if (menusub == 3) {
			tos_set_direct_hdd(!tos_get_direct_hdd());
			menustate = MENU_MIST_STORAGE1;

			// no direct hhd emulation: Both ACSI entries are enabled
			// or direct hhd emulation for ACSI0: Only second ACSI entry is enabled
		}
		else if ((menusub == 4) || (!tos_get_direct_hdd() && (menusub == 5))) {
			char disk_idx = menusub - (tos_get_direct_hdd() ? 1 : 2);
			iprintf("Select image for disk %d\n", disk_idx);

			if (tos_disk_is_inserted(disk_idx)) {
				tos_insert_disk(disk_idx, NULL);
				menustate = MENU_MIST_STORAGE1;
			}
			else
				SelectFile("HD ", 0, MENU_MIST_STORAGE_FILE_SELECTED, MENU_MIST_STORAGE1, 0);

		}
		else if (tos_get_direct_hdd() ? (menusub == 5) : (menusub == 6)) {
			menustate = MENU_MIST_MAIN1;
			menusub = 2;
		}
	}
	*/
}

void menu::menuMistStorageFileSelected()
{
	/*
	if (menusub < 2)
		tos_insert_disk(menusub, SelectedPath);
	else
	{
		char disk_idx = menusub - (tos_get_direct_hdd() ? 1 : 2);
		iprintf("Insert image for disk %d\n", disk_idx);
		tos_insert_disk(disk_idx, SelectedPath);
	}
	*/

	menustate = MENU_MIST_STORAGE1;
}

void menu::menuMistSystem1()
{
	osd& osd = osd::instance();

	menumask = 0xFF;
	osd.setTitle("System", false);

	/*
	strcpy(s, " Memory:    ");
	strcat(s, config_tos_mem[(tos_system_ctrl() >> 1) & 7]);
	OsdWrite(0, s, menusub == 0, 0);

	strcpy(s, " CPU:       ");
	strcat(s, config_cpu_msg[(tos_system_ctrl() >> 4) & 3]);
	OsdWrite(1, s, menusub == 1, 0);

	strcpy(s, " TOS:       ");
	strcat(s, tos_get_image_name());
	OsdWrite(2, s, menusub == 2, 0);

	strcpy(s, " Cartridge: ");
	strcat(s, tos_get_cartridge_name());
	OsdWrite(3, s, menusub == 3, 0);

	strcpy(s, " USB I/O:   ");
	strcat(s, "NONE"); //config_tos_usb[tos_get_cdc_control_redirect()]);
	OsdWrite(4, s, menusub == 4, 0);

	OsdWrite(5, " Reset", menusub == 5, 0);
	OsdWrite(6, " Cold boot", menusub == 6, 0);

	OsdWrite(7, STD_EXIT, menusub == 7, 0);
	*/

	parentstate = menustate;
	menustate = MENU_MIST_SYSTEM2;
}

void menu::menuMistSystem2()
{
	int menu = 0;

	if (menu)
	{
		menustate = MENU_MIST_MAIN1;
		menusub = 3;
	}

	/*
	if (select)
	{
		switch (menusub)
		{
		case 0:
			{ // RAM
				int mem = (tos_system_ctrl() >> 1) & 7;   // current memory config
				mem++;
				if (mem > 5) mem = 3;                 // cycle 4MB/8MB/14MB
				tos_update_sysctrl((tos_system_ctrl() & ~0x0e) | (mem << 1));
				tos_reset(1);
				menustate = MENU_MIST_SYSTEM1;
			}
			break;

		case 1: { // CPU
			int cpu = (tos_system_ctrl() >> 4) & 3;   // current cpu config
			cpu = (cpu + 1) & 3;
			if (cpu == 2) cpu = 3;                 // skip unused config
			tos_update_sysctrl((tos_system_ctrl() & ~0x30) | (cpu << 4));
			tos_reset(0);
			menustate = MENU_MIST_SYSTEM1;
		} break;

		case 2:  // TOS
			SelectFile("IMG", 0, MENU_MIST_SYSTEM_FILE_SELECTED, MENU_MIST_SYSTEM1, 0);
			break;

		case 3:  // Cart
				 // if a cart name is set, then remove it
			if (tos_cartridge_is_inserted()) {
				tos_load_cartridge("");
				menustate = MENU_MIST_SYSTEM1;
			}
			else
				SelectFile("IMG", 0, MENU_MIST_SYSTEM_FILE_SELECTED, MENU_MIST_SYSTEM1, 0);
			break;

		case 4:
			menustate = MENU_MIST_SYSTEM1;
			break;

		case 5:  // Reset
			tos_reset(0);
			menustate = MENU_NONE1;
			break;

		case 6:  // Cold Boot
			tos_reset(1);
			menustate = MENU_NONE1;
			break;

		case 7:
			menustate = MENU_MIST_MAIN1;
			menusub = 3;
			break;
		}
	}
	*/
}

void menu::menuMistSystemFileSelected()
{
	/*
	if (menusub == 2)
	{
		tos_upload(SelectedPath);
		menustate = MENU_MIST_SYSTEM1;
	}

	if (menusub == 3)
	{
		tos_load_cartridge(SelectedPath);
		menustate = MENU_MIST_SYSTEM1;
	}
	*/
}

void menu::menuMistVideo1()
{
	osd& osd = osd::instance();

	menumask = 0x7f;
	osd.setTitle("A/V", false);

	/*
	strcpy(s, " Screen:        ");
	if (tos_system_ctrl() & TOS_CONTROL_VIDEO_COLOR) strcat(s, "Color");
	else                                            strcat(s, "Mono");
	OsdWrite(0, s, menusub == 0, 0);

	// Viking card can only be enabled with max 8MB RAM
	enable = (tos_system_ctrl() & 0xe) <= TOS_MEMCONFIG_8M;
	strcpy(s, " Viking/SM194:  ");
	strcat(s, ((tos_system_ctrl() & TOS_CONTROL_VIKING) && enable) ? "on" : "off");
	OsdWrite(1, s, menusub == 1, enable ? 0 : 1);

	// Blitter is always present in >= STE
	enable = (tos_system_ctrl() & (TOS_CONTROL_STE | TOS_CONTROL_MSTE)) ? 1 : 0;
	strcpy(s, " Blitter:       ");
	strcat(s, ((tos_system_ctrl() & TOS_CONTROL_BLITTER) || enable) ? "on" : "off");
	OsdWrite(2, s, menusub == 2, enable);

	strcpy(s, " Chipset:       ");
	// extract  TOS_CONTROL_STE and  TOS_CONTROL_MSTE bits
	strcat(s, atari_chipset[(tos_system_ctrl() >> 23) & 3]);
	OsdWrite(3, s, menusub == 3, 0);

	OsdWrite(4, " Video adjust              \x16", menusub == 4, 0);

	strcpy(s, " YM-Audio:      ");
	strcat(s, stereo[(tos_system_ctrl() & TOS_CONTROL_STEREO) ? 1 : 0]);
	OsdWrite(5, s, menusub == 5, 0);
	OsdWrite(6, "", 0, 0);

	OsdWrite(7, STD_EXIT, menusub == 6, 0);
	*/

	parentstate = menustate;
	menustate = MENU_MIST_VIDEO2;
}

void menu::menuMistVideo2()
{
	int menu = 0;

	if (menu)
	{
		menustate = MENU_MIST_MAIN1;
		menusub = 4;
	}

	/*
	if (select)
	{
		switch (menusub)
		{
		case 0:
			tos_update_sysctrl(tos_system_ctrl() ^ TOS_CONTROL_VIDEO_COLOR);
			menustate = MENU_MIST_VIDEO1;
			break;

		case 1:
			// viking/sm194
			tos_update_sysctrl(tos_system_ctrl() ^ TOS_CONTROL_VIKING);
			menustate = MENU_MIST_VIDEO1;
			break;

		case 2:
			if (!(tos_system_ctrl() & TOS_CONTROL_STE))

				tos_update_sysctrl(tos_system_ctrl() ^ TOS_CONTROL_BLITTER);
				menustate = MENU_MIST_VIDEO1;
			}
			break;

		case 3:
			{
				unsigned long chipset = (tos_system_ctrl() >> 23) + 1;
				if (chipset == 4) chipset = 0;
				tos_update_sysctrl(tos_system_ctrl() & ~(TOS_CONTROL_STE | TOS_CONTROL_MSTE) |
					(chipset << 23));
				menustate = MENU_MIST_VIDEO1;
			}
			break;

		case 4:
			menustate = MENU_MIST_VIDEO_ADJUST1;
			menusub = 0;
			break;

		case 5:
			tos_update_sysctrl(tos_system_ctrl() ^ TOS_CONTROL_STEREO);
			menustate = MENU_MIST_VIDEO1;
			break;

		case 6:
			menustate = MENU_MIST_MAIN1;
			menusub = 4;
			break;
		}
	}
	*/
}

void menu::menuMistVideoAdjust1()
{
	osd& osd = osd::instance();

	menumask = 0x1F;
	osd.setTitle("V-adjust");

	/*
	OsdWrite(0, "", 0, 0);

	strcpy(s, " PAL mode:    ");
	if (tos_system_ctrl() & TOS_CONTROL_PAL50HZ) strcat(s, "50Hz");
	else                                      strcat(s, "56Hz");
	OsdWrite(1, s, menusub == 0, 0);

	strcpy(s, " Scanlines:   ");
	strcat(s, scanlines[(tos_system_ctrl() >> 20) & 3]);
	OsdWrite(2, s, menusub == 1, 0);

	OsdWrite(3, "", 0, 0);

	siprintf(s, " Horizontal:  %d", tos_get_video_adjust(0));
	OsdWrite(4, s, menusub == 2, 0);

	siprintf(s, " Vertical:    %d", tos_get_video_adjust(1));
	OsdWrite(5, s, menusub == 3, 0);

	OsdWrite(6, "", 0, 0);

	OsdWrite(7, STD_EXIT, menusub == 4, 0);
	*/

	parentstate = menustate;
	menustate = MENU_MIST_VIDEO_ADJUST2;
}

void menu::menuMistVideoAdjust2()
{
	int menu = 0;

	if (menu)
	{
		menustate = MENU_MIST_VIDEO1;
		menusub = 4;
	}

	/*
	// use left/right to adjust video position
	if (left || right) {
		if ((menusub == 2) || (menusub == 3)) {
			if (left && (tos_get_video_adjust(menusub - 2) > -100))
				tos_set_video_adjust(menusub - 2, -1);

			if (right && (tos_get_video_adjust(menusub - 2) < 100))
				tos_set_video_adjust(menusub - 2, +1);

			menustate = MENU_MIST_VIDEO_ADJUST1;
		}
	}

	if (select) {
		switch (menusub) {
		case 0:
			tos_update_sysctrl(tos_system_ctrl() ^ TOS_CONTROL_PAL50HZ);
			menustate = MENU_MIST_VIDEO_ADJUST1;
			break;

		case 1: {
			// next scanline state
			int scan = ((tos_system_ctrl() >> 20) + 1) & 3;
			tos_update_sysctrl((tos_system_ctrl() & ~TOS_CONTROL_SCANLINES) | (scan << 20));
			menustate = MENU_MIST_VIDEO_ADJUST1;
		} break;

			// entries 2 and 3 use left/right

		case 4:
			menustate = MENU_MIST_VIDEO1;
			menusub = 4;
			break;
		}
	}
	*/
}

void menu::menuMain1()
{
	/*
	OsdSetSize(16);
	menumask = 0xFF0;	// b01110000 Floppy turbo, Harddisk options & Exit.
	OsdSetTitle("Minimig", OSD_ARROW_RIGHT);
	helptext = helptexts[HELPTEXT_MAIN];

	OsdWrite(0, "", 0, 0);

	// floppy drive info
	// We display a line for each drive that's active
	// in the config file, but grey out any that the FPGA doesn't think are active.
	// We also print a help text in place of the last drive if it's inactive.
	for (i = 0; i < 4; i++)
	{
		if (i == config.floppy.drives + 1)
			OsdWrite(i+1, " KP +/- to add/remove drives", 0, 1);
		else
		{
			strcpy(s, " dfx: ");
			s[3] = i + '0';
			if (i <= drives)
			{
				menumask |= (1 << i);	// Make enabled drives selectable

				if (df[i].status & DSK_INSERTED) // floppy disk is inserted
				{
					char *p;
					if (p = strrchr(df[i].name, '/'))
					{
						p++;
					}
					else
					{
						p = df[i].name;
					}

					int len = strlen(p);
					if (len > 22) len = 21;
					strncpy(&s[6], p, len);
					s[6 + len] = ' ';
					s[6 + len + 1] = 0;
					s[6 + len + 2] = 0;
					if (!(df[i].status & DSK_WRITABLE)) s[6 + len + 1] = '\x17'; // padlock icon for write-protected disks
				}
				else // no floppy disk
				{
					strcat(s, "* no disk *");
				}
			}
			else if (i <= config.floppy.drives)
			{
				strcat(s, "* active after reset *");
			}
			else
				strcpy(s, "");
			OsdWrite(i+1, s, menusub == i, (i>drives) || (i>config.floppy.drives));
		}
	}
	siprintf(s, " Floppy disk turbo : %s", config.floppy.speed ? "on" : "off");
	OsdWrite(5, s, menusub == 4, 0);
	OsdWrite(6, "", 0, 0);

	OsdWrite(7, "  Hard disk settings \x16", menusub == 5, 0);
	OsdWrite(8, "    chipset settings \x16", menusub == 6, 0);
	OsdWrite(9, "     memory settings \x16", menusub == 7, 0);
	OsdWrite(10, "      video settings \x16", menusub == 8, 0);
	OsdWrite(11, "", 0, 0);

	OsdWrite(12, "    save configuration", menusub == 9, 0);
	OsdWrite(13, "    load configuration", menusub == 10, 0);
	OsdWrite(14, "", 0, 0);

	OsdWrite(15, STD_EXIT, menusub == 11, 0);
	*/

	menustate = MENU_MAIN2;
	parentstate = MENU_MAIN1;
}

void menu::menuMain2()
{
	/*
	if (menu)
		menustate = MENU_NONE1;
	else if (plus && (config.floppy.drives<3))
	{
		config.floppy.drives++;
		ConfigFloppy(config.floppy.drives, config.floppy.speed);
		menustate = MENU_MAIN1;
	}
	else if (minus && (config.floppy.drives>0))
	{
		config.floppy.drives--;
		ConfigFloppy(config.floppy.drives, config.floppy.speed);
		menustate = MENU_MAIN1;
	}
	else if (select)
	{
		if (menusub < 4)
		{
			if (df[menusub].status & DSK_INSERTED) // eject selected floppy
			{
				df[menusub].status = 0;
				menustate = MENU_MAIN1;
			}
			else
			{
				df[menusub].status = 0;
				SelectFile("ADF", SCAN_DIR, MENU_FILE_SELECTED, MENU_MAIN1, 1);
			}
		}
		else if (menusub == 4)	// Toggle floppy turbo
		{
			config.floppy.speed ^= 1;
			ConfigFloppy(config.floppy.drives, config.floppy.speed);
			menustate = MENU_MAIN1;
		}
		else if (menusub == 5)	// Go to harddrives page.
		{
			t_hardfile[0] = config.hardfile[0];
			t_hardfile[1] = config.hardfile[1];
			menustate = MENU_SETTINGS_HARDFILE1;
			menusub = 0;
		}
		else if (menusub == 6)
		{
			menustate = MENU_SETTINGS_CHIPSET1;
			menusub = 0;
		}
		else if (menusub == 7)
		{
			menustate = MENU_SETTINGS_MEMORY1;
			menusub = 0;
		}
		else if (menusub == 8)
		{
			menustate = MENU_SETTINGS_VIDEO1;
			menusub = 0;
		}
		else if (menusub == 9)
		{
			menusub = 0;
			menustate = MENU_SAVECONFIG_1;
		}
		else if (menusub == 10)
		{
			menusub = 0;
			menustate = MENU_LOADCONFIG_1;
		}
		else if (menusub == 11)
			menustate = MENU_NONE1;
	}
	else if (c == KEY_BACKSPACE) // eject all floppies
	{
		for (i = 0; i <= drives; i++)
			df[i].status = 0;

		menustate = MENU_MAIN1;
	}
	else if (right)
	{
		menustate = MENU_8BIT_SYSTEM1;
		menusub = 0;
	}
	*/
}

void menu::menuFileSelected()
{
	/*
	InsertFloppy(&df[menusub], SelectedPath);
	menustate = MENU_MAIN1;
	menusub++;
	if (menusub > drives)
		menusub = 6;
		*/
}

void menu::menuLoadConfig1()
{
	helptext = helptexts[HELPTEXT_NONE];

	/*
	if (parentstate != menustate)	// First run?
	{
		menumask = 0x200;
		if (ConfigurationExists(0)) menumask |= 0x001;
		if (ConfigurationExists(1)) menumask |= 0x002;
		if (ConfigurationExists(2)) menumask |= 0x004;
		if (ConfigurationExists(3)) menumask |= 0x008;
		if (ConfigurationExists(4)) menumask |= 0x010;
		if (ConfigurationExists(5)) menumask |= 0x020;
		if (ConfigurationExists(6)) menumask |= 0x040;
		if (ConfigurationExists(7)) menumask |= 0x080;
		if (ConfigurationExists(8)) menumask |= 0x100;
	}
	parentstate = menustate;
	OsdSetTitle("Load", 0);

	OsdWrite(0, "", 0, 0);
	OsdWrite(1, "", 0, 0);
	OsdWrite(2, "          Default", menusub == 0, (menumask & 1) == 0);
	OsdWrite(3, "", 0, 0);
	OsdWrite(4, "          1", menusub == 1, (menumask & 2) == 0);
	OsdWrite(5, "          2", menusub == 2, (menumask & 4) == 0);
	OsdWrite(6, "          3", menusub == 3, (menumask & 8) == 0);
	OsdWrite(7, "          4", menusub == 4, (menumask & 0x10) == 0);
	OsdWrite(8, "          5", menusub == 5, (menumask & 0x20) == 0);
	OsdWrite(9, "          6", menusub == 6, (menumask & 0x40) == 0);
	OsdWrite(10, "          7", menusub == 7, (menumask & 0x80) == 0);
	OsdWrite(11, "          8", menusub == 8, (menumask & 0x100) == 0);
	for (int i = 12; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == 9, 0);
	*/

	menustate = MENU_LOADCONFIG_2;
}

void menu::menuLoadConfig2()
{
	/*
	if (down)
	{
		if (menusub < 9)
			menusub++;
		menustate = MENU_LOADCONFIG_1;
	}
	else if (select)
	{
		if (menusub < 9)
		{
			OsdDisable();
			LoadConfiguration(menusub);
			MinimigReset();
			menustate = MENU_NONE1;
		}
		else
		{
			menustate = MENU_MAIN1;
			menusub = 10;
		}
	}

	if (menu) // exit menu
	{
		menustate = MENU_MAIN1;
		menusub = 10;
	}
	*/
}

/*
 * Show file list of a current folder
 */
void menu::menuFileSelect1()
{
	osd& osd = osd::instance();

	helptext = helptexts[HELPTEXT_NONE];
	osd.setTitle("Select");

	//PrintDirectory();
	menustate = MENU_FILE_SELECT2;
}

/*
 * Handle file selection process for Minimig
 */
void menu::menuFileSelect2()
{
	menumask = 0;

	/*
	ScrollLongName(); // scrolls file name if longer than display line

	if (c == KEY_HOME)
	{
		ScanDirectory(SelectedPath, SCAN_INIT, fs_pFileExt, fs_Options);
		menustate = MENU_FILE_SELECT1;
	}

	if (c == KEY_BACKSPACE)
	{
		if (fs_Options & SCAN_UMOUNT)
		{
			for (int i = 0; i < OsdGetSize(); i++) OsdWrite(i, "", 0, 0);
			OsdWrite(OsdGetSize() / 2, "   Unmounting the image", 0, 0);
			usleep(1500000);
			SelectedPath[0] = 0;
			menustate = fs_MenuSelect;
		}
	}

	if ((c == KEY_PAGEUP) || (c == KEY_LEFT))
	{
		ScanDirectory(SelectedPath, SCAN_PREV_PAGE, fs_pFileExt, fs_Options);
		menustate = MENU_FILE_SELECT1;
	}

	if ((c == KEY_PAGEDOWN) || (c == KEY_RIGHT))
	{
		ScanDirectory(SelectedPath, SCAN_NEXT_PAGE, fs_pFileExt, fs_Options);
		menustate = MENU_FILE_SELECT1;
	}

	if (down) // scroll down one entry
	{
		ScanDirectory(SelectedPath, SCAN_NEXT, fs_pFileExt, fs_Options);
		menustate = MENU_FILE_SELECT1;
	}

	if (up) // scroll up one entry
	{
		ScanDirectory(SelectedPath, SCAN_PREV, fs_pFileExt, fs_Options);
		menustate = MENU_FILE_SELECT1;
	}

	if ((i = GetASCIIKey(c))>1)
	{
		// find an entry beginning with given character
		ScanDirectory(SelectedPath, i, fs_pFileExt, fs_Options);
		menustate = MENU_FILE_SELECT1;
	}

	if (select)
	{
		if(DirItem[iSelectedEntry].d_type == DT_DIR)
		{
			changeDir(DirItem[iSelectedEntry].d_name);
			menustate = MENU_FILE_SELECT1;
		}
		else
		{
			if (nDirEntries)
			{
				if(strlen(SelectedPath)) strcat(SelectedPath, "/");
				strcat(SelectedPath, DirItem[iSelectedEntry].d_name);

				menustate = fs_MenuSelect;
			}
		}
	}

	if (menu)
	{
		menustate = fs_MenuCancel;
	}
	 */
}

void menu::menuReset1()
{
	/*
	int m = 0;
	if (user_io_core_type() == CORE_TYPE_MINIMIG2)
		m = 1;
	helptext = helptexts[HELPTEXT_NONE];
	OsdSetTitle("Reset", 0);
	menumask = 0x03;	// Yes / No
	parentstate = menustate;

	OsdWrite(0, "", 0, 0);
	OsdWrite(1, m ? "       Reset Minimig?" : "       Reset settings?", 0, 0);
	OsdWrite(2, "", 0, 0);
	OsdWrite(3, "             yes", menusub == 0, 0);
	OsdWrite(4, "             no", menusub == 1, 0);
	OsdWrite(5, "", 0, 0);
	OsdWrite(6, "", 0, 0);

	for (int i = 7; i < OsdGetSize(); i++)
		OsdWrite(i, "", 0, 0);
	*/

	menustate = MENU_RESET2;
}

void menu::menuReset2()
{
	/*
	m = 0;
	if (user_io_core_type() == CORE_TYPE_MINIMIG2)
		m = 1;

	if (select && menusub == 0)
	{
		if (m)
		{
			menustate = MENU_NONE1;
			MinimigReset();
		}
		else
		{
			char *filename = user_io_create_config_name();
			unsigned long status = user_io_8bit_set_status(0, 0xffffffff);
			iprintf("Saving config to %s\n", filename);
			FileSaveConfig(filename, &status, 4);
			menustate = MENU_8BIT_MAIN1;
			menusub = 0;
		}
	}

	if (menu || (select && (menusub == 1))) // exit menu
	{
		menustate = MENU_8BIT_SYSTEM1;
		menusub = 3;
	}
	break;
	*/
}

void menu::menuSaveConfig1()
{
	helptext = helptexts[HELPTEXT_NONE];
	menumask = 0x3ff;
	parentstate = menustate;

	/*
	OsdSetTitle("Save", 0);

	OsdWrite(0, "", 0, 0);
	OsdWrite(1, "", 0, 0);
	OsdWrite(2, "        Default", menusub == 0, 0);
	OsdWrite(3, "", 0, 0);
	OsdWrite(4,  "        1", menusub == 1, 0);
	OsdWrite(5,  "        2", menusub == 2, 0);
	OsdWrite(6,  "        3", menusub == 3, 0);
	OsdWrite(7,  "        4", menusub == 4, 0);
	OsdWrite(8,  "        5", menusub == 5, 0);
	OsdWrite(9,  "        6", menusub == 6, 0);
	OsdWrite(10, "        7", menusub == 7, 0);
	OsdWrite(11, "        8", menusub == 8, 0);
	for (int i = 12; i < OsdGetSize() - 1; i++)
		OsdWrite(i, "", 0, 0);
	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == 9, 0);
	*/

	menustate = MENU_SAVECONFIG_2;
}

void menu::menuSaveConfig2()
{
	/*
	if (menu)
	{
		menustate = MENU_MAIN1;
		menusub = 9;
	}

	else if (up)
	{
		if (menusub > 0)
			menusub--;
		menustate = MENU_SAVECONFIG_1;
	}
	else if (down)
	{
		//            if (menusub < 3)
		if (menusub < 9)
			menusub++;
		menustate = MENU_SAVECONFIG_1;
	}
	else if (select)
	{
		if (menusub<9)
		{
			SaveConfiguration(menusub);
			menustate = MENU_NONE1;
		}
		else
		{
			menustate = MENU_MAIN1;
			menusub = 9;
		}
	}
	if (menu) // exit menu
	{
		menustate = MENU_MAIN1;
		menusub = 9;
	}
	 */
}

void menu::menuSettingsChipset1()
{
	osd& osd = osd::instance();

	helptext = helptexts[HELPTEXT_CHIPSET];
	menumask = 0;
	//osd.setTitle(("Chipset", OSD_ARROW_LEFT | OSD_ARROW_RIGHT);

	/*
	OsdWrite(0, "", 0, 0);
	strcpy(s, "         CPU : ");
	strcat(s, config_cpu_msg[config.cpu & 0x03]);
	OsdWrite(1, s, menusub == 0, 0);
	strcpy(s, "       Turbo : ");
	strcat(s, config_turbo_msg[(config.cpu >> 2) & 0x03]);
	OsdWrite(2, s, menusub == 1, 0);
	OsdWrite(3, "", 0, 0);
	strcpy(s, "       Video : ");
	strcat(s, config.chipset & CONFIG_NTSC ? "NTSC" : "PAL");
	OsdWrite(4, s, menusub == 2, 0);
	strcpy(s, "     Chipset : ");
	strcat(s, config_chipset_msg[(config.chipset >> 2) & 7]);
	OsdWrite(5, s, menusub == 3, 0);
	OsdWrite(6, "", 0, 0);
	strcpy(s, "     CD32Pad : ");
	strcat(s, config_cd32pad_msg[(config.autofire >> 2) & 1]);
	OsdWrite(7, s, menusub == 4, 0);
	strcpy(s, "    Joy Swap : ");
	strcat(s, (config.autofire & 0x8)? "ON" : "OFF");
	OsdWrite(8, s, menusub == 5, 0);
	for (int i = 9; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == 6, 0);
	*/

	menustate = MENU_SETTINGS_CHIPSET2;
}

void menu::menuSettingsChipset2()
{
	/*
	if (down && menusub < 6)
	{
		menusub++;
		menustate = MENU_SETTINGS_CHIPSET1;
	}

	if (up && menusub > 0)
	{
		menusub--;
		menustate = MENU_SETTINGS_CHIPSET1;
	}

	if (select)
	{
		if (menusub == 0)
		{
			menustate = MENU_SETTINGS_CHIPSET1;
			int _config_cpu = config.cpu & 0x3;
			_config_cpu += 1;
			if (_config_cpu == 0x02) _config_cpu += 1;
			config.cpu = (config.cpu & 0xfc) | (_config_cpu & 0x3);
			ConfigCPU(config.cpu);
		}
		else if (menusub == 1)
		{
			menustate = MENU_SETTINGS_CHIPSET1;
			int _config_turbo = (config.cpu >> 2) & 0x3;
			_config_turbo += 1;
			config.cpu = (config.cpu & 0x3) | ((_config_turbo & 0x3) << 2);
			ConfigCPU(config.cpu);
		}
		else if (menusub == 2)
		{
			config.chipset ^= CONFIG_NTSC;
			menustate = MENU_SETTINGS_CHIPSET1;
			ConfigChipset(config.chipset);
		}
		else if (menusub == 3)
		{
			switch (config.chipset & 0x1c) {
			case 0:
				config.chipset = (config.chipset & 3) | CONFIG_A1000;
				break;
			case CONFIG_A1000:
				config.chipset = (config.chipset & 3) | CONFIG_ECS;
				break;
			case CONFIG_ECS:
				config.chipset = (config.chipset & 3) | CONFIG_AGA | CONFIG_ECS;
				break;
			case (CONFIG_AGA | CONFIG_ECS) :
				config.chipset = (config.chipset & 3) | 0;
				break;
			}

			menustate = MENU_SETTINGS_CHIPSET1;
			ConfigChipset(config.chipset);
		}
		else if (menusub == 4)
		{
			config.autofire ^= 0x4;
			menustate = MENU_SETTINGS_CHIPSET1;
			ConfigAutofire(config.autofire, 0x4);
		}
		else if (menusub == 5)
		{
			config.autofire ^= 0x8;
			menustate = MENU_SETTINGS_CHIPSET1;
			ConfigAutofire(config.autofire, 0x8);
		}
		else if (menusub == 6)
		{
			menustate = MENU_MAIN1;
			menusub = 6;
		}
	}

	if (menu)
	{
		menustate = MENU_MAIN1;
		menusub = 6;
	}
	else if (right)
	{
		menustate = MENU_SETTINGS_MEMORY1;
		menusub = 0;
	}
	else if (left)
	{
		menustate = MENU_SETTINGS_VIDEO1;
		menusub = 0;
	}
	*/
}

void menu::menuSettingsMemory1()
{
	osd& osd = osd::instance();

	helptext = helptexts[HELPTEXT_MEMORY];
	menumask = 0x3F;
	parentstate = menustate;

	//osd.setTitle("Memory", OSD_ARROW_LEFT | OSD_ARROW_RIGHT);

	/*
	OsdWrite(0, "", 0, 0);
	strcpy(s, "      CHIP  : ");
	strcat(s, config_memory_chip_msg[config.memory & 0x03]);
	OsdWrite(1, s, menusub == 0, 0);
	strcpy(s, "      SLOW  : ");
	strcat(s, config_memory_slow_msg[config.memory >> 2 & 0x03]);
	OsdWrite(2, s, menusub == 1, 0);
	strcpy(s, "      FAST  : ");
	strcat(s, config_memory_fast_msg[config.memory >> 4 & 0x03]);
	OsdWrite(3, s, menusub == 2, 0);

	OsdWrite(4, "", 0, 0);

	strcpy(s, "      ROM   : ");
	strncat(s, config.kickstart, 15);
	OsdWrite(5, s, menusub == 3, 0);

	strcpy(s, "      HRTmon: ");
	strcat(s, (config.memory & 0x40) ? "enabled " : "disabled");
	OsdWrite(6, s, menusub == 4, 0);

	for (int i = 7; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == 5, 0);
	*/

	menustate = MENU_SETTINGS_MEMORY2;
}

void menu::menuSettingsMemory2()
{
	/*
	if (select)
	{
		if (menusub == 0)
		{
			config.memory = ((config.memory + 1) & 0x03) | (config.memory & ~0x03);
			menustate = MENU_SETTINGS_MEMORY1;
			ConfigMemory(config.memory);
		}
		else if (menusub == 1)
		{
			config.memory = ((config.memory + 4) & 0x0C) | (config.memory & ~0x0C);
			menustate = MENU_SETTINGS_MEMORY1;
			ConfigMemory(config.memory);
		}
		else if (menusub == 2)
		{
			config.memory = ((config.memory + 0x10) & 0x30) | (config.memory & ~0x30);
			menustate = MENU_SETTINGS_MEMORY1;
			ConfigMemory(config.memory);
		}
		else if (menusub == 3)
		{
			SelectFile("ROM", 0, MENU_ROMFILE_SELECTED, MENU_SETTINGS_MEMORY1, 1);
		}
		else if (menusub == 4)
		{
			config.memory ^= 0x40;
			ConfigMemory(config.memory);
			menustate = MENU_SETTINGS_MEMORY1;
		}
		else if (menusub == 5)
		{
			menustate = MENU_MAIN1;
			menusub = 7;
		}
	}

	if (menu)
	{
		menustate = MENU_MAIN1;
		menusub = 7;
	}
	else if (right)
	{
		menustate = MENU_SETTINGS_VIDEO1;
		menusub = 0;
	}
	else if (left)
	{
		menustate = MENU_SETTINGS_CHIPSET1;
		menusub = 0;
	}
	*/
}

void menu::menuSettingsHardfile1()
{
	// FIXME!  Nasty race condition here.  Changing HDF type has immediate effect
	// which could be disastrous if the user's writing to the drive at the time!
	// Make the menu work on the copy, not the original, and copy on acceptance,
	// not on rejection.

	osd& osd = osd::instance();

	helptext = helptexts[HELPTEXT_HARDFILE];
	osd.setTitle("Harddisks");

	parentstate = menustate;
	menumask = 0x21;	// b00100001 - On/off & exit enabled by default...

	/*
	if (config.enable_ide)
		menumask |= 0x0a;  // b00001010 - HD0 and HD1 type
	OsdWrite(0, "", 0, 0);
	strcpy(s, "   A600/A1200 IDE : ");
	strcat(s, config.enable_ide ? "on " : "off");
	OsdWrite(1, s, menusub == 0, 0);
	OsdWrite(2, "", 0, 0);

	strcpy(s, " Master : ");
	if (config.hardfile[0].enabled == (HDF_FILE | HDF_SYNTHRDB))
		strcat(s, "Hardfile (filesys)");
	else
		strcat(s, config_hdf_msg[config.hardfile[0].enabled & HDF_TYPEMASK]);
	OsdWrite(3, s, config.enable_ide ? (menusub == 1) : 0, config.enable_ide == 0);
	if (config.hardfile[0].present)
	{
		strcpy(s, "                                ");
		strncpy(&s[7], config.hardfile[0].long_name, 21);
	}
	else
		strcpy(s, "       ** file not found **");

	enable = config.enable_ide && ((config.hardfile[0].enabled&HDF_TYPEMASK) == HDF_FILE);
	if (enable)
		menumask |= 0x04;	// Make hardfile selectable
	OsdWrite(4, s, enable ? (menusub == 2) : 0, enable == 0);

	OsdWrite(5, "", 0, 0);
	strcpy(s, "  Slave : ");
	if (config.hardfile[1].enabled == (HDF_FILE | HDF_SYNTHRDB))
		strcat(s, "Hardfile (filesys)");
	else
		strcat(s, config_hdf_msg[config.hardfile[1].enabled & HDF_TYPEMASK]);
	OsdWrite(6, s, config.enable_ide ? (menusub == 3) : 0, config.enable_ide == 0);
	if (config.hardfile[1].present) {
		strcpy(s, "                                ");
		strncpy(&s[7], config.hardfile[1].long_name, 21);
	}
	else
		strcpy(s, "       ** file not found **");
	enable = config.enable_ide && ((config.hardfile[1].enabled&HDF_TYPEMASK) == HDF_FILE);
	if (enable)
		menumask |= 0x10;	// Make hardfile selectable
	OsdWrite(7, s, enable ? (menusub == 4) : 0, enable == 0);

	for (int i = 8; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == 5, 0);
	*/

	menustate = MENU_SETTINGS_HARDFILE2;
}

void menu::menuSettingsHardfile2()
{
	/*
	if (select)
	{
		if (menusub == 0)
		{
			config.enable_ide = (config.enable_ide == 0);
			menustate = MENU_SETTINGS_HARDFILE1;
		}
		if (menusub == 1)
		{
			if (config.hardfile[0].enabled == HDF_FILE)
			{
				config.hardfile[0].enabled |= HDF_SYNTHRDB;
			}
			else if (config.hardfile[0].enabled == (HDF_FILE | HDF_SYNTHRDB))
			{
				config.hardfile[0].enabled = 0;
			}
			else
			{
				config.hardfile[0].enabled = HDF_FILE;
			}
			menustate = MENU_SETTINGS_HARDFILE1;
		}
		else if (menusub == 2)
		{
			SelectFile("HDF", 0, MENU_HARDFILE_SELECTED, MENU_SETTINGS_HARDFILE1, 1);
		}
		else if (menusub == 3)
		{
			if (config.hardfile[1].enabled == HDF_FILE)
			{
				config.hardfile[1].enabled |= HDF_SYNTHRDB;
			}
			else if (config.hardfile[1].enabled == (HDF_FILE | HDF_SYNTHRDB))
			{
				config.hardfile[1].enabled = 0;
			}
			else
			{
				config.hardfile[1].enabled = HDF_FILE;
			}
			menustate = MENU_SETTINGS_HARDFILE1;
		}
		else if (menusub == 4)
		{
			SelectFile("HDF", 0, MENU_HARDFILE_SELECTED, MENU_SETTINGS_HARDFILE1, 1);
		}
		else if (menusub == 5) // return to previous menu
		{
			menustate = MENU_HARDFILE_EXIT;
		}
	}

	if (menu) // return to previous menu
	{
		menustate = MENU_HARDFILE_EXIT;
	}
	*/
}

void menu::menuHardfileSelected()
{
	/*
	if (menusub == 2) // master drive selected
	{
		// Read RDB from selected drive and determine type...
		memcpy((void*)config.hardfile[0].long_name, SelectedPath, sizeof(config.hardfile[0].long_name));
		switch (GetHDFFileType(SelectedPath))
		{
		case HDF_FILETYPE_RDB:
			config.hardfile[0].enabled = HDF_FILE;
			config.hardfile[0].present = 1;
			menustate = MENU_SETTINGS_HARDFILE1;
			break;
		case HDF_FILETYPE_DOS:
			config.hardfile[0].enabled = HDF_FILE | HDF_SYNTHRDB;
			config.hardfile[0].present = 1;
			menustate = MENU_SETTINGS_HARDFILE1;
			break;
		case HDF_FILETYPE_UNKNOWN:
			config.hardfile[0].present = 1;
			if (config.hardfile[0].enabled == HDF_FILE)	// Warn if we can't detect the type
				menustate = MENU_SYNTHRDB1;
			else
				menustate = MENU_SYNTHRDB2_1;
			menusub = 0;
			break;
		case HDF_FILETYPE_NOTFOUND:
		default:
			config.hardfile[0].present = 0;
			menustate = MENU_SETTINGS_HARDFILE1;
			break;
		}
	}

	if (menusub == 4) // slave drive selected
	{
		memcpy((void*)config.hardfile[1].long_name, SelectedPath, sizeof(config.hardfile[1].long_name));
		switch (GetHDFFileType(SelectedPath))
		{
		case HDF_FILETYPE_RDB:
			config.hardfile[1].enabled = HDF_FILE;
			config.hardfile[1].present = 1;
			menustate = MENU_SETTINGS_HARDFILE1;
			break;
		case HDF_FILETYPE_DOS:
			config.hardfile[1].enabled = HDF_FILE | HDF_SYNTHRDB;
			config.hardfile[1].present = 1;
			menustate = MENU_SETTINGS_HARDFILE1;
			break;
		case HDF_FILETYPE_UNKNOWN:
			config.hardfile[1].present = 1;
			if (config.hardfile[1].enabled == HDF_FILE)	// Warn if we can't detect the type...
				menustate = MENU_SYNTHRDB1;
			else
				menustate = MENU_SYNTHRDB2_1;
			menusub = 0;
			break;
		case HDF_FILETYPE_NOTFOUND:
		default:
			config.hardfile[1].present = 0;
			menustate = MENU_SETTINGS_HARDFILE1;
			break;
		}
	}
	*/
}

void menu::menuHardfileExit()
{
	/*
	if (memcmp(config.hardfile, t_hardfile, sizeof(t_hardfile)) != 0)
	{
		menustate = MENU_HARDFILE_CHANGED1;
		menusub = 1;
	}
	else
	{
		menustate = MENU_MAIN1;
		menusub = 5;
	}
	*/
}

/*
 * Hardfile configuration has changed, ask user if he wants to use the new settings
 */
void menu::menuHardfileChanged1()
{
	osd& osd = osd::instance();

	menumask = 0x03;
	parentstate = menustate;
	osd.setTitle("Confirm");

	/*
	OsdWrite(0, "", 0, 0);
	OsdWrite(1, "    Changing configuration", 0, 0);
	OsdWrite(2, "      requires reset.", 0, 0);
	OsdWrite(3, "", 0, 0);
	OsdWrite(4, "       Reset Minimig?", 0, 0);
	OsdWrite(5, "", 0, 0);
	OsdWrite(6, "             yes", menusub == 0, 0);
	OsdWrite(7, "             no", menusub == 1, 0);

	for (int i = 8; i < OsdGetSize(); i++) OsdWrite(i, "", 0, 0);
	*/

	menustate = MENU_HARDFILE_CHANGED2;
}

void menu::menuHardfileChanged2()
{
	/*
	if (select)
	{
		if (menusub == 0) // yes
		{
			// FIXME - waiting for user-confirmation increases the window of opportunity for file corruption!

			if ((config.hardfile[0].enabled != t_hardfile[0].enabled)
				|| (strcmp(config.hardfile[0].long_name, t_hardfile[0].long_name) != 0))
			{
				OpenHardfile(0);
				//					if((config.hardfile[0].enabled == HDF_FILE) && !FindRDB(0))
				//						menustate = MENU_SYNTHRDB1;
			}
			if (config.hardfile[1].enabled != t_hardfile[1].enabled
				|| (strcmp(config.hardfile[1].long_name, t_hardfile[1].long_name) != 0))
			{
				OpenHardfile(1);
				//					if((config.hardfile[1].enabled == HDF_FILE) && !FindRDB(1))
				//						menustate = MENU_SYNTHRDB2_1;
			}

			if (menustate == MENU_HARDFILE_CHANGED2)
			{
				MinimigReset();
				menustate = MENU_NONE1;
			}
		}
		else if (menusub == 1) // no
		{
			memcpy(config.hardfile, t_hardfile, sizeof(t_hardfile)); // restore configuration
			menustate = MENU_MAIN1;
			menusub = 3;
		}
	}

	if (menu)
	{
		memcpy(config.hardfile, t_hardfile, sizeof(t_hardfile)); // restore configuration
		menustate = MENU_MAIN1;
		menusub = 3;
	}
	*/
}

void menu::menuSynthRDB1()
{
	osd& osd = osd::instance();

	menumask = 0x01;
	parentstate = menustate;
	osd.setTitle("Warning");

	/*
	OsdWrite(0, "", 0, 0);
	OsdWrite(1, " No partition table found -", 0, 0);
	OsdWrite(2, " Hardfile image may need", 0, 0);
	OsdWrite(3, " to be prepped with HDToolbox,", 0, 0);
	OsdWrite(4, " then formatted.", 0, 0);
	OsdWrite(5, "", 0, 0);
	OsdWrite(6, "", 0, 0);
	OsdWrite(7, "             OK", menusub == 0, 0);

	for (int i = 8; i < OsdGetSize(); i++)
		OsdWrite(i, "", 0, 0);
	*/

	menustate = MENU_SYNTHRDB2;
}

void menu::menuSynthRDB2_1()
{
	osd& osd = osd::instance();

	menumask = 0x01;
	parentstate = menustate;
	osd.setTitle("Warning");

	/*
	OsdWrite(0, "", 0, 0);
	OsdWrite(1, " No filesystem recognised.", 0, 0);
	OsdWrite(2, " Hardfile may need formatting", 0, 0);
	OsdWrite(3, " (or may simply be an", 0, 0);
	OsdWrite(4, " unrecognised filesystem)", 0, 0);
	OsdWrite(5, "", 0, 0);
	OsdWrite(6, "", 0, 0);
	OsdWrite(7, "             OK", menusub == 0, 0);

	for (int i = 8; i < OsdGetSize(); i++) OsdWrite(i, "", 0, 0);
	*/

	menustate = MENU_SYNTHRDB2;
}

void menu::menuSynthRDB2()
{
	/*
	if (select || menu)
	{
		if (menusub == 0) // OK
			menustate = MENU_SETTINGS_HARDFILE1;
	}
	*/
}

void menu::menuSettingsVideo1()
{
	osd& osd = osd::instance();

	menumask = 0x0F;
	parentstate = menustate;
	helptext = 0; // helptexts[HELPTEXT_VIDEO];

	//osd.setTitle("Video", OSD_ARROW_LEFT | OSD_ARROW_RIGHT);

	/*
	OsdWrite(0, "", 0, 0);
	OsdWrite(1, "", 0, 0);
	strcpy(s, "  Scanlines     : ");
	strcat(s, config_scanlines_msg[config.scanlines & 0x3]);
	OsdWrite(2, s, menusub == 0, 0);
	strcpy(s, "  Video area by : ");
	strcat(s, config_blank_msg[(config.scanlines >> 6) & 3]);
	OsdWrite(3, s, menusub == 1, 0);
	strcpy(s, "  Aspect Ratio  : ");
	strcat(s, config_ar_msg[(config.scanlines >> 4) & 1]);
	OsdWrite(4, s, menusub == 2, 0);
	OsdWrite(5, "", 0, 0);
	OsdWrite(6, "", 0, 0);
	for (int i = 7; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
	OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == 3, 0);
	*/

	menustate = MENU_SETTINGS_VIDEO2;
}

void menu::menuSettingsVideo2()
{
	/*
	if (select)
	{
		if (menusub == 0)
		{
			config.scanlines = ((config.scanlines + 1) & 0x03) | (config.scanlines & 0xfc);
			if ((config.scanlines & 0x03) > 2)
				config.scanlines = config.scanlines & 0xfc;
			menustate = MENU_SETTINGS_VIDEO1;
			ConfigVideo(config.filter.hires, config.filter.lores, config.scanlines);
		}
		else if (menusub == 1)
		{
			config.scanlines &= ~0x80;
			config.scanlines ^= 0x40;
			menustate = MENU_SETTINGS_VIDEO1;
			ConfigVideo(config.filter.hires, config.filter.lores, config.scanlines);
		}
		else if (menusub == 2)
		{
			config.scanlines &= ~0x20; // reserved for auto-ar
			config.scanlines ^= 0x10;
			menustate = MENU_SETTINGS_VIDEO1;
			ConfigVideo(config.filter.hires, config.filter.lores, config.scanlines);
		}
		else if (menusub == 3)
		{
			menustate = MENU_MAIN1;
			menusub = 8;
		}
	}

	if (menu)
	{
		menustate = MENU_MAIN1;
		menusub = 8;
	}
	else if (right)
	{
		menustate = MENU_SETTINGS_CHIPSET1;
		menusub = 0;
	}
	else if (left)
	{
		menustate = MENU_SETTINGS_MEMORY1;
		menusub = 0;
	}
	*/
}

void menu::menuRomfileSelected()
{
	menusub = 1;
	menustate = MENU_ROMFILE_SELECTED1;

	menuRomfileSelected1();
}

void menu::menuRomfileSelected1()
{
	osd& osd = osd::instance();

	menumask = 0x03;
	parentstate = menustate;
	osd.setTitle("Confirm");

	/*
	OsdWrite(0, "", 0, 0);
	OsdWrite(1, "       Reload Kickstart?", 0, 0);
	OsdWrite(2, "", 0, 0);
	OsdWrite(3, "              yes", menusub == 0, 0);
	OsdWrite(4, "              no", menusub == 1, 0);
	OsdWrite(5, "", 0, 0);
	OsdWrite(6, "", 0, 0);
	OsdWrite(7, "", 0, 0);
	for (int i = 8; i < OsdGetSize(); i++) OsdWrite(i, "", 0, 0);
	*/

	menustate = MENU_ROMFILE_SELECTED2;
}

void menu::menuRomfileSelected2()
{
	/*
	if (select)
	{
		if (menusub == 0)
		{
			memcpy((void*)config.kickstart, SelectedPath, sizeof(config.kickstart));
			// reset bootscreen cursor position
			BootHome();
			OsdDisable();
			EnableOsd();
			spi8(OSD_CMD_RST);
			rstval = (SPI_RST_CPU | SPI_CPU_HLT);
			spi8(rstval);
			DisableOsd();
			UploadKickstart(config.kickstart);
			EnableOsd();
			spi8(OSD_CMD_RST);
			rstval = (SPI_RST_USR | SPI_RST_CPU);
			spi8(rstval);
			DisableOsd();
			EnableOsd();
			spi8(OSD_CMD_RST);
			rstval = 0;
			spi8(rstval);
			DisableOsd();

			menustate = MENU_NONE1;
		}
		else if (menusub == 1)
		{
			menustate = MENU_SETTINGS_MEMORY1;
			menusub = 2;
		}
	}

	if (menu)
	{
		menustate = MENU_SETTINGS_MEMORY1;
		menusub = 2;
	}
	*/
}

void menu::menuFirmware1()
{
	osd& osd = osd::instance();

	helptext = helptexts[HELPTEXT_NONE];
	parentstate = menustate;

	//osd.setTitle(is_menu_core() ? "Settings" : "FW & Core", 0);

	/*
	siprintf(s, "   ARM  s/w ver. %s", version + 5);
	OsdWrite(0, "", 0, 0);
	OsdWrite(1, s, 0, 0);

	if (is_menu_core())
	{
		menumask = 7;
		OsdWrite(2, "", 0, 0);
		if (getStorage(0))
		{
			OsdWrite(3, "      Using USB storage", 0, 0);
			OsdWrite(4, "      Switch to SD card", menusub == 0, 0);
		}
		else
		{
			if (getStorage(1))
			{
				OsdWrite(3, " No USB found, using SD card", 0, 0);
				OsdWrite(4, "      Switch to SD card", menusub == 0, 0);
			}
			else
			{
				OsdWrite(3, "        Using SD card", 0, 0);
				OsdWrite(4, "    Switch to USB storage", menusub == 0, !isUSBMounted());
			}
		}
		OsdWrite(5, "", 0, 0);
		OsdWrite(6,  "           NOTE:", 0, 0);
		OsdWrite(7,  "  USB storage takes longer", 0, 0);
		OsdWrite(8,  "     time to initialize", 0, 0);
		OsdWrite(9, "       upon cold boot.", 0, 0);
		OsdWrite(10, "  Use OSD or USER button to", 0, 0);
		OsdWrite(11, "     cancel USB waiting.", 0, 0);
		OsdWrite(12, "", 0, 0);
		OsdWrite(13, "", 0, 0);
		OsdWrite(14, "       Remap keyboard", menusub == 1, 0);
		OsdWrite(15, "   Define joystick buttons", menusub == 2, 0);
		menustate = MENU_STORAGE;
	}
	else
	{
		menumask = 3;
		OsdWrite(2, "", 0, 0);
		OsdWrite(3, "", 0, 0);
		int len = strlen(OsdCoreName());
		if (len > 30) len = 30;
		int sp = (30 - len) / 2;
		s[0] = 0;
		for (int i = 0; i < sp; i++) strcat(s, " ");
		char *s2 = s + strlen(s);
		char *s3 = OsdCoreName();
		for (int i = 0; i < len; i++) *s2++ = *s3++;
		*s2++ = 0;
		OsdWrite(4, s, 0, 0);
		OsdWrite(5, "", 0, 0);
		OsdWrite(6, "       Change FPGA core", menusub == 0, 0);
		for (int i = 7; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
		OsdWrite(OsdGetSize() - 1, STD_EXIT, menusub == 1, 0);
		menustate = MENU_FIRMWARE2;
	}
	*/
}

void menu::menuFirmware2()
{
	/*
	if (menu)
	{
		switch (user_io_core_type())
		{
		case CORE_TYPE_MIST:
			menusub = 5;
			menustate = MENU_MIST_MAIN1;
			break;
		case CORE_TYPE_ARCHIE:
			menusub = 3;
			menustate = MENU_ARCHIE_MAIN1;
			break;
		default:
			menusub = 0;
			menustate = (is_menu_core()) ? MENU_NONE1 : MENU_8BIT_SYSTEM1;
			break;
		}
	}
	else if (select)
	{
		if (menusub == 0)
		{
			SelectFile("RBF", 0, MENU_FIRMWARE_CORE_FILE_SELECTED, MENU_FIRMWARE1, 0);
		}
		else if (menusub == 1)
		{
			switch (user_io_core_type())
			{
			case CORE_TYPE_MIST:
				menusub = 5;
				menustate = MENU_MIST_MAIN1;
				break;
			case CORE_TYPE_ARCHIE:
				menusub = 3;
				menustate = MENU_ARCHIE_MAIN1;
				break;
			default:
				menusub = 0;
				menustate = (is_menu_core()) ? MENU_NONE1 : MENU_8BIT_SYSTEM1;
				break;
			}
		}
	}
	*/
}

void menu::menuFirmwareCoreFileSelected()
{
	osd& osd = osd::instance();

	// close OSD now as the new core may not even have one
	osd.hide();

	//fpga_load_rbf(SelectedPath);

	menustate = MENU_NONE1;
}

void menu::menuStorage()
{
	/*
	if (menu)
	{
		switch (user_io_core_type())
		{
		case CORE_TYPE_MIST:
			menusub = 5;
			menustate = MENU_MIST_MAIN1;
			break;
		case CORE_TYPE_ARCHIE:
			menusub = 3;
			menustate = MENU_ARCHIE_MAIN1;
			break;
		default:
			menusub = 0;
			menustate = MENU_NONE1;
			break;
		}
	}
	else if (select)
	{
		switch (menusub)
		{
		case 0:
			if (getStorage(1) || isUSBMounted()) setStorage(!getStorage(1));
			break;
		case 1:
			start_map_setting(0);
			menustate = MENU_KBDMAP;
			menusub = 0;
			break;
		case 2:
			joy_bcount = 12;
			strcpy(joy_bnames[0], "BUTTON 1");
			strcpy(joy_bnames[1], "BUTTON 2");
			strcpy(joy_bnames[2], "BUTTON 3");
			strcpy(joy_bnames[3], "BUTTON 4");
			strcpy(joy_bnames[4], "RIGHT (Alt/M)");
			strcpy(joy_bnames[5], "LEFT (Alt/M)");
			strcpy(joy_bnames[6], "DOWN (Alt/M)");
			strcpy(joy_bnames[7], "UP (Alt/M)");
			strcpy(joy_bnames[8], "L.MOUSE");
			strcpy(joy_bnames[9], "R.MOUSE");
			strcpy(joy_bnames[10], "M.MOUSE");
			strcpy(joy_bnames[11], "Mouse Emu/Sniper");
			start_map_setting(17);
			menustate = MENU_JOYDIGMAP;
			menusub = 0;
			break;
		}
	}
	*/
}


void menu::menuKbdmap()
{
	osd& osd = osd::instance();

	helptext = 0;
	menumask = 1;
	osd.setTitle("Keyboard");
	menustate = MENU_KBDMAP1;
	parentstate = MENU_KBDMAP;

	/*
	for (int i = 0; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
	OsdWrite(OsdGetSize() - 1, "           cancel", menusub == 0, 0);
	flag = 0;
	*/
}

void menu::menuKbdmap1()
{
	/*
	if (!get_map_button())
	{
		OsdWrite(3, "     Press key to remap", 0, 0);
		s[0] = 0;
		if(flag)
		{
			sprintf(s, "  on keyboard ID: %04x:%04x", get_map_vid(), get_map_pid());
		}
		OsdWrite(5, s, 0, 0);
	}
	else
	{
		flag = 1;
		sprintf(s, "   Press key to map %02X to", get_map_button() & 0xFF);
		OsdWrite(3, s, 0, 0);
		OsdWrite(5, "      on any keyboard", 0, 0);
	}

	OsdWrite(OsdGetSize() - 1, "           finish", menusub == 0, 0);

	if (select || menu)
	{
		finish_map_setting(menu);
		menustate = MENU_FIRMWARE1;
		menusub = 1;
	}
	*/
}

void menu::menuJoydigmap()
{
	/*
	helptext = 0;
	menumask = 1;
	OsdSetTitle("Joystick", 0);
	menustate = MENU_JOYDIGMAP1;
	parentstate = MENU_JOYDIGMAP;
	for (int i = 0; i < OsdGetSize() - 1; i++) OsdWrite(i, "", 0, 0);
	OsdWrite(OsdGetSize() - 1, "           cancel", menusub == 0, 0);
	*/
}

void menu::menuJoydigmap1()
{
	/*
	p = 0;
	if (get_map_button() < 4)
	{
		p = joy_button_map[get_map_button()];
	}
	else if(joy_bcount)
	{
		p = (get_map_button() < joy_bcount + 4) ? joy_bnames[get_map_button() - 4] : joy_button_map[8+get_map_type()];
	}
	else
	{
		p = (get_map_button() < 8) ? joy_button_map[get_map_button()] : joy_button_map[8 + get_map_type()];
	}


	{
		s[0] = 0;
		int len = (30-(strlen(p)+7))/2;
		while (len > 0)
		{
			strcat(s, " ");
			len--;
		}
	}
	strcat(s, "Press: ");
	strcat(s, p);
	OsdWrite(3, s, 0, 0);
	if (get_map_button())
	{
		if(get_map_type()) OsdWrite(OsdGetSize() - 1, "    finish (SPACE - skip)", menusub == 0, 0);
		else OsdWrite(OsdGetSize() - 1, "", 0, 0);

		sprintf(s, "   %s ID: %04x:%04x", get_map_type() ? "Joystick" : "Keyboard", get_map_vid(), get_map_pid());
		OsdWrite(5, s, 0, 0);
	}

	if (select || menu || get_map_button() >= (joy_bcount ? joy_bcount + 5 : 9))
	{
		finish_map_setting(menu);
		menustate = is_menu_core() ? MENU_FIRMWARE1 : MENU_8BIT_SYSTEM1;
		menusub = is_menu_core() ? 2 : 1;
	}
	*/
}

void menu::menuError()
{
	//if (menu)
	//	menustate = MENU_NONE1;
}

void menu::menuInfo()
{
	/*
	if (menu)
		menustate = MENU_NONE1;
	else if (CheckTimer(menu_timer))
		menustate = MENU_NONE1;
	*/
}

bool menu::changeCurrentFolder(const char *folderpath)
{
	bool result = false;

	return result;
}

