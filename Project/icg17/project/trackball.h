#pragma once
#include "icg_helper.h"


using namespace glm;

class Trackball {
public:
    Trackball() : radius_(1.0f) {}

    // this function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void BeingDrag(float x, float y) {
        anchor_pos_ = vec3(x, y, 0.0f);
        ProjectOntoSurface(anchor_pos_);
    }

    // this function is called while the user moves the curser around while the
    // left mouse button is still pressed.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    // returns the rotation of the trackball in matrix form.
    mat4 Drag(float x, float y) {
        vec3 current_pos = vec3(x, y, 0.0f);
        ProjectOntoSurface(current_pos);

        // TODO 3 : Calculate the rotation given the projections of the anocher
        // point and the current position. The rotation axis is given by the cross
        // product of the two projected points, and the angle between them can be
        // used as the magnitude of the rotation.
        // you might want to scale the rotation magnitude by a scalar factor.
        // p.s. No need for using complicated quaternions as suggested inthe wiki
        // article.

        vec3 vector = cross(current_pos, anchor_pos_);
        float angle = acos(dot(current_pos, anchor_pos_)/(length(current_pos)*length(anchor_pos_)));

        mat4 rotationMatrix = rotate(IDENTITY_MATRIX, angle, vector);
        return rotationMatrix;
    }

private:
    // projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by 'radius_'.
    void ProjectOntoSurface(vec3& p) const {
        // TODO 2 DONE : Implement this function. Read above link for details.
        // Check hover inside sphere
        float x2 = p.x * p.x;
        float y2 = p.y * p.y;
        float radius2 = radius_*radius_;

        if( x2 + y2 <= (radius2/2.0)){
            // Formula inside sphere is z(x,y) = sqrt(r^2 - x^2 + y^2)
            p.z = sqrt(radius2 - (x2 + y2));
        }else{
            // Formula outside (hyperbolic) is r*r/2 / sqrt(x^2+x^2)
            p.z = (radius2/2.0f) / sqrt(x2+y2);
        }

        normalize(p);
    }

    float radius_;
    vec3 anchor_pos_;
    mat4 rotation_;
};
