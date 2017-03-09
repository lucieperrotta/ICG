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

        float angle = acos(dot(current_pos, anchor_pos_)/(current_pos.length()*anchor_pos_.length()));

        mat4 rotation = IDENTITY_MATRIX;
        rotation[0][0] = cos(angle);
        rotation[1][0] = -sin(angle);
        rotation[0][1] = sin(angle);
        rotation[1][1] = cos(angle);

        // TODO 3: Calculate the rotation given the projections of the anocher
        // point and the current position. The rotation axis is given by the cross
        // product of the two projected points, and the angle between them can be
        // used as the magnitude of the rotation.
        // you might want to scale the rotation magnitude by a scalar factor.
        // p.s. No need for using complicated quaternions as suggested inthe wiki
        // article.
        return rotation;
    }

private:
    // projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by 'radius_'.
    void ProjectOntoSurface(vec3& p) const {
        // TODO 2 DONE : Implement this function. Read above link for details.
        // Formula inside sphere is z(x,y) = sqrt(r^2 - x^2 + y^2)
        if(p[0] <= 1 && p[0] >= -1 && p[1] <= 1 && p[1] >= -1){
            p[2] = sqrt(1 - (p[0]*p[0] + p[1]*p[1]));
        }else{
            // Formula outside (hyperbolic) is r*r/2 / sqrt(x^2+x^2)
            p[2] = 1/(2*sqrt(p[0]*p[0] + p[1]*p[1]));
        }
    }

    float radius_;
    vec3 anchor_pos_;
    mat4 rotation_;
};
