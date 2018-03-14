#include "hdmipll.h"

#include "../../common/logger/logger.h"


#include "cstring"
#include "hdmi.h"
#include "../../fpga/fpgadevice.h"
#include "../../fpga/fpgaconnector.h"
#include "../../fpga/fpgacommand.h"

// Standard video modes pre-defined
HDMIVideoMode HDMIPLL::m_videoModes[] =
{
	{ { 1280, 110,  40, 220,  720,  5,  5, 20 }, 74.25 },
	{ { 1024,  24, 136, 160,  768,  3,  6, 29 }, 65.0 },
	{ { 720,  16,  62,  60,  480,  9,  6, 30 }, 27.0 },
	{ { 720,  12,  64,  68,  576,  5,  5, 39 }, 27.0 },
	{ { 1280,  48, 112, 248, 1024,  1,  3, 38 }, 108 },
	{ { 800,  40, 128,  88,  600,  1,  4, 23 }, 40.0 },
	{ { 640,  16,  96,  48,  480, 10,  2, 33 }, 25.175 },
	{ { 1280, 440,  40, 220,  720,  5,  5, 20 }, 74.25 },
	{ { 1920,  88,  44, 148, 1080,  4,  5, 36 }, 148.5 },
	{ { 1920, 528,  44, 148, 1080,  4,  5, 36 }, 148.5 }
};

// Allow HDMI pixel clock frequency in a range [20:200] MHz
bool HDMIPLL::isValidHDMIFrequency(double freq)
{
	bool result = false;

	if (freq >= 20.0f && freq < 200.1f)
	{
		result = true;
	}

	return result;
}

// Calculate PLL parameters for specified <freqPixelclock>
// Input: freqPixelclock - target pixel clock (in MHz)
// Output: M, K, C PLL parameters
//		M - Feedback counter multiply factor value
//		K - Fractional multiply factor value
//		C - Post-scale divide factor value
// See more: https://www.altera.com/en_US/pdfs/literature/ug/altera_pll.pdf
bool HDMIPLL::getPLL(double freqPixelclock, uint32_t *M, uint32_t *K, uint32_t *C)
{
	bool result = false;

	if (!isValidHDMIFrequency(freqPixelclock))
	{
		LOGERROR("Invalid HDMI pixel clock frequency");
		return result;
	}

	LOGWARN("%s: Doesn't work after porting yet!", __PRETTY_FUNCTION__);
	return result;

	// Limits:
	// Multiply Factor (M-Counter)		[1-512]				- Specifies the multiply factor of M-counter
	// Fractional Multiply Factor (K)	[1 to (2^Fcout-1)]	- Specifies the fractional multiply factor of DSM. Fcout is the value of fractional carry out parameter.
	// Divide Factor (C-Counter)			[1-512]				- Specifies the divide factor for the output clock (C-counter)
	//
	// Divide Factor (N-Counter)			[1-512]				- Specifies the divide factor of N-counter

	uint32_t multiplierCoeff = 1;
	uint32_t feedbackMultiply_M;		// M-value
	uint32_t fractionalMultiply_K;	// K-value
	uint32_t postScaleDivide_C; 		// C-value

	// Select max multiplier to stay within 400MHz pre-scaled range
	while ((freqPixelclock * multiplierCoeff) < 400.0)
	{
		multiplierCoeff++;
	}

	TRACE("Calculating PLL parameters for freq: %.3f MHz ...", freqPixelclock);

	int iterations = 0;
	while (true)
	{
		iterations++;
		if (iterations > 100)
		{
			LOGWARN("Unable to find PLL coefficients within %d iterations. FAIL.", iterations);
			return result;
		}

		DEBUG("C=%d, ", multiplierCoeff);
		*C = multiplierCoeff;

		double fvco = freqPixelclock * multiplierCoeff;
		DEBUG("Fvco=%f, ", fvco);

		uint32_t m = (uint32_t)(fvco / 50);
		DEBUG("M=%d, ", m);
		*M = m;

		double ko = ((fvco / 50.0) - m);
		DEBUG("K_orig=%f, ", ko);

		uint32_t k = (uint32_t)(ko * 4294967296);
		if (k == 0)
			k = 1;

		DEBUG("K=%u. ", k);
		*K = k;

		if (ko && (ko <= 0.05f || ko >= 0.95f))
		{
			if (fvco > 1500.f)
			{
				DEBUG("Fvco > 1500MHz. Cannot calculate PLL parameters!");
				return 0;
			}

			DEBUG("K_orig is outside desired range try next C0\n");
			multiplierCoeff++;
		}
		else
		{
			DEBUG("Calculated successfully");
			result = true;
			break;
		}
	}

	return result;
}

uint32_t HDMIPLL::getPLLDivisor(uint32_t div)
{
	uint32_t result;

	if (div & 1)
	{
		result = 0x20000 | (((div / 2) + 1) << 8) | (div / 2);
	}
	else
	{
		result = ((div / 2) << 8) | (div / 2);
	}

	return result;
}

// See details: https://www.altera.com/en_US/pdfs/literature/ug/altera_pll.pdf
// See details: https://www.altera.com/en_US/pdfs/literature/an/an661.pdf
// PLL Reconfiguration calculator: https://www.altera.com/content/dam/altera-www/global/en_US/others/literature/an/PLL_Reconfiguration_Calculator.xlsx
HDMIVideoModePacket* HDMIPLL::getStandardVideoModePacket(int idxVideoMode)
{
	HDMIVideoModePacket* result = nullptr;

	// Initial validation
	if ((size_t)idxVideoMode >= sizeof(m_videoModes) / sizeof (m_videoModes[0]))
	{
		LOGWARN("getStandardVideoModePacket: Invalid idxVideoMode parameter: %d", idxVideoMode);
		return result;
	}

	// Alloc HDMIVideoModePacket object. Should be deleted by the caller.
	result = new HDMIVideoModePacket();

	// Calculate PLL coefficients based on target pixel clock frequency
	uint32_t M, K, C;
	getPLL(m_videoModes[idxVideoMode].freqPixelClock, &M, &K, &C);

	result->nVideoMode = idxVideoMode;

	// Transfer video mode to packet structure
	//memcpy(result, &m_videoModes[idxVideoMode].vmodes, sizeof(m_videoModes[0].vmodes));
	for (int i = 0; i < 8; i++)
	{
		result->videoMode.vmodes[i] = m_videoModes[idxVideoMode].vmodes[i];
	}

	// Transfer PLL register values
	// All values calculated via PLL Reconfiguration calculator: https://www.altera.com/content/dam/altera-www/global/en_US/others/literature/an/PLL_Reconfiguration_Calculator.xlsx
	// Documentation: AN661 - Implementing Fractional PLL Reconfiguration with Altera PLL and Altera PLL Reconfig IP Cores
	result->pllRegisters =
	{
		{ 0x0004, M },			// 0b000100 - Counter (M). Size: 18-bits
		{ 0x0003, 0x10000 },		// 0b000011 - Counter(N). Size: 18-bits
		{ 0x0005, C },			// 0b000101 - Counter(C). Size: 23-bits
		{ 0x0009, 2 },			// 0b001001 - Charge Pump Setting. Size: 3-bits
		{ 0x0008, 7 },			// 0b001000 - Bandwidth setting. Size: 4-bits
		{ 0x0007, K }			// 0b000111 - Counter Fractional Value (K)
	};

	// PLL settings
	/*
	result->packet.v9 = 4;
	result->packet.pllM = M;

	result->packet.v11 = 3;
	result->packet.v12 = 0x10000;

	result->packet.v13 = 5;
	result->packet.pllC = C;

	result->packet.v15 = 9;
	result->packet.v16 = 2;

	result->packet.v17 = 8;
	result->packet.v18 = 7;

	result->packet.v19 = 7;
	result->packet.pllK = K;
	*/

	return result;
}

void HDMIPLL::parseCustomVideoMode(const string& mode)
{
	/*
	char *cfg = mist_cfg.video_conf;

	int cnt = 0;
	while (*cfg)
	{
		char *next;
		if (cnt == 9 && vitems[0] == 1)
		{
			double Fout = strtod(cfg, &next);
			if (cfg == next || (Fout < 20.f || Fout > 200.f))
			{
				printf("Error parsing video_mode parameter: ""%s""\n", mist_cfg.video_conf);
				return 0;
			}

			uint32_t M, K, C;
			if (!getPLL(Fout, &M, &K, &C)) return 0;

			vitems[9] = 4;
			vitems[10] = getPLLdiv(M);
			vitems[11] = 3;
			vitems[12] = 0x10000;
			vitems[13] = 5;
			vitems[14] = getPLLdiv(C);
			vitems[15] = 9;
			vitems[16] = 2;
			vitems[17] = 8;
			vitems[18] = 7;
			vitems[19] = 7;
			vitems[20] = K;
			break;
		}

		uint32_t val = strtoul(cfg, &next, 0);
		if (cfg == next || (*next != ',' && *next))
		{
			printf("Error parsing video_mode parameter: ""%s""\n", mist_cfg.video_conf);
			return 0;
		}

		if (cnt < 32) vitems[cnt] = val;
		if (*next == ',') next++;
		cfg = next;
		cnt++;
	}

	if (cnt == 1)
	{
		printf("Set predefined video_mode to %d\n", vitems[0]);
		return vitems[0];
	}

	if ((vitems[0] == 0 && cnt < 21) || (vitems[0] == 1 && cnt < 9))
	{
		printf("Incorrect amount of items in video_mode parameter: %d\n", cnt);
		return 0;
	}

	if (vitems[0] > 1)
	{
		printf("Incorrect video_mode parameter\n");
		return 0;
	}

	return -1;
	*/
}

void HDMIPLL::parseVideoMode(const string& mode)
{
	/*
	// always 0. Use custom parameters.
	mist_cfg.video_mode = 0;

	int mode = parse_custom_video_mode();
	if (mode >= 0)
	{
		if (mode >= 8) mode = 0;
		for (int i = 0; i < 8; i++)
		{
			vitems[i + 1] = vmodes[mode].vpar[i];
		}

		uint32_t M, K, C;
		getPLL(vmodes[mode].Fpix, &M, &K, &C);

		vitems[9] = 4;
		vitems[10] = getPLLdiv(M);
		vitems[11] = 3;
		vitems[12] = 0x10000;
		vitems[13] = 5;
		vitems[14] = getPLLdiv(C);
		vitems[15] = 9;
		vitems[16] = 2;
		vitems[17] = 8;
		vitems[18] = 7;
		vitems[19] = 7;
		vitems[20] = K;
	}

	printf("Send HDMI parameters:\n");
	spi_uio_cmd_cont(UIO_SET_VIDEO);
	printf("video: ");
	for (int i = 1; i <= 8; i++)
	{
		spi_w(vitems[i]);
		printf("%d, ", vitems[i]);
	}
	printf("\nPLL: ");
	for (int i = 9; i < 21; i++)
	{
		printf("0x%X, ", vitems[i]);
		if (i & 1) spi_w(vitems[i]);
		else
		{
			spi_w(vitems[i]);
			spi_w(vitems[i] >> 16);
		}
	}

	printf("\n");
	DisableIO();
	*/
}

// Send video mode data to FPGA
// Command: UIO_SET_VIDEO
// Video mode data: 8x16-bit words
// PLL register values: pairs of address(16-bit) and values (32-bit)
void HDMIPLL::setVideoMode(HDMIVideoModePacket* modePacket)
{
	FPGADevice& fpga = FPGADevice::instance();
	FPGAConnector& connector = *(fpga.connector);
	FPGACommand& command = *(fpga.command);

	if (command.startIO())
	{
		command.sendCommand(UIO_SET_VIDEO);

		// Video mode data from HDMIVideoModeType / HDMIVideoMode
		for (int i = 0; i < 8; i++)
		{
			connector.transferWord(modePacket->videoMode.vmodes[i]);
		}

		// PLL registers data
		for (PLLPortVector::const_iterator it = modePacket->pllRegisters.begin(); it != modePacket->pllRegisters.end(); it++)
		{
			// Transfer PLL register address (16-bits)
			connector.transferWord(it->first);

			// Transfer PLL register value
			uint32_t value = it->second;
			connector.transferWord(value);
			connector.transferWord(value >> 16);
		}

		command.endIO();
	}
	else
	{
		LOGWARN("%s: Unable to set HDMI video mdoe", __PRETTY_FUNCTION__);
	}
}
