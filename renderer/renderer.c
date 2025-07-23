#define SDL_MAIN_USE_CALLBACKS 1 // Use callbacks instead of main()

#include "../parser/html-parser.h"
#include "../parser/css-parser.h"
#include "../utils/fetch.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
HTMLNode *DOM = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) { // This function runs once on start 
    SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("bem", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    const char *url = "https://www.tuhs.org/";
	char *htmlInput = NULL;
	char *cssInput = NULL;

	int res = fetchHTMLAndCSS(url, &htmlInput, &cssInput);
	if (res == 0) {
		extractMetaData(&htmlInput);
	}

	DOM = parser(htmlInput);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) { // This function runs when an event occurs (mouse input, key presses, etc...)
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}


// TODO: Add browser default CSSOM
static int yPos = 0;
void renderNode(HTMLNode* node) {
	switch(node->type) {
		case TEXT:
            SDL_RenderDebugText(renderer, 0, yPos, node->content);
            yPos += 7;
			break;
		default:
			break;
	}

	for (size_t i = 0; i < node->childCount; i++) {
		renderNode(node->children[i]);
	}
}

SDL_AppResult SDL_AppIterate(void *appstate) { // This function runs once per frame.
    // const double now = ((double)SDL_GetTicks()) / 1000.0;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer); // Clear window using draw color

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    yPos = 0;
    renderNode(DOM);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) { // This function runs once on app quit.
    // SDL will clean up the window/renderer for us.
}
