#include "duck_hunt.h"

int main(void)
{
    Game game;
    sfEvent event;
    
    if (!init_game(&game))
        return (84);
    
    while (sfRenderWindow_isOpen(game.window))
    {
        while (sfRenderWindow_pollEvent(game.window, &event))
        {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_setMouseCursorVisible(game.window, sfTrue);
                sfRenderWindow_close(game.window);
            }
            if (event.type == sfEvtMouseButtonPressed) {
                if (game.state == STATE_PLAYING || game.state == STATE_GAME_OVER)
                    handle_mouse_click(&game, event.mouseButton.x, event.mouseButton.y);
                else if (game.state == STATE_MENU)
                    handle_menu_mouse_click(&game, event.mouseButton.x, event.mouseButton.y);
            }
            if (event.type == sfEvtMouseMoved) {
                game.mouse_position.x = event.mouseMove.x;
                game.mouse_position.y = event.mouseMove.y;
                if (game.state == STATE_MENU)
                    handle_menu_mouse_move(&game, event.mouseMove.x, event.mouseMove.y);
            }
            if (event.type == sfEvtKeyPressed && game.state == STATE_MENU)
                handle_menu_input(&game, event.key.code);
            if (event.type == sfEvtKeyPressed && game.state == STATE_COUNTDOWN && !game.countdown_started)
                game.countdown_started = 1;
            if (event.type == sfEvtKeyPressed && game.state == STATE_GAME_OVER && 
                event.key.code == sfKeySpace) {
                game.state = STATE_MENU;
                sfRenderWindow_setMouseCursorVisible(game.window, sfTrue);
            }
        }
        
        if (game.state == STATE_MENU)
            update_menu(&game);
        else
            update_game(&game);
            
        render_game(&game);
    }
    
    cleanup_game(&game);
    return (0);
}