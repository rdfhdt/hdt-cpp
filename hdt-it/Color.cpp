/*
 * Color.cpp
 *
 *  Created on: 04/03/2011
 *      Author: mck
 */

#include <iostream>
#include <math.h>

#include "Color.h"
#include "constants.h"

 Color::Color() {
 	setRGBA(1,1,1,1);
 }

 Color::~Color() {
 }


void Color::use() {

}

/** Invert Color
 * @param c Description of the param.
 * @return void
 */
void Color::invert() {
	r = 1 - r;
	g = 1 - g;
	b = 1 - b;
}

void Color::setRGB(double r, double g, double b) {
	this->r = r;
	this->g = g;
	this->b = b;
}

void Color::setRGBA(double r, double g, double b, double a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

/** HSV to RGB
 * @param h Description of the param.
 * @param s Description of the param.
 * @param v Description of the param.
 * @param c Description of the param.
 * @return void
 */
void Color::setHSV(double h, double s, double v) {
        // H is given on [0, 6]. S and V are given on [0, 1].
	// RGB are each returned on [0, 1].
	float m, n, f;
	int i;

	if (h == -1) {
		setRGB(v, v, v);
		return;
	}

	i = floor(h);
	f = h - i;
	if (!(i & 1))
		f = 1 - f; // if i is even
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
	case 6:
	case 0:
		setRGB(v, n, m);
		break;
	case 1:
		setRGB(n, v, m);
		break;
	case 2:
		setRGB(m, v, n);
		break;
	case 3:
		setRGB(m, n, v);
		break;
	case 4:
		setRGB(n, m, v);
		break;
	case 5:
		setRGB(v, m, n);
		break;
    }
}

QColor Color::asQColor()
{
    QColor c(255*r, 255*g, 255*b, 255*a);
    return c;
}

/**Get Color 2
 * @param val Description of the param.
 * @param vmin Description of the param.
 * @param vmax Description of the param.
 * @param c Description of the param.
 * @return void
 */
void ColorRamp2::apply(Color *c, double val, double vmin, double vmax) {
    double total = vmax - vmin;
    double prop = (vmin + val / total);

    if(vmax==0.0) {
        c->setRGB(1.0, 0.0, 0.0);
        return;
    }

    //std::cout << "h: " << fmod(prop*100,6) << std::endl;
    //std::cout << "\tmin: " << vmin << " val: "<< val << " max: " << vmax << " Prop: " << prop << " dv: " << total << std::endl;
#ifdef SCREEN
    c->setHSV(fmod(prop*100,6), 1.0 , 0.5 + 0.5 * prop);
	c->invert();
#else
    c->setHSV(fmod(prop*100,6), 1.0, 0.3+0.7*prop);
	//c->invert();
#endif
	//	printf("(%.1f, %.1f, %.1f) %.1f\n", val, vmin, vmax, prop);
}

/** Get Color
 * @param v Description of the param.
 * @param vmin Description of the param.
 * @param vmax Description of the param.
 * @param c Description of the param.
 * @return void
 */
void ColorRamp1::apply(Color *c, double v, double vmin, double vmax) {
	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;

	double dv = vmax - vmin;

	if (v < (vmin + 0.25 * dv)) {
		c->setRGB(0, 4 * (v - vmin) / dv, 1);
	} else if (v < (vmin + 0.5 * dv)) {
		c->setRGB(0, 1 + 4 * (vmin + 0.25 * dv - v) / dv, 1);
	} else if (v < (vmin + 0.75 * dv)) {
		c->setRGB(4 * (v - vmin - 0.5 * dv) / dv, 1, 0);
	} else {
		c->setRGB(1, 1 + 4 * (vmin + 0.75 * dv - v) / dv, 0);
	}
}



