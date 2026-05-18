#ifndef UTILS_H
#define UTILS_H

#define EARTH_RADIUS    6371.0
#define INPUT_SIZE      100

char    *input_str( void );
int     input_int_range(int min, int max);
int     input_int();
int     lev_distance(const char *a, const char *b);
void    to_lower(char *s);
int     strcmp_from(char *s1, char *s2, char c);
double  toDegrees(double radians);
double  toRadians(double degree);
Position midpoint(Position a, Position b);
double  haversine(Position posA, Position posB);


#endif