#ifndef FPGA_FPGACOMMAND_H_
#define FPGA_FPGACOMMAND_H_

#include <pthread.h>

// Forward declaration. Header included from fpgacommand.cpp
class FPGAConnector;

class FPGACommand
{
protected:
	// Physical transfers done using FPGAConnector
	FPGAConnector *connector = nullptr;

	// Ensure that command scope untouched during the whole execution with mutex guard
	pthread_mutex_t mutex;

public:
	FPGACommand(FPGAConnector *connector);
	virtual ~FPGACommand();
};

#endif /* FPGA_FPGACOMMAND_H_ */
