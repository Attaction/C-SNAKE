#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "struct.h"
#include "food.h"
#include "snake.h"

//Definialom az iranyokat.
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

//Mindenhol ugyanazt a windowot es ugyanazt a renderert hasznalom, ezert extern-nel hivom meg oket.
extern SDL_Window *window;
extern SDL_Renderer *renderer;

//Definialom itt is az ablak szelesseget es magassagat.
#define WIDTH 800
#define HEIGHT 600

bool hitFood(Node *node, Food *food) {
    //Ellenorizzuk, hogy a kigyo eltalalta-e a kajat es visszaterunk true-val, ha elatalalta.
    return (node->x == food->x) && (node->y == food->y);
}

//Ez a fuggveny ellenorzi, hogy falnak mentunk-e vagy sem.
bool hitWall(Snake *snake) {
    //Ha a kigyo fejenek x koordinataja kisebb mint 0, azaz kiment a bal oldalan az ablaknak, akkor terjen vissza true-val.
    if (snake->head->x < 0) {
        return true;
    }
    //Ha a kigyo fejenek x koordinataja nagyobb mint az ablak szelessege, azaz az ablak jobb oldalan kiment a kigyo, akkor is terjen vissza true-val.
    else if (snake->head->x > WIDTH + 5) {
        return true;
    }
    //Ha a kigyo fejenek y koordinataja kisebb, mint 0, azaz a kigyo kiment a palya tetejen, akkor terjen vissza true-val.
    else if (snake->head->y < 0) {
        return true;
    }
    //Ha a kigyo fejenek y koordinataja nagyobb mint az ablak magassaga, akkor a kigyo kiment az ablak aljan, akkor terjen vissza true-val.
    else if (snake->head->y > HEIGHT + 5) {
        return true;
    }
    //Ha az elozoek kozul semelyik nem igaz, akkor a kigyo nem hagyta el a palyat, ekkor terjen vissza false-al.
    return false;
}

//Ellenorizzuk, hogy a kigyo nekiment-e a sajat farkanak.
bool hitTail(Snake *snake, bool game_over) {
    //Csinalunk egy mozgo pointert a lancolt listankhoz, ami alapbol az utolso elemre mutat, es innen megy vissza addig, ameddig a fejet el nem eri.
    Node *node = snake->tail;
    while (node->prev->prev != NULL) {
        //Ha a fej x koordinataja megegyezik a mozgo pointer (kigyo testenek egyik eleme) x es y koordinatajaval, akkor sajat magaba fordult a kigyo. Ilyenkor terjen vissza true-val.
        if (snake->head->x == node->x && snake->head->y == node->y) {
            return true;
        }
        node = node->prev;
    }
    //Ha pedig nem ment neki, akkor terjen vissza false-al.
    return false;
}

//A kigyo ugy mozog, hogy mindig, amikor meghivjuk ezt a fuggvenyt, akkor letrehozunk egy uj elemet, amit alapbol a kigyo fejevel egy helre teszunk.
//Majd megnezzuk, hogy milyen iranyba megy a kigyo es annak megfeleloen megvaltoztatjuk az uj fej x vagy y koordinatajat es kirajzoljuk.
//Ezt kovetoen megnezzuk, hogy eltalaltuk-e a kajat. Amennyiben nem talaltuk el, akkor a farkanak az utolso elemet letoroljuk a kepernyorol es felszabaditjuk.
//Az eddigi utolso elemre mutato pointert (snake->tail) atiranyitjuk az elotte levo elemre.
//Abban az esetben, ha a kigyoval eltalaltuk a kajat, akkor ezt a torlest elhagyjuk, ezaltal a kigyonk eggyel hosszabb lesz es a palyra generalunk egy uj kajat.
void snakeUpdate(Snake *snake, Food *food, bool game_over) {
    Node *newstart = newNode();
    snake->head->prev = newstart;
    newstart->next = snake->head;
    newstart->x = snake->head->x;
    newstart->y = snake->head->y;
    snake->head = newstart;
    switch (snake->direction) {
        case UP:
            newstart->y -= 10;
            break;
        case DOWN:
            newstart->y += 10;
            break;
        case LEFT:
            newstart->x -= 10;
            break;
        case RIGHT:
            newstart->x += 10;
            break;
    }
    boxRGBA(renderer, newstart->x, newstart->y, newstart->x + newstart->size, newstart->y + newstart->size, 0, 255, 0 ,255);
    if (!hitFood(snake->head->next, food)) {
        struct Node *oldtail = snake->tail;
        snake->tail->prev->next = NULL;
        snake->tail = oldtail->prev;
        boxRGBA(renderer, oldtail->x, oldtail->y, oldtail->x + oldtail->size, oldtail->y + oldtail->size, 0, 0, 0, 255);
        free(oldtail);
    } else {
        newFood(snake, food);
        snake->length++;
    }
}

//A jatek vegen vegigmegyunk a kigyon NULL pointerig es felszabaditjuk a kigyo farkat. Ezutan felszabaditjuk a kigyo fejet is.
void freeSnake(Snake *snake) {
    while(snake->tail->prev != NULL) {
        Node *temp = snake->tail->prev;
        free(snake->tail);
        snake->tail = temp;
    }
    free(snake->head);
}