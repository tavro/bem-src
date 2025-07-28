#define SDL_MAIN_USE_CALLBACKS 1 // Use callbacks instead of main()

#include "../parser/html-parser.h"
#include "../parser/css-parser.h"
#include "../utils/fetch.h"
#include "./resources/tiny-ttf.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int scrollOffset = 0;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static TTF_Font *font = NULL;
static TTF_Font *boldFont = NULL;

HTMLNode *DOM = NULL;

static char urlInput[1024] = "https://www.example.com"; // tuhs.org/";
static int urlInputLength = 0;
static int isTypingUrl = 1;

TTF_Font* LoadFontSafely(const char* fontPath, double ptSize, TTF_Font** oldFont) {
    if (*oldFont) {
        TTF_CloseFont(*oldFont);
        *oldFont = NULL;
    }

    TTF_Font* newFont = TTF_OpenFont(fontPath, ptSize);
    if (!newFont) {
        return NULL;
    }

    *oldFont = newFont;
    return newFont;
}

void freeDOM(HTMLNode* node) {
    if (!node) return;
    for (size_t i = 0; i < node->childCount; ++i) {
        freeDOM(node->children[i]);
    }
    free(node->children);
    free(node->content);
    free(node->name);
    free(node);
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) { // This function runs once on start 
    SDL_SetAppMetadata("bem", "1.0", "com.tavro.bem");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("bem", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialise SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // font = TTF_OpenFontIO(SDL_IOFromConstMem(tiny_ttf, tiny_ttf_len), true, 16);
    font = TTF_OpenFont("./resources/DejaVuSansMonoForConso1as.ttf", 16);
    boldFont = TTF_OpenFont("./resources/DejaVuSansMonoForConso1as-Bold.ttf", 16);
    if (!font || !boldFont) {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    char *htmlInput = NULL;
    char *cssInput = NULL;
	int res = fetchHTMLAndCSS(urlInput, &htmlInput, &cssInput);
    if (res == 0) {
        extractMetaData(&htmlInput);
        DOM = parser(htmlInput);
    }

    SDL_StartTextInput(window);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) { // This function runs when an event occurs (mouse input, key presses, etc...)
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (isTypingUrl) {
        if (event->type == SDL_EVENT_TEXT_INPUT) {
            if (urlInputLength < sizeof(urlInput) - 1) {
                urlInput[urlInputLength++] = event->text.text[0];
                urlInput[urlInputLength] = '\0';
            }
        } else if (event->type == SDL_EVENT_KEY_DOWN) {
            if (event->key.scancode == SDL_SCANCODE_BACKSPACE && urlInputLength > 0) {
                urlInput[--urlInputLength] = '\0';
            } else if (event->key.scancode == SDL_SCANCODE_RETURN) {
                char *htmlInput = NULL;
                char *cssInput = NULL;

                int res = fetchHTMLAndCSS(urlInput, &htmlInput, &cssInput);
                if (res == 0) {
                    extractMetaData(&htmlInput);
                    if (DOM) {
                        freeDOM(DOM);
                    }
                    DOM = parser(htmlInput);
                }
                isTypingUrl = false;
            }
        }
    }

    if (!isTypingUrl && event->type == SDL_EVENT_KEY_DOWN) {
        const int scrollStep = 20;
        if (event->key.scancode == SDL_SCANCODE_DOWN) {
            scrollOffset -= scrollStep;
        } else if (event->key.scancode == SDL_SCANCODE_UP) {
            scrollOffset += scrollStep;
        }
    }

    return SDL_APP_CONTINUE;
}

typedef struct FontData {
    float verticalMargin;
    float horizontalMargin;
    float fontSize;
    int bold;
} FontData;

FontData* handleNode(HTMLNode* node) {
    FontData* data = NULL;

    if (node == NULL || node->parent == NULL || node->parent->name == NULL) {
        return NULL;
    }

    data = (FontData*)malloc(sizeof(FontData));
    if (!data) {
        perror("malloc failed");
        return NULL;
    }

    if (strcmp(node->parent->name, "p") == 0) {
        data->verticalMargin = 16.0;
        data->horizontalMargin = 0.0;
        data->fontSize = 16.0;
        data->bold = 0;
    } else if (strcmp(node->parent->name, "h1") == 0) {
        data->verticalMargin = (16.0 * 2.0) * 0.67;
        data->horizontalMargin = 0.0;
        data->fontSize = 16.0 * 2.0;
        data->bold = 1;
    } else if (strcmp(node->parent->name, "h2") == 0) {
        data->verticalMargin = (16.0 * 1.5) * 0.83;
        data->horizontalMargin = 0.0;
        data->fontSize = 16.0 * 1.5;
        data->bold = 1;
    } else if (strcmp(node->parent->name, "h3") == 0) {
        data->verticalMargin = (16.0 * 1.17);
        data->horizontalMargin = 0.0;
        data->fontSize = 16.0 * 1.17;
        data->bold = 1;
    } else if (strcmp(node->parent->name, "h4") == 0) {
        data->verticalMargin = (16.0) * 1.33;
        data->horizontalMargin = 0.0;
        data->fontSize = 16.0;
        data->bold = 1;
    } else if (strcmp(node->parent->name, "h5") == 0) {
        data->verticalMargin = (16.0 * 0.83) * 1.67;
        data->horizontalMargin = 0.0;
        data->fontSize = 16.0 * 0.83;
        data->bold = 1;
    } else if (strcmp(node->parent->name, "h6") == 0) {
        data->verticalMargin = (16.0 * 0.67) * 2.33;
        data->horizontalMargin = 0.0;
        data->fontSize = 16.0 * 0.67;
        data->bold = 1;
    } else {
        // printf("bem does not support text element %s yet.\n", node->parent->name);
        free(data);
        return NULL;
    }

    return data;
}

// TODO: Add browser default CSSOM
static int yPos = 0;
void renderNode(HTMLNode* node) {
    switch (node->type) {
        case TEXT: {
            FontData* data = handleNode(node);
            SDL_Color textColor = {255, 255, 255, 255};
            size_t textLen = strlen(node->content);

            if (data) {
                font = LoadFontSafely("./resources/DejaVuSansMonoForConso1as.ttf", data->fontSize, &font);
                boldFont = LoadFontSafely("./resources/DejaVuSansMonoForConso1as-Bold.ttf", data->fontSize, &boldFont);

                SDL_Surface* textSurface = TTF_RenderText_Solid(data->bold ? boldFont : font, node->content, textLen, textColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

                SDL_FRect dstRect = {
                    .x = data->horizontalMargin,
                    .y = (float)(yPos + data->verticalMargin + scrollOffset),
                    .w = (float)textSurface->w,
                    .h = (float)textSurface->h
                };

                if (dstRect.y + dstRect.h < 30 || dstRect.y > 480) {
                    yPos += textSurface->h + (int)data->verticalMargin;
                    SDL_DestroySurface(textSurface);
                    SDL_DestroyTexture(textTexture);
                    free(data);
                    return;
                }

                SDL_RenderTexture(renderer, textTexture, NULL, &dstRect);
                yPos += textSurface->h + (int)data->verticalMargin;

                SDL_DestroySurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
            free(data);
            break;
        }
        default:
            break;
    }

    for (size_t i = 0; i < node->childCount; i++) {
        renderNode(node->children[i]);
    }
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    SDL_FRect inputBarRect = {0, 0, 640, 30};
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &inputBarRect);

    if (font) {
        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(font, urlInput, strlen(urlInput), textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FRect dst = {.x = 5, .y = 7, .w = (float)surface->w, .h = (float)surface->h};
            SDL_RenderTexture(renderer, texture, NULL, &dst);
            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
    }

    yPos = 40;
    renderNode(DOM);

    if (scrollOffset > 0) scrollOffset = 0;
    if (yPos < 480) scrollOffset = 0;
    else if (-scrollOffset > yPos - 480)
        scrollOffset = -(yPos - 480);

    int totalContentHeight = yPos - scrollOffset;
    float scrollRatio = 480.0f / (float)totalContentHeight;
    float scrollbarHeight = 480.0f * scrollRatio;
    if (scrollbarHeight < 20) scrollbarHeight = 20;

    float scrollbarY = (-scrollOffset / (float)totalContentHeight) * 480.0f;
    if (scrollbarY < 30) scrollbarY = 30;

    SDL_FRect scrollbar = {
        .x = 625,
        .y = scrollbarY,
        .w = 10,
        .h = scrollbarHeight
    };
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &scrollbar);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) { // This function runs once on app quit.
    // SDL will clean up the window/renderer for us.
}
