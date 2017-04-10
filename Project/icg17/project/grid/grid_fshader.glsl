#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform vec3 light_pos;
uniform float time;
uniform sampler2D tex;

in vec2 uv;
in vec4 vpoint_mv;

out vec3 color;

void main() {

    //color = vec3(1,1,1);
    color = vec3(0,0,0);

    vec3 x = dFdx(vpoint_mv).xyz;
    vec3 y = dFdy(vpoint_mv).xyz;
    vec3 normal_mv = cross(x,y);
    vec3 light_dir = light_pos - vpoint_mv.xyz;
    vec3 view_dir = -vpoint_mv.xyz;

    // ambient term
    vec3 ambient = ka * La;
    color += ambient;

    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);

    if(lambert > 0.0) {
        // diffuse term
        vec3 diffuse = Ld*kd*lambert;
        color += diffuse;

        // specular term
        vec3 v = normalize(view_dir);
        vec3 reflect = reflect(-l, n);
        vec3 specular = Ls*ks*pow(max(dot(reflect,v), 0.0), alpha);
        color += specular;
    }

    //color = vpoint_mv.xyz;
}
