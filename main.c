/*
 * main.c - A hypnosis simulation application.
 *
 * Cross-compiles on Linux for Windows.
 * This is a simulation for entertainment and informational
 * purposes only and is not a real medical tool.
 */

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 1024 // Increased width for a widescreen feel
#define SCREEN_HEIGHT 600 // Returned to a more standard height
#define PI 3.14159265

// --- Globals ---
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
float rotation_angle = 0.0f;

const char* script[] = {
    // Introduction & Pre-talk
    "This simulation is based on common clinical techniques. Please get comfortable and ensure you will not be disturbed.",
    "The goal is simply to achieve a state of deep physical and mental relaxation. Let's begin.",

    // Induction: Eye Fixation
    "Please allow your eyes to focus on the very center of the spiral on the screen.",
    "As you focus, just allow your eyes to relax. You don't need to stare intently, just rest your gaze there.",
    "As you continue to watch, you may notice your eyelids are beginning to feel heavy... pleasantly heavy and relaxed.",
    "It's a perfectly natural feeling, like at the end of a long day when you are ready for a restful sleep.",
    "Allow them to feel so heavy that they just want to close. And when you are ready, you can let them close.",
    "Or you can continue to watch the screen, whichever feels more comfortable for you.",

    // Deepening: Progressive Relaxation
    "Now, bring your awareness to the muscles in your face and jaw. Allow them to become soft and limp.",
    "Let that feeling of relaxation flow down into your neck and shoulders... letting go of all the tension.",
    "The relaxation flows down your arms, all the way to your fingertips.",
    "Notice your breathing... slow, calm, and regular. Each breath you exhale carries away any remaining tension.",
    "This pleasant feeling of heaviness flows down through your body... down into your legs... all the way to your toes.",
    
    // Deepening: Staircase Metaphor
    "Now, imagine you are standing at the top of a short, comfortable staircase.",
    "There are ten steps leading down to a place of even deeper, more profound relaxation.",
    "In a moment, I will count down from ten to one, and with each number, you will take a step down.",
    "Ten... taking the first step down, feeling yourself drifting twice as deep.",
    "Nine... each muscle in your body becoming even more relaxed.",
    "Eight... deeper and deeper with every step.",
    "Seven... just letting go completely.",
    "Six... halfway there now.",
    "Five... every sound around you only helps you to relax even more.",
    "Four... more and more calm and peaceful.",
    "Three... almost at the bottom now.",
    "Two... deeper and deeper still.",
    "One... step off the bottom step into a state of wonderful, deep relaxation.",

    // Re-alerting / Awakening
    "Now it is time to return to your normal, fully alert state of awareness.",
    "I will count from one to five. At the count of five, you will be fully awake, feeling calm, refreshed, and alert.",
    "One... slowly, calmly, easily, you are returning.",
    "Two... feeling the energy returning to your arms and legs.",
    "Three... becoming more aware of the room around you.",
    "Four... your eyes are ready to open, feeling clear and refreshed.",
    "Five. Eyes open, fully awake, feeling wonderful. The session is complete."
};
const int SCRIPT_LINES = sizeof(script) / sizeof(const char*);
int current_line = 0;
float line_timer = 0.0f;
float alpha = 0.0f; // For fading
int fade_state = 0; // 0:fading in, 1:holding, 2:fading out


// --- Function Prototypes ---
int init();
void cleanup();
void draw_spiral(float angle_offset);
void render_text(const char* text, float alpha_mod);
void update(float dt);
void render();

// --- Main Function ---
int main(int argc, char* argv[]) {
    if (init() != 0) {
        cleanup();
        return 1;
    }

    // --- Main Loop ---
    int is_running = 1;
    SDL_Event e;
    Uint32 last_time = SDL_GetTicks();

    while (is_running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                is_running = 0;
            }
        }

        Uint32 current_time = SDL_GetTicks();
        float dt = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        update(dt);
        render();
    }

    cleanup();
    return 0;
}


// --- Logic ---
void update(float dt) {
    rotation_angle += 0.5f * dt; // Control rotation speed

    line_timer += dt;

    // Handle text fading state machine
    switch(fade_state) {
        case 0: // Fading in
            alpha += 80.0f * dt;
            if (alpha >= 255.0f) {
                alpha = 255.0f;
                fade_state = 1; // Hold
                line_timer = 0; // Reset timer for hold duration
            }
            break;
        case 1: // Holding
            if (line_timer > 4.0f) { // Hold for 4 seconds
                fade_state = 2; // Fade out
            }
            break;
        case 2: // Fading out
            alpha -= 80.0f * dt;
            if (alpha <= 0.0f) {
                alpha = 0.0f;
                fade_state = 0; // Fade in next line
                current_line = (current_line + 1) % SCRIPT_LINES;
            }
            break;
    }
}


// --- Rendering ---
void draw_spiral(float angle_offset) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    float cx = SCREEN_WIDTH / 2.0f;
    float cy = SCREEN_HEIGHT / 2.0f;

    for (float angle = 0; angle < 20 * PI; angle += 0.05) {
        // Archimedean spiral formula: r = a * theta
        float r = 10 * angle;
        
        float current_angle = angle + angle_offset;

        float x = cx + r * cos(current_angle);
        float y = cy + r * sin(current_angle);
        
        // Only draw points within a certain radius to create the circular illusion
        if (r < SCREEN_HEIGHT / 2.5) { // Adjusted spiral size for new height
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
}

void render_text(const char* text, float alpha_mod) {
    if (!font || !text) return;
    
    SDL_Color color = {200, 220, 255, 255}; // Light blue text
    // Use TTF_RenderText_Blended_Wrapped to wrap long lines of text.
    // The last argument is the maximum width in pixels.
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, color, SCREEN_WIDTH - 100);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_SetTextureAlphaMod(texture, (Uint8)alpha_mod);

    SDL_Rect dest_rect = {
        (SCREEN_WIDTH - surface->w) / 2, // Center the wrapped text block
        SCREEN_HEIGHT - 120, // Move text up to give more space
        surface->w,
        surface->h
    };

    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}


void render() {
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255); // Dark blue background
    SDL_RenderClear(renderer);

    draw_spiral(rotation_angle);
    
    if (current_line < SCRIPT_LINES) {
        render_text(script[current_line], alpha);
    }
    
    SDL_RenderPresent(renderer);
}

// --- Initialization and Cleanup ---
int init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Hypnosis Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return 1;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return 1;

    // IMPORTANT: Requires 'font.ttf' in the same directory
    font = TTF_OpenFont("font.ttf", 28);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        printf("Please ensure 'font.ttf' is in the same directory.\n");
        SDL_Delay(5000);
        return 1;
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return 0;
}

void cleanup() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
