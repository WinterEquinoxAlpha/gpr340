#ifndef POINT2D_H
#define POINT2D_H
#include <iostream>

struct Point2D {
public:
    Point2D(int x = 0, int y = 0): x(x), y(y){};

    int x, y;
    bool operator== (const Point2D& rhs) const;
    bool operator!= (const Point2D& rhs) const;
    Point2D& operator= (const Point2D& rhs);
    friend std::ostream& operator << (std::ostream& os, const Point2D& p);
};

#endif //POINT2D_H
