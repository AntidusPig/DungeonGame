#include<iostream>

#ifndef V2DI_H
#define V2DI_H

class v2di{ // 2d vector for integers
        public:
            int x;
            int y;
            v2di():x(0),y(0){}
            v2di(int x, int y):x(x),y(y){}
            v2di(const v2di& other):x(other.x),y(other.y){}
            v2di& operator=(const v2di& other){
                if (this == &other){return *this;}
                x = other.x;
                y = other.y;
                return *this;
            }
            v2di& operator+=(const v2di& other){
                x += other.x;
                y += other.y;
                return *this;
            }
            v2di& operator-=(const v2di& other){
                x -= other.x;
                y -= other.y;
                return *this;
            }
            v2di& operator*=(const int other){
                x *= other;
                y *= other;
                return *this;
            }
            bool adjacent(const v2di& other) const{
                int i=abs(this->x-other.x);
                int j =abs(other.y-this->y);
                return i<=1 && j<=1;
            }
    };
    bool operator==(const v2di& obj, const v2di& other){
        return (obj.x == other.x)&&(obj.y==other.y);
    }
    v2di operator+(const v2di& obj, const v2di& other){
                v2di res;
                res.x = obj.x+other.x;
                res.y = obj.y+other.y;
                return res;
            }
    v2di operator-(const v2di& obj, const v2di& other){
                v2di res;
                res.x = obj.x-other.x;
                res.y = obj.y-other.y;
                return res;
            }
    v2di operator*(const v2di& obj, const int other){
                v2di res;
                res.x = obj.x*other;
                res.y = obj.y*other;
                return res;
            }
    v2di operator/(const v2di& obj, const int other){
                v2di res;
                res.x = obj.x/other;
                res.y = obj.y/other;
                return res;
            }
    std::wostream& operator<<(std::wostream& wos, const v2di& obj){
        wos << "[ " << obj.x << ", " << obj.y << " ]";
        return wos;
    }

    std::wistream& operator>>(std::wistream& wis, v2di& obj){
        if (wis >> obj.x >> obj.y){
            return wis;
        }else{
            wis.setstate(std::ios::failbit);
            return wis;
        }
    }

#endif
