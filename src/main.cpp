#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

constexpr int WIDTH = 1100;
constexpr int HEIGHT = 760;
constexpr float PI = 3.14159265358979323846f;

struct Vec3 {
    float x{}, y{}, z{};
    Vec3 operator+(const Vec3& b) const { return {x+b.x,y+b.y,z+b.z}; }
    Vec3 operator-(const Vec3& b) const { return {x-b.x,y-b.y,z-b.z}; }
    Vec3 operator*(float s) const { return {x*s,y*s,z*s}; }
};

static Vec3 cross(const Vec3& a, const Vec3& b) {
    return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
}
static float dot(const Vec3& a, const Vec3& b) {
    return a.x*b.x+a.y*b.y+a.z*b.z;
}
static float length(const Vec3& v) { return std::sqrt(dot(v,v)); }
static Vec3 normalize(const Vec3& v) {
    float l = length(v);
    return l > 1e-6f ? v*(1.0f/l) : Vec3{};
}

struct Mat4 {
    float m[4][4]{};
    static Mat4 identity() {
        Mat4 r{};
        for (int i=0;i<4;++i) r.m[i][i]=1.0f;
        return r;
    }
    static Mat4 translation(float x,float y,float z) {
        Mat4 r=identity(); r.m[0][3]=x; r.m[1][3]=y; r.m[2][3]=z; return r;
    }
    static Mat4 scale(float x,float y,float z) {
        Mat4 r=identity(); r.m[0][0]=x; r.m[1][1]=y; r.m[2][2]=z; return r;
    }
    static Mat4 rotateX(float a) {
        Mat4 r=identity(); float c=std::cos(a),s=std::sin(a);
        r.m[1][1]=c; r.m[1][2]=-s; r.m[2][1]=s; r.m[2][2]=c; return r;
    }
    static Mat4 rotateY(float a) {
        Mat4 r=identity(); float c=std::cos(a),s=std::sin(a);
        r.m[0][0]=c; r.m[0][2]=s; r.m[2][0]=-s; r.m[2][2]=c; return r;
    }
    static Mat4 rotateZ(float a) {
        Mat4 r=identity(); float c=std::cos(a),s=std::sin(a);
        r.m[0][0]=c; r.m[0][1]=-s; r.m[1][0]=s; r.m[1][1]=c; return r;
    }
    Vec3 transformPoint(const Vec3& v) const {
        float x=m[0][0]*v.x+m[0][1]*v.y+m[0][2]*v.z+m[0][3];
        float y=m[1][0]*v.x+m[1][1]*v.y+m[1][2]*v.z+m[1][3];
        float z=m[2][0]*v.x+m[2][1]*v.y+m[2][2]*v.z+m[2][3];
        float w=m[3][0]*v.x+m[3][1]*v.y+m[3][2]*v.z+m[3][3];
        if (std::abs(w)>1e-6f && std::abs(w-1.0f)>1e-6f) return {x/w,y/w,z/w};
        return {x,y,z};
    }
};
static Mat4 operator*(const Mat4& a,const Mat4& b) {
    Mat4 r{};
    for(int i=0;i<4;++i) for(int j=0;j<4;++j)
        for(int k=0;k<4;++k) r.m[i][j]+=a.m[i][k]*b.m[k][j];
    return r;
}

struct Color { uint8_t r,g,b; };
struct Vertex { Vec3 p; };
struct Triangle { int a,b,c; Color color; };
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};

static void addTriangle(Mesh& mesh, Vec3 a, Vec3 b, Vec3 c, Color color) {
    int base=static_cast<int>(mesh.vertices.size());
    mesh.vertices.push_back({a}); mesh.vertices.push_back({b}); mesh.vertices.push_back({c});
    mesh.triangles.push_back({base,base+1,base+2,color});
}

static void addCylinder(Mesh& mesh, Vec3 start, Vec3 end, float radius, Color color, int sides=10) {
    Vec3 axis=normalize(end-start);
    if(length(axis)<1e-5f) return;
    Vec3 helper = std::abs(axis.y)<0.9f ? Vec3{0,1,0} : Vec3{1,0,0};
    Vec3 u=normalize(cross(axis,helper))*radius;
    Vec3 v=normalize(cross(axis,u))*radius;
    for(int i=0;i<sides;++i) {
        float a0=2*PI*i/sides, a1=2*PI*(i+1)/sides;
        Vec3 p0=start+u*std::cos(a0)+v*std::sin(a0);
        Vec3 p1=start+u*std::cos(a1)+v*std::sin(a1);
        Vec3 q0=end+u*std::cos(a0)+v*std::sin(a0);
        Vec3 q1=end+u*std::cos(a1)+v*std::sin(a1);
        addTriangle(mesh,p0,q0,p1,color);
        addTriangle(mesh,p1,q0,q1,color);
        addTriangle(mesh,start,p1,p0,color);
        addTriangle(mesh,end,q0,q1,color);
    }
}

static void addSphere(Mesh& mesh, Vec3 center, float radius, Color color,
                      int rings=7, int sectors=10) {
    for(int i=0;i<rings;++i) {
        float p0=-PI/2.0f+PI*i/rings;
        float p1=-PI/2.0f+PI*(i+1)/rings;
        for(int j=0;j<sectors;++j) {
            float t0=2*PI*j/sectors, t1=2*PI*(j+1)/sectors;
            auto point=[&](float p,float t) {
                return center+Vec3{radius*std::cos(p)*std::cos(t),
                                   radius*std::sin(p),
                                   radius*std::cos(p)*std::sin(t)};
            };
            Vec3 a=point(p0,t0), b=point(p1,t0), c=point(p1,t1), d=point(p0,t1);
            addTriangle(mesh,a,b,d,color);
            addTriangle(mesh,b,c,d,color);
        }
    }
}

static Mesh createDNA() {
    Mesh mesh;
    constexpr int STEPS=34;
    constexpr float radius=1.55f;
    constexpr float rise=0.34f;
    constexpr float twist=2.0f*PI/10.5f;
    const Color backboneA{65,190,255}, backboneB{255,105,145};
    const Color baseColors[4]={{255,190,65},{90,220,145},{180,125,255},{255,105,105}};
    Vec3 previousA{}, previousB{};
    for(int i=0;i<STEPS;++i) {
        float angle=i*twist;
        float y=(i-(STEPS-1)*0.5f)*rise;
        Vec3 a{radius*std::cos(angle),y,radius*std::sin(angle)};
        Vec3 b{-radius*std::cos(angle),y,-radius*std::sin(angle)};
        if(i>0) {
            addCylinder(mesh,previousA,a,0.105f,backboneA,9);
            addCylinder(mesh,previousB,b,0.105f,backboneB,9);
        }
        addSphere(mesh,a,0.16f,backboneA,6,8);
        addSphere(mesh,b,0.16f,backboneB,6,8);
        Color pair=baseColors[i%4];
        addCylinder(mesh,a,b,0.055f,pair,8);
        addSphere(mesh,a+ (b-a)*0.27f,0.105f,pair,6,8);
        addSphere(mesh,a+ (b-a)*0.73f,0.105f,pair,6,8);
        previousA=a; previousB=b;
    }
    return mesh;
}


struct ScreenVertex { float x,y,z; };
class SoftwareRenderer {
public:
    std::vector<uint32_t> pixels;
    std::vector<float> depth;
    SoftwareRenderer() : pixels(WIDTH*HEIGHT,0xFF080D18), depth(WIDTH*HEIGHT,1e9f) {}

    void clear() {
        std::fill(pixels.begin(),pixels.end(),0xFF080D18);
        std::fill(depth.begin(),depth.end(),1e9f);
    }
    void putPixel(int x,int y,float z,Color c) {
        if(x<0||x>=WIDTH||y<0||y>=HEIGHT) return;
        int idx=y*WIDTH+x;
        if(z<depth[idx]) {
            depth[idx]=z;
            pixels[idx]=0xFF000000u|(uint32_t(c.r)<<16)|(uint32_t(c.g)<<8)|uint32_t(c.b);
        }
    }
    void drawTriangle(ScreenVertex a,ScreenVertex b,ScreenVertex c,Color color) {
        float area=(b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);
        if(std::abs(area)<0.001f) return;
        int minX=std::max(0,(int)std::floor(std::min({a.x,b.x,c.x})));
        int maxX=std::min(WIDTH-1,(int)std::ceil(std::max({a.x,b.x,c.x})));
        int minY=std::max(0,(int)std::floor(std::min({a.y,b.y,c.y})));
        int maxY=std::min(HEIGHT-1,(int)std::ceil(std::max({a.y,b.y,c.y})));
        for(int y=minY;y<=maxY;++y) for(int x=minX;x<=maxX;++x) {
            float px=x+0.5f, py=y+0.5f;
            float w0=((b.x-px)*(c.y-py)-(b.y-py)*(c.x-px))/area;
            float w1=((c.x-px)*(a.y-py)-(c.y-py)*(a.x-px))/area;
            float w2=1.0f-w0-w1;
            if(w0>=-0.001f&&w1>=-0.001f&&w2>=-0.001f) {
                float z=w0*a.z+w1*b.z+w2*c.z;
                putPixel(x,y,z,color);
            }
        }
    }
    bool project(const Vec3& p,ScreenVertex& out) {
        // Camera is at (0,0,-9), looking toward positive Z.
        float cameraZ=p.z+9.0f;
        if(cameraZ<0.2f) return false;
        constexpr float focal=720.0f;
        out.x=WIDTH*0.5f+(p.x*focal/cameraZ);
        out.y=HEIGHT*0.5f-(p.y*focal/cameraZ);
        out.z=cameraZ;
        return true;
    }
    void render(const Mesh& mesh,const Mat4& model) {
        for(const Triangle& tri:mesh.triangles) {
            Vec3 a=model.transformPoint(mesh.vertices[tri.a].p);
            Vec3 b=model.transformPoint(mesh.vertices[tri.b].p);
            Vec3 c=model.transformPoint(mesh.vertices[tri.c].p);
            Vec3 normal=normalize(cross(b-a,c-a));
            Vec3 center=(a+b+c)*(1.0f/3.0f);
            Vec3 view=normalize(Vec3{0,0,-9}-center);
            if(dot(normal,view)<=0.0f) continue; // Back-face culling.
            ScreenVertex sa,sb,sc;
            if(!project(a,sa)||!project(b,sb)||!project(c,sc)) continue;
            Vec3 light=normalize(Vec3{-0.5f,0.8f,-1.0f});
            float diffuse=std::max(0.0f,dot(normal,light));
            float intensity=0.30f+0.70f*diffuse;
            Color shaded{
                (uint8_t)std::min(255.0f,tri.color.r*intensity),
                (uint8_t)std::min(255.0f,tri.color.g*intensity),
                (uint8_t)std::min(255.0f,tri.color.b*intensity)
            };
            drawTriangle(sa,sb,sc,shaded);
        }
    }
};

int main(int argc,char** argv) {
    if(SDL_Init(SDL_INIT_VIDEO)!=0) {
        std::cerr<<"SDL_Init failed: "<<SDL_GetError()<<'\n';
        return 1;
    }
    SDL_Window* window=SDL_CreateWindow("3D DNA | CPU Software Renderer",
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_SHOWN);
    if(!window) {
        std::cerr<<"Window creation failed: "<<SDL_GetError()<<'\n';
        SDL_Quit(); return 1;
    }
    SDL_Renderer* display=SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    if(!display) {
        std::cerr<<"SDL software display creation failed: "<<SDL_GetError()<<'\n';
        SDL_DestroyWindow(window); SDL_Quit(); return 1;
    }
    SDL_Texture* texture=SDL_CreateTexture(display,SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,WIDTH,HEIGHT);
    if(!texture) {
        std::cerr<<"Texture creation failed: "<<SDL_GetError()<<'\n';
        SDL_DestroyRenderer(display); SDL_DestroyWindow(window); SDL_Quit(); return 1;
    }

    Mesh dna=createDNA();
    SoftwareRenderer rasterizer;
    float rx=0.18f, ry=0.0f, rz=0.0f;
    float tx=0.0f, ty=0.0f, scale=1.0f;
    bool running=true, animate=true;
    uint32_t last=SDL_GetTicks();

    std::cout<<"Controls: arrows rotate | W/A/S/D translate | +/- scale\\n"
             <<"X/Y/Z rotate axis | Space pause | R reset | Esc quit\\n";

    while(running) {
        uint32_t now=SDL_GetTicks();
        float dt=std::min(0.05f,(now-last)/1000.0f);
        last=now;
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type==SDL_QUIT) running=false;
            if(event.type==SDL_KEYDOWN) {
                SDL_Keycode key=event.key.keysym.sym;
                switch(key) {
                    case SDLK_ESCAPE: running=false; break;
                    case SDLK_SPACE: animate=!animate; break;
                    case SDLK_r: rx=0.18f; ry=rz=tx=ty=0; scale=1; break;
                    case SDLK_x: rx+=0.15f; break;
                    case SDLK_y: ry+=0.15f; break;
                    case SDLK_z: rz+=0.15f; break;
                    case SDLK_UP: rx-=0.12f; break;
                    case SDLK_DOWN: rx+=0.12f; break;
                    case SDLK_LEFT: ry-=0.12f; break;
                    case SDLK_RIGHT: ry+=0.12f; break;
                    case SDLK_w: ty+=0.25f; break;
                    case SDLK_s: ty-=0.25f; break;
                    case SDLK_a: tx-=0.25f; break;
                    case SDLK_d: tx+=0.25f; break;
                    case SDLK_EQUALS: case SDLK_PLUS: scale=std::min(2.5f,scale*1.1f); break;
                    case SDLK_MINUS: scale=std::max(0.4f,scale/1.1f); break;
                }
            }
        }
        if(animate) ry+=dt*0.55f;

        Mat4 model=Mat4::translation(tx,ty,0)*
                   Mat4::rotateZ(rz)*Mat4::rotateY(ry)*Mat4::rotateX(rx)*
                   Mat4::scale(scale,scale,scale);
        rasterizer.clear();
        rasterizer.render(dna,model);
        SDL_UpdateTexture(texture,nullptr,rasterizer.pixels.data(),WIDTH*sizeof(uint32_t));
        SDL_RenderClear(display);
        SDL_RenderCopy(display,texture,nullptr,nullptr);
        SDL_RenderPresent(display);
        SDL_Delay(1);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(display);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
