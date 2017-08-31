#ifndef COMMON_SYSTEM_SYSMANAGER_H_
#define COMMON_SYSTEM_SYSMANAGER_H_

class sysmanager
{
public:
	static int getProcessID();
	static char* getApplicationName();
	static void restartApplication();

	static const char* getDataRootDir();

private:
	sysmanager() {};
};

#endif /* COMMON_SYSTEM_SYSMANAGER_H_ */
