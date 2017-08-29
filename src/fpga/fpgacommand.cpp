#include "fpgacommand.h"

#include "fpgaconnector.h"

FPGACommand::FPGACommand(FPGAConnector *connector)
{
	this->connector = connector;
}

FPGACommand::~FPGACommand()
{
}

