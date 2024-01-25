bool hitFood(Node *node, Food *food);
bool hitWall(Snake *snake);
bool hitTail(Snake *snake, bool game_over);
void snakeUpdate(Snake *snake, Food *food, bool game_over);
void freeSnake(Snake *snake);