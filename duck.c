#include "duck_hunt.h"

void spawn_target(Target *target)
{
    target->position.x = random_range(TARGET_SIZE, WINDOW_WIDTH - TARGET_SIZE);
    target->position.y = random_range(TARGET_SIZE + 100, WINDOW_HEIGHT - TARGET_SIZE - 100);
    
    // More varied speed ranges for different character personalities
    float speed_variation = (float)random_range(60, 180) / 100.0f;
    float speed = TARGET_SPEED * speed_variation;
    float angle = (float)random_range(0, 360) * M_PI / 180.0f;
    target->velocity.x = cos(angle) * speed;
    target->velocity.y = sin(angle) * speed;
    
    // Add individual animation offset for variety
    target->run_cycle = (float)random_range(0, 100) / 100.0f * M_PI;
    
    target->alive = 1;
    target->animation_time = 0;
    target->size_multiplier = 1.0f;
    target->anim_state = ANIM_RUNNING;
    target->death_timer = 0;
    target->run_cycle = 0;
    target->facing_right = (target->velocity.x > 0) ? 1 : 0;
    
    sfUint8 r = random_range(100, 255);
    sfUint8 g = random_range(100, 255);
    sfUint8 b = random_range(150, 255);
    target->color = (sfColor){r, g, b, 255};
    
    if (!target->clock)
        target->clock = sfClock_create();
    else
        sfClock_restart(target->clock);
}

void update_target(Target *target)
{
    if (!target->alive)
        return;
        
    target->animation_time += 0.1f;
    // Variable run speed based on velocity for more dynamic animation
    float speed_factor = sqrt(target->velocity.x * target->velocity.x + target->velocity.y * target->velocity.y) / TARGET_SPEED;
    target->run_cycle += 0.15f + (speed_factor * 0.2f);
    
    // Update animation state
    if (target->anim_state == ANIM_DYING) {
        target->death_timer += 0.05f;
        if (target->death_timer >= DEATH_ANIMATION_TIME) {
            target->anim_state = ANIM_DEAD;
        }
    }
    
    // Only move if running
    if (target->anim_state == ANIM_RUNNING) {
        // Update position based on velocity
        target->position.x += target->velocity.x;
        target->position.y += target->velocity.y;
        
        // Update facing direction
        if (target->velocity.x > 0.1f) target->facing_right = 1;
        else if (target->velocity.x < -0.1f) target->facing_right = 0;
    }
    
    // Bounce off screen edges (only when running)
    if (target->anim_state == ANIM_RUNNING) {
        float radius = STICKMAN_SIZE * target->size_multiplier;
        
        if (target->position.x - radius <= 0 || target->position.x + radius >= WINDOW_WIDTH)
        {
            target->velocity.x = -target->velocity.x;
            target->position.x = fmax(radius, fmin(WINDOW_WIDTH - radius, target->position.x));
        }
        
        if (target->position.y - radius <= 0 || target->position.y + radius >= WINDOW_HEIGHT)
        {
            target->velocity.y = -target->velocity.y;
            target->position.y = fmax(radius, fmin(WINDOW_HEIGHT - radius, target->position.y));
        }
        
        // More realistic momentum with slight acceleration/deceleration
        float momentum = 0.9985f + (float)random_range(0, 30) / 10000.0f;
        target->velocity.x *= momentum;
        target->velocity.y *= momentum;
        
        // Add subtle random direction changes for more organic movement
        if (random_range(0, 1000) < 3) { // 0.3% chance per frame
            float direction_change = (float)random_range(-10, 10) * M_PI / 180.0f;
            float current_speed = sqrt(target->velocity.x * target->velocity.x + target->velocity.y * target->velocity.y);
            float current_angle = atan2(target->velocity.y, target->velocity.x) + direction_change;
            target->velocity.x = cos(current_angle) * current_speed;
            target->velocity.y = sin(current_angle) * current_speed;
        }
    }
    
    float life_time = sfTime_asSeconds(sfClock_getElapsedTime(target->clock));
    if (life_time > 3.0f && target->anim_state == ANIM_RUNNING)
        target->alive = 0;
    
    // Remove dead targets after death animation
    if (target->anim_state == ANIM_DEAD)
        target->alive = 0;
}

void draw_target(Game *game, Target *target)
{
    if (!target->alive)
        return;
        
    if (target->anim_state == ANIM_RUNNING) {
        draw_stickman_running(game, target);
    } else if (target->anim_state == ANIM_DYING || target->anim_state == ANIM_DEAD) {
        draw_stickman_dying(game, target);
    }
}

// Fonction utilitaire (déjà écrite pour dying)
void draw_limb(Game *game, sfVector2f pos, sfVector2f size, sfColor color, sfVector2f origin, float rotation)
{
    sfRectangleShape *limb = sfRectangleShape_create();
    sfRectangleShape_setSize(limb, size);
    sfRectangleShape_setFillColor(limb, color);
    sfRectangleShape_setOrigin(limb, origin);
    sfRectangleShape_setPosition(limb, pos);
    sfRectangleShape_setRotation(limb, rotation);
    sfRenderWindow_drawRectangleShape(game->window, limb, NULL);
    sfRectangleShape_destroy(limb);
}

void draw_stickman_running(Game *game, Target *target)
{
    sfVector2f pos = target->position;
    float size = STICKMAN_SIZE * target->size_multiplier;
    sfColor color = target->color;
    
    float cycle = target->run_cycle * 6; 
    
    float body_lean = sin(cycle) * 12;
    float bob = sin(cycle * 2) * 3;
    float shoulder_roll = sin(cycle * 1.5f) * 5;
    float flip = target->facing_right ? 1 : -1;
    
    float leg1_base = sin(cycle) * 40;
    float leg2_base = sin(cycle + M_PI) * 40;
    float leg1_angle = leg1_base + sin(cycle * 2) * 5;
    float leg2_angle = leg2_base + sin((cycle + M_PI) * 2) * 5;
    
    float arm1_angle = sin(cycle + M_PI) * 30 + shoulder_roll;
    float arm2_angle = sin(cycle) * 30 - shoulder_roll;
    float torso_twist = sin(cycle * 0.7f) * 3;
    
    // --- Tête ---
    sfCircleShape *head = sfCircleShape_create();
    sfCircleShape_setRadius(head, size * 0.12f);
    sfCircleShape_setFillColor(head, color);
    sfVector2f head_pos = {
        pos.x - size * 0.12f + flip * body_lean * 0.3f,
        pos.y - size * 0.45f - bob
    };
    sfCircleShape_setPosition(head, head_pos);
    sfRenderWindow_drawCircleShape(game->window, head, NULL);
    sfCircleShape_destroy(head);
    
    // --- Corps ---
    float torso_width = 3 + sin(cycle * 0.3f) * 0.5f;
    draw_limb(
        game,
        (sfVector2f){pos.x, pos.y - size * 0.3f - bob},
        (sfVector2f){torso_width, size * 0.35f},
        color,
        (sfVector2f){torso_width/2, 0},
        flip * body_lean * 0.7f + torso_twist
    );
    
    // --- Bras supérieurs ---
    sfVector2f upper_arm1_pos = {
        pos.x + flip * 2,
        pos.y - size * 0.28f - bob + flip * body_lean * 0.1f
    };
    draw_limb(game, upper_arm1_pos, (sfVector2f){2, size * 0.18f}, color, (sfVector2f){1, 0}, flip * arm1_angle);
    
    sfVector2f upper_arm2_pos = {
        pos.x + flip * 2,
        pos.y - size * 0.28f - bob + flip * body_lean * 0.1f
    };
    draw_limb(game, upper_arm2_pos, (sfVector2f){2, size * 0.18f}, color, (sfVector2f){1, 0}, flip * arm2_angle);
    
    // --- Avant-bras ---
    float upper_arm1_end_x = upper_arm1_pos.x + flip * sin(flip * arm1_angle * M_PI / 180) * size * 0.18f;
    float upper_arm1_end_y = upper_arm1_pos.y + cos(flip * arm1_angle * M_PI / 180) * size * 0.18f;
    float forearm1_angle = arm1_angle + sin(cycle + M_PI/4) * 20 + sin(cycle * 3) * 5;
    draw_limb(game, (sfVector2f){upper_arm1_end_x, upper_arm1_end_y}, (sfVector2f){2, size * 0.15f}, color, (sfVector2f){1, 0}, flip * forearm1_angle);
    
    float upper_arm2_end_x = upper_arm2_pos.x + flip * sin(flip * arm2_angle * M_PI / 180) * size * 0.18f;
    float upper_arm2_end_y = upper_arm2_pos.y + cos(flip * arm2_angle * M_PI / 180) * size * 0.18f;
    float forearm2_angle = arm2_angle + sin(cycle + M_PI + M_PI/4) * 20 + sin((cycle + M_PI) * 3) * 5;
    draw_limb(game, (sfVector2f){upper_arm2_end_x, upper_arm2_end_y}, (sfVector2f){2, size * 0.15f}, color, (sfVector2f){1, 0}, flip * forearm2_angle);
    
    // --- Cuisses ---
    sfVector2f thigh1_pos = {pos.x, pos.y + size * 0.05f - bob};
    draw_limb(game, thigh1_pos, (sfVector2f){3, size * 0.2f}, color, (sfVector2f){1.5f, 0}, flip * leg1_angle);
    
    float thigh1_end_x = pos.x + flip * sin(flip * leg1_angle * M_PI / 180) * size * 0.2f;
    float thigh1_end_y = pos.y + size * 0.05f - bob + cos(flip * leg1_angle * M_PI / 180) * size * 0.2f;
    float knee_bend1 = sin(cycle + M_PI/2) * 25;
    if (sin(cycle) < -0.7f) knee_bend1 += 15;
    float lower_leg1_angle = leg1_angle + knee_bend1;
    draw_limb(game, (sfVector2f){thigh1_end_x, thigh1_end_y}, (sfVector2f){2, size * 0.2f}, color, (sfVector2f){1, 0}, flip * lower_leg1_angle);
    
    sfVector2f thigh2_pos = {pos.x, pos.y + size * 0.05f - bob};
    draw_limb(game, thigh2_pos, (sfVector2f){3, size * 0.2f}, color, (sfVector2f){1.5f, 0}, flip * leg2_angle);
    
    float thigh2_end_x = pos.x + flip * sin(flip * leg2_angle * M_PI / 180) * size * 0.2f;
    float thigh2_end_y = pos.y + size * 0.05f - bob + cos(flip * leg2_angle * M_PI / 180) * size * 0.2f;
    float knee_bend2 = sin(cycle + M_PI + M_PI/2) * 25;
    if (sin(cycle + M_PI) < -0.7f) knee_bend2 += 15;
    float lower_leg2_angle = leg2_angle + knee_bend2;
    draw_limb(game, (sfVector2f){thigh2_end_x, thigh2_end_y}, (sfVector2f){2, size * 0.2f}, color, (sfVector2f){1, 0}, flip * lower_leg2_angle);
}

// void draw_limb(Game *game, sfVector2f pos, sfVector2f size, sfColor color, sfVector2f origin, float rotation)
// {
//     sfRectangleShape *limb = sfRectangleShape_create();
//     sfRectangleShape_setSize(limb, size);
//     sfRectangleShape_setFillColor(limb, color);
//     sfRectangleShape_setOrigin(limb, origin);
//     sfRectangleShape_setPosition(limb, pos);
//     sfRectangleShape_setRotation(limb, rotation);
//     sfRenderWindow_drawRectangleShape(game->window, limb, NULL);
//     sfRectangleShape_destroy(limb);
// }

void draw_stickman_dying(Game *game, Target *target)
{
    sfVector2f pos = target->position;
    float size = STICKMAN_SIZE * target->size_multiplier;
    sfColor color = target->color;
    
    float fade = 1.0f - (target->death_timer / DEATH_ANIMATION_TIME);
    color.a = (sfUint8)(fade * 255);
    
    float death_progress = target->death_timer / DEATH_ANIMATION_TIME;
    float fall_offset = death_progress * size * 0.3f;
    float rotation = death_progress * 90; // Rotate 90 degrees during death
    
    // --- Tête ---
    sfCircleShape *head = sfCircleShape_create();
    sfCircleShape_setRadius(head, size * 0.15f);
    sfCircleShape_setFillColor(head, color);
    sfVector2f head_pos = {pos.x - size * 0.15f, pos.y - size * 0.4f + fall_offset};
    sfCircleShape_setPosition(head, head_pos);
    sfRenderWindow_drawCircleShape(game->window, head, NULL);
    sfCircleShape_destroy(head);
    
    // --- Corps ---
    draw_limb(
        game,
        (sfVector2f){pos.x, pos.y + fall_offset},
        (sfVector2f){3, size * 0.4f},
        color,
        (sfVector2f){1.5f, size * 0.2f},
        rotation
    );
    
    // --- Bras ---
    draw_limb(
        game,
        (sfVector2f){pos.x, pos.y - size * 0.15f + fall_offset},
        (sfVector2f){3, size * 0.25f},
        color,
        (sfVector2f){1.5f, 0},
        -45 + rotation
    );
    
    draw_limb(
        game,
        (sfVector2f){pos.x, pos.y - size * 0.15f + fall_offset},
        (sfVector2f){3, size * 0.25f},
        color,
        (sfVector2f){1.5f, 0},
        45 + rotation
    );
    
    // --- Jambes ---
    draw_limb(
        game,
        (sfVector2f){pos.x - 3, pos.y + size * 0.2f + fall_offset},
        (sfVector2f){3, size * 0.3f},
        color,
        (sfVector2f){1.5f, 0},
        -30 + rotation
    );
    
    draw_limb(
        game,
        (sfVector2f){pos.x + 3, pos.y + size * 0.2f + fall_offset},
        (sfVector2f){3, size * 0.3f},
        color,
        (sfVector2f){1.5f, 0},
        30 + rotation
    );
}

void kill_target(Target *target)
{
    if (target->anim_state == ANIM_RUNNING) {
        target->anim_state = ANIM_DYING;
        target->death_timer = 0;
        target->velocity.x = 0;
        target->velocity.y = 0;
    }
}

int is_point_in_circle(float px, float py, float cx, float cy, float radius)
{
    float dx = px - cx;
    float dy = py - cy;
    return (dx * dx + dy * dy <= radius * radius);
}

int is_target_clicked(Target *target, int x, int y)
{
    if (!target->alive || target->anim_state != ANIM_RUNNING)
        return (0);
        
    sfVector2f pos = target->position;
    float size = STICKMAN_SIZE * target->size_multiplier;
    float cycle = target->run_cycle * 8;
    
    float body_lean = sin(cycle) * 8;
    float bob = sin(cycle * 2) * 2;
    
    float flip = target->facing_right ? 1 : -1;
    
    sfVector2f head_render_pos = {
        pos.x - size * 0.12f + flip * body_lean * 0.3f,  // Top-left corner X (same as rendering)
        pos.y - size * 0.45f - bob                       // Top-left corner Y (same as rendering)
    };
    
    sfVector2f head_center = {
        head_render_pos.x + size * 0.12f,  // Center X = corner X + radius
        head_render_pos.y + size * 0.12f   // Center Y = corner Y + radius
    };
    
    return is_point_in_circle(x, y, head_center.x, head_center.y, size * 0.13f);
}

void reset_target(Target *target)
{
    target->position = (sfVector2f){0, 0};
    target->velocity = (sfVector2f){0, 0};
    target->alive = 0;
    target->animation_time = 0;
    target->size_multiplier = 1.0f;
    target->color = sfWhite;
    target->clock = NULL;
    target->anim_state = ANIM_RUNNING;
    target->death_timer = 0;
    target->run_cycle = 0;
    target->facing_right = 1;
}