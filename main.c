#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>

#define MAX(A, B) (((A) > (B)) ? (A) : (B))

enum {
    MAX_VELOCITY = 40,
    MIN_VELOCITY = 10,
    SCREEN_WIDTH = 800,
    SCREEN_HEIGHT = 600
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

void set_ball_velocity(struct Entity *entity) {
    if (entity->pos.x > SCREEN_WIDTH) {
        entity->x_velocity = rand_ball_velocity() * (-1);
    }

    if (entity->pos.y > SCREEN_HEIGHT) {
        entity->y_velocity = rand_ball_velocity() * (-1);
    }

    if (entity->pos.x <= 0) {
        entity->x_velocity = rand_ball_velocity();
    }

    if (entity->pos.y <= 0) {
        entity->y_velocity = rand_ball_velocity();
    }

    entity->pos.x += entity->x_velocity;
    entity->pos.y += entity->y_velocity;
}

int game_loop() {
    SDL_Surface *backbuffer = SDL_SetVideoMode(
        SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);

    struct Entity ball = {
        .pos = {
            .x = SCREEN_WIDTH / 2,
            .y = SCREEN_HEIGHT / 2,
            .h = 10,
            .w = 10
        },
        .image = load_image("assets/ball.bmp"),
        .x_velocity = 10,
        .y_velocity = 10
    };

    struct Entity player_paddle = {
        .pos = {
            .x = 20,
            .y = SCREEN_HEIGHT / 2,
            .h = 100,
            .w = 20
        },
        .image = load_image("assets/player.bmp"),
        .x_velocity = 10,
        .y_velocity = 20
    };

    SDL_Surface *background_image = load_image("assets/background.bmp");

    while (game_running()) {
        SDL_BlitSurface(background_image, NULL, backbuffer, NULL);
        SDL_BlitSurface(ball.image, NULL, backbuffer, &ball.pos);
        SDL_BlitSurface(
            player_paddle.image,
            NULL,
            backbuffer,
            &player_paddle.pos);

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
        set_ball_velocity(&ball);

        SDL_Flip(backbuffer);
        SDL_Delay(20);
    }
    return 0;
}

int main(void) {
    int ret_code;
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    ret_code = game_loop();
    SDL_Quit();
    return ret_code;
}
