#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <SDL_ttf.h>

#define n1 3
#define n2 1
#define n3 0
#define n4 3
#define N (10 + n3)
#define max_rand 199
#define min_rand 0
#define k (1.0 - n3 * 0.02 - n4 * 0.005 - 0.25)
#define WIDTH 800
#define HEIGHT 600
#define seed (1000 * n1 + 100 * n2 + 10 * n3 + n4)
#define arrowAngle  (M_PI / 5.0)
#define sizeMult 3
#define shiftAngle (M_PI / 18.0)

struct test {
    double matrix[N][N];
};

typedef struct node {
    int key;
    int x;
    int y;
    int pos;
    struct node *next_node;
} l_list;

l_list *l_list_init(int key, int x, int y, int pos);

l_list *addto_list(l_list *l_pointer, int key, int x, int y, int pos);

l_list *delfrom_start(l_list *l_pointer);

l_list *find_num(l_list *l_pointer, int key);

struct test GenerateDirectedMatrix();

struct test GenerateUndirectedMatrix(struct test s2);

void PrintMatrix(struct test matrix, FILE *fptr);

void SDL_RenderClearScreen(SDL_Renderer *Renderer);

void SDL_RenderDrawCircle(SDL_Renderer *renderer, int32_t centreX, int32_t centreY, int32_t radius);

l_list *drawGraph(SDL_Renderer *renderer, SDL_Window *window, struct test matrix, l_list *list_ptr);

void SDL_RenderBezierCurve(SDL_Renderer *renderer, int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4);

void drawConnections(SDL_Renderer *renderer, l_list *node1, l_list *node2,
                     int r, int dir, int width, int height, int gap2);

void SDL_RenderDrawArrowHead(SDL_Renderer *renderer, int endX, int endY, int gap, double angle);

void drawNodeNumber(SDL_Renderer *renderer, int number, int x, int y, int gap);

int main(int argc, char *argv[]) {
    srand(seed);

    FILE *fptr;

    TTF_Init();

    struct test directedMatrix = GenerateDirectedMatrix();
    struct test undirectedMatrix = GenerateUndirectedMatrix(directedMatrix);

    PrintMatrix(directedMatrix, fptr);
    PrintMatrix(undirectedMatrix, fptr);

    l_list *list1_ptr, *list2_ptr;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *directedWindow = SDL_CreateWindow("Directed Graph", SDL_WINDOWPOS_UNDEFINED,
                                                  SDL_WINDOWPOS_UNDEFINED, WIDTH,
                                                  HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *directedRenderer = SDL_CreateRenderer(directedWindow, -1,
                                                        SDL_RENDERER_ACCELERATED);

    SDL_Window *undirectedWindow = SDL_CreateWindow("Undirected Graph", SDL_WINDOWPOS_UNDEFINED,
                                                    SDL_WINDOWPOS_UNDEFINED, WIDTH,
                                                    HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *undirectedRenderer = SDL_CreateRenderer(undirectedWindow, -1,
                                                          SDL_RENDERER_ACCELERATED);

    SDL_SetWindowResizable(directedWindow, SDL_TRUE);
    SDL_SetWindowResizable(undirectedWindow, SDL_TRUE);

    SDL_Event event;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_CLOSE)) {
                quit = 1;
            }
        }

        // Clear the screen
        SDL_RenderClearScreen(directedRenderer);
        SDL_RenderClearScreen(undirectedRenderer);

        SDL_SetRenderDrawColor(undirectedRenderer, 255, 255, 255, 0);
        SDL_SetRenderDrawColor(directedRenderer, 255, 255, 255, 0);

        list1_ptr = drawGraph(undirectedRenderer, undirectedWindow,
                              undirectedMatrix, list1_ptr);
        list2_ptr = drawGraph(directedRenderer, directedWindow,
                              directedMatrix, list2_ptr);

        SDL_RenderPresent(undirectedRenderer);
        SDL_RenderPresent(directedRenderer);
    }

    SDL_DestroyWindow(directedWindow);
    SDL_DestroyRenderer(directedRenderer);

    SDL_DestroyWindow(undirectedWindow);
    SDL_DestroyRenderer(undirectedRenderer);

    SDL_Quit();

    while (list1_ptr != NULL)
        list1_ptr = delfrom_start(list1_ptr);
    while (list2_ptr != NULL)
        list2_ptr = delfrom_start(list2_ptr);

    fclose(fopen("Output.txt", "w"));
    return 0;
}

struct test GenerateDirectedMatrix() {
    struct test s1;
    double a;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            a = (rand() % (max_rand + 1 - min_rand) + min_rand) / 100.0;
            a *= k;
            s1.matrix[i][j] = a < 1.0 ? 0 : 1;
        }
    }
    return s1;
}

struct test GenerateUndirectedMatrix(struct test s2) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (s2.matrix[i][j] == 1)
                s2.matrix[j][i] = 1;
        }
    }
    return s2;
}

void PrintMatrix(struct test matrix, FILE *fptr) {
    fptr = fopen("Output.txt", "a");
    fprintf(fptr, "\n");
    fprintf(fptr, "Here is your matrix:\n\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            fprintf(fptr, " %2.0lf", matrix.matrix[i][j]);
        }
        fprintf(fptr, "\n");
    }
    fprintf(fptr, "\n");
    fclose(fptr);
}

void SDL_RenderDrawCircle(SDL_Renderer *renderer, int32_t centreX, int32_t centreY, int32_t radius) {
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

void SDL_RenderClearScreen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

l_list *drawGraph(SDL_Renderer *renderer, SDL_Window *window, struct test matrix, l_list *list_ptr) {
    int width, height, xTopCircles, xLowCircles, yCircles, turn = 1, mid;
    int gap, gap1, gap2, gap3, r, key = 1, pos = 1, flag = 1;

    mid = (abs(N - 5)) / 4;
    yCircles = 2 + mid * 2;
    xLowCircles = (N - yCircles) / 2;
    xTopCircles = N - xLowCircles - yCircles;
    SDL_GetWindowSize(window, &width, &height);

    r = (height > width || (N >= 7 && N < 9)) ? width / (int) ((sizeMult + 1) * (xTopCircles + 1) + sizeMult) :
            height / ((sizeMult + 1) * (yCircles / 2 + 1) + sizeMult);

    gap = r * sizeMult;
    gap1 = (width - 2 * gap - (xTopCircles + 1) * 2 * r) / xTopCircles;
    gap2 = (height - (yCircles / 2 + 1) * 2 * r) / (yCircles / 2 + 2);
    gap3 = (width - 2 * gap - (xLowCircles + 1) * 2 * r) / xLowCircles;

    SDL_RenderDrawCircle(renderer, gap + r, gap2 + r, r);
    drawNodeNumber(renderer, key, gap + r, gap2 + r, gap);
    list_ptr = l_list_init(key, gap + r, gap2 + r, pos);
    key++;

    while(key <= N) {
        switch (turn % 4) {
            case 1:
                for (int j = 1; j < xTopCircles; ++j) {
                    SDL_RenderDrawCircle(renderer, gap + r + j * (2 * r + gap1),
                                         gap2 + r, r);
                    drawNodeNumber(renderer, key, gap + r + j * (2 * r + gap1),
                                   gap2 + r, gap);
                    list_ptr = addto_list(list_ptr, key,
                                          gap + r + j * (2 * r + gap1), gap2 + r, pos);
                    key++;
                }
                break;
            case 2:
                for (int j = 0; j < yCircles / 2; ++j) {
                    SDL_RenderDrawCircle(renderer, width - gap - r,
                                         (gap2 + r) + j * (gap2 + 2 * r), r);
                    drawNodeNumber(renderer, key, width - gap - r,
                                   (gap2 + r) + j * (gap2 + 2 * r), gap);
                    list_ptr = addto_list(list_ptr, key,
                                          width - gap - r, (gap2 + r) + j * (gap2 + 2 * r), pos);
                    key++;
                }
                break;
            case 3:
                for (int j = 0; j < xLowCircles; ++j) {
                    SDL_RenderDrawCircle(renderer, width - gap - r - j * (2 * r + gap3),
                                         height - gap2 - r, r);
                    drawNodeNumber(renderer, key, width - gap - r - j * (2 * r + gap3),
                                   height - gap2 - r, gap);
                    list_ptr = addto_list(list_ptr, key,
                                          width - gap - r - j * (2 * r + gap3), height - gap2 - r, pos);
                    key++;
                }
                break;
            case 0:
                for (int j = 0; j < yCircles / 2; ++j) {
                    SDL_RenderDrawCircle(renderer, gap + r,
                                         height - (r + gap2) - j * (2 * r + gap2), r);
                    drawNodeNumber(renderer, key, gap + r,
                                   height - (r + gap2) - j * (2 * r + gap2), gap);
                    list_ptr = addto_list(list_ptr, key,
                                          gap + r, height - (r + gap2) - j * (2 * r + gap2), pos);
                    key++;
                }
                break;
        }
        turn++;
        pos++;
    }
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (matrix.matrix[i][j] == 1) {
                l_list *node1 = find_num(list_ptr, i + 1);
                l_list *node2 = find_num(list_ptr, j + 1);
                if (i == j) {
                    if (node1->pos >= 3) flag = -1;
                    SDL_RenderDrawCircle(renderer,
                                         (int) (node1->x - flag * (r + (double) r / 2 - flag)
                                                           * cos(M_PI / 4)),
                                         (int) (node1->y - flag * (r + (double) r / 2 - flag)
                                                           * sin(M_PI / 4)),
                                         r / 2);
                    SDL_RenderDrawArrowHead(renderer, (int) (node1->x - flag * r * cos(M_PI / 4)),
                                            (int) (node1->y - flag * r * sin(M_PI / 4)), gap,
                                            3 * M_PI / 4 - flag * M_PI / 15);
                } else {
                    int dir = 1;
                    drawConnections(renderer, node1, node2, r, dir, width, height, gap2);
                    if (matrix.matrix[j][i] == 1) {
                        matrix.matrix[j][i] = 0;
                        dir = -1;
                        drawConnections(renderer, node2, node1, r, dir, width, height, gap2);
                    }
                }
            }
        }
    }
    return list_ptr;
}


l_list *l_list_init(int key, int x, int y, int pos) {
    l_list *l_pointer;
    l_pointer = malloc(sizeof(struct node));
    *l_pointer = (l_list) {
            .key = key,
            .x = x,
            .y = y,
            .pos = pos,
            .next_node = NULL
    };
    return l_pointer;
}

l_list *addto_list(l_list *l_pointer, int key, int x, int y, int pos) {
    l_list *new_node;
    new_node = malloc(sizeof(struct node));
    new_node->key = key;
    new_node->x = x;
    new_node->y = y;
    new_node->pos = pos;
    new_node->next_node = l_pointer;
    return new_node;
}

l_list *delfrom_start(l_list *l_pointer) {
    l_list *node_ptr;
    node_ptr = l_pointer->next_node;
    free(l_pointer);
    return node_ptr;
}

l_list *find_num(l_list *l_pointer, int key) {
    l_list *this_node = l_pointer;

    while (this_node != NULL) {
        if (this_node->key == key) {
            return this_node;
        } else {
            this_node = this_node->next_node;
        }
    }
    return NULL;
}

void SDL_RenderBezierCurve(SDL_Renderer *renderer, int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4) {
    double xu, yu, u;
    for (u = 0.0; u <= 1.0; u += 0.0005) {
        xu = pow(1 - u, 3) * x1 + 3 * u * pow(1 - u, 2) * x2 + 3 * pow(u, 2) * (1 - u) * x3
             + pow(u, 3) * x4;
        yu = pow(1 - u, 3) * y1 + 3 * u * pow(1 - u, 2) * y2 + 3 * pow(u, 2) * (1 - u) * y3
             + pow(u, 3) * y4;
        SDL_RenderDrawPoint(renderer, (int) xu, (int) yu);
    }
}

void drawConnections(SDL_Renderer *renderer, l_list *node1, l_list *node2,
                     int r, int dir, int width, int height, int gap2) {
    int startX = node1->x, startY = node1->y, endX = node2->x, endY = node2->y;
    int mid1X, mid1Y, mid2X, mid2Y;
    int midX = (node1->x + node2->x) / 2;
    int midY = (node1->y + node2->y) / 2;
    int dirX = (node1->x - width / 2) <= 0 ? -1 : 1;
    int dirY = (node1->y - height / 2) <= 0 ? -1 : 1;
    int gap = sizeMult * r;
    double angle1, angle2;
    if ((abs(node1->key - node2->key) == 1) ||
    ((node1->key == 1 || node2->key == 1) && (node1->key == N || node2->key == N))) {
        angle1 = atan2(endY - startY, endX - startX);
        SDL_RenderDrawLine(renderer, startX + (int) ((double) r * cos(angle1 + shiftAngle)),
                           startY + (int) ((double) r * sin(angle1 + shiftAngle)),
                           endX - (int) ((double) r * cos(angle1 - shiftAngle)),
                           endY - (int) ((double) r * sin(angle1 - shiftAngle)));
        SDL_RenderDrawArrowHead(renderer, endX - (int) ((double) r * cos(angle1 - shiftAngle)),
                                endY - (int) ((double) r * sin(angle1 - shiftAngle)), gap, angle1);
    } else {
        if (startX == endX && (startX == gap + r || startX == width - gap - r)) {
            mid1X = (int)((double)(startX + midX) / 2 + gap * dir * dirX);
            mid1Y = (startY + midY) / 2;
            mid2X = (int)((double)(midX + endX) / 2 + gap * dir * dirX);
            mid2Y = (midY + endY) / 2;
            angle1 = atan2(mid1Y - startY, mid1X - startX);
            angle2 = atan2(endY - mid2Y, endX - mid2X);
            SDL_RenderBezierCurve(renderer, startX + (int) ((double) r * cos(angle1)), mid1X,
                                  mid2X, endX - (int) ((double) r * cos(angle2)),
                                  startY + (int) ((double) r * sin(angle1)), mid1Y,
                                  mid2Y, endY - (int) ((double) r * sin(angle2)));
            SDL_RenderDrawArrowHead(renderer, endX - (int) ((double) r * cos(angle2)),
                                    endY - (int) ((double) r * sin(angle2)), gap, angle2);
        } else if (startY == endY && (startY == gap2 + r || startY == height - gap2 - r)) {
            mid1X = (startX + midX) / 2;
            mid1Y = (int)((double)(startY + midY) / 2 + gap * dir * dirY);
            mid2X = (midX + endX) / 2;
            mid2Y = (int)((double)(midY + endY) / 2 + gap * dir * dirY);
            angle1 = atan2(mid1Y - startY, mid1X - startX);
            angle2 = atan2(endY - mid2Y, endX - mid2X);
            SDL_RenderBezierCurve(renderer, startX + (int) ((double) r * cos(angle1)), mid1X,
                                  mid2X, endX - (int) ((double) r * cos(angle2)),
                                  startY + (int) ((double) r * sin(angle1)), mid1Y,
                                  mid2Y, endY - (int) ((double) r * sin(angle2)));
            SDL_RenderDrawArrowHead(renderer, endX - (int) ((double) r * cos(angle2)),
                                    endY - (int) ((double) r * sin(angle2)), gap, angle2);
        } else {
            angle1 = atan2(endY - startY, endX - startX);
            SDL_RenderDrawLine(renderer, startX + (int) ((double) r * cos(angle1 + shiftAngle)),
                               startY + (int) ((double) r * sin(angle1 + shiftAngle)),
                               endX - (int) ((double) r * cos(angle1 - shiftAngle)),
                               endY - (int) ((double) r * sin(angle1 - shiftAngle)));
            SDL_RenderDrawArrowHead(renderer, endX - (int) ((double) r * cos(angle1 - shiftAngle)),
                                    endY - (int) ((double) r * sin(angle1 - shiftAngle)), gap, angle1);
        }
    }
}

void SDL_RenderDrawArrowHead(SDL_Renderer *renderer, int endX, int endY, int gap, double angle) {
    double arrowSize = (double) gap / 3 / sizeMult;

    double x1 = endX - arrowSize * cos(angle + arrowAngle);
    double y1 = endY - arrowSize * sin(angle + arrowAngle);
    double x2 = endX - arrowSize * cos(angle - arrowAngle);
    double y2 = endY - arrowSize * sin(angle - arrowAngle);

    SDL_Vertex vertex_1 = {{(float) endX, (float) endY},
                           {255, 255, 255, 255},
                           {1, 1}};
    SDL_Vertex vertex_2 = {{(float) x1, (float) y1},
                           {255, 255, 255, 255},
                           {1, 1}};
    SDL_Vertex vertex_3 = {{(float) x2, (float) y2},
                           {255, 255, 255, 255},
                           {1, 1}};

    SDL_Vertex vertices[] = {
            vertex_1,
            vertex_2,
            vertex_3
    };

    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

void drawNodeNumber(SDL_Renderer *renderer, int number, int x, int y, int gap) {
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font *font = TTF_OpenFont("arial.ttf", gap / 3);
    char numberString[4];

    snprintf(numberString, sizeof(numberString), "%d", number);

    SDL_Surface *surface = TTF_RenderText_Solid(font, numberString, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

    SDL_Rect rect = {x - texW / 2, y - texH / 2, texW, texH};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}
