#ifndef FPGA_USERIO_H_
#define FPGA_USERIO_H_

#include <stdint.h>

// One can still use bit masking and operations.
// But for better code readability packed structures/union representation created as well.
union gpo
{
	// Full 32 bits immediate access
	uint32_t dword;

	// 16 bits of data (access either bytes or single word)
	union
	{
		struct
		{
			uint8_t data_byte0;
			uint8_t data_byte1;
		};

		uint16_t data_word;
	};

	// 16 bits of control (access either bytes, or word, or bit-fields)
    union
	{
		struct
		{
			uint8_t ctrl_byte0;
			uint8_t ctrl_byte1;
		};

		uint16_t ctrl_word;

		struct
		{
			bool gap16: 1;
			bool gap17: 1;
			bool strobe: 1;
			bool fpga_enable: 1;
			bool osd_enable: 1;
			bool io_enable: 1;
			bool dmode_enable: 1;
			bool gap23: 1;
			bool gap24: 1;
			bool gap25: 1;
			bool gap26: 1;
			bool gap27: 1;
			bool gap28: 1;
			bool btn_osd: 1;
			bool btn_user: 1;
			bool fpga_ready: 1;
		};
	};
};

union gpi
{
	// Full 32 bits immediate access
	uint32_t dword;

	// 16 bits of data (access either bytes or single word)
	union
	{
		struct
		{
			uint8_t data_byte0;
			uint8_t data_byte1;
		};

		uint16_t data_word;
	};

	// 16 bits of control (access either bytes, or word, or bit-fields)
    union
	{
		struct
		{
			uint8_t ctrl_byte0;
			uint8_t ctrl_byte1;
		};

		uint16_t ctrl_word;

		struct
		{
			bool gap16: 1;
			bool gap17: 1;
			bool ack: 1;
			bool gap19: 1;
			bool gap20: 1;
			bool gap21: 1;
			bool gap22: 1;
			bool gap23: 1;
			bool gap24: 1;
			bool gap25: 1;
			bool gap26: 1;
			bool gap27: 1;
			bool gap28: 1;
			bool gap29: 1;
			bool led_disk: 1;
			bool fpga_ready: 1;
		};
	};
};

class user_io
{
public:
	user_io();
	virtual ~user_io();
};

#endif /* FPGA_USERIO_H_ */
