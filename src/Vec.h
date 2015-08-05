//
//  Vec.h
//  alf
//
//  Created by Guillaume GALES on 02/05/2014.
//  Copyright (c) 2014 Guillaume Gales. All rights reserved.
//

#ifndef __alf__Vec__
#define __alf__Vec__


#include <iostream>

namespace GG {
    
    /**
     * Defines a 3D vector (with an extra integer field).
     */
    class Vec {
    
    public:
        
        double x;
        double y;
        double z;
        
        int w;
        
        double u;
        double v;
        double alpha;
        
        //Constructors
        Vec():x(0), y(0), z(0), w(0) {}
        Vec(double a, double b, double c):x(a), y(b), z(c), w(0) {}
        Vec(double a, double b, double c, int d):x(a), y(b), z(c), w(d) {}
        Vec(float a, float b, float c):x((float)a), y((float)b), z((float)c), w(0) {}
        Vec(float a, float b, float c, int d):x((float)a), y((float)b), z((float)c), w(d) {}
        Vec(float* ptr, int offset):x((double)ptr[offset*3]), y((double)ptr[offset*3+1]), z((double)ptr[offset*3+2]), w(0) {}
        Vec(int a, int b, int c):x((double)a), y((double)b), z((double)c), w(0) {}
        Vec(int a, int b, int c, int d):x((double)a), y((double)b), z((double)c), w(d) {}
        
        friend std::ostream& operator<< (std::ostream &out, Vec &v){
            std::cout.precision(2);
            out << "[" << v.x << " " << v.y << " " << v.z << "]';"; return out;
            std::cout.precision(0);
        }
        
        static bool wComp(const Vec &u, const Vec &v){return u.w >= v.w;}
        struct vComp {
            bool operator() (const Vec &u, const Vec &v) const{
                return u.v<v.v;
            }
        };
        
        //Member functions
        double norm2(){ return (x*x)+(y*y)+(z*z);}
        double norm(){ return sqrt(norm2());}
        void unit(){ double n=norm(); x/=n; y/=n; z/=n; }
        
        //Static member functions
        static double dot(Vec a, Vec b){ return (a.x*b.x)+(a.y*b.y)+(a.z*b.z); }
        
        static Vec interRayPlane(Vec l0, Vec l, Vec P0, Vec n,
                                 double &d){
            Vec l0P0=P0-l0;
            d=dot(l0P0,n)/dot(l,n);
            return (d*l)+l0;
        }
        
        //Operator overloading
        friend Vec operator+(const Vec &u, const Vec &v)  {
            Vec result;
            result.x = u.x+v.x;
            result.y = u.y+v.y;
            result.z = u.z+v.z;
            return result;
        }
        
        friend Vec operator- (const Vec &u, const Vec &v)  {
            Vec result;
            result.x = u.x-v.x;
            result.y = u.y-v.y;
            result.z = u.z-v.z;
            return result;
        }
        
        friend Vec operator*(const Vec &v, const double &s)  {
            Vec result;
            result.x = v.x*s;
            result.y = v.y*s;
            result.z = v.z*s;
            return result;
        }
        friend Vec operator*(const double &s, const Vec &v)  {
            return v*s;
        }
        
        
        
    }; //end class
    
    
    
} //end namepsace

#endif /* defined(__alf__Vec__) */