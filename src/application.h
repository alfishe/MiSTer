#ifndef APPLICATION_H_
#define APPLICATION_H_

class Application
{
public:
	// Singleton instance
	static Application& instance();
	Application(const Application& that) = delete; 			// Copy constructor is forbidden here (C++11 feature)
	Application& operator =(Application const&) = delete;		// Disable assignments
	virtual ~Application();
private:
	Application() {}; 										// Disallow direct instances creation

public:
	void onStart();
	void onTerminate();
};

#endif /* APPLICATION_H_ */
