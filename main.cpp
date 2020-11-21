#include <cstdio>
#include <cmath>
#include <vector>
#include <cstring>
#include <glm/glm.hpp>
#include <SDL.h>
#include <functional>
#include <unistd.h>

#define dims 2

namespace glm {
    typedef vec<dims,double,defaultp> vec_g;
}

const static double g_const = 6.67430E-11;

class object {
public:
    double mass;
    glm::vec_g velocity;
    glm::vec_g position;

    object(double massin, glm::vec_g pos) : mass(massin), position(pos) {;}
    object(double massin, glm::vec_g pos, glm::vec_g vel) : mass(massin), position(pos), velocity(vel) {;}

    void updateForce(glm::vec_g force) {
        velocity+=force/mass;
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
    glm::vec_g distance = glm::abs(obj2.position-obj1.position);
    glm::vec_g unitvec = (obj2.position-obj1.position)/distance;
    glm::vec_g force = -g_const*obj1.mass*obj2.mass/(distance*distance)*unitvec;
    obj2.updateForce(force);
}

int main(int argc, char** argv) {
    SDL_Rend rend{};
    rend.init(800);
    std::vector<object*> objs;
    object test1(5.97237*glm::pow(10,24),glm::vec_g(0, 0),glm::vec_g(0,0));
    object test2(7.342*glm::pow(10,22),glm::vec_g(384402*glm::pow(10,3),384402*glm::pow(10,3)),glm::vec_g(0,-10000));
    objs.emplace_back(&test1);
    objs.emplace_back(&test2);
    double Scale = glm::pow(10,6);
    bool quit = false;
    while(!quit) {
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
        gravitate(test1, test2);
        //gravitate(test2, test1);
        //test1.updatePos();
        test2.updatePos();
        //test1.printPos();
        //test2.printPos();
        rend.draw(objs, Scale);
        //usleep(1);
    }
}
