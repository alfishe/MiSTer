#ifndef IO_INPUT_MOUSEEMULATOR_H_
#define IO_INPUT_MOUSEEMULATOR_H_

class MouseEmulator
{
protected:
	// Fields
	int prev_dx = 0;
	int prev_dy = 0;

public:
	MouseEmulator();
	virtual ~MouseEmulator();
};

#endif /* IO_INPUT_MOUSEEMULATOR_H_ */
