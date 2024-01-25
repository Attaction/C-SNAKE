#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "debugmalloc.h"
#include "struct.h"
#include "start.h"
#include "food.h"
#include "snake.h"

//Definialjuk az iranyokat intekkent.
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//Definialjuk a jatek alap szelesseget es magassagat.
#define WIDTH 800
#define HEIGHT 600

//Megadjuk a jatek menuinek a valtozoit.
bool game_over = false;
bool game_close = false;

//A feher szin meghatarozasa.
SDL_Color White = {255, 255, 255};

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;

//Letrehozzuk az ablakot es a benne megjeleno renderert
void sdl_init(char const *felirat, int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer, TTF_Font **pfont) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(felirat, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);
    
    TTF_Init();

    *pfont = font;
    *pwindow = window;
    *prenderer = renderer;
}

void startup(struct Snake *snake, struct Food *food) {
    //Letrehozzuk a jatek alapallapotat, azaz a kigyot berakjuk a palya kozepere es lerakunk egy kajat neki.
    snake->head = newNode();
    snake->head->x = WIDTH / 2;
    snake->head->y = HEIGHT / 2;
    snake->head->size = 10;
    snake->length = 1;
    snake->direction = UP;
    snake->tail = snake->head;
    //Lerakunk a palyara egy kajat.
    newFood(snake, food);
    boxRGBA(renderer, snake->head->x, snake->head->y, snake->head->x + snake->head->size, snake->head->y + snake->head->size, 0, 255, 0, 255);
}

//Elmentjuk a rekordokat egy kulon max.txt nevu fileba
void IntoFile(int score) {
    FILE *fp;
    //Megnyitjuk a max.txt filet append modeban, hogy hozza tudjunk irni a regi eredmenyek utan.
    fp = fopen("max.txt", "a");
    if (fp == NULL) {SDL_Log("Nem sikerult megnyitni a fajlt."); exit(1);}
    time_t t = time(NULL);
    //Letrehozzuk a jelenlegi idot egy strukturaban.
    struct tm tm = *localtime(&t);
    //Ido alapjan eltaroljuk az elert pontszamot es egy uj sort nyitunk, hogy a kovektezo eredmeny bekeruljon oda.
    fprintf(fp, "%d-%02d-%02d %02d:%02d SCORE : ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
    fprintf(fp, "%d\n", score);
    //Bezarjuk a megnyitott filet.
    fclose(fp);
}

//A jatek fo iranyito fule.
void gameLoop(int highscore) {
    //Letoroljuk a kepernyot es feketere szinezzuk.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    //Letrehozzul a kigyot es a kajat.
    Snake snake;
    Food food;

    //Meghivjuk az alaphelyzetbe allito fuggvenyt.
    startup(&snake, &food);
    SDL_RenderPresent(renderer);
    
    //Letrehozunk egy event valtozot.
    SDL_Event event;

    //A jatekunk game_over-ig folyamatosan fut.
    
    while (!game_over && !game_close) {
        //Bekerunk egy eventet az event valtozonkba.
        SDL_PollEvent(&event);

        //Ameddig nem kapunk egy keyboard eventet, addig a kigyonk folyamatosan mozog az elore megadott iranyba.
        while (event.key.type != SDL_KEYDOWN && !game_over) {
            snakeUpdate(&snake, &food, game_over);
            if(hitWall(&snake)) {game_over = true;}
            if(snake.length > 1) {
                if (hitTail(&snake, game_over)) {
                    game_over = true;
                }
            }
            //Letrehozunk egy formalt outputot, amiben a kigyo aktualis hossza van. Az ablak nevet atirjuk, hogy benne legyen a kigyonk hossza.
            char score[256];
            sprintf(score, "SNAKE | SCORE: %i", snake.length);
            SDL_SetWindowTitle(window, score);
            SDL_RenderPresent(renderer);
            SDL_PollEvent(&event);
            SDL_Delay(150);
        }

        //Amennyiben kapunk keyboardeventet, megnezzuk melyik az.
        switch (event.key.keysym.sym) {
            //Q megnyomasara kilepunk a jatekbol.
            case SDLK_q:
                if (snake.length > highscore) {highscore = snake.length;}
                IntoFile(highscore);
                game_close = true;
                break;
            //A nyilak megnyomasara megnezzuk, hogy nem az ellenkezo iranyba megy-e a kigyonk. Ha arra megy, akkor a kigyonk meghal es a jatek veget er.
            //Ha nem az ellenkezo iranyba megy, akkor megvaltoztatjuk az iranyat a kigyonknak.
            case SDLK_UP:
                if (snake.direction == DOWN) {game_over = true;}
                snake.direction = UP;
                break;
            case SDLK_DOWN:
                if (snake.direction == UP) {game_over = true;}
                snake.direction = DOWN;
                break;
            case SDLK_LEFT:
                if (snake.direction == RIGHT) {game_over = true;}
                snake.direction = LEFT;
                break;
            case SDLK_RIGHT:
                if (snake.direction == LEFT) {game_over = true;}
                snake.direction = RIGHT;
                break;
        }
    }
    if (snake.length > highscore) {highscore = snake.length;}
    //Halal utan felszabaditjuk a kigyonak lefoglalt dinamikus memoriahelyet.
    freeSnake(&snake);
    //Halalkor visszavaltoztatjuk az ablak nevet.
    SDL_SetWindowTitle(window, "SNAKE");
    if (game_over) {
        //Halal eseten beszinezzuk a teljes kepernyot.
        SDL_RenderClear(renderer);
        boxRGBA(renderer, 0, 0, WIDTH, HEIGHT, 49, 1, 112, 255);

        //Letrehozzuk a nagyobb, illetve a kisebb betutipust es meretet nekik.
        TTF_Font *endbigfont = TTF_OpenFont("comic.ttf", 70);
        TTF_Font *scorefont = TTF_OpenFont("comic.ttf", 35);
        TTF_Font *endsmallfont = TTF_OpenFont("comic.ttf", 15);

        //Kiirjuk a halaluzenetet
        SDL_Surface *deathmsg;
        SDL_Texture *deathmsg_t;
        SDL_Rect deathmsgpos = {0, 0, 0, 0};
        deathmsg = TTF_RenderUTF8_Blended(endbigfont, "YOU DIED", White);
        deathmsg_t = SDL_CreateTextureFromSurface(renderer, deathmsg);
        deathmsgpos.x = (WIDTH - deathmsg->w) / 2;
        deathmsgpos.y = (HEIGHT - deathmsg->h) / 2;
        deathmsgpos.w = deathmsg->w;
        deathmsgpos.h = deathmsg->h;
        SDL_RenderCopy(renderer, deathmsg_t, NULL, &deathmsgpos);
        
        char endbuffer[256];
        sprintf(endbuffer, "YOUR SCORE: %i", snake.length);
        SDL_Surface *score;
        SDL_Texture *score_t;
        SDL_Rect scorepos = {0, 0, 0, 0};
        score = TTF_RenderUTF8_Blended(scorefont, endbuffer, White);
        score_t = SDL_CreateTextureFromSurface(renderer, score);
        scorepos.x = (WIDTH - score->w) / 2;
        scorepos.y = (HEIGHT - deathmsg->h + 150) / 2;
        scorepos.w = score->w;
        scorepos.h = score->h;
        SDL_RenderCopy(renderer, score_t, NULL, &scorepos);

        //A vegso kepernyon megjelenitjuk a tovabblepesi lehetosegeket (Q kilepes, P uj jatek).
        SDL_Surface *endcontrols;
        SDL_Texture *endcontrols_t;
        SDL_Rect endcontrolspos = {0, 0, 0, 0};
        endcontrols = TTF_RenderUTF8_Blended(endsmallfont, "PRESS (P) TO PLAY AGAIN. PRESS (Q) TO QUIT.", White);
        endcontrols_t = SDL_CreateTextureFromSurface(renderer, endcontrols);
        endcontrolspos.x = (WIDTH - endcontrols->w) / 2;
        endcontrolspos.y = HEIGHT - endcontrols->h;
        endcontrolspos.w = endcontrols->w;
        endcontrolspos.h = endcontrols->h;
        SDL_RenderCopy(renderer, endcontrols_t, NULL, &endcontrolspos);

        //Varjuk a billentyu bemenetet, hogy eldontsuk mi legyen a kovetkezo event.
        SDL_RenderPresent(renderer);
        SDL_WaitEvent(&event);
        while (event.type != SDL_KEYDOWN) {
            SDL_WaitEvent(&event);
        }
        if (event.key.keysym.sym == SDLK_q) {
            IntoFile(highscore);
            game_close = true;
        }
        if (event.key.keysym.sym == SDLK_p) {
            game_over = false;
        }
    }
}


int main(int argc, char *argv[]) {
    sdl_init("SNAKE", WIDTH, HEIGHT, &window, &renderer, &font);

    startMenu(game_close, White);
    int highscore = 1;
    while (!game_close) {
        gameLoop(highscore);
    }

    SDL_Quit();
    TTF_CloseFont(font);

    return 0;
}
