//
// Created by Per Hüpenbecker on 10.11.24.
//

#ifndef SIMPLELINEARREGRESSION_COMMON_H
#define SIMPLELINEARREGRESSION_COMMON_H

#include <stddef.h>

// Standard DataPoint struct for x and y values. The attributes x and y are defined as pointers to
// allow for separate caching of the values.

typedef struct {
    double* x;
    size_t x_dimensions;
    double* y;
} DataPoint;

// Datastructure for handling the dataset cosisting of DataPoints.

typedef struct {
    DataPoint* data_points;
    size_t data_points_count;
} DataSet;


typedef struct {
    double*
};





#endif //SIMPLELINEARREGRESSION_COMMON_H
