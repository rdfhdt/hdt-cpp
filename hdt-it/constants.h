/*
 * constants.h
 *
 *  Created on: 04/03/2011
 *      Author: mck
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define TIMER_DELAY 50
#define RENDER_NUM_POINTS 100000

#define SCREEN

#ifndef SCREEN
#define TEXT_COLOR 0.0, 0.0, 0.0, 1.0
#define CROSS_COLOR 1.0, 0.0, 0.0, 1.0
#define TEXT_BACKGROUND_COLOR 1.0, 1.0, 1.0, 0.9
#define BACKGROUND_COLOR 1, 1, 1, 1
#define SHARED_AREA_COLOR 0.9, 0.9, 0.9, 1.0
#define SHARED_AREA_BORDER_COLOR 0.1, 0.1, 0.1, 1.0
#define AXIS_COLOR 0.1, 0.1, 0.1, 1.0
#define GRID_COLOR 0.3, 0.3, 0.3, 1.0
#define RDF_POINT_SIZE 1.5
#else
#define TEXT_COLOR 1.0, 1.0, 1.0, 1.0
#define CROSS_COLOR 0.0, 0.0, 0.0, 1.0
#define TEXT_BACKGROUND_COLOR 0.0, 0.0, 0.0, 0.7
#define BACKGROUND_COLOR 0, 0, 0, 1
#define SHARED_AREA_COLOR 0.1, 0.1, 0.1, 1.0
#define SHARED_AREA_BORDER_COLOR 0.2, 0.2, 0.2, 1.0
#define AXIS_COLOR 0.8, 0.8, 0.8, 1.0
#define GRID_COLOR 0.2, 0.2, 0.2, 1.0
#define RDF_POINT_SIZE 1
#endif


#endif /* CONSTANTS_H_ */
