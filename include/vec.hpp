#pragma once
#include <cmath>

struct Vec2;
struct Vec3;
struct Vec4;

struct Vec2 {
    float x, y;

    Vec2 operator+(float o) const;
    Vec2 operator+(const Vec2 o) const;
    Vec2 operator-(float o) const;
    Vec2 operator-(const Vec2 o) const;
    Vec2 operator*(float o) const;
    float operator*(const Vec2 o) const;

    Vec2 &operator+=(float o);
    Vec2 &operator+=(const Vec2 o);
    Vec2 &operator-=(float o);
    Vec2 &operator-=(const Vec2 o);
    Vec2 &operator*=(float o);

    Vec3 cross(const Vec2 o) const;
    float magnitude() const;
    float perspDivide() const;
    Vec3 normalize() const;
    operator Vec3() const;
    operator Vec4() const;
    Vec2 normal(const Vec2 o) const;
};

struct Vec3 {
    float x, y, z;
    Vec3 operator+(float o) const;
    Vec3 operator+(const Vec3 o) const;
    Vec3 operator-(float o) const;
    Vec3 operator-(const Vec3 o) const;
    Vec3 operator*(float o) const;
    float operator*(const Vec3 o) const;

    Vec3 &operator+=(float o);
    Vec3 &operator+=(const Vec3 o);
    Vec3 &operator-=(float o);
    Vec3 &operator-=(const Vec3 o);
    Vec3 &operator*=(float o);

    Vec3 cross(const Vec3 o) const;
    float magnitude() const;
    Vec2 perspDivide() const;
    Vec4 normalize() const;
    operator Vec2() const;
    operator Vec4() const;
    Vec3 normal(const Vec3 o1, const Vec3 o2) const;
};

struct Vec4 {
    float x, y, z, w;
    Vec4 operator+(float o) const;
    Vec4 operator+(const Vec4 o) const;
    Vec4 operator-(float o) const;
    Vec4 operator-(const Vec4 o) const;
    Vec4 operator*(float o) const;
    float operator*(const Vec4 o) const;

    Vec4 &operator+=(float o);
    Vec4 &operator+=(const Vec4 o);
    Vec4 &operator-=(float o);
    Vec4 &operator-=(const Vec4 o);
    Vec4 &operator*=(float o);

    Vec4 cross(const Vec4 o) const;
    float magnitude() const;
    Vec3 perspDivide() const;
    operator Vec2() const;
    operator Vec3() const;
    Vec4 normal(const Vec4 o1, const Vec4 o2) const;
};
