#include "duck_hunt.h"

int init_game(Game *game)
{
    memset(game, 0, sizeof(Game));
    
    game->window = sfRenderWindow_create((sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
                                        "Aim Trainer Pro", sfResize | sfClose, NULL);
    if (!game->window)
        return (0);
        
        
    create_space_texture(&game->target_texture, 0);
    create_space_texture(&game->planet_texture, 1);
    
    game->target_sprite = sfSprite_create();
    game->planet_sprite = sfSprite_create();
    sfSprite_setTexture(game->target_sprite, game->target_texture, sfTrue);
    sfSprite_setTexture(game->planet_sprite, game->planet_texture, sfTrue);
    sfSprite_setScale(game->planet_sprite, (sfVector2f){2.0f, 2.0f});
    
    game->font = sfFont_createFromFile("assets/Valorant Font.ttf");
    if (!game->font) {
        printf("Warning: Could not load Valorant font, trying to create minimal font\n");
        game->font = sfFont_createFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
        if (!game->font) {
            game->font = sfFont_createFromFile("/System/Library/Fonts/Arial.ttf"); // macOS
            if (!game->font) {
                game->font = sfFont_createFromFile("/Windows/Fonts/arial.ttf"); // Windows
                if (!game->font) {
                    printf("Error: No usable font found, text will not display correctly\n");
                    return (0);
                }
            }
        }
    }
    
    game->score_text = sfText_create();
    game->timer_text = sfText_create();
    game->title_text = sfText_create();
    
    if (game->score_text && game->font) {
        sfText_setFont(game->score_text, game->font);
        sfText_setCharacterSize(game->score_text, 24);
        sfText_setColor(game->score_text, (sfColor){0, 255, 255, 255});
    }
    
    if (game->timer_text && game->font) {
        sfText_setFont(game->timer_text, game->font);
        sfText_setCharacterSize(game->timer_text, 24);
        sfText_setColor(game->timer_text, (sfColor){255, 165, 0, 255});
    }
    
    if (game->title_text && game->font) {
        sfText_setFont(game->title_text, game->font);
        sfText_setCharacterSize(game->title_text, 48);
        sfText_setColor(game->title_text, (sfColor){255, 255, 255, 255});
    }
    
    for (int i = 0; i < 4; i++) {
        game->menu_text[i] = sfText_create();
        if (game->menu_text[i] && game->font) {
            sfText_setFont(game->menu_text[i], game->font);
            sfText_setCharacterSize(game->menu_text[i], 32);
            sfText_setColor(game->menu_text[i], (sfColor){255, 255, 255, 255});
        }
    }
    
    game->background_music = sfMusic_createFromFile("assets/valorant_music.mp3");
    
    game->state = STATE_MENU;
    sfRenderWindow_setMouseCursorVisible(game->window, sfTrue);
    game->score = 0;
    game->hits = 0;
    game->misses = 0;
    game->total_targets = 0;
    game->game_timer = 0;
    game->countdown_timer = COUNTDOWN_TIME;
    game->countdown_started = 0;
    game->music_enabled = 1;
    game->selected_menu_item = 0;
    game->hovered_menu_item = -1;
    game->mouse_position = (sfVector2i){WINDOW_WIDTH/2, WINDOW_HEIGHT/2}; // Center by default
    game->score_count = 0;
    
    game->game_clock = sfClock_create();
    game->spawn_clock = sfClock_create();
    game->planet_pos = (sfVector2f){WINDOW_WIDTH - 150, WINDOW_HEIGHT - 150};
    game->planet_rotation = 0;
    
    for (int i = 0; i < MAX_TARGETS; i++)
        reset_target(&game->targets[i]);
    
    for (int i = 0; i < MAX_STARS; i++)
    {
        game->stars[i].position.x = random_range(0, WINDOW_WIDTH);
        game->stars[i].position.y = random_range(0, WINDOW_HEIGHT);
        game->stars[i].brightness = (float)random_range(50, 255) / 255.0f;
        game->stars[i].twinkle_speed = (float)random_range(1, 5);
    }
    
    for (int i = 0; i < MAX_PARTICLES; i++)
        game->particles[i].life = 0;
    
    load_scores(game);
    srand(time(NULL));
    return (1);
}

void update_game(Game *game)
{
    float delta_time = sfTime_asSeconds(sfClock_restart(game->game_clock));
    
    if (game->state == STATE_COUNTDOWN)
    {
        if (game->countdown_started) {
            game->countdown_timer -= delta_time;
            if (game->countdown_timer <= 0)
            {
                game->state = STATE_PLAYING;
                game->game_timer = GAME_DURATION;
                sfClock_restart(game->spawn_clock);
            }
        }
        return;
    }
    
    if (game->state != STATE_PLAYING)
        return;
    
    game->game_timer -= delta_time;
    if (game->game_timer <= 0)
    {
        game->state = STATE_GAME_OVER;
        sfRenderWindow_setMouseCursorVisible(game->window, sfTrue);
        save_scores(game);
        return;
    }
    
    if (sfTime_asSeconds(sfClock_getElapsedTime(game->spawn_clock)) > 1.0f)
    {
        for (int i = 0; i < MAX_TARGETS; i++)
        {
            if (!game->targets[i].alive)
            {
                spawn_target(&game->targets[i]);
                game->total_targets++;
                sfClock_restart(game->spawn_clock);
                break;
            }
        }
    }
    
    for (int i = 0; i < MAX_TARGETS; i++)
        if (game->targets[i].alive)
            update_target(&game->targets[i]);
    
    for (int i = 0; i < MAX_STARS; i++)
    {
        float time = sfTime_asSeconds(sfClock_getElapsedTime(game->game_clock));
        game->stars[i].brightness = 0.3f + 0.7f * (sin(time * game->stars[i].twinkle_speed) + 1) / 2;
    }
    
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (game->particles[i].life > 0)
        {
            game->particles[i].position.x += game->particles[i].velocity.x * delta_time * 100;
            game->particles[i].position.y += game->particles[i].velocity.y * delta_time * 100;
            
            game->particles[i].velocity.y += delta_time * 200;
            game->particles[i].velocity.x *= 0.98f;
            game->particles[i].velocity.y *= 0.98f;
            
            game->particles[i].life -= delta_time * 1.5f;
            
            game->particles[i].color.a = (sfUint8)(game->particles[i].life * 255);
        }
    }
    
    game->planet_rotation += delta_time * 30;
    update_display(game);
}

void render_game(Game *game)
{
    sfColor space_color = {5, 5, 20, 255};
    sfRenderWindow_clear(game->window, space_color);
    
    for (int i = 0; i < MAX_STARS; i++)
    {
        sfCircleShape *star = sfCircleShape_create();
        sfCircleShape_setRadius(star, 1.0f);
        sfColor star_color = {255, 255, 255, (sfUint8)(game->stars[i].brightness * 255)};
        sfCircleShape_setFillColor(star, star_color);
        sfCircleShape_setPosition(star, game->stars[i].position);
        sfRenderWindow_drawCircleShape(game->window, star, NULL);
        sfCircleShape_destroy(star);
    }
    
    sfSprite_setPosition(game->planet_sprite, game->planet_pos);
    sfSprite_setRotation(game->planet_sprite, game->planet_rotation);
    sfRenderWindow_drawSprite(game->window, game->planet_sprite, NULL);
    
    if (game->state == STATE_MENU)
    {
        render_menu(game);
    }
    else if (game->state == STATE_COUNTDOWN)
    {
        if (!game->countdown_started) {
            // Show "Press any key to start countdown"
            sfText_setString(game->title_text, "Press any key to start!");
            sfText_setCharacterSize(game->title_text, 48);
            sfText_setColor(game->title_text, (sfColor){255, 255, 255, 255});
            sfFloatRect bounds = sfText_getLocalBounds(game->title_text);
            sfText_setPosition(game->title_text, (sfVector2f){
                WINDOW_WIDTH/2 - bounds.width/2, WINDOW_HEIGHT/2 - bounds.height/2});
            sfRenderWindow_drawText(game->window, game->title_text, NULL);
        } else {
            // Show countdown numbers
            static char countdown_str[32];
            sprintf(countdown_str, "%.0f", game->countdown_timer + 1);
            sfText_setString(game->title_text, countdown_str);
            sfText_setCharacterSize(game->title_text, 120);
            sfText_setColor(game->title_text, (sfColor){255, 255, 0, 255});
            sfFloatRect bounds = sfText_getLocalBounds(game->title_text);
            sfText_setPosition(game->title_text, (sfVector2f){
                WINDOW_WIDTH/2 - bounds.width/2, WINDOW_HEIGHT/2 - bounds.height/2});
            sfRenderWindow_drawText(game->window, game->title_text, NULL);
        }
    }
    else if (game->state == STATE_PLAYING)
    {
        for (int i = 0; i < MAX_TARGETS; i++)
            if (game->targets[i].alive)
                draw_target(game, &game->targets[i]);
        
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            if (game->particles[i].life > 0)
            {
                sfCircleShape *particle = sfCircleShape_create();
                
                float size = 2.0f + (game->particles[i].life * 4.0f);
                sfCircleShape_setRadius(particle, size);
                
                sfColor color = game->particles[i].color;
                color.a = (sfUint8)(game->particles[i].life * 255);
                sfCircleShape_setFillColor(particle, color);
                
                sfVector2f pos = {
                    game->particles[i].position.x - size,
                    game->particles[i].position.y - size
                };
                sfCircleShape_setPosition(particle, pos);
                
                sfRenderWindow_drawCircleShape(game->window, particle, NULL);
                sfCircleShape_destroy(particle);
            }
        }
        
        if (game->font)
        {
            sfRenderWindow_drawText(game->window, game->score_text, NULL);
            sfRenderWindow_drawText(game->window, game->timer_text, NULL);
        }
        
        draw_crosshair(game);
    }
    else if (game->state == STATE_GAME_OVER)
    {
        static char result_str[256];
        float accuracy = game->total_targets > 0 ? (float)game->hits / game->total_targets * 100 : 0;
        sprintf(result_str, "Game Over!\nScore: %d\nAccuracy: %.1f%%\nPress SPACE to continue", 
                game->score, accuracy);
        
        sfText_setString(game->title_text, result_str);
        sfText_setCharacterSize(game->title_text, 36);
        sfText_setColor(game->title_text, (sfColor){0, 255, 255, 255});
        sfFloatRect bounds = sfText_getLocalBounds(game->title_text);
        sfText_setPosition(game->title_text, (sfVector2f){
            WINDOW_WIDTH/2 - bounds.width/2, WINDOW_HEIGHT/2 - bounds.height/2});
        sfRenderWindow_drawText(game->window, game->title_text, NULL);
    }
    
    sfRenderWindow_display(game->window);
}

void cleanup_game(Game *game)
{
    save_scores(game);
    if (game->window) {
        sfRenderWindow_setMouseCursorVisible(game->window, sfTrue);
        sfRenderWindow_destroy(game->window);
    }
    if (game->target_texture)
        sfTexture_destroy(game->target_texture);
    if (game->planet_texture)
        sfTexture_destroy(game->planet_texture);
    if (game->target_sprite)
        sfSprite_destroy(game->target_sprite);
    if (game->planet_sprite)
        sfSprite_destroy(game->planet_sprite);
    
    if (game->font)
        sfFont_destroy(game->font);
        
    if (game->score_text)
        sfText_destroy(game->score_text);
    if (game->timer_text)
        sfText_destroy(game->timer_text);
    if (game->title_text)
        sfText_destroy(game->title_text);
    
    for (int i = 0; i < 4; i++)
        if (game->menu_text[i])
            sfText_destroy(game->menu_text[i]);
    
    if (game->background_music)        
        sfMusic_destroy(game->background_music);
    if (game->game_clock)
        sfClock_destroy(game->game_clock);
    if (game->spawn_clock)
        sfClock_destroy(game->spawn_clock);
    
    for (int i = 0; i < MAX_TARGETS; i++)
        if (game->targets[i].clock)
            sfClock_destroy(game->targets[i].clock);
}

void handle_mouse_click(Game *game, int x, int y)
{
    if (game->state != STATE_PLAYING)
        return;
        
    int hit = 0;
    for (int i = 0; i < MAX_TARGETS; i++)
    {
        if (game->targets[i].alive && is_target_clicked(&game->targets[i], x, y))
        {
            create_explosion(game, game->targets[i].position);
            kill_target(&game->targets[i]);
            game->score += 1;
            game->hits++;
            hit = 1;
            break;
        }
    }
    
    if (!hit)
        game->misses++;
}