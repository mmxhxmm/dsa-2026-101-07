#ifndef UTILS_H
#define UTILS_H

#define EARTH_RADIUS 6371.0
#define INPUT_SIZE 10077

#include "../hdr/common.h"

int hash_function(long long intersection_id, int table_size);
char *input_str(void);
int input_int_range(int min, int max);
int input_int();
int lev_distance(const char *a, const char *b);
void to_lower(char *s);
int strcmp_from(char *s1, char *s2, char c);
double toDegrees(double radians);
double toRadians(double degree);
Position midpoint(Position a, Position b);
double haversine(Position posA, Position posB);
void latlon_to_xy(double lat_ref, double lon_ref,
                  double lat, double lon,
                  double *x, double *y);

#endif