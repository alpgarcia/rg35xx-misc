#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
    SDL_Surface *image = IMG_Load("image.jpg");

    if (image == NULL)
    {
        std::cout << "No se pudo cargar la imagen." << std::endl;
        return 1;
    }

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = image->w;
    rect.h = 1;

    SDL_Rect topBar;
    topBar.x = 0;
    topBar.y = 0;
    topBar.w = screen->w;

    SDL_Rect sideBar;
    sideBar.x = 0;
    sideBar.y = 51;
    sideBar.w = 50;

    int colorSwitch = 0;

    bool running = true;
    SDL_Event event;

    int i = 0;

    while (running)
    {

        // Alternar entre azul y amarillo
        Uint32 color = (colorSwitch % 2 == 0) ? SDL_MapRGB(screen->format, 0, 0, 255) : SDL_MapRGB(screen->format, 255, 255, 0);

        // calcula un valor random entre 1 y 5
        int h = rand() % 7 + 4; // Altura de la barra
        topBar.h = h;
        sideBar.h = h;

        // Limitar la altura de la barra a 50 píxeles
        if ((topBar.y + topBar.h) > 50)
        {
            topBar.h = 50 - topBar.y;
        }

        if (topBar.y < 50)
        { 
            // Limitar la altura de la barra a 50 píxeles
            SDL_FillRect(screen, &topBar, color);
            SDL_UpdateRect(screen, topBar.x, topBar.y, topBar.w, topBar.h);

            SDL_Rect bottomBar;
            bottomBar.x = topBar.x;
            bottomBar.y = (screen->h) - 50 + topBar.y;
            bottomBar.w = topBar.w;
            bottomBar.h = topBar.h;
            SDL_FillRect(screen, &bottomBar, color);
            SDL_UpdateRect(screen, bottomBar.x, bottomBar.y, bottomBar.w, bottomBar.h);

            topBar.y += topBar.h; // Mover la barra hacia abajo
        }
        else
        {
            topBar.y = 0; // Reiniciar la posición de la barra a la parte superior de la pantalla
        }

        // Al igual que la top y la bottom bar, pintamos por bloques de 50px de alto
        // Si se sobrrepasa y=100 (50 de la top bar más 50 del bloque inicial de la side bar),
        // se ajusta la altura del bloque para que termine en y=100.
        if ((sideBar.y + sideBar.h) > 100)
        {
            sideBar.h = 100 - sideBar.y;
        }

        // Mientras estemos dentro del primer bloque que va de y=50 a y=100, 
        // pintamos lo mismo en cada bloque de 50px de la side bar a ambos lados de la
        // pantalla.
        if (sideBar.y < 100)
        {
            for (int y = sideBar.y; y < screen->h - 50; y += 50)
            {
                SDL_Rect nextLeftBarPart;
                nextLeftBarPart.x = sideBar.x;
                nextLeftBarPart.y = y;
                nextLeftBarPart.w = sideBar.w;
                nextLeftBarPart.h = sideBar.h;
                SDL_FillRect(screen, &nextLeftBarPart, color);
                SDL_UpdateRect(screen, nextLeftBarPart.x, nextLeftBarPart.y, nextLeftBarPart.w, nextLeftBarPart.h);
            
                SDL_Rect nextRightBarPart;
                nextRightBarPart.x = screen->w - sideBar.w;
                nextRightBarPart.y = y;
                nextRightBarPart.w = sideBar.w;
                nextRightBarPart.h = sideBar.h;
                SDL_FillRect(screen, &nextRightBarPart, color);
                SDL_UpdateRect(screen, nextRightBarPart.x, nextRightBarPart.y, 
                               nextRightBarPart.w, nextRightBarPart.h);
            
            }

            sideBar.y += sideBar.h;
        }
        // Si ya hemos pintado todos los bloques de 50px de la side bar, volvemos a la posición inicial
        else
        {
            sideBar.y = 51;
        }

        // Pintamos la imagen central
        if (i <= image->h)
        {
            rect.y = i;

            SDL_BlitSurface(image, &rect, screen, &rect);

            // SDL_Flip(screen);

            SDL_UpdateRect(screen, rect.x, rect.y, rect.w, rect.h);

            i++;
        }

        // SDL_Delay(5); // Ajusta este valor para controlar la velocidad de carga de la imagen
        colorSwitch++;

        // Wait for key press
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                default:
                    std::cout << "Press Esc to exit." << std::endl;
                    break;
                }
            }
        }
    }

    SDL_FreeSurface(image);
    SDL_Quit();

    return 0;
}
