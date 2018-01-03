#include "messagecenter.h"

MessageCenter& MessageCenter::instance()
{
	static MessageCenter instance;

	return instance;
}

// Synonym to instance()
MessageCenter& MessageCenter::defaultCenter()
{
	return instance();
}

MessageCenter::MessageCenter()
{
	init();
}

MessageCenter::~MessageCenter()
{
	dispose();
}

bool MessageCenter::init()
{
	bool result = false;

	// Explicitly start processing queue
	m_queue.start();

	return result;
}

void MessageCenter::dispose()
{
	m_queue.stop();
}
