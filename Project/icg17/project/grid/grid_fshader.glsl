#version 330
uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform vec3 light_pos;
uniform sampler2D tex;
uniform float time;

in vec2 uv;
in vec4 vpoint_mv;
in mat4 MV;

out vec3 color;

void main() {

    // PHONG SHADING
    color = vec3(0,0,0);

    // compute normal : compute
    float delta = 0.03; // how much we want to be precize : distance between 2 points we take to compute gradient
    // vectors of difference of elevation (r coordinate of texture) between 2 points
    float xz = texture(tex, uv + vec2(delta/2.0, 0)).r - texture(tex, uv - vec2(delta/2.0, 0)).r;
    float yz = texture(tex, uv + vec2(0, delta/2.0)).r - texture(tex, uv - vec2(0, delta/2.0)).r;
    vec3 x = vec3(delta, 0, xz);
    vec3 y = vec3(0, delta, yz);
    vec3 normal = normalize(cross(x,y));

    vec3 normal_mv = vec3(MV * vec4(normal, 0));
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
}
