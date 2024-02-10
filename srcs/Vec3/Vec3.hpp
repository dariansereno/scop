#pragma once

class Vec3 {
  private:
    
  public:
    double _x;
    double _y;
    double _z;
    double _w;

    Vec3();
    Vec3(double x, double y, double z);
    Vec3(double x, double y);
    Vec3(double x, double y, double z, double w);
    Vec3(const double &oth);

    ~Vec3();

    void print(void);
};