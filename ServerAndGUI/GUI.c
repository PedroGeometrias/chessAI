// Autor : Pedro De Oliveira Haro
// Motivo : Interface Grafica que permite usuarios jogarem contra minha Inteligencia Artificial

// Eu odeio Winodows
#ifdef _WIN32
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
// Includes que fincionam
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

// outros includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "communicationLayer.h"
#include "stack.h"

// representacao do tabuleiro na memoria
typedef struct{
    char pieces[12];
    int board[8][8]; 
}Board;

// Essa struct representa o tabuleiro e o painel em conjunto, a interface grafica do programa
typedef struct {
    // Tamanhos do tabuleiro e painel 
    int x_size, y_size, side_panel;
    // struct que representa a janela
    SDL_Window* window;
    // struct que representa o renderizador de texturas e imagens
    SDL_Renderer* renderer;
    // quadrado das pecas
    SDL_Rect quadrado; 
    SDL_Rect undu_button;
    SDL_Texture* spritesheet; 
}GUI;

// Defines

// buffer de comunicacao
#define BUFFER 100 
// caminho da fonte
#define FONT_PATH "DinaRemasterII.woff"
// IP local
#define IP "127.0.0.1"
// socket
#define SOCKET 8080

// prototipo das funcoes
SDL_Rect get_piece_sprite_by_index(int piece_index);
void create_GUI(GUI* gui);
void draw_board(GUI* gui, Board* board);
void free_gui(GUI* gui);
void fen_to_2dArr(Board* board, const char* fen, char* turn, 
                  char* castling, char* en_passant, int* halfmove_clock, 
                  int* fullmove_number);
char* generate_fen_from_board(Board* board, int from_x, int from_y, 
                              int to_x, int to_y, char* castling, char* en_passant, 
                              int halfmove_clock, int fullmove_number);
void update_piece_positions(GUI* gui, Board* board, int from_x, int from_y, int to_x, int to_y);
void draw_pieces_from_array(GUI* gui, Board* board);
void update_side_panel(GUI* gui, int move_count, Board* board);
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y);
int is_board_valid(Board* board);
char* undu_button(STACK* stack);

int main(int argc, char *argv[]) {

    // abstracoes necessarias
    STACK stack;
    GUI gui;
    Board board;
    int sock;
    struct sockaddr_in serverAddress;

    //  painel
    char* move_history[BUFFER];
    int move_count = 0;

    // variaveis de comunicacao com o server
    char buffer[BUFFER] = {0};

    // peca seleciona tem estado -1
    int selected_x = -1, selected_y = -1;

    // Strings necessaria para formatar o FEN
    char turn = 'w';                    
    char castling[] = "KQkq";          
    char en_passant[] = "-";          
    int halfmove_clock = 0;          
    int fullmove_number = 1;        


    // inicializando comunicacao com o server
    initializePlatform();
    sock = createSocket();
    setupServerAddress(&serverAddress, IP, SOCKET);
    connectToServer(sock, &serverAddress);

    // FEN Inicial, parte importante para o GUI é apenas tudo que esta para tras do 'w'
    char *initialFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    init_stack(&stack, 1);
    push(&stack, initialFEN);
    fen_to_2dArr(&board, peek(&stack),&turn, castling, en_passant, &halfmove_clock, &fullmove_number);
    create_GUI(&gui);

    // SDL_Event controla eventos da janela, como click do mouse e clicar no x 
    SDL_Event event;
    int running = 1;

    gui.undu_button.x = gui.x_size + 175;
    gui.undu_button.y = 420;
    gui.undu_button.w = 150;
    gui.undu_button.h = 100;


    // game loop
    while (running) {
        // exitando janela
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                printf("Quit event detected. Exiting...\n");
                running = 0;
                // clicar na janela
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if(!is_empty(&stack)){
                    if (event.button.x >= gui.undu_button.x && event.button.x <= (gui.undu_button.x + gui.undu_button.w) &&
                        event.button.y >= gui.undu_button.y && event.button.y <= (gui.undu_button.y + gui.undu_button.h)) {
                        char* undu_result = undu_button(&stack);
                        fen_to_2dArr(&board, peek(&stack), &turn, castling, en_passant, &halfmove_clock, &fullmove_number);
                        if (undu_result) {
                            draw_board(&gui, &board);
                            free(undu_result);  
                        } else {
                            printf("Undo not possible. Stack is empty.\n");
                        }
                    }
                }
                // posicao completa do click
                int raw_x = event.button.x;
                int raw_y = event.button.y;

                printf("Mouse clicked at: x=%d, y=%d\n", raw_x, raw_y);

                // posicao no quadrado do tabuleiro
                int board_x = raw_x / gui.quadrado.w;
                int board_y = raw_y / gui.quadrado.h;

                printf("Mapped to board position: x=%d, y=%d\n", board_x, board_y);

                // verificando se clicou no tabuleiro
                if (board_x < 0 || board_x >= 8 || board_y < 0 || board_y >= 8) {
                    printf("Click outside the board. Ignoring.\n");
                    continue;
                }
                // caso estado normal nao mude, vc clicou em uma peca

                if (selected_x == -1 && selected_y == -1) {
                    if (board.board[board_y][board_x] != '.') {
                        selected_x = board_x;
                        selected_y = board_y;
                        printf("Selected piece at (%d, %d)\n", selected_x, selected_y);
                    }
                } else {
                    // movendo a peca no tabuleiro da memoria
                    printf("Moving piece from (%d, %d) to (%d, %d)\n", selected_x, selected_y, board_x, board_y);
                    board.board[board_y][board_x] = board.board[selected_y][selected_x];
                    board.board[selected_y][selected_x] = '.';

                    // dando update nas posicoes
                    update_piece_positions(&gui, &board, selected_x, selected_y, board_x, board_y);
                    selected_x = -1;
                    selected_y = -1;

                    // copiando o novo fen para o buffer local 
                    push(&stack,generate_fen_from_board(&board, selected_x, selected_y, board_x, board_y, castling, en_passant, halfmove_clock, fullmove_number));

                    sendDataToServer(sock, peek(&stack));
                    printf("Sent updated FEN to server: %s\n", peek(&stack));

                    printf("pushing new Fen to stack");

                    // limpando a memoria com o lixo
                    memset(buffer, 0, sizeof(buffer));
                    // recebendo dados do servidor
                    receiveDataFromServer(sock, buffer, sizeof(buffer));
                    push(&stack, buffer);
                    fen_to_2dArr(&board, peek(&stack), &turn, castling, en_passant, &halfmove_clock, &fullmove_number);


                    // atualizo o painel
                    move_history[move_count] = strdup(peek(&stack));
                    move_count++;


                    // desenhando as pecas
                    draw_pieces_from_array(&gui, &board);
                }
            }
        }

        if(buffer && is_board_valid(&board) && strlen(buffer) > 0 ){
            // renderizo o painel
            update_side_panel(&gui, move_count, &board);
        }

        // desenhando o tabuleiro
        draw_board(&gui, &board);

    }

    // limpando a memoria dps da execucao

    for (int i = 0; i < move_count; ++i) {
        if (move_history[i]) {
            free(move_history[i]);
            move_history[i] = NULL;
        }
    }

    if (sock >= 0) {
        close(sock);
    }

    cleanupPlatform();

    free_gui(&gui);

    TTF_Quit();
    return 0;
}


// converto A string com notacao FEN para uma array de duas dimensoes, que é uma representacao na memoria 
// do tabuleiro
void fen_to_2dArr(Board* board, const char* fen, char* turn, char* castling, char* en_passant, int* halfmove_clock, int* fullmove_number) {
    int row = 0, col = 0;
    // limpo o lugar de memoria da array
    memset(board->board, 0, sizeof(board->board));

    int i = 0;
    // enquanto tiver caracteres e eles nao sejam um espaco, pois espaco tbm é um limitador do FEN
    while (fen[i] && fen[i] != ' ') {
        char c = fen[i++];
        // limitador de coluna é "/", ent avanco para a proxiam linha
        if (c == '/') {
            row++;
            col = 0;
            // caracteres numericos sao o numero de quadrados vaziosj, ent converto para um int e somo nas colunas
        } else if (c >= '1' && c <= '8') {
            col += c - '0'; 
        } else {
            // boto na array geral do tabuleiro
            board->board[row][col++] = c; 
        }
    }

    // Essas strings nao sao utilizadas no jogo, porem tenho que manter elas em mente pois elas sao usadas pela a 
    // ia, ja que a ia aceita apenas FEN's completos para a conversao de tensores
    *turn = fen[++i]; 
    i += 2;

    // castacao
    int j = 0;
    while (fen[i] != ' ') {
        castling[j++] = fen[i++];
    }
    castling[j] = '\0';

    i++;
    j = 0;
    // en passant
    while (fen[i] != ' ') {
        en_passant[j++] = fen[i++];
    }
    en_passant[j] = '\0';

    i++;
    // rock
    *halfmove_clock = 0;
    while (fen[i] != ' ') {
        *halfmove_clock = *halfmove_clock * 10 + (fen[i++] - '0');
    }

    i++;
    // movimento
    *fullmove_number = 0;
    while (fen[i] != '\0') {
        *fullmove_number = *fullmove_number * 10 + (fen[i++] - '0');
    }
}

// resposta que sera enviada ao servidor
char* generate_fen_from_board(Board* board, int from_x, int from_y, int to_x, int to_y, char* castling, char* en_passant, int halfmove_clock, int fullmove_number) {
    // buffer
    static char fen[200];
    int idx = 0;

    // o tabuleiro salvo na memoria tem 8*8
    for (int row = 0; row < 8; ++row) {
        // contador de casas vaizias
        int empty_count = 0;
        for (int col = 0; col < 8; ++col) {
            // pecas do tabuleiro na string
            char piece = board->board[row][col];
            // peca igual a zero é um quadrado vazio
            if (piece == 0) { 
                ++empty_count;
            } else {
                // caso o numero de quadrados vaizios esceda 0, converto para string e passo pro proximo caracter do fen
                if (empty_count > 0) {
                    fen[idx++] = '0' + empty_count;
                    empty_count = 0;
                }
                // caso nao seja valor numerico passo para o fen direto
                fen[idx++] = piece;
            }
        }
        // converto caso nao tenha pecas direto
        if (empty_count > 0) {
            fen[idx++] = '0' + empty_count;
        }
        // adiciono o separador do tabuleiro em fen
        if (row < 7) {
            fen[idx++] = '/';
        }
    }

    // formatacao do FEN, essa parte não importa, porem tenho que passar para a IA converter para tensor, porem o valor
    // é sempre o mesmo
    fen[idx] = '\0';
    printf("Debug: FEN after board parsing: %s\n", fen);

    fen[idx++] = ' ';
    fen[idx++] = 'b';

    fen[idx++] = ' ';
    if (strlen(castling) > 0) {
        strcpy(&fen[idx], castling);
        idx += strlen(castling);
    } else {
        fen[idx++] = '-';
    }

    fen[idx++] = ' ';
    if (board->board[to_y][to_x] == 'P' && abs(to_y - from_y) == 2) {
        en_passant[0] = 'a' + to_x;      
        en_passant[1] = '3';            
        en_passant[2] = '\0';
        strcpy(&fen[idx], en_passant);
        idx += strlen(en_passant);
    } else {
        fen[idx++] = '-';
    }

    fen[idx++] = ' ';
    if (board->board[to_y][to_x] == 'P' || board->board[to_y][to_x] != 0) {
        halfmove_clock = 0;
    } else {
        ++halfmove_clock;
    }
    idx += sprintf(&fen[idx], "%d", halfmove_clock);

    fen[idx++] = ' ';
    idx += sprintf(&fen[idx], "%d", fullmove_number);

    fen[idx] = '\0';

    printf("Debug: Final Generated FEN: %s\n", fen);

    return fen;
}


// cada peca tem um representante numerico
int get_piece_index(char piece) {
    switch (piece) {
        // pecas brancas
        case 'P': return 1; 
        case 'R': return 2; 
        case 'B': return 3; 
        case 'N': return 4; 
        case 'K': return 5; 
        case 'Q': return 6; 
        // pecas negras
        case 'p': return 7; 
        case 'r': return 8; 
        case 'b': return 9; 
        case 'n': return 10; 
        case 'k': return 11; 
        case 'q': return 12; 
        // quadrado vazio
        default: return 0;  
    }
}

// limpo o quadrado anterior ao movimento
void update_piece_positions(GUI* gui, Board* board, int from_x, int from_y, int to_x, int to_y) {
    SDL_Rect sprite, dest;

    // quadrado anterior
    gui->quadrado.x = from_x * gui->quadrado.w;
    gui->quadrado.y = from_y * gui->quadrado.h;
    // limpando com a cor correta
    if ((from_x + from_y) % 2 == 0) {
        SDL_SetRenderDrawColor(gui->renderer, 255, 204, 153, 255); 
    } else {
        SDL_SetRenderDrawColor(gui->renderer, 204, 102, 0, 255);  
    }
    // aplicando a cor
    SDL_RenderFillRect(gui->renderer, &gui->quadrado);

    // redesenhaho a peca no novo quadrado
    int piece_index = get_piece_index(board->board[to_y][to_x]);
    // a peca tem de existir
    if (piece_index != 0) {
        sprite = get_piece_sprite_by_index(piece_index);

        dest.x = to_x * gui->quadrado.w;
        dest.y = to_y * gui->quadrado.h;
        dest.w = gui->quadrado.w;
        dest.h = gui->quadrado.h;

        SDL_RenderCopy(gui->renderer, gui->spritesheet, &sprite, &dest);
    }

    SDL_RenderPresent(gui->renderer);
}

// funcao que desenha os sprites na tela baseado no estado atual do board
void draw_pieces_from_array(GUI* gui, Board* board) {
    // variaveis necessarias para quadrados no SDL2
    SDL_Rect sprite, dest;

    // retangulos no SDL tem 4 variaveis
    // (x,y) : posicao
    // (w,h) : largura e altura
    dest.w = gui->quadrado.w; 
    dest.h = gui->quadrado.h;

    // navego o tabuleiro e pego cada index
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            // peca na posicao atual
            int piece_index = get_piece_index(board->board[y][x]);
            // caso a peca não tenha o valor zero, ela tera um sprite correspondente 
            if (piece_index != 0) { 
                // essa funcao recorta um sprite do sprite sheet
                sprite = get_piece_sprite_by_index(piece_index); 
                // calculo da posicoes, dest é um retangulo representante da posicao
                dest.x = x * gui->quadrado.w; 
                dest.y = y * gui->quadrado.h; 
                // renderizo a peca no tabuleiro
                SDL_RenderCopy(gui->renderer, gui->spritesheet, &sprite, &dest); 
            }
        }
    }
}

// funcao para carregar o sprite sheet
void load_spritesheet(GUI* gui) {
    gui->spritesheet = IMG_LoadTexture(gui->renderer, "pecas.png"); 
    if (!gui->spritesheet) {
        printf("Error loading spritesheet: %s\n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }
}

// funcao que corta o sprite sheet 
SDL_Rect get_piece_sprite_by_index(int piece_index) {
    SDL_Rect sprite;
    // tamanho dos sprites
    int piece_size = 16; 
    int row = (piece_index - 1) / 6; 
    int col = (piece_index - 1) % 6;
    sprite.x = col * piece_size;
    sprite.y = row * piece_size;
    sprite.w = piece_size;
    sprite.h = piece_size;
    return sprite;
}


void create_GUI(GUI* gui) {
    // inicializando fontes
    if (TTF_Init() != 0) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
    // inicialzo o sdl com todas a propriedades de janela, como som e eventos
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // tabuleiro tem 640*640 px, permitindo 64 quadrados com 80 pixeis cada
    gui->x_size = 640;
    gui->y_size = 640;
    // painel de informacoes tem 640*300 px, um display de informacoes como historico de jogo e melhor jogada de acordo
    // com a IA
    gui->side_panel = 500;
    // propriedades da janela
    gui->window = SDL_CreateWindow(
        // titulo
        "pedroChess",
        // posicao inical da janela é central
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        // tamango total da janela, incluindo painel e tabuleiro
        gui->x_size + gui->side_panel,
        gui->y_size,
        // janela é vizivel
        SDL_WINDOW_SHOWN
    );

    // caso janela nao seja criada
    if (!gui->window) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // renderer desenha as coisas na tela, accelerated permite aceleraca de  gpu
    gui->renderer = SDL_CreateRenderer(gui->window, -1, SDL_RENDERER_ACCELERATED);
    if (!gui->renderer) {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(gui->window);
        SDL_Quit();
        exit(1);
    }

    // carrego spritesheet
    load_spritesheet(gui);

    // quadrados do tabuleiro
    gui->quadrado.w = 80;
    gui->quadrado.h = 80;


    SDL_SetRenderDrawColor(gui->renderer, 0, 255, 0, 255); 
    SDL_RenderFillRect(gui->renderer, &gui->undu_button);

    // cor de fundo da janela é preto
    SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 255);
    SDL_RenderClear(gui->renderer);
    SDL_RenderPresent(gui->renderer);
}

// desenho o tabuleiro
void draw_board(GUI* gui, Board* board){
    // desenho os quadrados do tabuleiro usando SDL_RECT
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            gui->quadrado.x = x * gui->quadrado.w;
            gui->quadrado.y = y * gui->quadrado.h;

            if ((x + y) % 2 == 0) {
                SDL_SetRenderDrawColor(gui->renderer, 255, 204, 153, 255);
            } else {
                SDL_SetRenderDrawColor(gui->renderer, 204, 102, 0, 255);
            }
            SDL_RenderFillRect(gui->renderer, &gui->quadrado);
        }
    }
    // desenho as pecas depois de desenhar o tabuleiro
    draw_pieces_from_array(gui, board);  
    SDL_RenderPresent(gui->renderer);
}

// funcao de limpamento
void free_gui(GUI* gui) {

    if (gui->renderer) {
        SDL_DestroyRenderer(gui->renderer);
    }
    if (gui->window) {
        SDL_DestroyWindow(gui->window);
    }
    SDL_Quit();
}

// renderizando o tabuleiro em texto no painel
void render_2d_array(GUI* gui, Board* board) {
    // cor do tabuleiro é verde
    SDL_Color green = {0, 255, 0, 255};

    // usando a fonte passada no define
    TTF_Font* font = TTF_OpenFont(FONT_PATH, 16);

    // caso a fonte nao seja localizada ou tenha algum problema
    if (!font) {
        printf("Error loading font for 2D array: %s\n", TTF_GetError());
        return;
    }

    int x_offset = 800; 
    int y_offset = 100;

    for (int row = 0; row < 8; ++row) {
        char row_buffer[50] = ""; 
        int buffer_index = 0; 
        for (int col = 0; col < 8; ++col) {
            char piece = board->board[row][col] ? board->board[row][col] : '.';
            row_buffer[buffer_index++] = piece; 
            row_buffer[buffer_index++] = ' ';  
        }
        row_buffer[buffer_index] = '\0'; 
        render_text(gui->renderer, font, row_buffer, green, x_offset, y_offset);
        y_offset += 20; 
    }

    TTF_CloseFont(font);
}

// update do painel
void update_side_panel(GUI* gui, int move_count,Board* board) {

    // cor especifica
    SDL_Color green = {0, 255, 0, 255};
    // tamanho do painel
    SDL_Rect panel = {640, 0, 300, 640};

    // setando a cor
    SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 255);  
    // renderizando o retangulo
    SDL_RenderFillRect(gui->renderer, &panel);

    // carregando a fonte 
    TTF_Font* font = TTF_OpenFont(FONT_PATH, 16);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return;
    }

    // renderizando meu nome
    render_text(gui->renderer, font, "Pedro De Oliveira Haro", green, 650, 10);
    // espacameto
    int y_offset = 50;

    render_text(gui->renderer, font, "Board Array:", green, 650, y_offset);
    y_offset -= 100;
    render_2d_array(gui, board);

    // renderizando o quadrado
    SDL_SetRenderDrawColor(gui->renderer, 0, 255, 0, 255); 
    SDL_RenderFillRect(gui->renderer, &gui->undu_button);

    SDL_RenderPresent(gui->renderer);
    TTF_CloseFont(font);
}

// renderizar texto no painel
void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y) {

    // SDL_Surface é uma maneira de organizar pixeis em uma array de 2 dimensoes
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, color);
    if (!text_surface) {
        printf("Error rendering text: %s\n", TTF_GetError());
        return;
    }

    //  Aplicando o renderizador ao text_surface
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (!text_texture) {
        printf("Error creating text texture: %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        return;
    }

    // area retangular do texto
    SDL_Rect text_rect = {x, y, text_surface->w, text_surface->h};

    // renderizando a textura
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    // limpando a memoria
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

// verifico se o tabuleiro atual é valido, pois ele precisa existir para ser renderizado
int is_board_valid(Board* board) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // O elemento do indice especifico precisa ser diferente de zero para ele poder ser renderizado
            if (board->board[row][col] != 0) { 
                return 1; 
            }
        }
    }
    return 0; 
}



char* undu_button(STACK* stack) {
    if (is_empty(stack)) {
        return NULL;  
    }
    return pop(stack);  
}

