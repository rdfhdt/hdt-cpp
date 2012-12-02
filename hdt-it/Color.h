/*
 * Color.h
 *
 *  Created on: 04/03/2011
 *      Author: mck
 */

#ifndef COLOR_H_
#define COLOR_H_

#include <QColor>

class Color {
public:
	double r,g,b,a;

public:
	Color();
	Color(double r, double g, double b);
	Color(double r, double g, double b, double a);
	virtual ~Color();

	void use();
	void invert();

	void setRGB(double r, double g, double b);
	void setRGBA(double r, double g, double b, double a);
	void setHSV(double h, double s, double v);
    QColor asQColor();
};

class ColorRamp {
public:
	virtual void apply(Color *co, double v, double vmin, double vmax)=0;
};


class ColorRamp1 : public ColorRamp {
public:
	void apply(Color *co, double v, double vmin, double vmax);
};


class ColorRamp2 : public ColorRamp {
public:
	void apply(Color *co, double v, double vmin, double vmax);
};

#endif /* COLOR_H_ */
