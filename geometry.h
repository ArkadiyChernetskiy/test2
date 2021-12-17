#include <iostream>
#include <vector>
#include <cmath>

using std::vector;
using std::initializer_list;
using std::pair;
using std::make_pair;
using std::abs;


const double epsilon = 1e-6;
bool equal(double first, double second) {
    return abs(first - second) <= epsilon;
}
double squaring(double t) {
    return t * t;
}


class Line;

////////////    POINT    ////////////
struct Point {
    double x = 0.0;
    double y = 0.0;
    Point() = default;
    ~Point() = default;
    Point(double x, double y) : x(x), y(y) {}

    Point rotate(const Point& center, double angle) const;
    void scale(const Point& center, double k);
    void reflex(const Point center);
    void reflex(const Line axis); //определение после Line
};
Point Point::rotate(const Point& center, double angle) const{ // угол в радианах!!
    double new_x = center.x + cos(angle) * (this->x - center.x) - sin(angle) * (this->y - center.y);
    double new_y = center.y + sin(angle) * (this->x - center.x) + cos(angle) * (this->y - center.y);
    return Point(new_x, new_y);
}
void Point::scale(const Point& center, double k) {
    x = (x - center.x) * k + center.x;
    y = (y - center.y) * k + center.y;
}
void Point::reflex(const Point center) {
    x = 2 * center.x - x;
    y = 2 * center.y - y;
}
bool operator==(const Point& point1, const Point& point2) {
    return equal(point1.x, point2.x) && equal(point1.y, point2.y);
}
bool operator!=(const Point& point1, const Point& point2) {
    return !(point1 == point2);
}
Point middle_point(const Point& point1, const Point& point2) {
    return Point((point1.x + point2.x) / 2, (point1.y + point2.y) / 2);
}
double points_distance(const Point& point1, const Point& point2) {
    return sqrt(squaring(point1.x - point2.x) + squaring(point1.y - point2.y));
}


////////////    LINE    ////////////
// Ax  + By + C = 0
class Line {
public:
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;
    Line() = default;
    Line(const Point&, const Point&);
    Line(double k, double b) : a(-k), b(1), c(-b) {}
    Line(const Point& point, double k) : a(-k), b(1) {
        c = point.x * k - point.y;
    }
    Line(const double a, const double b, const double c): a(a), b(b), c(c) {};
    ~Line() = default;
};
Line::Line(const Point& point1, const Point& point2) {
    a = point1.y - point2.y;
    b = point2.x - point1.x;
    c = point1.x * point2.y - point2.x * point1.y;
}
bool operator==(const Line& line1, const Line& line2) {
    if (equal(line1.a * line2.b, line2.a * line1.b))
        if (equal(line1.b * line2.c, line2.b * line1.c))
            return true;
    return false;
}
bool operator!=(const Line& line1, const Line& line2) {
    return !(line1 == line2);
}
bool is_intersection(const Line& line1, const Line& line2) {
    return !equal(line1.a * line2.b, line1.b * line2.a);
}
bool is_parallel(const Line& line1, const Line& line2) {
    return line1 != line2 && !is_intersection(line1, line2);
}
bool is_perpendicular(const Line& line1, const Line& line2) {
    return equal(line1.a * line2.a, -line1.b * line2.b);
}
Point cross(const Line& line1, const Line& line2) {
    Point tmp;
    tmp.x = (line1.b * line2.c - line2.b * line1.c) / (line1.a * line2.b - line2.a * line1.b);
    tmp.y = (line2.a * line1.c - line1.a * line2.c) / (line1.a * line2.b - line2.a * line1.b);
    return tmp;
}
Line perpendicular(const Line& line) {
    return Line(line.b, -line.a, 0);
}
Line middle_perpendicular(const Point& point1, const Point& point2) {
    Point middle = middle_point(point1, point2);
    Line axis(point1, point2);
    Line result = perpendicular(axis);
    result.c = -result.a * middle.x - result.b * middle.y;
    return result;
}


void Point::reflex(const Line axis) {
    Line tmp = perpendicular(axis);
    tmp.c = -tmp.a * x - tmp.b * y;
    scale(cross(tmp, axis), -1);
}


////////////    SHAPE    ////////////
class Shape {
public:
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool operator==(const Shape& another) const = 0;
    virtual bool operator!=(const Shape& another) const = 0;
    virtual bool isCongruentTo(const Shape&) const = 0;
    virtual bool isSimilarTo(const Shape&) const = 0;
    virtual bool containsPoint(Point point) const = 0;

    virtual void rotate(Point center, double angle) = 0;
    virtual void reflex(Point center) = 0;
    virtual void reflex(Line axis) = 0;
    virtual void scale(Point center, double k) = 0;
    virtual ~Shape() = 0;
};
Shape::~Shape() {}


class Polygon : public Shape {
protected:
    vector <Point> vertices;
public:
    Polygon() = default;
    Polygon(const vector<Point>&);
    Polygon(initializer_list<Point>);
    ~Polygon() = default;
    int verticesCount() const;
    vector<Point> getVertices();
    bool isConvex() const;

    double perimeter() const override;
    double area() const override;
    bool operator==(const Shape&) const override;
    bool operator!=(const Shape&) const override;
    bool isCongruentTo(const Shape&) const override;
    bool isSimilarTo(const Shape&) const override;
    bool containsPoint(Point point) const override;
    void rotate(Point center, double angle) override;
    void reflex(Point center) override;
    void reflex(Line axis) override;
    void scale(Point center, double coefficient) override;
};
Polygon::Polygon(const vector<Point>& vec) : vertices(vec) {}
Polygon::Polygon(initializer_list<Point> vec) : vertices(vec) {}
int Polygon::verticesCount() const {
    return vertices.size();
}
vector<Point> Polygon::getVertices() {
    return vertices;
}
bool Polygon::isConvex() const {
    // [ <p_i,p_i+1> , <p_i+1,p_i+2> ]
    double P01_x = vertices[1].x - vertices[0].x, P01_y = vertices[1].y - vertices[0].y;
    double P12_x = vertices[2].x - vertices[1].x, P12_y = vertices[2].y - vertices[1].y;
    double determinant = P01_x * P12_y - P12_x * P01_y;
    bool is_positive;
    if (determinant > 0) is_positive = true;
    else is_positive = false;
    int size = verticesCount();
    for (int i = 1; i < size; ++i) {
        P01_x = vertices[(i + 1) % size].x - vertices[i].x;
        P01_y = vertices[(i + 1) % size].y - vertices[i].y;
        P12_x = vertices[(i + 2) % size].x - vertices[(i + 1) % size].x;
        P12_y = vertices[(i + 2) % size].y - vertices[(i + 1) % size].y;
        determinant = P01_x * P12_y - P12_x * P01_y;
        if ((determinant > 0 && !is_positive) || (determinant < 0 && is_positive))
            return false;
    }
    return true;
}
double Polygon::perimeter() const {
    double P = 0;
    int size = verticesCount();
    for (int i = 0; i < size; ++i) {
        P += points_distance(vertices[i], vertices[(i + 1) % size]);
    }
    return P;
}
double Polygon::area() const {
    double S = 0;
    int size = verticesCount();
    for (int i = 0; i < size; ++i) {
        S += vertices[i].x * vertices[(i + 1) % size].y - vertices[i].y * vertices[(i + 1) % size].x;
    }
    return abs(S / 2);
}
bool Polygon::operator==(const Shape& another) const {
    auto polygon_ptr = dynamic_cast<const Polygon*>(&another);
    if (!polygon_ptr) return false;
    if (verticesCount() != polygon_ptr->verticesCount()) return false;
    Point first_point = polygon_ptr->vertices[0];
    int match_index = -1;
    for (int i = 0; i < verticesCount(); ++i) {
        if (vertices[i] == first_point) {
            match_index = i;
            break;
        }
    }
    if (match_index == -1) return false;
    bool is_equal = true;
    // обход в одну сторону
    for(int i = 0; i < verticesCount(); ++i) {
        if (polygon_ptr->vertices[i] != vertices[(match_index + i) % verticesCount()]) {
            is_equal = false;
            break;
        }
    }
    if (is_equal) return true;
    // пробуем обход в другую сторону
    is_equal = true;
    for(int i = 0; i < verticesCount(); ++i) {
        int j;
        if (match_index - i < 0) j = verticesCount() + (match_index - i);
        else j = match_index - i;
        if (polygon_ptr->vertices[i] != vertices[j]) {
            is_equal = false;
            break;
        }
    }
    return is_equal;
}
bool Polygon::operator!=(const Shape& another) const {
    return !(*this == another);
}
bool Polygon::isCongruentTo(const Shape& another) const {
    auto polygon_ptr = dynamic_cast<const Polygon*>(&another);
    if (!polygon_ptr) return false;
    if (verticesCount() != polygon_ptr->verticesCount()) return false;
    vector<double> side_len_this, side_len_that;
    for (int i = 0; i < verticesCount() - 1; ++i) {
        side_len_this.push_back(points_distance(vertices[i], vertices[i + 1]));
        side_len_that.push_back(points_distance(polygon_ptr->vertices[i], polygon_ptr->vertices[i + 1]));
    }
    side_len_this.push_back(points_distance(vertices[verticesCount() - 1], vertices[0]));
    side_len_that.push_back(points_distance(polygon_ptr->vertices[verticesCount() - 1], polygon_ptr->vertices[0]));
    bool is_equal = true;
    int match_index = -1, first_index = 0;
    double first_len = side_len_that[0];
    while (match_index < verticesCount()) {
        for (int i = first_index; i < verticesCount(); ++i) {
            if (equal(side_len_this[i], first_len)) {
                match_index = i;
                break;
            }
        }
        if (match_index == -1) return false;
        // обход в одну сторону
        for(int i = 0; i < verticesCount(); ++i) {
            if (!equal(side_len_that[i],side_len_this[(match_index + i) % verticesCount()])) {
                is_equal = false;
                break;
            }
        }
        if (is_equal) return true;
        // пробуем обход в другую сторону
        is_equal = true;
        for(int i = 0; i < verticesCount(); ++i) {
            int j;
            if (match_index - i < 0) j = verticesCount() + (match_index - i);
            else j = match_index - i;
            if (!equal(side_len_that[i],side_len_this[j])) {
                is_equal = false;
                break;
            }
        }
        if (is_equal) return true;
        first_index = match_index + 1;
        match_index = -1;
    }
    return is_equal;
}
bool Polygon::isSimilarTo(const Shape& another) const {
    auto polygon_ptr = dynamic_cast<const Polygon*>(&another);
    if (!polygon_ptr) return false;
    double ratio = perimeter() / polygon_ptr->perimeter();
    return equal(squaring(ratio), area() / polygon_ptr->area());
}
bool Polygon::containsPoint(Point point) const {
    bool is_inside = false;
    int j = verticesCount() - 1;
    for (int i = 0; i < verticesCount(); i++) {
        if (((vertices[i].y < point.y && vertices[j].y >= point.y) || (vertices[j].y < point.y && vertices[i].y >= point.y))
            && (vertices[i].x + (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) * (vertices[j].x - vertices[i].x) < point.x))
            is_inside = !is_inside;
        j = i;
    }
    return is_inside;
}
void Polygon::rotate(Point center, double angle) {
    angle *= M_PI / 180;
    for (int i = 0; i < verticesCount(); ++i)
        vertices[i] = vertices[i].rotate(center, angle);
}
void Polygon::reflex(Point center) {
    scale(center, -1);
}
void Polygon::reflex(Line axis) {
    for (int i = 0; i < verticesCount(); ++i)
        vertices[i].reflex(axis);
}
void Polygon::scale(Point center, double k) {
    for (int i = 0; i < verticesCount(); ++i)
        vertices[i].scale(center, k);
}


////////////    ELLIPSE    ////////////
// x^2/a^2 + y^2/b^2 = 1;
class Ellipse : public Shape {
protected:
    double a = 1.0;
    double b = 1.0;
    pair<Point, Point> FF;
public:
    Ellipse() = default;
    Ellipse(const Point&, const Point&, double);
    ~Ellipse() = default;
    pair<Point,Point> focuses() const;
    pair<Line, Line> directrices() const;
    double eccentricity() const;
    Point center() const;

    double perimeter() const override;
    double area() const override;
    bool operator==(const Shape&) const override;
    bool operator!=(const Shape&) const override;
    bool isCongruentTo(const Shape&) const override;
    bool isSimilarTo(const Shape&) const override;
    bool containsPoint(Point point) const override;
    void rotate(Point center, double angle) override;
    void reflex(Point center) override;
    void reflex(Line axis) override;
    void scale(Point center, double k) override;
};
Ellipse::Ellipse(const Point& point1, const Point& point2, double sum_focal) {
    FF = make_pair(point1, point2);
    a = sum_focal / 2;
    b = sqrt(squaring(a) - squaring(points_distance(point1, point2) / 2));
}
pair<Point,Point> Ellipse::focuses() const {
    return FF;
}
pair<Line, Line> Ellipse::directrices() const {
    Line axis(FF.first, FF.second);
    std::pair <Line, Line> result {perpendicular(axis), perpendicular(axis)};
    result.first.c = a * a / sqrt(a * a - b * b) * sqrt(squaring(result.first.a) + squaring(result.first.b))
                     - result.first.a * center().x - result.first.b * center().y;
    result.second.c = - a * a / sqrt(a * a - b * b) * sqrt(squaring(result.second.a) + squaring(result.second.b))
                      - result.second.a * center().x - result.second.b * center().y;
    return result;
}
double Ellipse::eccentricity() const {
    return sqrt(1 - squaring(b)/squaring(a));
}
Point Ellipse::center() const {
    return middle_point(FF.first, FF.second);
}
double Ellipse::perimeter() const {
    return M_PI * (3 * (a + b) - sqrt((3 * a + b) * (a + 3 * b)));
}
double Ellipse::area() const {
    return M_PI * a * b;
}
bool Ellipse::operator==(const Shape& another) const {
    auto ellipse_ptr = dynamic_cast<const Ellipse*>(&another);
    if (!ellipse_ptr) return false;
    if ((FF.first == ellipse_ptr->FF.first && FF.second == ellipse_ptr->FF.second) && equal(a, ellipse_ptr->a))
        return true;
    return (FF.first == ellipse_ptr->FF.second && FF.second == ellipse_ptr->FF.first) && equal(a, ellipse_ptr->a);
}
bool Ellipse::operator!=(const Shape& another) const {
    return !(*this == another);
}
bool Ellipse::isCongruentTo(const Shape& another) const {
    auto ellipse_ptr = dynamic_cast<const Ellipse*>(&another);
    if (!ellipse_ptr) return false;
    return equal(points_distance(FF.first, FF.second), points_distance(ellipse_ptr->FF.first, ellipse_ptr->FF.first)) &&
           equal(a, ellipse_ptr->a);
}
bool Ellipse::isSimilarTo(const Shape& another) const {
    auto ellipse_ptr = dynamic_cast<const Ellipse*>(&another);
    if (!ellipse_ptr) return false;
    return equal(eccentricity(), ellipse_ptr->eccentricity());
}
bool Ellipse::containsPoint(Point point) const {
    double sum_focal = points_distance(FF.first, point) + points_distance(FF.second, point);
    return sum_focal < a * 2 || equal(sum_focal, 2 * a);
}
void Ellipse::rotate(Point center, double angle) {
    angle *= M_PI / 180 ;
    FF.first = FF.first.rotate(center, angle);
    FF.second = FF.second.rotate(center, angle);
}
void Ellipse::reflex(Point center) {
    scale(center, -1);
}
void Ellipse::reflex(Line axis) {
    FF.first.reflex(axis);
    FF.second.reflex(axis);
}
void Ellipse::scale(Point center, double k) {
    FF.first.scale(center, k);
    FF.second.scale(center, k);
    a *= abs(k);
    b *= abs(k);
}


////////////    CIRCLE    ////////////
class Circle : public Ellipse {
public:
    Circle() : Ellipse() {};
    Circle(const Point&, double);
    ~Circle() = default;
    double radius() const;
};
Circle::Circle(const Point& center, double radius) : Ellipse(center, center, 2 * radius) {}
double Circle::radius() const {
    return a;
}


////////////    RECTANGLE    ////////////
class Rectangle : public Polygon {
public:
    Rectangle() : Polygon() {};
    Rectangle(const Point&, const Point&, double k);
    ~Rectangle() = default;
    Point center() const;
    pair<Line, Line> diagonals() const;
};
Rectangle::Rectangle(const Point& point1, const Point& point2, double k) : Polygon{point1, Point(0, 0), point2, Point(0, 0)} {
    double angle = 180 - 2 * atan(k) * 180 / M_PI;
    Point middle = middle_point(point1, point2);
    vertices[1] = point1.rotate(middle, angle);
    vertices[3] = point2.rotate(middle, angle);
}
Point Rectangle::center() const {
    return middle_point(vertices[0], vertices[2]);
}
pair<Line, Line> Rectangle::diagonals() const {
    return {Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3])};
}


////////////    SQUARE    ////////////
class Square : public Rectangle {
public:
    Square() : Rectangle() {};
    Square(const Point&, const Point&);
    ~Square() = default;
    Circle circumscribedCircle() const;
    Circle inscribedCircle() const;
};
Square::Square(const Point& point1, const Point& point2) : Rectangle(point1, point2, 1) {}
Circle Square::circumscribedCircle() const {
    return Circle(middle_point(vertices[0], vertices[2]),
                  points_distance(vertices[0], vertices[2]) / 2);
}
Circle Square::inscribedCircle() const {
    return Circle(middle_point(vertices[0], vertices[2]),
                  points_distance(vertices[0], vertices[1]) / 2);
}

////////////    TRIANGLE    ////////////
class Triangle : public Polygon {
public:
    Triangle() : Polygon() {};
    Triangle(const Point&, const Point&, const Point&);
    ~Triangle() = default;
    Circle circumscribedCircle();
    Circle inscribedCircle();
    Point centroid();
    Point orthocenter();
    Line EulerLine();
    Circle ninePointsCircle();
};
Triangle::Triangle(const Point& point1, const Point& point2, const Point& point3) : Polygon{point1, point2, point3} {}
Circle Triangle::circumscribedCircle() {
    Line mp1 = middle_perpendicular(vertices[0], vertices[1]);
    Line mp2 = middle_perpendicular(vertices[1], vertices[2]);
    return Circle(cross(mp1, mp2), points_distance(cross(mp1, mp2), vertices[0]));
}
Circle Triangle::inscribedCircle() {
    double a = points_distance(vertices[1], vertices[2]);
    double b = points_distance(vertices[2], vertices[0]);
    double c = points_distance(vertices[0], vertices[1]);
    Point tmp;
    tmp.x = (vertices[0].x * a + vertices[1].x * b + vertices[2].x * c) / (a + b + c);
    tmp.y = (vertices[0].y * a + vertices[1].y * b + vertices[2].y * c) / (a + b + c);
    return Circle(tmp, 2 * area() / perimeter());
}
Point Triangle::centroid() {
    double x = (vertices[0].x + vertices[1].x + vertices[2].x) / 3;
    double y = (vertices[0].y + vertices[1].y + vertices[2].y) / 3;
    return Point(x, y);
}
Point Triangle::orthocenter() {
    Line ha = perpendicular(Line(vertices[0], vertices[1]));
    ha.c = -ha.a * vertices[2].x - ha.b * vertices[2].y;
    Line hb = perpendicular(Line(vertices[1], vertices[2]));
    hb.c = -hb.a * vertices[0].x - hb.b * vertices[0].y;
    return cross(ha, hb);
}
Line Triangle::EulerLine() {
    return Line(circumscribedCircle().center(), orthocenter());
}

Circle Triangle::ninePointsCircle() {
    Point center = middle_point(circumscribedCircle().center(), orthocenter());
    return Circle(center, circumscribedCircle().radius()/2);
}