#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define WIDTH 640
#define HEIGHT 480
#define IMG_PATH "image.jpg"
#define DELAY 50

// Funcion que redimensiona una imagen dada a 640x480
SDL_Surface *resize (SDL_Surface * img) {
  // Comprueba si la imagen ya tiene el tamaño deseado
  if (img->w == 640 && img->h == 480) {
    return img;
  }

  // Crea una nueva superficie con el tamaño deseado
  SDL_Surface *resized = SDL_CreateRGBSurface (
      SDL_SWSURFACE, 640, 480, img->format->BitsPerPixel, 0, 0, 0, 0);

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

/**
 * Dibuja una imagen en la pantalla
 * 
 * @param img La imagen a dibujar
 * @param screen La pantalla en la que dibujar la imagen
 * @param row_h La altura en píxels de cada fila. Debe ser múltiplo de el ancho de cada línea
 *  (una fila contiene n líneas, donde se ha de cumplir que row_h = n * line_w).
 * @param line_w El ancho en píxels de cada línea. Debe ser menor o igual que row_h.
 * @param from_y La línea desde la que empezar a dibujar
 * @param to_y La línea en la que terminar de dibujar
 
*/
void drawImage(SDL_Surface* img, SDL_Surface* screen, int row_h, int line_w, int from_y, int to_y) {
    // Obtiene el ancho y el alto de la imagen
    int w = img->w;
    int h = img->h;

    printf("w: %d\n", w);
    printf("h: %d\n", h);
    printf("img->format->BitsPerPixel: %d\n", img->format->BitsPerPixel);

    // Crea un rectángulo con las coordenadas y el tamaño de la imagen
    SDL_Rect tgt_rect;
    tgt_rect.x = (WIDTH - w) / 2;
    tgt_rect.y = (HEIGHT - h) / 2;
    tgt_rect.w = w;
    tgt_rect.h = h;

    SDL_Rect src_rect;
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = w;
    src_rect.h = h;

    // Recorre las líneas (de ancho line_w)
    for (int i = 0; i < row_h; i += line_w) {
      // Itera sobre las filas (de altura row_h) (cada fila tiene row_h líneas)
      for (int j = from_y + i; j < to_y; j += row_h) {
        src_rect.y = j;
        src_rect.h = line_w;
        tgt_rect.y = ((HEIGHT - h) / 2) + j;
        tgt_rect.h = line_w;

        SDL_BlitSurface(img, &src_rect, screen, &tgt_rect);

        // Actualiza la pantalla
        // SDL_Flip(screen);
        SDL_UpdateRect(screen, tgt_rect.x, tgt_rect.y, tgt_rect.w, tgt_rect.h);

        // Añade un retraso
        //SDL_Delay(DELAY);
      }
    }
  }

int main (int argc, char *argv []) {
  
  // Inicializa SDL y SDL_image
  SDL_Init (SDL_INIT_VIDEO);
  IMG_Init (IMG_INIT_PNG);

  // Crea una superficie que representa la pantalla
  SDL_Surface *screen = SDL_SetVideoMode (WIDTH, HEIGHT, 32, SDL_SWSURFACE);

  // Carga la imagen
  SDL_Surface *img_orig = IMG_Load (IMG_PATH);

  // Comprueba si la imagen se ha cargado correctamente
  if (img_orig == NULL) {
    printf ("Error al cargar la imagen: %s\n", IMG_GetError ());
    return 1;
  }

  // Convierte la imagen a la profundidad de color de la pantalla
  SDL_Surface *img = convertToScreen(img_orig, screen);
  if (img == NULL) {
    return 1;
  }

  printf("img->format->BitsPerPixel: %d\n", img->format->BitsPerPixel);

  // Redimensiona la imagen
  img = resize(img);
  if (img == NULL) {
    return 1;
  }

  printf("w: %d, h: %d\n", img->w, img->h);
  
  // Convierte el color de la imagen a blanco y negro
  SDL_Surface *img_bw = blackAndWhite(img);
  if (img_bw == NULL) {
    return 1;
  }

  // Draw b&w image to the screen
  int third = img_bw->h / 3;
  for (int i = 0; i < img_bw->h; i += third) {
    drawImage(img_bw, screen, 8, 1, i, i + third);
  }

  // Draw color image to the screen
  drawImage(img, screen, 8, 8, 0, img->h);

  // Espera a que se pulse una tecla
  //SDL_Event event;
  //SDL_WaitEvent (&event);

  // Libera la memoria de la imagen
  SDL_FreeSurface (img_orig);
  SDL_FreeSurface (img);
  SDL_FreeSurface (img_bw);

  // Finaliza SDL y SDL_image
  IMG_Quit ();
  SDL_Quit ();

  return 0;
}