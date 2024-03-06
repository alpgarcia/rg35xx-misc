#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <iostream>
#include <fstream>

const int BTN_A = 0;

const int WIDTH = 640;
const int HEIGHT = 480;
const int BORDER = 40;
const int COLOR_ROW_H = 8;
const int COLOR_DEPTH = 32;
const char* FONT = "zxSpectrumStrict.ttf";
const int FONT_SIZE = 16;


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

SDL_Surface *blackAndWhite (SDL_Surface * img, int threshold) {

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
        
        // comprobamos si está más cerca de 0 o de 255
        gray = (gray < threshold) ? 0 : 255;

        dest_pixels[i] = SDL_MapRGB(img_bw->format, gray, gray, gray);
    }

    SDL_UnlockSurface(img);
    SDL_UnlockSurface(img_bw);

    return img_bw;

}

void printFPS(TTF_Font* font, int fps, int current_process, int completed, int total, std::string message, SDL_Surface* buffer, Uint32 bg_color) {
    
    Sint16 x = BORDER;
    Sint16 y = HEIGHT - BORDER - FONT_SIZE;
    Uint16 bar_height = FONT_SIZE;
    Uint16 bar_len = WIDTH - (BORDER * 2);
    Uint16 bar_progress = total == 0 ? 0 : (bar_len * completed) / total;

    // Draw progress bar
    SDL_Rect bar_rect = {x, y, bar_len, bar_height};
    SDL_FillRect(buffer, &bar_rect, bg_color);
    
    SDL_Rect progress_rect = {x, y, bar_progress, bar_height};
    SDL_FillRect(buffer, &progress_rect, SDL_MapRGB(buffer->format, 0, 207, 21));

    std::string msg =  "[" + std::to_string(current_process) + "/" + std::to_string(total) + "] " + message +
                       " -- FPS: " + std::to_string(fps);
    
    // Truncate line if it's too long
    if (msg.length() > 65) {
        msg = msg.substr(0, 62) + "...";
    }

    SDL_Surface* raw_text_surface = TTF_RenderText_Blended(
        font, msg.c_str(), {0, 0, 0});
    
    if (!raw_text_surface) {
        return;
    }

    SDL_Surface* text_surface = SDL_DisplayFormatAlpha(raw_text_surface);
    SDL_FreeSurface(raw_text_surface);

    if(!text_surface) {
        return;
    }

    SDL_Rect msg_rect = {(static_cast<Sint16>(x + 5)), y, 0, 0}; 
    SDL_BlitSurface(text_surface, NULL, buffer, &msg_rect);

    SDL_FreeSurface(text_surface);
}


int main(int argc, char *argv[])
{

    // Comprobamos que la llamada incluya la ruta de la imagen
    // y la del fichero de estado
    if (argc != 4) {
        printf ("Usage %s <imagen> <status_file> <bw_threshold>\n", argv[0]);
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    IMG_Init(IMG_INIT_JPG);

    printf("Joysticks: %d\n", SDL_NumJoysticks());
    SDL_Joystick *joystick = NULL;
    if (SDL_NumJoysticks() > 0) {
        joystick = SDL_JoystickOpen(0);
        SDL_JoystickEventState(SDL_ENABLE);
    }
    

    // SDL_Surface *screen = SDL_SetVideoMode(WIDTH, HEIGHT, COLOR_DEPTH, 
    //                                        SDL_FULLSCREEN | SDL_HWSURFACE);
    SDL_Surface *screen = SDL_SetVideoMode(WIDTH, HEIGHT, COLOR_DEPTH, SDL_HWSURFACE);

    SDL_Surface *buffer = SDL_CreateRGBSurface(
        SDL_SWSURFACE, WIDTH, HEIGHT, COLOR_DEPTH, 0, 0, 0, 0);

    const Uint32 BG_COLOR = SDL_MapRGB(buffer->format, 207, 207, 207);

    SDL_Surface *img_orig = IMG_Load(argv[1]);

    if (img_orig == NULL) {
        printf ("Error al cargar la imagen: %s\n", IMG_GetError ());
        return 1;

    } else {
        SDL_Surface *temp = SDL_DisplayFormat(img_orig);
        SDL_FreeSurface(img_orig);
        img_orig = temp;
    }

    // Inicializamos la fuente
    if (TTF_Init() == -1) {
        std::cerr << "Unable to initialize TTF: " << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    TTF_Font* font = TTF_OpenFont(FONT, FONT_SIZE);
    // or TTF_HINTING_LIGHT, TTF_HINTING_MONO, TTF_HINTING_NONE
    TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
    // 1 to enable, 0 to disable
    TTF_SetFontKerning(font, 1); 

    // Ocultamos el cursor
    SDL_ShowCursor(SDL_DISABLE);

    
    // Convierte la imagen a la profundidad de color de la pantalla
    SDL_Surface *img = convertToScreen(img_orig, screen);
    //SDL_FreeSurface (img_orig);
    if (img == NULL) {
        return 1;
    }

    printf("img->format->BitsPerPixel: %d\n", img->format->BitsPerPixel);

    // Redimensiona la imagen
    img = resize(img, 
                 buffer->w - (BORDER * 2), 
                 buffer->h - (BORDER * 2) - FONT_SIZE);
    if (img == NULL) {
        return 1;
    }

    printf("w: %d, h: %d\n", img->w, img->h);
    
    // Convierte el color de la imagen a blanco y negro
    SDL_Surface *img_bw = blackAndWhite(img, std::stoi(argv[3]));
    if (img_bw == NULL) {
        return 1;
    }

    SDL_Rect img_rect;
    img_rect.x = 0;
    img_rect.y = 0;
    img_rect.w = img->w;
    img_rect.h = 1;
    SDL_Rect target_rect;
    target_rect.x = BORDER;
    target_rect.y = BORDER;
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
    Uint32 readTimer = 0;

    int i = 0;
    int third = img_bw->h / 3;
    int current_third = 0;
    int n_row = 0;
    bool bw = true;

    std::ifstream status_file(argv[2]);
    std::string status_message = "Loading, please wait...";
    int nscripts = 0;
    int current_process = 0;
    int completed = 0;

    if (!status_file) {
         printf ("Error leer el fichero de estado.\n");
        return 1;
    }

    // Pintamos el fondo al estilo zx spectrum
    SDL_Rect bg_rect = {0, 0, WIDTH, HEIGHT};
    SDL_FillRect(buffer, &bg_rect, BG_COLOR);

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

        // Pintamos la imagen central en blanco y negro si:
        //  1. el flag de blanco y negro está activo
        //  2. la linea actual por la que vamos pintando es mayor que el
        //     ancho del borde
        //  3. la fila de la imagen que vamos a pintar es menor que la altura
        //     de la imagen que estamos pintando
        if (bw &&
            i > BORDER && 
            img_rect.y <= img_bw->h) { 

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
                if (current_third > 0) {
                    n_row = (third * current_third) + 1;
                    img_rect.y = n_row;
                    target_rect.y = n_row + BORDER;
                }
                current_third++;
                
            } else if (img_rect.y > third * current_third) {
                
                // Cambio de fila dentro del tercio
                n_row++;
                img_rect.y = n_row;
                target_rect.y = n_row + BORDER;

            }

            // std::cout << "rect y AFTER: " << img_rect.y << std::endl;
            
        }

        // std::cout << "i: " << i << std::endl;
        if (bw && i < buffer->h - BORDER - FONT_SIZE) {
            i++;
        
        } else if (bw && i == buffer->h - BORDER - FONT_SIZE) {
            i = BORDER;
            img_rect.y = 0;
            target_rect.y = BORDER;
            img_rect.h = COLOR_ROW_H;
            target_rect.h = COLOR_ROW_H;
            bw = false;
        }

        // Coloreamos la imagen
        if (!bw && i < buffer->h - BORDER - FONT_SIZE) {
            SDL_BlitSurface(img, &img_rect, buffer, &target_rect);
            i += COLOR_ROW_H;
            img_rect.y += COLOR_ROW_H;
            target_rect.y += COLOR_ROW_H;
        }

        // SDL_Delay(5); // Ajusta este valor para controlar la velocidad de carga de la imagen
        colorSwitch++;

        // Wait for key press
        while (SDL_PollEvent(&event)) {

            // printf("event.type: %d\n", event.type);
            
            if (event.type == SDL_QUIT) {
                running = false;

            } else if (event.type == SDL_JOYBUTTONDOWN) {

                // printf("Button: %d\n", event.jbutton.button);

                switch (event.jbutton.button) {
                case BTN_A: 
                    running = false;
                    break;

                }
            
            } else if (event.type == SDL_KEYDOWN) {

                // printf("event.key.keysym.sym: %d\n", event.key.keysym.sym);                

                switch (event.key.keysym.sym) {
                
                case SDLK_ESCAPE:
                    running = false;
                    break;
                
                default:
                    // printf("Press Esc to exit.\n");
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

        // Read status file every 2 seconds
        if (SDL_GetTicks() - readTimer >= 2000) {

            std::string line;
            bool read = false;

            // Read all available lines, in case there are more than one
            // The last one tell us the current status of the init process
            while( std::getline( status_file, line ) ) {
                status_message = line;
                read = true;
            }

            // Clear the error state to continue reading next time
            if( status_file.eof() ) {
                status_file.clear() ;
            }

            if (read) {
                // Get the action and update the status message accordingly
                std::string action = status_message.substr(0, status_message.find(" "));
                if (action == "FINISH") {
                    status_message = "Finished!";
                    running = false;

                } else if (action == "SCRIPTS") {
                    // convert the string to an integer
                    nscripts = std::stoi(status_message.substr(status_message.find(" ") + 1));
                    status_message = "Starting system...";
                
                } else if (action == "START") {
                    status_message = status_message.substr(status_message.find(" ") + 1);
                    if (nscripts > 0 && current_process < nscripts) {
                        current_process++;
                    } else {
                        printf("ERROR: declared number of scripts is less than the actual number of scripts executed\n");
                    }

                } else if (action == "END") {
                    status_message = status_message.substr(status_message.find(" ") + 1);
                    if (nscripts > 0 && completed < nscripts) {
                        completed++;
                    } else {
                        printf("ERROR: declared number of scripts is less than the actual number of scripts executed\n");
                    }

                } else if (action == "PRINT") {
                    status_message = status_message.substr(status_message.find(" ") + 1);

                } else {
                    // set to blank when the action is not found or recognized
                    status_message = "";

                }
            }

            readTimer = SDL_GetTicks();
        }

        // printf("%d\n", fps);
        // Clear & Print FPS and status message
        printFPS(font, fps, current_process, completed, nscripts, status_message, buffer, BG_COLOR);

        SDL_BlitSurface(buffer, NULL, screen, NULL);
        SDL_Flip(screen);

        frameCount++;
    }

    status_file.close();
    // Clean status file contents
    std::ofstream ofs;
    ofs.open("init_status.log", std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    if (SDL_NumJoysticks() > 0) {
        SDL_JoystickClose(joystick);
    }

    TTF_CloseFont(font);

    SDL_FreeSurface(img);
    SDL_FreeSurface(img_bw);
    SDL_FreeSurface(buffer);
    SDL_FreeSurface(screen);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
