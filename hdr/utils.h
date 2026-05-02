#ifndef UTILS_H
#define UTILS_H


char *input_str(int max);
int input_int();
int lev_distance(const char *a, const char *b);
double toDegrees(double radians);
double toRadians(double degree);
Position midpoint(Position a, Position b);
double haversine(Position posA, Position posB);


#endif