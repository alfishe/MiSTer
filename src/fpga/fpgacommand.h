#ifndef FPGA_FPGACOMMAND_H_
#define FPGA_FPGACOMMAND_H_

#include "../common/logger/logger.h"

#include <stdint.h>
#include <pthread.h>

#include "fpgadevice.h"
#include "fpgaconnector.h"

// Forward declaration. Header included from fpgacommand.cpp
class FPGAConnector;
class FPGADevice;

// Core type value should be unlikely to be returned by broken cores
enum class CoreType: uint8_t
{
	CORE_TYPE_MENU = 0x00,		// Introduced separate type of core specifically for menu
	CORE_TYPE_UNKNOWN = 0x55,
	CORE_TYPE_DUMB = 0xA0,		// core without any io controller interaction
	CORE_TYPE_PACE = 0xA2,		// core from pacedev.net (joystick only)
	CORE_TYPE_MIST = 0xA3,		// mist atari st core
	CORE_TYPE_8BIT = 0xA4,		// atari 800/c64 like core
	CORE_TYPE_MINIMIG2 = 0xA5,	// new Minimig with AGA
	CORE_TYPE_ARCHIE = 0xA6		// Acorn Archimedes
};

class FPGACommand
{
protected:
	// Physical transfers done using FPGAConnector and FPGADevice
	FPGAConnector *connector = nullptr;
	FPGADevice *fpga = nullptr;

	// Ensure that command scope untouched during the whole execution with mutex guard
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

public:
	FPGACommand(FPGAConnector *connector);
	FPGACommand(const FPGACommand& that) = delete; // Copy constructor is forbidden here (C++11 feature)
	virtual ~FPGACommand();

	CoreType getCoreType();
	char* getCoreName();

	// OSD commands
	bool startOSD();
	void endOSD();
	void sendOSDCommand(uint8_t cmd);
	void sendOSDCommand(uint8_t cmd, uint8_t param);
	void sendOSDCommand(uint8_t cmd, uint16_t param);
	void sendOSDCommand(uint8_t cmd, uint32_t param);

	// IO commands
	bool startIO();
	void endIO();
	void sendIOCommand(uint8_t cmd);
	void sendIOCommand(uint8_t cmd, uint8_t param);
	void sendIOCommand(uint8_t cmd, uint16_t param);
	void sendIOCommand(uint8_t cmd, uint32_t param);

	// Raw commands / parameter level methods
	void sendCommand(uint8_t cmd);
	void sendCommand(uint8_t cmd, uint8_t param);
	void sendCommand(uint8_t cmd, uint16_t param);
	void sendCommand(uint8_t cmd, uint32_t param);

protected:
	// Helper methods
	__inline bool checkExecution() __attribute__((always_inline))
	{
		bool result = true;

		if (connector == nullptr || fpga == nullptr || !fpga->isInitialized)
		{
			LOGERROR("FPGA device/connector infrastructure was not not initialized properly\n");

			result = false;
		}
		else
		{
			if (pthread_mutex_trylock(&mutex) != 0)
			{
				LOGERROR("Unable to execute FPGA command when other command is still in progress\n");

				result = false;
			}
		}

		return result;
	}

	__inline void endExecution()  __attribute__((always_inline))
	{
		pthread_mutex_unlock(&mutex);
	}

	__inline void send8(uint8_t value) __attribute__((always_inline))
	{
		connector->transferByte(value);
	}

	/*
	 * Send 2 bytes value (MSB)
	 */
	__inline void send16(uint16_t value) __attribute__((always_inline))
	{
		connector->transferByte(value >> 8);
		connector->transferByte(value);
	}

	/*
	 * Send 3 bytes value (MSB)
	 */
	__inline void send24(uint32_t value) __attribute__((always_inline))
	{
		connector->transferByte(value >> 16);
		connector->transferByte(value >> 8);
		connector->transferByte(value);
	}

	/*
	 * Send 4 bytes value (MSB)
	 */
	__inline void send32(uint32_t value) __attribute__((always_inline))
	{
		connector->transferByte(value >> 24);
		connector->transferByte(value >> 16);
		connector->transferByte(value >> 8);
		connector->transferByte(value);
	}
};

// UserIO command codes
#define UIO_STATUS      0x00
#define UIO_BUT_SW      0x01

// codes as used by minimig (amiga)
#define UIO_JOYSTICK0   0x02  // also used by 8 bit
#define UIO_JOYSTICK1   0x03  // -"-
#define UIO_MOUSE       0x04  // -"-
#define UIO_KEYBOARD    0x05  // -"-
#define UIO_KBD_OSD     0x06  // keycodes used by OSD only

// codes as used by MiST (atari)
// directions (in/out) are from an io controller view
#define UIO_IKBD_OUT    0x02
#define UIO_IKBD_IN     0x03
#define UIO_SERIAL_OUT  0x04
#define UIO_SERIAL_IN   0x05
#define UIO_PARALLEL_IN 0x06
#define UIO_MIDI_OUT    0x07
#define UIO_MIDI_IN     0x08
#define UIO_ETH_MAC     0x09
#define UIO_ETH_STATUS  0x0a
#define UIO_ETH_FRM_IN  0x0b
#define UIO_ETH_FRM_OUT 0x0c
#define UIO_SERIAL_STAT 0x0d

#define UIO_JOYSTICK2   0x10  // also used by minimig and 8 bit
#define UIO_JOYSTICK3   0x11  // -"-
#define UIO_JOYSTICK4   0x12  // -"-
#define UIO_JOYSTICK5   0x13  // -"-

// codes as currently used by 8bit only
#define UIO_GET_STRING  0x14
#define UIO_SET_STATUS  0x15
#define UIO_GET_SDSTAT  0x16  // read status of sd card emulation
#define UIO_SECTOR_RD   0x17  // SD card sector read
#define UIO_SECTOR_WR   0x18  // SD card sector write
#define UIO_SET_SDCONF  0x19  // send SD card configuration (CSD, CID)
#define UIO_ASTICK      0x1a
#define UIO_SIO_IN      0x1b  // serial in
#define UIO_SET_SDSTAT  0x1c  // set sd card status
#define UIO_SET_SDINFO  0x1d  // send info about mounted image
#define UIO_SET_STATUS2 0x1e  // 32bit status
#define UIO_GET_KBD_LED 0x1f  // keyboard LEDs control
#define UIO_SET_VIDEO   0x20  // set HDMI video mode 0: 1280x720p60(TV), 1: 1280x1024p60(PC), 2-255: reserved
#define UIO_PS2_CTL     0x21  // get PS2 control from supported cores

// codes as used by 8bit (atari 800, zx81) via SS2
#define UIO_GET_STATUS  0x50
#define UIO_SECTOR_SND  0x51
#define UIO_SECTOR_RCV  0x52
#define UIO_FILE_TX     0x53
#define UIO_FILE_TX_DAT 0x54
#define UIO_FILE_INDEX  0x55
#define UIO_FILE_INFO   0x56

// ao486 direct memory access
#define UIO_DMA_WRITE   0x61
#define UIO_DMA_READ    0x62
#define UIO_DMA_SDIO    0x63

#define JOY_RIGHT       0x01
#define JOY_LEFT        0x02
#define JOY_DOWN        0x04
#define JOY_UP          0x08
#define JOY_BTN_SHIFT   4
#define JOY_BTN1        0x10
#define JOY_BTN2        0x20
#define JOY_BTN3        0x40
#define JOY_BTN4        0x80
#define JOY_MOVE        (JOY_RIGHT|JOY_LEFT|JOY_UP|JOY_DOWN)

#define BUTTON1         0x01
#define BUTTON2         0x02

// virtual gamepad buttons
#define JOY_A      JOY_BTN1
#define JOY_B      JOY_BTN2
#define JOY_SELECT JOY_BTN3
#define JOY_START  JOY_BTN4
#define JOY_X      0x100
#define JOY_Y      0x200
#define JOY_L      0x400
#define JOY_R      0x800
#define JOY_L2     0x1000
#define JOY_R2     0x2000
#define JOY_L3     0x4000
#define JOY_R3     0x8000

// keyboard LEDs control
#define KBD_LED_CAPS_CONTROL  0x01
#define KBD_LED_CAPS_STATUS   0x02
#define KBD_LED_CAPS_MASK     (KBD_LED_CAPS_CONTROL | KBD_LED_CAPS_STATUS)
#define KBD_LED_NUM_CONTROL   0x04
#define KBD_LED_NUM_STATUS    0x08
#define KBD_LED_NUM_MASK      (KBD_LED_NUM_CONTROL | KBD_LED_NUM_STATUS)
#define KBD_LED_SCRL_CONTROL  0x10
#define KBD_LED_SCRL_STATUS   0x20
#define KBD_LED_SCRL_MASK     (KBD_LED_SCRL_CONTROL | KBD_LED_SCRL_STATUS)
#define KBD_LED_FLAG_MASK     0xC0
#define KBD_LED_FLAG_STATUS   0x40

#define CONF_VGA_SCALER         0x04
#define CONF_CSYNC              0x08
#define CONF_FORCED_SCANDOUBLER 0x10
#define CONF_YPBPR              0x20
#define CONF_AUDIO_48K          0x40

// user io status bits (currently only used by 8bit)
#define UIO_STATUS_RESET   0x01

#define UIO_STOP_BIT_1   0
#define UIO_STOP_BIT_1_5 1
#define UIO_STOP_BIT_2   2

#define UIO_PARITY_NONE  0
#define UIO_PARITY_ODD   1
#define UIO_PARITY_EVEN  2
#define UIO_PARITY_MARK  3
#define UIO_PARITY_SPACE 4

#define UIO_PRIORITY_KEYBOARD 0
#define UIO_PRIORITY_GAMEPAD  1

typedef enum { EMU_NONE, EMU_MOUSE, EMU_JOY0, EMU_JOY1 } emu_mode_t;

#endif /* FPGA_FPGACOMMAND_H_ */
