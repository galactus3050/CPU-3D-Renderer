#include <limits>
#include "model.h"
#include "our_gl.h"

extern mat<4,4> ModelView;
extern mat<4,4> Projection;

struct Shader : IShader {
    const Model &model;
    vec3 uniform_l;
    mat<3,2> varying_uv;
    mat<3,3> varying_nrm;
    mat<3,3> view_tri;

    Shader(const vec3 l, const Model &m) : model(m) {
        uniform_l = normalized((ModelView*vec4{l.x, l.y, l.z, 0.}).xyz());
    }

    virtual void vertex(const int iface, const int nthvert, vec4& gl_Position) {
        vec3 n = model.normal(iface, nthvert);
        vec3 v = model.vert(iface, nthvert);
        gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        varying_uv[nthvert]  = model.uv(iface, nthvert);
        varying_nrm[nthvert] = (ModelView.invert_transpose() * vec4{n.x, n.y, n.z, 0.}).xyz();
        view_tri[nthvert]    = gl_Position.xyz();
        gl_Position = Projection * gl_Position;
    }

    virtual bool fragment(const vec3 bar, TGAColor &gl_FragColor) const {
        vec3 bn = normalized(bar * varying_nrm);
        vec2 uv = bar * varying_uv;

        mat<3,3> AI = mat<3,3>{ {view_tri[1] - view_tri[0], view_tri[2] - view_tri[0], bn} }.invert();
        vec3 i = AI * vec3{varying_uv[1].x - varying_uv[0].x, varying_uv[2].x - varying_uv[0].x, 0};
        vec3 j = AI * vec3{varying_uv[1].y - varying_uv[0].y, varying_uv[2].y - varying_uv[0].y, 0};
        mat<3,3> B = mat<3,3>{ { normalized(i), normalized(j), bn } }.transpose();

        vec3 n = normalized(B * model.normal(uv));
        vec3 r = normalized(n * (n * uniform_l)*2 - uniform_l);
        double diff = std::max(0., n * uniform_l);
        double spec = std::pow(std::max(-r.z, 0.), 5+sample2D(model.specular(), uv)[0]);

        TGAColor c = sample2D(model.diffuse(), uv);
        for (int i : {0,1,2})
            gl_FragColor[i] = std::min<int>(10 + c[i]*(diff + spec), 255);
        return false;
    }
};

int main(int argc, char** argv) {
    if (2>argc) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width  = 800;
    constexpr int height = 800;
    constexpr vec3 light_dir{1,1,1};
    constexpr vec3       eye{1,1,3};
    constexpr vec3    center{0,0,0};
    constexpr vec3        up{0,1,0};

    lookat(eye, center, up);
    viewport(width/8, height/8, width*3/4, height*3/4);
    projection(norm(eye-center));
    std::vector<double> zbuffer(width*height, std::numeric_limits<double>::max());

    TGAImage framebuffer(width, height, TGAImage::RGB);
    for (int m=1; m<argc; m++) {
        Model model(argv[m]);
        Shader shader(light_dir, model);
        for (int t=0; t<model.nfaces(); t++) {
            vec4 clip_vert[3];
            for (int v : {0,1,2})
                shader.vertex(t, v, clip_vert[v]);
            rasterize(clip_vert, shader, framebuffer, zbuffer);
        }
    }
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
