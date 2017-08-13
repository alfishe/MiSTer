#ifndef GUI_OSD_OSD_H_
#define GUI_OSD_OSD_H_

class osd {
public:
	static osd* instance();

	osd();
	virtual ~osd();

	void clear();
	void enable(unsigned char mode);
	void disable();

	void setTitle(char *title, int arrowDirection);
	void printText(unsigned char line, char *text, unsigned int start, unsigned int width, unsigned int offset, bool invert);

};

#endif /* GUI_OSD_OSD_H_ */
