#ifndef DUCK_HUNT_H
#define DUCK_HUNT_H

#define _USE_MATH_DEFINES
#include <SFML/Graphics.h>
#include <SFML/Audio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define MAX_TARGETS 5
#define TARGET_SPEED 1.5f
#define TARGET_SIZE 40
#define MAX_STARS 200
#define MAX_PARTICLES 50
#define GAME_DURATION 45.0f
#define COUNTDOWN_TIME 5.0f
#define STICKMAN_SIZE 90
#define DEATH_ANIMATION_TIME 1.0f

typedef enum {
    STATE_MENU,
    STATE_COUNTDOWN,
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

typedef enum {
    ANIM_RUNNING,
    ANIM_DYING,
    ANIM_DEAD
} AnimationState;

typedef struct {
    sfVector2f position;
    sfVector2f velocity;
    int alive;
    sfClock *clock;
    float animation_time;
    sfColor color;
    float size_multiplier;
    AnimationState anim_state;
    float death_timer;
    float run_cycle;
    int facing_right;
} Target;

typedef struct {
    sfVector2f position;
    float brightness;
    float twinkle_speed;
} Star;

typedef struct {
    sfVector2f position;
    sfVector2f velocity;
    float life;
    sfColor color;
} Particle;

typedef struct {
    sfRenderWindow *window;
    sfTexture *target_texture;
    sfTexture *planet_texture;
    sfSprite *target_sprite;
    sfSprite *planet_sprite;
    sfFont *font;
    sfText *score_text;
    sfText *timer_text;
    sfText *title_text;
    sfText *menu_text[4];
    sfMusic *background_music;
    Target targets[MAX_TARGETS];
    Star stars[MAX_STARS];
    Particle particles[MAX_PARTICLES];
    GameState state;
    int score;
    int hits;
    int misses;
    int total_targets;
    float game_timer;
    float countdown_timer;
    int countdown_started;
    sfClock *game_clock;
    sfClock *spawn_clock;
    sfVector2f planet_pos;
    float planet_rotation;
    int music_enabled;
    int selected_menu_item;
    int hovered_menu_item;
    sfVector2i mouse_position;
    float best_scores[10];
    int score_count;
} Game;

// game.c
int init_game(Game *game);
void update_game(Game *game);
void render_game(Game *game);
void cleanup_game(Game *game);
void handle_mouse_click(Game *game, int x, int y);

// game.c
int init_game(Game *game);
void update_game(Game *game);
void render_game(Game *game);
void cleanup_game(Game *game);
void handle_mouse_click(Game *game, int x, int y);

// duck.c
void spawn_target(Target *target);
void update_target(Target *target);
void draw_target(Game *game, Target *target);
int is_target_clicked(Target *target, int x, int y);
void reset_target(Target *target);
void draw_stickman_running(Game *game, Target *target);
void draw_stickman_dying(Game *game, Target *target);
void kill_target(Target *target);

// utils.c
float get_distance(sfVector2f pos1, sfVector2f pos2);
int random_range(int min, int max);
void create_space_texture(sfTexture **texture, int type);
void update_display(Game *game);
void create_explosion(Game *game, sfVector2f position);
void draw_crosshair(Game *game);

// menu.c
void render_menu(Game *game);
void update_menu(Game *game);
void handle_menu_input(Game *game, sfKeyCode key);
void handle_menu_mouse_move(Game *game, int x, int y);
void handle_menu_mouse_click(Game *game, int x, int y);
int get_menu_item_at_position(int x, int y);
void load_scores(Game *game);
void save_scores(Game *game);

#endif