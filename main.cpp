#include <cstdio>
#include <cmath>
#include <vector>
#include <Tracy.hpp>
#include <glm/glm.hpp>
#include <SDL.h>

#define dims 2

namespace glm {
    typedef vec<dims,double,defaultp> vec_g;
}

const static double g_const = 6.67430E-11;
const static double speed = 1;
class object {
public:
    double mass;
    std::string name;
    glm::vec_g velocity;
    glm::vec_g position;

    object(double massin, glm::vec_g pos, const std::string& name) : mass(massin), position(pos), name(name) {;}
    object(double massin, glm::vec_g pos, glm::vec_g vel, const std::string& name) : mass(massin), position(pos), velocity(vel), name(name) {;}

    void updateForce(glm::vec_g force) {
        velocity+=(force*speed)/mass;
    }

    void updatePos() {
        position+=velocity;
    }

    void printPos() {
#if dims == 3
        printf("----Position-----\nx=%f\ny=%f\nz=%f\n",position.x,position.y,position.z);
#elif dims == 2
        printf("----Position-----\nx=%f\ny=%f\n",position.x,position.y);
#endif
    }

    void printVel() {
#if dims == 3
        printf("Velocity = x: %f y: %f z: %f\n",velocity.x,velocity.y,velocity.z);
#elif dims == 2
        printf("Velocity = x: %f y: %f\n",velocity.x,velocity.y);
#endif
    }
};

class SDL_Rend {
public:
    SDL_Renderer* sdlRenderer;
    SDL_Window* sdlWindow;
    void init(int size) {
        SDL_Init(SDL_INIT_VIDEO);
        sdlWindow = SDL_CreateWindow("GravityLaw",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,size,size,SDL_WINDOW_ALWAYS_ON_TOP);
        sdlRenderer = SDL_CreateRenderer(sdlWindow,-1,SDL_RENDERER_ACCELERATED);
    }
    void draw(const std::vector<object*>& objects,double _Scale) {
        SDL_Rect temp;
        SDL_SetRenderDrawColor(sdlRenderer,0,0,0,0);
        SDL_RenderClear(sdlRenderer);
        SDL_SetRenderDrawColor(sdlRenderer,255,255,255,0);
        for (auto obj : objects) {
            temp = SDL_Rect{(int)std::round(obj->position.x/_Scale)+390,
                            (int)std::round(obj->position.y/_Scale)+390,
                            20,
                            20
            };
            SDL_RenderDrawRect(sdlRenderer,&temp);
        }
        SDL_RenderPresent(sdlRenderer);
    }
};

void gravitate(object& obj1,object& obj2) {
    double distance = glm::distance(obj2.position,obj1.position);
    glm::vec_g unitvec = (obj2.position-obj1.position)/distance;
    glm::vec_g force = -g_const*obj1.mass*obj2.mass/(distance*distance)*unitvec;
    obj2.updateForce(force);
}

int main(int argc, char** argv) {
    SDL_Rend rend{};
    rend.init(800);
    std::vector<object*> objs;
    object test1(8*glm::pow(10,25),glm::vec_g(0, 0),glm::vec_g(0,0),"Earth");
    object test2(7.342*glm::pow(10,22),glm::vec_g(384402*glm::pow(10,3),0),glm::vec_g(-1000,-1000),"Moon");
    objs.emplace_back(&test1);
    objs.emplace_back(&test2);
    double Scale = glm::pow(10,6);
    bool quit = false;
    while(!quit) {
        {
            ZoneScopedNC("Event handling - SDL",0xff0000);
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_MOUSEWHEEL) {
                    if (event.wheel.y > 0) // scroll up
                    {
                        Scale -= glm::pow(10, 5.5);
                    } else if (event.wheel.y < 0) // scroll down
                    {
                        Scale += glm::pow(10, 5.5);
                    }
                }
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
            }
        }
        {
            ZoneScopedNC("Gravity Calculations",0x00ff00);
            gravitate(test1, test2);
            gravitate(test2, test1);
        }
        {
            ZoneScopedNC("Updating positions",0x00ff00);
            test1.updatePos();
            test2.updatePos();
        }
        //test1.printPos();
        //test2.printPos();
        {
            ZoneScopedNC("Draw objects to screen - SDL",0xff0000);
            rend.draw(objs, Scale);
        }
#if 0
        {
            ZoneScopedNC("Plot data - Tracy",0x0000ff);
            int i = 0;
            for (auto obj : objs) {
                TracyPlot((obj->name+" Velocity X").c_str(),obj->velocity.x);
                TracyPlot((obj->name+" Velocity Y").c_str(),obj->velocity.y);
#if dims==3
                TracyPlot((obj->name+" Velocity Z").c_str(),obj->velocity.z);
#endif
                i++;
            }
        }
#endif
        FrameMark;
        //usleep(1);
    }
}
