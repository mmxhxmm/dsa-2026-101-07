#ifndef UTILS_H
#define UTILS_H

void    to_lower(char *s);
int     strcmp_from(char *s1, char *s2, char c);
char    *input_str(int max);
int     lev_distance(const char *a, const char *b);

int input_int();
double toDegrees(double radians);
double toRadians(double degree);
Position midpoint(Position a, Position b);
double haversine(Position posA, Position posB);


#endif