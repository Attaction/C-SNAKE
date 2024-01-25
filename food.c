#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "struct.h"
#include "food.h"
//#include "snake.h"

extern SDL_Window *window;
extern SDL_Renderer *renderer;

#define WIDTH 800
#define HEIGHT 600

//Ellenorizzuk, hogy az uj kaja ne keruljon a kigyo testere.
bool foodOnSnake(Snake *snake, Food *food) {
    //Ehhez veszunk egz mozgo pointert, ami alapbol a kigyo fejere mutat, es vegigmegyunk a mozgo pointerrel a kigyo testen.
    Node *node = snake->head;
    while (node->next != NULL) {
        //Ellenorizzuk, hoygy a lerakni keszulo kajat nem a kigyo testere rakjuk-e. Ha megis, akkor visszaterunk true-val.
        if(food->x == node->x && food->y == node->y) {
            return true;
        }
        node = node->next;
    }
    //Ha nem raktuk ra, akkor visszaterunk false-al.
    return false;
}

void newFood(Snake *snake, Food *food) {
    //Randomizalva kirajzolunk minden egyes hivasra egy uj etelt, amit a Food strukturara mutato food pointerbe mentunk el.
    food->y = rand() % ((HEIGHT + 10) / 10) * 10;
    food->x = rand() % ((WIDTH + 10) / 10) * 10;
    //Ha a foodOnSnake fuggveny true-val ter vissza, akkor addig hozunk letre uj x es y koordinatakat a kajanak, ameddig az nem kerul a kigyo testere.
    while (foodOnSnake(snake, food)) {
        food->y = rand() % ((HEIGHT + 10) / 10) * 10;
        food->x = rand() % ((WIDTH + 10) / 10) * 10;
    }
    boxRGBA(renderer, food->x, food->y, food->x + 10, food->y + 10, 255, 0, 0, 255);
}