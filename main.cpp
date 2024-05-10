#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <fstream>

#undef main
#define SIN 0
#define COS 1
#define PI 3.14159f

int GCD(int a, int b) {

    if(a == 0)
        return b;
    if(b == 0 || a == b)
        return a;
    if(a > b)
        return GCD(a - b, b);
    return GCD(a, b - a);

}

SDL_Surface *OptimizedSurface(SDL_Surface *windowSurface, std::string filePath)
{

    return SDL_ConvertSurface(IMG_Load(filePath.c_str()), windowSurface->format, 0);
}

float Module(float num1, float num2)
{

    if (num1 < 0)
    {
        while (num1 < 0)
        {
            num1 += num2;
        }
    }
    else
    {
        while (num1 >= num2)
        {
            num1 -= num2;
        }
    }

    return num1;
}

void FormatAngle(float *angle) //Making angle become >= 0 and < 360
{

    *angle = Module(*angle, 360.0f);
}

SDL_Texture *LoadTexture(SDL_Renderer *renderer, std::string filePath)
{

    return SDL_CreateTextureFromSurface(renderer, IMG_Load(filePath.c_str()));
}

float degreeTrig(float angle, int mode) //Calculates sin and cos in degrees 
{

    float radAngle = angle * PI / 180.0f;

    if (mode)
        return cos(radAngle);
    return sin(radAngle);
}

int MAINLENGTH = 140;
int LENGTHRATIO1;
int LENGTHRATIO2;
int STICKWIDTH = 10;
int MAINSPEED = 250;
const int TILESIZE = 3;
const int WIDTH = 1920;
const int HEIGHT = 1080;
int RATIO1;
int RATIO2;
int frames = 0;

int main() {

    SDL_Init(SDL_INIT_VIDEO);
    int imgFlags = IMG_INIT_PNG;
    IMG_Init(imgFlags);
    
    std::fstream input;
    input.open("INPUT.txt", std::ios::in);
    if(input.is_open()) {
	input >> LENGTHRATIO1 >> LENGTHRATIO2 >> RATIO1 >> RATIO2;
        input.close();
    } else SDL_Quit(); //Reads data from INPUT.TXT

    SDL_Window *window = SDL_CreateWindow("ADHD Cured", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *mainRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_Rect imgRect, stick1, stick2;
    SDL_Texture *tileTexture = LoadTexture(mainRenderer, "sprites/tile.png");
    SDL_Texture *stickTexture = LoadTexture(mainRenderer, "sprites/stick.png");
    SDL_Surface *windowSurface = SDL_GetWindowSurface(window);
    SDL_Texture *wholeImage = SDL_CreateTexture(mainRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    SDL_Point rotationPoint;
    rotationPoint.x = STICKWIDTH / 2;
    rotationPoint.y = 0;
    stick1.w = stick2.w = STICKWIDTH;
    stick1.h = MAINLENGTH;
    stick2.h = MAINLENGTH / LENGTHRATIO1 * LENGTHRATIO2;
    stick1.x = WIDTH / 2 - STICKWIDTH / 2;
    stick1.y = HEIGHT / 2;
    stick2.x = degreeTrig(0, COS) * MAINLENGTH + WIDTH / 2 - STICKWIDTH / 2;
    stick2.y = degreeTrig(0, SIN) * MAINLENGTH + HEIGHT / 2;

    bool gameLoop = true;
    bool drawActive = true;
    imgRect.x = MAINLENGTH * (LENGTHRATIO1 / LENGTHRATIO2 + 1);
    imgRect.y = 0;
    imgRect.w = imgRect.h = TILESIZE;
    float deltaTime, currAngle1, currAngle2, prevAngle;
    currAngle1 = 0;
    currAngle2 = 0;
    int prevTime;
    int currTime = SDL_GetTicks();
    float FPS;
    bool badFPS = false;
    int turnsCount = 0;
    SDL_Event ev; //Setting everything up

    SDL_Delay(1000);
    int startTicks = SDL_GetTicks();

    while (gameLoop)
    {
        prevTime = currTime;
        currTime = SDL_GetTicks() - startTicks;
        deltaTime = (currTime - prevTime) / 1000.0f;
        FPS = 1 / deltaTime; //Calculating deltatime and FPS

        while (SDL_PollEvent(&ev))
        {

            if (ev.type == SDL_QUIT)
                gameLoop = false; //Checking for X button press

        }

        
        SDL_RenderClear(mainRenderer);

        SDL_SetRenderTarget(mainRenderer, wholeImage);
        SDL_RenderCopy(mainRenderer, tileTexture, NULL, &imgRect);
        SDL_SetRenderTarget(mainRenderer, NULL); //Rendering new tile on wholeTexture

        SDL_RenderCopy(mainRenderer, wholeImage, NULL, NULL);
        SDL_RenderCopyEx(mainRenderer, stickTexture, NULL, &stick1, currAngle1 - 90, &rotationPoint, SDL_FLIP_NONE);
        SDL_RenderCopyEx(mainRenderer, stickTexture, NULL, &stick2, currAngle2 - 90, &rotationPoint, SDL_FLIP_NONE);
        SDL_RenderPresent(mainRenderer); //Rendering everything
        prevAngle = currAngle1;
        imgRect.x = degreeTrig(currAngle1, COS) * MAINLENGTH + degreeTrig(currAngle2, COS) * MAINLENGTH / LENGTHRATIO1 * LENGTHRATIO2 + WIDTH / 2;
        imgRect.y = degreeTrig(currAngle1, SIN) * MAINLENGTH + degreeTrig(currAngle2, SIN) * MAINLENGTH / LENGTHRATIO1 * LENGTHRATIO2 + HEIGHT / 2; //Changing image position
	
        if (turnsCount < abs(RATIO1) / GCD(abs(RATIO1), abs(RATIO2))) //Checking if drawing ended
        {
            currAngle1 = float(MAINSPEED * currTime) / 1000.0f;
            currAngle2 = float(MAINSPEED) / RATIO1 * RATIO2 * currTime / 1000.0f;
        }
        else
        {
            currAngle1 = currAngle2 = 0.0f;
        }
        FormatAngle(&currAngle1);
        FormatAngle(&currAngle2); //Changing angle
        if (std::abs(currAngle1 - prevAngle) > 180)
            turnsCount++;
        stick2.x = degreeTrig(currAngle1, COS) * MAINLENGTH + WIDTH / 2 - STICKWIDTH / 2;
        stick2.y = degreeTrig(currAngle1, SIN) * MAINLENGTH + HEIGHT / 2;
    }

    SDL_DestroyWindow(window);
    window = nullptr;
    SDL_Quit(); //Quitting SDL
}
