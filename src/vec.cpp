#include <vec.hpp>

Vec2 Vec2::operator+(float o) const {return {x+o, y+o};}
Vec2 Vec2::operator+(const Vec2 o) const {return {x+o.x, y+o.y};}
Vec2 Vec2::operator-(float o) const {return {x-o, y-o};}
Vec2 Vec2::operator-(const Vec2 o) const {return {x-o.x, y-o.y};}
Vec2 Vec2::operator*(float o) const {return {x*o, y*o};}
float Vec2::operator*(const Vec2 o) const {return x*o.x + y*o.y;}

Vec2 &Vec2::operator+=(float o) {*this = *this + o; return *this;}
Vec2 &Vec2::operator+=(const Vec2 o) {*this = *this + o; return *this;}
Vec2 &Vec2::operator-=(float o) {*this = *this - o; return *this;}
Vec2 &Vec2::operator-=(const Vec2 o) {*this = *this - o; return *this;}
Vec2 &Vec2::operator*=(float o) {*this = *this * o; return *this;}

Vec3 Vec2::cross(const Vec2 o) const {
    return {0.0f, 0.0f, x*o.y - y *o.x};
}
float Vec2::magnitude() const {
    return std::sqrt(*this * *this);
}
float Vec2::perspDivide() const {
    return x / y;
}
Vec3 Vec2::normalize() const {
    auto factor = magnitude();
    return {x/factor, y/factor, 1/factor};
}
Vec2::operator Vec3() const {
    return {x, y, 0.0f};
}
Vec2::operator Vec4() const {
    return (Vec4)((Vec3)*this);
}
Vec2 Vec2::normal(const Vec2 o) const {
    float dx = o.x - x;
    float dy = o.y - y;
    return {-dy, dx};
}

Vec3 Vec3::operator+(float o) const {return {x+o, y+o, z+o};}
Vec3 Vec3::operator+(const Vec3 o) const {return {x+o.x, y+o.y, z+o.z};}
Vec3 Vec3::operator-(float o) const {return {x-o, y-o, z-o};}
Vec3 Vec3::operator-(const Vec3 o) const {return {x-o.x, y-o.y, z-o.z};}
Vec3 Vec3::operator*(float o) const {return {x*o, y*o, z*o};}
float Vec3::operator*(const Vec3 o) const {return x*o.x + y*o.y + z*o.z;}

Vec3 &Vec3::operator+=(float o) {*this = *this + o; return *this;}
Vec3 &Vec3::operator+=(const Vec3 o) {*this = *this + o; return *this;}
Vec3 &Vec3::operator-=(float o) {*this = *this - o; return *this;}
Vec3 &Vec3::operator-=(const Vec3 o) {*this = *this - o; return *this;}
Vec3 &Vec3::operator*=(float o) {*this = *this * o; return *this;}

Vec3 Vec3::cross(const Vec3 o) const {
    return {
        y * o.z - z * o.y,
        z * o.x - x * o.z,
        x * o.y - y * o.x
    };
}
float Vec3::magnitude() const {
    return std::sqrt(*this * *this);
}
Vec2 Vec3::perspDivide() const {
    return {x/z, y/z};
}
Vec4 Vec3::normalize() const {
    auto factor = magnitude();
    return {x/factor, y/factor, z/factor, 1/factor};
}
Vec3::operator Vec2() const {
    return perspDivide();
}
Vec3::operator Vec4() const {
    return normalize();
}
Vec3 Vec3::normal(const Vec3 o1, const Vec3 o2) const {
    return (o1 - *this).cross(o2 - *this);
}

Vec4 Vec4::operator+(float o) const {return (perspDivide() + o).normalize();}
Vec4 Vec4::operator+(const Vec4 o) const {return (perspDivide() + o.perspDivide()).normalize();}
Vec4 Vec4::operator-(float o) const {return (perspDivide() - o).normalize();}
Vec4 Vec4::operator-(const Vec4 o) const {return (perspDivide() - o.perspDivide()).normalize();}
Vec4 Vec4::operator*(float o) const {return (perspDivide() * o).normalize();}
float Vec4::operator*(const Vec4 o) const {return perspDivide() * o.perspDivide();}

Vec4 &Vec4::operator+=(float o) {*this = *this + o; return *this;}
Vec4 &Vec4::operator+=(const Vec4 o) {*this = *this + o; return *this;}
Vec4 &Vec4::operator-=(float o) {*this = *this - o; return *this;}
Vec4 &Vec4::operator-=(const Vec4 o) {*this = *this - o; return *this;}
Vec4 &Vec4::operator*=(float o) {*this = *this * o; return *this;}

Vec4 Vec4::cross(const Vec4 o) const {
    return perspDivide().cross(o.perspDivide()).normalize();
}
float Vec4::magnitude() const {
    return std::sqrt(*this * *this);
}
Vec3 Vec4::perspDivide() const {
    return {x/w, y/w, z/w};
}
Vec4::operator Vec2() const {
    return (Vec2)((Vec3)*this);
}
Vec4::operator Vec3() const {
    return perspDivide();
}
Vec4 Vec4::normal(const Vec4 o1, const Vec4 o2) const {
    return perspDivide().normal(o1.perspDivide(), o2.perspDivide()).normalize();
}
