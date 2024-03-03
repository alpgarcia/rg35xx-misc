#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>


const int WIDTH = 640;
const int HEIGHT = 480;
const int BORDER = 40;
const int COLOR_ROW = 8;


// Funcion que redimensiona una imagen dada a 640x480
SDL_Surface *resize (SDL_Surface * img, int w, int h) {
  // Comprueba si la imagen ya tiene el tamaño deseado
  if (img->w == w && img->h == h) {
    return img;
  }

  // Crea una nueva superficie con el tamaño deseado
  SDL_Surface *resized = SDL_CreateRGBSurface (
      SDL_SWSURFACE, w, h, img->format->BitsPerPixel, 0, 0, 0, 0);

  printf("w: %d, h: %d\n", resized->w, resized->h);

  // Comprueba si la nueva superficie se ha creado correctamente
  if (resized == NULL) {
    printf ("Error al crear la nueva superficie: %s\n", SDL_GetError ());
    return NULL;
  }

  // Redimensiona la imagen
  if (SDL_SoftStretch (img, NULL, resized, NULL) != 0) {
    printf ("Error al redimensionar la imagen: %s\n", SDL_GetError ());
    return NULL;
  }

  // Libera la memoria de la imagen original
  // SDL_FreeSurface (img);

  // Usa la nueva imagen redimensionada
  // img = resized;

  // printf("w: %d, h: %d\n", img->w, img->h);

  return resized;
}

// Funcion que convierte una imagen a la profundidad de color de la pantalla
SDL_Surface *convertToScreen (SDL_Surface * img, SDL_Surface * screen) {
  // Comprueba si la imagen ya tiene la profundidad de color de la pantalla
  if (img->format->BitsPerPixel == screen->format->BitsPerPixel) {
    return img;
  }

  // Crea una nueva superficie con la profundidad de color de la pantalla
  SDL_Surface *converted = SDL_ConvertSurface (img, screen->format, 0);

  // Comprueba si la nueva superficie se ha creado correctamente
  if (converted == NULL) {
    printf ("Error al convertir la imagen: %s\n", SDL_GetError ());
    return NULL;
  }

  // Libera la memoria de la imagen original
  // SDL_FreeSurface (img);

  // Usa la nueva imagen convertida
  // img = converted;

  // printf("img->format->BitsPerPixel 3: %d\n", img->format->BitsPerPixel);

  return converted;
}

SDL_Surface *blackAndWhite (SDL_Surface * img) {

  SDL_Surface *img_bw = SDL_CreateRGBSurface (
      SDL_SWSURFACE, img->w, img->h, img->format->BitsPerPixel, 0, 0, 0, 0);

  SDL_LockSurface(img);
  SDL_LockSurface(img_bw);
  
  Uint32 pixelCount = img->w * img->h;
  Uint32* src_pixels = (Uint32*)img->pixels;
  Uint32* dest_pixels = (Uint32*)img_bw->pixels;

  for (Uint32 i = 0; i < pixelCount; ++i) {
      Uint8 r, g, b;
      SDL_GetRGB(src_pixels[i], img->format, &r, &g, &b);
      Uint8 gray = 0.299*r + 0.587*g + 0.114*b;
      dest_pixels[i] = SDL_MapRGB(img_bw->format, gray, gray, gray);
  }

  SDL_UnlockSurface(img);
  SDL_UnlockSurface(img_bw);

  return img_bw;

}


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    SDL_Surface *screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, 
                                           SDL_FULLSCREEN | SDL_HWSURFACE);
    // SDL_Surface *screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE);

    SDL_Surface *buffer = SDL_CreateRGBSurface(
        SDL_SWSURFACE, WIDTH, HEIGHT, 32, 0, 0, 0, 0);

    SDL_Surface *img_orig = IMG_Load("jetpack.jpg");

    if (img_orig == NULL) {
        printf ("Error al cargar la imagen: %s\n", IMG_GetError ());
        return 1;

    } else {
        SDL_Surface *temp = SDL_DisplayFormat(img_orig);
        SDL_FreeSurface(img_orig);
        img_orig = temp;
    }

    
    // Convierte la imagen a la profundidad de color de la pantalla
    SDL_Surface *img = convertToScreen(img_orig, screen);
    //SDL_FreeSurface (img_orig);
    if (img == NULL) {
        return 1;
    }

    printf("img->format->BitsPerPixel: %d\n", img->format->BitsPerPixel);

    // Redimensiona la imagen
    img = resize(img, buffer->w - (BORDER * 2), buffer->h - (BORDER * 2));
    if (img == NULL) {
        return 1;
    }

    printf("w: %d, h: %d\n", img->w, img->h);
    
    // Convierte el color de la imagen a blanco y negro
    SDL_Surface *img_bw = blackAndWhite(img);
    if (img_bw == NULL) {
        return 1;
    }


    SDL_Rect img_rect;
    img_rect.x = 0;
    img_rect.y = 0;
    img_rect.w = img->w;
    img_rect.h = 1;
    SDL_Rect target_rect;
    target_rect.x = (buffer->w - img->w) / 2;
    target_rect.y = (buffer->h - img->h) / 2;
    target_rect.w = img->w;
    target_rect.h = 1;

    SDL_Rect topBar;
    topBar.x = 0;
    topBar.y = 0;
    topBar.w = buffer->w;

    SDL_Rect sideBar;
    sideBar.x = 0;
    sideBar.y = BORDER + 1;
    sideBar.w = BORDER;

    int colorSwitch = 0;

    bool running = true;
    SDL_Event event;

    int fps = 0;
    int frameCount = 0;
    Uint32 fpsTimer = 0;

    int i = 0;
    int third = img_bw->h / 3;
    int current_third = 0;
    int n_row = 0;
    bool bw = true;

    while (running) {

        // Alternar entre azul y amarillo
        Uint32 color = (colorSwitch % 2 == 0) ? SDL_MapRGB(buffer->format, 0, 0, 255) : SDL_MapRGB(buffer->format, 255, 255, 0);

        // calcula un valor random entre 1 y 5 para la altura de la barra
        int h = rand() % 7 + 4;
        topBar.h = h;
        sideBar.h = h;

        // Limitar la altura de la barra al ancho del borde
        if ((topBar.y + topBar.h) > BORDER) {
            topBar.h = BORDER - topBar.y;
        }

        if (topBar.y < BORDER) {

            // Limitar la altura de la barra al ancho del borde
            SDL_FillRect(buffer, &topBar, color);

            SDL_Rect bottomBar;
            bottomBar.x = topBar.x;
            bottomBar.y = (buffer->h) - BORDER + topBar.y;
            bottomBar.w = topBar.w;
            bottomBar.h = topBar.h;
            SDL_FillRect(buffer, &bottomBar, color);

            // Mover la barra hacia abajo
            topBar.y += topBar.h;

        } else {
            // Reiniciar la posición de la barra a la parte superior de la pantalla
            topBar.y = 0; 
        }

        // Al igual que la top y la bottom bar, pintamos por bloques de 50px de alto
        // Si se sobrrepasa y=100 (50 de la top bar más 50 del bloque inicial de la side bar),
        // se ajusta la altura del bloque para que termine en y=100.
        if ((sideBar.y + sideBar.h) > (BORDER * 2)) {
            sideBar.h = (BORDER * 2) - sideBar.y;
        }

        // Mientras estemos dentro del primer bloque que va de y=50 a y=100, 
        // pintamos lo mismo en cada bloque de 50px de la side bar a ambos lados de la
        // pantalla.
        if (sideBar.y < (BORDER * 2)) {

            for (int y = sideBar.y; y < buffer->h - BORDER; y += BORDER) {

                SDL_Rect nextLeftBarPart;
                nextLeftBarPart.x = sideBar.x;
                nextLeftBarPart.y = y;
                nextLeftBarPart.w = sideBar.w;
                nextLeftBarPart.h = sideBar.h;
                SDL_FillRect(buffer, &nextLeftBarPart, color);
            
                SDL_Rect nextRightBarPart;
                nextRightBarPart.x = buffer->w - sideBar.w;
                nextRightBarPart.y = y;
                nextRightBarPart.w = sideBar.w;
                nextRightBarPart.h = sideBar.h;
                SDL_FillRect(buffer, &nextRightBarPart, color);
            
            }

            sideBar.y += sideBar.h;

        } else {

            // Si ya hemos pintado todos los bloques de 50px de la side bar, 
            // volvemos a la posición inicial
            sideBar.y = BORDER + 1;
        }

        // Pintamos la imagen central
        if (bw &&
            (i > (buffer->h - img_bw->h) / 2) && 
            (img_rect.y < img_bw->h)) { 

            SDL_BlitSurface(img_bw, &img_rect, buffer, &target_rect);

            // std::cout << "i - BORDER: " << i - BORDER << std::endl;
            // std::cout << "rect y: " << img_rect.y << std::endl;
            // std::cout << "third: " << third << std::endl;
            // std::cout << "current third: " << current_third << std::endl;

            // Siguiente línea
            img_rect.y = img_rect.y + 8;
            target_rect.y = target_rect.y + 8;

            if ((i - BORDER - 1) % third == 0) {
                
                // Cambio de tercio
                n_row = (third * current_third) + 1;
                img_rect.y = n_row;
                target_rect.y = n_row + ((buffer->w - img_bw->w) / 2);
                
                current_third++;
                
            } else if (img_rect.y > third * current_third) {
                
                // Cambio de fila dentro del tercio
                n_row++;
                img_rect.y = n_row;
                target_rect.y = n_row + ((buffer->w - img_bw->w) / 2);

            }
            
        }

        // std::cout << "i: " << i << std::endl;
        if (bw && i < buffer->h - BORDER) {
            i++;
        
        } else if (bw && i == buffer->h - BORDER) {
            i = BORDER;
            img_rect.y = 0;
            target_rect.y = (buffer->w - img->w) / 2;
            img_rect.h = COLOR_ROW;
            target_rect.h = COLOR_ROW;
            bw = false;
        }

        // Coloreamos la imagen
        if (!bw && i < buffer->h - BORDER) {
            SDL_BlitSurface(img, &img_rect, buffer, &target_rect);
            i += COLOR_ROW;
            img_rect.y += COLOR_ROW;
            target_rect.y += COLOR_ROW;
        }

        // SDL_Delay(5); // Ajusta este valor para controlar la velocidad de carga de la imagen
        colorSwitch++;

        // Wait for key press
        while (SDL_PollEvent(&event)) {
            
            if (event.type == SDL_QUIT) {
                running = false;
            
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                
                case SDLK_ESCAPE:
                    running = false;
                    break;
                
                default:
                    printf("Press Esc to exit.\n");
                    break;
                
                }
            }
        }

        // Handle FPS information
        if (SDL_GetTicks() - fpsTimer >= 1000) {
            fps = frameCount;
            frameCount = 0;
            fpsTimer = SDL_GetTicks();
        }

        SDL_BlitSurface(buffer, NULL, screen, NULL);
        SDL_Flip(screen);

        printf("%d\n", fps);

        frameCount++;
    }

    SDL_FreeSurface(img);
    SDL_FreeSurface(img_bw);
    SDL_FreeSurface(buffer);
    SDL_FreeSurface(screen);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
