#include "fpgaconnector.h"

#include "fpgadevice.h"

FPGAConnector::FPGAConnector(FPGADevice *fpga)
{
	this->fpga = fpga;
}

FPGAConnector::~FPGAConnector()
{
}

void FPGAConnector::enableFPGA()
{
	enableByMask(SSPI_FPGA_EN);
}

void FPGAConnector::disableFPGA()
{
	disableByMask(SSPI_FPGA_EN);
}

void FPGAConnector::enableOSD()
{
	enableByMask(SSPI_OSD_EN);
}

void FPGAConnector::disableOSD()
{
	disableByMask(SSPI_OSD_EN);
}

void FPGAConnector::enableIO()
{
	enableByMask(SSPI_IO_EN);
}

void FPGAConnector::disableIO()
{
	disableByMask(SSPI_IO_EN);
}

void FPGAConnector::enableDMode()
{
	enableByMask(SSPI_DM_EN);
}

void FPGAConnector::disableDMode()
{
	disableByMask(SSPI_DM_EN);
}

