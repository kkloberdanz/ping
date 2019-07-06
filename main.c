#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>

#define MAX(A, B) (((A) > (B)) ? (A) : (B))

enum {
    MAX_VELOCITY = 20,
    MIN_VELOCITY = 5,
    SCREEN_WIDTH = 800,
    SCREEN_HEIGHT = 600
};

enum GameState {
    RUNNING,
    PLAYER_SCORED,
    ENEMY_SCORED
};

struct Entity {
    SDL_Rect pos;
    SDL_Surface *image;
    int x_velocity;
    int y_velocity;
};

SDL_Surface *load_image(char *filename) {
    SDL_Surface *image_loaded = NULL;
    SDL_Surface *processed_image = NULL;
  
    image_loaded = SDL_LoadBMP(filename);
  
    if (image_loaded != NULL) {
        processed_image = SDL_DisplayFormat(image_loaded);
        SDL_FreeSurface(image_loaded);
  
        if (processed_image != NULL) {
            int color_key = SDL_MapRGB(processed_image->format, 0xFF, 0, 0xFF);
            SDL_SetColorKey(processed_image, SDL_SRCCOLORKEY, color_key);
        }
    }
    return processed_image;
}

int rand_ball_velocity() {
    int random_num = rand() % MAX_VELOCITY;
    int velocity = MAX(random_num, MIN_VELOCITY);
    return velocity;
}

bool game_running() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;

            default:
                break;
        }
    }
    return true;
}

bool obj_touching(SDL_Rect *rect1, SDL_Rect *rect2) {
    if (rect1->x >= rect2->x + rect2->w) {
        return false;
    } else if (rect1->y >= rect2->y + rect2->h) {
        return false;
    } else if (rect2->x >= rect1->x + rect1->w) {
        return false;
    } else if (rect2->y >= rect1->y + rect1->h) {
        return false;
    } else {
        return true;
    }
}

enum GameState do_game_logic(
    struct Entity *ball,
    struct Entity *player_paddle,
    struct Entity *enemy_paddle) {

    if (obj_touching(&ball->pos, &player_paddle->pos)) {
        ball->x_velocity = rand_ball_velocity();
    } else if (obj_touching(&ball->pos, &enemy_paddle->pos)) {
        ball->x_velocity = rand_ball_velocity() * (-1);
    } else if (ball->pos.x > SCREEN_WIDTH) {
        return PLAYER_SCORED;
    } else if (ball->pos.x <= 0) {
        return ENEMY_SCORED;
    }

    if (ball->pos.y > SCREEN_HEIGHT) {
        ball->y_velocity = rand_ball_velocity() * (-1);
    } else if (ball->pos.y <= 0) {
        ball->y_velocity = rand_ball_velocity();
    }

    ball->pos.x += ball->x_velocity;
    ball->pos.y += ball->y_velocity;
    return RUNNING;
}

int game_loop() {
    SDL_Surface *backbuffer = SDL_SetVideoMode(
        SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);

    int player_score = 0;
    int enemy_score = 0;

/* ensure tail recursion */
restart_game: {
        printf("player: %d, enemy: %d\n", player_score, enemy_score);
        struct Entity ball = {
            .pos = {
                .x = SCREEN_WIDTH / 2,
                .y = SCREEN_HEIGHT / 2,
                .h = 10,
                .w = 10
            },
            .image = load_image("assets/ball.bmp"),
            .x_velocity = rand_ball_velocity(),
            .y_velocity = rand_ball_velocity()
        };

        struct Entity player_paddle = {
            .pos = {
                .x = 20,
                .y = SCREEN_HEIGHT / 2,
                .h = 100,
                .w = 20
            },
            .image = load_image("assets/player.bmp"),
            .x_velocity = 0,
            .y_velocity = 30
        };

        struct Entity enemy_paddle = {
            .pos = {
                .x = SCREEN_WIDTH - 20,
                .y = SCREEN_HEIGHT / 2,
                .h = 100,
                .w = 20
            },
            .image = load_image("assets/enemy.bmp"),
            .x_velocity = 0,
            .y_velocity = 10
        };

        SDL_Surface *background_image = load_image("assets/background.bmp");

        while (game_running()) {
            int enemy_direction = 0;
            SDL_BlitSurface(background_image, NULL, backbuffer, NULL);
            SDL_BlitSurface(ball.image, NULL, backbuffer, &ball.pos);

            SDL_BlitSurface(
                player_paddle.image,
                NULL,
                backbuffer,
                &player_paddle.pos);

            SDL_BlitSurface(
                enemy_paddle.image,
                NULL,
                backbuffer,
                &enemy_paddle.pos);


            unsigned char *keys = SDL_GetKeyState(NULL);
            if (keys[SDLK_DOWN]) {
                player_paddle.pos.y += player_paddle.y_velocity;
                if (player_paddle.pos.y > SCREEN_HEIGHT - player_paddle.pos.h) {
                    player_paddle.pos.y = SCREEN_HEIGHT - player_paddle.pos.h;
                }
            } else if (keys[SDLK_UP]) {
                player_paddle.pos.y -= player_paddle.y_velocity;
                if (player_paddle.pos.y < 0) {
                    player_paddle.pos.y = 0;
                }
            } else if (keys[SDLK_ESCAPE]) {
                return 0;
            }

            enum GameState game_state = do_game_logic(
                &ball,
                &player_paddle,
                &enemy_paddle);

            switch (game_state) {
                case ENEMY_SCORED:
                    enemy_score++;
                    goto restart_game;

                case PLAYER_SCORED:
                    player_score++;
                    goto restart_game;

                default:
                    break;
            }

            if (enemy_paddle.pos.y == ball.pos.y + 10) {
                enemy_direction = 0;
            } else if (enemy_paddle.pos.y < ball.pos.y + 10) {
                enemy_direction = 1;
            } else {
                enemy_direction = -1;
            }

            enemy_paddle.pos.y += enemy_direction * enemy_paddle.y_velocity;

            SDL_Flip(backbuffer);
            SDL_Delay(20);
        }
        return 0;
    }
}

int main(void) {
    int ret_code;
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    ret_code = game_loop();
    SDL_Quit();
    return ret_code;
}
