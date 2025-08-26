#include "duck_hunt.h"

float get_distance(sfVector2f pos1, sfVector2f pos2)
{
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    return sqrt(dx * dx + dy * dy);
}

int random_range(int min, int max)
{
    if (max <= min)
        return min;
    return min + rand() % (max - min + 1);
}

void create_space_texture(sfTexture **texture, int type)
{
    *texture = sfTexture_create(64, 64);
    if (!*texture)
        return;
        
    sfUint8 *pixels = malloc(64 * 64 * 4);
    if (!pixels)
        return;
    
    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            int i = (y * 64 + x) * 4;
            if (type == 0)
            {
                float dx = x - 32, dy = y - 32;
                float dist = sqrt(dx*dx + dy*dy);
                if (dist < 20)
                {
                    pixels[i] = 200;
                    pixels[i + 1] = 100;
                    pixels[i + 2] = 255;
                    pixels[i + 3] = 255;
                }
                else
                {
                    pixels[i] = pixels[i + 1] = pixels[i + 2] = 0;
                    pixels[i + 3] = 0;
                }
            }
            else
            {
                float dx = x - 32, dy = y - 32;
                float dist = sqrt(dx*dx + dy*dy);
                if (dist < 30)
                {
                    pixels[i] = 255;
                    pixels[i + 1] = 150;
                    pixels[i + 2] = 50;
                    pixels[i + 3] = 255;
                }
                else
                {
                    pixels[i] = pixels[i + 1] = pixels[i + 2] = 0;
                    pixels[i + 3] = 0;
                }
            }
        }
    }
    
    sfTexture_updateFromPixels(*texture, pixels, 64, 64, 0, 0);
    free(pixels);
}

void update_display(Game *game)
{
    static char score_str[64];
    static char timer_str[64];
    
    sprintf(score_str, "Score: %d", game->score);
    sprintf(timer_str, "Time: %.1f", game->game_timer);
    
    if (game->score_text)
    {
        sfText_setString(game->score_text, score_str);
        sfText_setPosition(game->score_text, (sfVector2f){10, 10});
        sfColor cyan = {0, 255, 255, 255};
        sfText_setColor(game->score_text, cyan);
        sfText_setCharacterSize(game->score_text, 24);
    }
    
    if (game->timer_text)
    {
        sfText_setString(game->timer_text, timer_str);
        sfText_setPosition(game->timer_text, (sfVector2f){10, 50});
        sfColor orange = {255, 165, 0, 255};
        sfText_setColor(game->timer_text, orange);
        sfText_setCharacterSize(game->timer_text, 24);
    }
}

void create_explosion(Game *game, sfVector2f position)
{
    int particles_created = 0;
    
    for (int i = 0; i < MAX_PARTICLES && particles_created < 20; i++)
    {
        if (game->particles[i].life <= 0)
        {
            game->particles[i].position = position;
            
            // Create more varied explosion patterns
            float angle = (float)(random_range(0, 360)) * M_PI / 180.0f;
            float speed = (float)random_range(100, 300) / 10.0f;
            
            // Add slight randomness to position for better spread
            game->particles[i].position.x += random_range(-5, 5);
            game->particles[i].position.y += random_range(-5, 5);
            
            game->particles[i].velocity.x = cos(angle) * speed;
            game->particles[i].velocity.y = sin(angle) * speed;
            game->particles[i].life = (float)random_range(80, 120) / 100.0f; // Varied lifetime
            
            // More colorful explosions
            if (particles_created < 8) {
                // Core explosion - bright colors
                sfUint8 r = random_range(255, 255);
                sfUint8 g = random_range(200, 255);
                sfUint8 b = random_range(0, 100);
                game->particles[i].color = (sfColor){r, g, b, 255};
            } else if (particles_created < 15) {
                // Middle ring - orange/red
                sfUint8 r = random_range(200, 255);
                sfUint8 g = random_range(100, 200);
                sfUint8 b = 0;
                game->particles[i].color = (sfColor){r, g, b, 255};
            } else {
                // Outer sparks - varied colors
                sfUint8 r = random_range(150, 255);
                sfUint8 g = random_range(50, 200);
                sfUint8 b = random_range(0, 150);
                game->particles[i].color = (sfColor){r, g, b, 255};
            }
            
            particles_created++;
        }
    }
}

void draw_crosshair(Game *game)
{
    sfVector2i mouse_pos = game->mouse_position;
    
    // Crosshair parameters
    float crosshair_size = 15.0f;
    float crosshair_thickness = 2.0f;
    float gap = 5.0f; // Gap in the center
    sfColor crosshair_color = {255, 255, 255, 200}; // Semi-transparent white
    
    // Horizontal lines (left and right)
    sfRectangleShape *h_left = sfRectangleShape_create();
    sfRectangleShape_setSize(h_left, (sfVector2f){crosshair_size, crosshair_thickness});
    sfRectangleShape_setFillColor(h_left, crosshair_color);
    sfRectangleShape_setPosition(h_left, (sfVector2f){
        mouse_pos.x - crosshair_size - gap, 
        mouse_pos.y - crosshair_thickness/2
    });
    sfRenderWindow_drawRectangleShape(game->window, h_left, NULL);
    sfRectangleShape_destroy(h_left);
    
    sfRectangleShape *h_right = sfRectangleShape_create();
    sfRectangleShape_setSize(h_right, (sfVector2f){crosshair_size, crosshair_thickness});
    sfRectangleShape_setFillColor(h_right, crosshair_color);
    sfRectangleShape_setPosition(h_right, (sfVector2f){
        mouse_pos.x + gap, 
        mouse_pos.y - crosshair_thickness/2
    });
    sfRenderWindow_drawRectangleShape(game->window, h_right, NULL);
    sfRectangleShape_destroy(h_right);
    
    // Vertical lines (top and bottom)
    sfRectangleShape *v_top = sfRectangleShape_create();
    sfRectangleShape_setSize(v_top, (sfVector2f){crosshair_thickness, crosshair_size});
    sfRectangleShape_setFillColor(v_top, crosshair_color);
    sfRectangleShape_setPosition(v_top, (sfVector2f){
        mouse_pos.x - crosshair_thickness/2, 
        mouse_pos.y - crosshair_size - gap
    });
    sfRenderWindow_drawRectangleShape(game->window, v_top, NULL);
    sfRectangleShape_destroy(v_top);
    
    sfRectangleShape *v_bottom = sfRectangleShape_create();
    sfRectangleShape_setSize(v_bottom, (sfVector2f){crosshair_thickness, crosshair_size});
    sfRectangleShape_setFillColor(v_bottom, crosshair_color);
    sfRectangleShape_setPosition(v_bottom, (sfVector2f){
        mouse_pos.x - crosshair_thickness/2, 
        mouse_pos.y + gap
    });
    sfRenderWindow_drawRectangleShape(game->window, v_bottom, NULL);
    sfRectangleShape_destroy(v_bottom);
    
    // Optional: Add center dot for precise aiming
    sfCircleShape *center_dot = sfCircleShape_create();
    sfCircleShape_setRadius(center_dot, 1.0f);
    sfCircleShape_setFillColor(center_dot, (sfColor){255, 0, 0, 150}); // Semi-transparent red
    sfCircleShape_setPosition(center_dot, (sfVector2f){
        mouse_pos.x - 1.0f, 
        mouse_pos.y - 1.0f
    });
    sfRenderWindow_drawCircleShape(game->window, center_dot, NULL);
    sfCircleShape_destroy(center_dot);
}