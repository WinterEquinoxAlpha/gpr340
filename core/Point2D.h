#ifndef POINT2D_H
#define POINT2D_H
#include <iostream>

struct Point2D {
public:
    Point2D(int x, int y): x(x), y(y){};
    Point2D() = default;
    int x, y;
    bool operator== (const Point2D& rhs) const;
    bool operator!= (const Point2D& rhs) const;
    Point2D& operator= (const Point2D& rhs);
    Point2D operator- (const Point2D& rhs) const;
    Point2D& operator+= (const Point2D& rhs);
    friend std::ostream& operator << (std::ostream& os, const Point2D& p);
};

#endif //POINT2D_H
