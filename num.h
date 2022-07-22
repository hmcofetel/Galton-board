#ifndef __NUM_H__
#define __NUM_H__ 
	
#include <math.h>
#include <iostream>
using namespace std;

float magnitude(float a[])
{
	return sqrt(a[0]*a[0] + a[1]*a[1]);
}

float distance(float a[], float b[])
{
	return sqrt( pow( b[0] - a[0]  ,2) + pow(b[1] - a[1]   ,2 )    );
}

float dot(float a[], float b[])
{
	return abs(a[0]*b[0] + a[1]*b[1]);
}

float *add(float a[], float b[])
{
	float *result = new float();	
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];

	return result;
}

float *get_projection_vect(float a[], float b[])
{
	float *result = new float();

	float mag = abs(a[0]*b[0] + a[1]*b[1])/(magnitude(b)*magnitude(b));

	result[0] = b[0]*mag;
	result[1] = b[1]*mag;
	return result;

}

#endif