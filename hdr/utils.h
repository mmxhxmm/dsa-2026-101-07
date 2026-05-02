#ifndef UTILS_H
#define UTILS_H

<<<<<<< HEAD
void    to_lower(char *s);
int     strcmp_from(char *s1, char *s2, char c);
char    *input_str(int max);
int     input_int();
int     lev_distance(const char *a, const char *b);
=======

char *input_str(int max);
int input_int();
int lev_distance(const char *a, const char *b);
double toDegrees(double radians);
double toRadians(double degree);
Position midpoint(Position a, Position b);
double haversine(Position posA, Position posB);

>>>>>>> 1506ffd ([LAB 4] Almost finished version)

#endif