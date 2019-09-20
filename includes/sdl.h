/* I wrote this header to be as portable as possible for other homebrew switch projects */
/* The only thing that needs to be changed will be the name / number of textures you want to load */
/* If you decide to use this header and add functions, please consider opening a pr for said functions */
/* I would greatly appreaciate it :) */

#ifndef _SDL_MEMES_H_
#define _SDL_MEMES_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_W    1280                                                            //width of the window
#define SCREEN_H    720                                                             //hight of the window

#define white       0
#define grey        1
#define black       2
#define pink        3
#define orange      4
#define yellow      5
#define brown       6
#define red         7
#define green       8
#define light_blue  9
#define blue        10
#define dark_blue   11
#define purple      12
#define dark_grey   13
#define jordy_blue  14
#define faint_blue  15

// buttons from NintendoExt
#define BUTTON_A            0xE0E0
#define BUTTON_B            0xE0E1
#define BUTTON_X            0xE0E2
#define BUTTON_Y            0xE0E3
#define BUTTON_L            0xE0E4
#define BUTTON_R            0xE0E5
#define BUTTON_ZL           0xE0E6
#define BUTTON_ZR           0xE0E7
#define BUTTON_SL           0xE0E8
#define BUTTON_SR           0xE0E9
#define BUTTON_UP           0xE0EB
#define BUTTON_DOWN         0xE0EC
#define BUTTON_LEFT         0xE0ED
#define BUTTON_RIGHT        0xE0EE
#define BUTTON_PLUS         0xE0EF
#define BUTTON_MINUS        0xE0F0
#define BUTTON_HOME         0xE0F4
#define BUTTON_SCREENSSHOT  0xE0F5
#define BUTTON_LS           0xE101
#define BUTTON_RS           0xE102
#define BUTTON_L3           0xE104
#define BUTTON_R3           0xE105

TTF_Font *fntSmall, *fntMedium, *fntLarge, *fntButton, *fntButtonBig;
SDL_Texture *background, *app_icon, *ams_icon, *ams_plus_icon, *reboot_icon, *hekate_icon, *error_icon;


SDL_Colour SDL_GetColour(int colour_option);                                        //pass the name of colour, returns the colour
SDL_Window* SDL_GetWindow(void);                                                    //get sdl window

void clearRenderer(void);                                                           //clear the screen
void updateRenderer(void);                                                          //update the screen

void imageLoad(SDL_Texture **texture, char *path);                                  //load image from texture
void imageLoadMem(SDL_Texture **texture, void *data, int size);                     //load image from memory

void drawText(TTF_Font *font, int x, int y, SDL_Color colour, const char *text);    //draw text to screen
void drawButton(TTF_Font *font, u_int16_t btn, int x, int y, SDL_Colour colour);    //draw button to screen
void drawImage(SDL_Texture *texture, int x, int y);                                 //draw image to screen from texture
void drawImageScale(SDL_Texture *texture, int x, int y, int w, int h);              //scale the image drawn to screen
void drawShape(SDL_Colour colour, int x, int y, int w, int h);                      //draw shap (rect)

void loadFonts(void);                                                               //load all fonts
void loadTextures(void);                                                            //load all textures
void destroyTextures(void);                                                         //destroy all textures

void sdlInit();                                                                     //init all sdl stuff
void sdlExit();                                                                     //clean and exit

#endif