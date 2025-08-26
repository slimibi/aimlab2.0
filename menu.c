#include "duck_hunt.h"

void render_menu(Game *game)
{
    static char menu_items[4][32] = {"PLAY", "MUSIC: ON", "SCOREBOARD", "EXIT"};
    
    if (game->music_enabled)
        strcpy(menu_items[1], "MUSIC: ON");
    else
        strcpy(menu_items[1], "MUSIC: OFF");
    
    if (game->title_text) {
        sfText_setString(game->title_text, "AIM TRAINER PRO");
        sfText_setCharacterSize(game->title_text, 48);
        sfText_setColor(game->title_text, (sfColor){255, 100, 255, 255});
        sfFloatRect title_bounds = sfText_getLocalBounds(game->title_text);
        sfText_setPosition(game->title_text, (sfVector2f){
            WINDOW_WIDTH/2 - title_bounds.width/2, 100});
        sfRenderWindow_drawText(game->window, game->title_text, NULL);
    }
    
    for (int i = 0; i < 4; i++)
    {
        if (game->menu_text[i]) {
            sfText_setString(game->menu_text[i], menu_items[i]);
            sfText_setCharacterSize(game->menu_text[i], 32);
            
            // Color logic: selected by keyboard OR hovered by mouse = yellow
            if (i == game->selected_menu_item || i == game->hovered_menu_item)
                sfText_setColor(game->menu_text[i], (sfColor){255, 255, 0, 255});
            else
                sfText_setColor(game->menu_text[i], (sfColor){0, 255, 255, 255});
            
            sfFloatRect bounds = sfText_getLocalBounds(game->menu_text[i]);
            sfText_setPosition(game->menu_text[i], (sfVector2f){
                WINDOW_WIDTH/2 - bounds.width/2, 250 + i * 80});
            sfRenderWindow_drawText(game->window, game->menu_text[i], NULL);
        }
    }
    
    if (game->selected_menu_item == 2)
    {
        static char scoreboard[512];
        strcpy(scoreboard, "TOP SCORES:\n\n");
        
        for (int i = 0; i < game->score_count && i < 5; i++)
        {
            char score_line[64];
            sprintf(score_line, "%d. %.0f points\n", i + 1, game->best_scores[i]);
            strcat(scoreboard, score_line);
        }
        
        sfText *score_display = sfText_create();
        sfText_setString(score_display, scoreboard);
        if (game->font) {
            sfText_setFont(score_display, game->font);
        }
        sfText_setCharacterSize(score_display, 24);
        sfText_setColor(score_display, (sfColor){255, 255, 255, 255});
        sfText_setPosition(score_display, (sfVector2f){50, 300});
        sfRenderWindow_drawText(game->window, score_display, NULL);
        sfText_destroy(score_display);
    }
}

void update_menu(Game *game)
{
    if (game->background_music) {
        if (game->music_enabled && sfMusic_getStatus(game->background_music) != sfPlaying)
        {
            sfMusic_setLoop(game->background_music, sfTrue);
            sfMusic_setVolume(game->background_music, 15); // Lower volume to prevent crackling
            sfMusic_setAttenuation(game->background_music, 0.1f); // Reduce 3D audio attenuation
            sfMusic_setPitch(game->background_music, 1.0f); // Ensure normal pitch
            sfMusic_play(game->background_music);
        }
        else if (!game->music_enabled && sfMusic_getStatus(game->background_music) == sfPlaying)
        {
            sfMusic_stop(game->background_music);
        }
    }
}

void handle_menu_input(Game *game, sfKeyCode key)
{
    if (key == sfKeyUp)
    {
        game->selected_menu_item--;
        if (game->selected_menu_item < 0)
            game->selected_menu_item = 3;
    }
    else if (key == sfKeyDown)
    {
        game->selected_menu_item++;
        if (game->selected_menu_item > 3)
            game->selected_menu_item = 0;
    }
    else if (key == sfKeyReturn || key == sfKeySpace)
    {
        switch (game->selected_menu_item)
        {
            case 0: // PLAY
                game->state = STATE_COUNTDOWN;
                sfRenderWindow_setMouseCursorVisible(game->window, sfFalse);
                game->score = 0;
                game->hits = 0;
                game->misses = 0;
                game->total_targets = 0;
                game->countdown_timer = COUNTDOWN_TIME;
                game->countdown_started = 0; // Reset countdown flag
                for (int i = 0; i < MAX_TARGETS; i++)
                    game->targets[i].alive = 0;
                break;
            case 1: // MUSIC TOGGLE
                game->music_enabled = !game->music_enabled;
                break;
            case 2: // SCOREBOARD - just display, already handled in render
                break;
            case 3: // EXIT
                sfRenderWindow_setMouseCursorVisible(game->window, sfTrue);
                sfRenderWindow_close(game->window);
                break;
        }
    }
}

void load_scores(Game *game)
{
    FILE *file = fopen("scores.dat", "rb");
    if (file)
    {
        fread(&game->score_count, sizeof(int), 1, file);
        if (game->score_count > 10) game->score_count = 10;
        fread(game->best_scores, sizeof(float), game->score_count, file);
        fclose(file);
    }
}

void save_scores(Game *game)
{
    if (game->score_count < 10)
    {
        game->best_scores[game->score_count] = game->score;
        game->score_count++;
    }
    else
    {
        for (int i = 0; i < 10; i++)
        {
            if (game->score > game->best_scores[i])
            {
                for (int j = 9; j > i; j--)
                    game->best_scores[j] = game->best_scores[j-1];
                game->best_scores[i] = game->score;
                break;
            }
        }
    }
    
    for (int i = 0; i < game->score_count - 1; i++) {
        for (int j = i + 1; j < game->score_count; j++) {
            if (game->best_scores[i] < game->best_scores[j]) {
                float temp = game->best_scores[i];
                game->best_scores[i] = game->best_scores[j];
                game->best_scores[j] = temp;
            }
        }
    }
    
    FILE *file = fopen("scores.dat", "wb");
    if (file)
    {
        fwrite(&game->score_count, sizeof(int), 1, file);
        fwrite(game->best_scores, sizeof(float), game->score_count, file);
        fclose(file);
    }
}

int get_menu_item_at_position(int x, int y)
{
    if (x < WINDOW_WIDTH/2 - 150 || x > WINDOW_WIDTH/2 + 150)
        return -1;
    
    for (int i = 0; i < 4; i++) {
        int item_y = 250 + i * 80;
        if (y >= item_y - 20 && y <= item_y + 40) {
            return i;
        }
    }
    return -1;
}

void handle_menu_mouse_move(Game *game, int x, int y)
{
    int hovered_item = get_menu_item_at_position(x, y);
    game->hovered_menu_item = hovered_item;
    
    if (hovered_item != -1 && hovered_item != game->selected_menu_item) {
        game->selected_menu_item = hovered_item;
    }
}

void handle_menu_mouse_click(Game *game, int x, int y)
{
    int clicked_item = get_menu_item_at_position(x, y);
    
    if (clicked_item != -1) {
        game->selected_menu_item = clicked_item;
        
        switch (clicked_item)
        {
            case 0: // PLAY
                game->state = STATE_COUNTDOWN;
                sfRenderWindow_setMouseCursorVisible(game->window, sfFalse);
                game->score = 0;
                game->hits = 0;
                game->misses = 0;
                game->total_targets = 0;
                game->countdown_timer = COUNTDOWN_TIME;
                game->countdown_started = 0; // Reset countdown flag
                for (int i = 0; i < MAX_TARGETS; i++)
                    game->targets[i].alive = 0;
                break;
            case 1: // MUSIC TOGGLE
                game->music_enabled = !game->music_enabled;
                break;
            case 2: // SCOREBOARD - just display, already handled in render
                break;
            case 3: // EXIT
                sfRenderWindow_setMouseCursorVisible(game->window, sfTrue);
                sfRenderWindow_close(game->window);
                break;
        }
    }
}