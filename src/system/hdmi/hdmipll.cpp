#include "hdmipll.h"

VideoMode HDMIPLL::m_videoModes[8] =
{
	{ {1280, 110, 40,  220, 720,  5,  5, 20}, 74.25  },
	{ {1024, 24,  136, 160, 768,  3,  6, 29}, 65     },
	{ {720,  16,  62,  60,  480,  9,  6, 30}, 27     },
	{ {720,  12,  64,  68,  576,  5,  5, 39}, 27     },
	{ {1280, 48,  112, 248, 1024, 1,  3, 38}, 108    },
	{ {800,  40,  128, 88,  600,  1,  4, 23}, 40     },
	{ {640,  16,  96,  48,  480,  10, 2, 33}, 25.175 },
	{ {1280, 440, 40,  220, 720,  5,  5, 20}, 74.25  }
};


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
