#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

const int BOARD_SIZE = 3;
const int WINDOW_WIDTH = 300;
const int WINDOW_HEIGHT = 300;
const int PADDING = 10;
const int CELL_SIZE = WINDOW_WIDTH / BOARD_SIZE;

enum class CellType { None, X, O };
enum class GameState { InProgress, XWins, OWins, Tie };

void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    const int diameter = (radius * 2);

    int x_c = (radius - 1);
    int y_c = 0;
    int tx = 1;
    int ty = 1;
    int error = (tx - diameter);

    while (x_c >= y_c) {
        // Draw octants
        SDL_RenderDrawPoint(renderer, x + x_c, y - y_c);
        SDL_RenderDrawPoint(renderer, x + y_c, y - x_c);
        SDL_RenderDrawPoint(renderer, x - y_c, y - x_c);
        SDL_RenderDrawPoint(renderer, x - x_c, y - y_c);
        SDL_RenderDrawPoint(renderer, x - x_c, y + y_c);
        SDL_RenderDrawPoint(renderer, x - y_c, y + x_c);
        SDL_RenderDrawPoint(renderer, x + y_c, y + x_c);
        SDL_RenderDrawPoint(renderer, x + x_c, y + y_c);

        if (error <= 0) {
            ++y_c;
            error += ty;
            ty += 2;
        } if (error > 0) {
            --x_c;
            tx += 2;
            error += (tx - diameter);
        }
    }
}

void draw_board(SDL_Renderer* renderer, const std::vector<CellType>& board) {
    // Set drawing color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Draw vertical lines
    for (int i = 1; i < BOARD_SIZE; i++) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_HEIGHT);
    }

    // Draw horizontal lines
    for (int i = 1; i < BOARD_SIZE; i++) {
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, WINDOW_WIDTH, i * CELL_SIZE);
    }

    // Draw X's and O's
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i * BOARD_SIZE + j] == CellType::X) {
                // Draw an X in the cell
                SDL_RenderDrawLine(renderer, j * CELL_SIZE, i * CELL_SIZE, (j + 1) * CELL_SIZE, (i + 1) * CELL_SIZE);
                SDL_RenderDrawLine(renderer, (j + 1) * CELL_SIZE, i * CELL_SIZE, j * CELL_SIZE, (i + 1) * CELL_SIZE);
            } else if (board[i * BOARD_SIZE + j] == CellType::O) {
                // Draw an O in the cell
                int centerX = j * CELL_SIZE + CELL_SIZE / 2;
                int centerY = i * CELL_SIZE + CELL_SIZE / 2;
                int radius = CELL_SIZE / 2 - PADDING;
                SDL_RenderDrawCircle(renderer, centerX, centerY, radius);
            }
        }
    }
}

CellType check_board(CellType board[BOARD_SIZE * BOARD_SIZE]) {
    // Check rows
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i += BOARD_SIZE) {
        if (board[i] != CellType::None && board[i] == board[i + 1] && board[i + 1] == board[i + 2]) {
            return board[i];
        }
    }
    
    // Check columns
    for (int j = 0; j < BOARD_SIZE; j++) {
        if (board[j] != CellType::None && board[j] == board[j + BOARD_SIZE] && board[j + BOARD_SIZE] == board[j + 2 * BOARD_SIZE]) {
            return board[j];
        }
    }
    
    // Check diagonal from top left to bottom right
    if (board[0] != CellType::None && board[0] == board[BOARD_SIZE + 1] && board[BOARD_SIZE + 1] == board[2 * BOARD_SIZE + 2]) {
        return board[0];
    }
    
    // Check diagonal from bottom left to top right
    if (board[2] != CellType::None && board[2] == board[BOARD_SIZE + 1] && board[BOARD_SIZE + 1] == board[2 * BOARD_SIZE]) {
        return board[2];
    }
    
    // Check if there are any empty cells left
    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
        if (board[i] == CellType::None) {
            return CellType::None;
        }
    }
    
    // If we reach here, it means the game is a tie
    return CellType::None;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::vector<CellType> board(BOARD_SIZE * BOARD_SIZE, CellType::None);
    GameState game_state = GameState::InProgress;
    CellType current_player = CellType::X;

    while (game_state == GameState::InProgress) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game_state = GameState::Tie;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                int row = y / CELL_SIZE;
                int col = x / CELL_SIZE;
                int index = row * BOARD_SIZE + col;

                if (board[index] == CellType::None) {
                    board[index] = current_player;

                    if (current_player == CellType::X) {
                        current_player = CellType::O;
                    } else {
                        current_player = CellType::X;
                    }
                }

                CellType winner = check_board(board.data());
                if (winner != CellType::None) {
                    if (winner == CellType::X) {
                        std::cout << "X wins!" << "\n";
                        game_state = GameState::XWins;
                    } else {
                        std::cout << "O wins!" << "\n";
                        game_state = GameState::OWins;
                    }
                } else {
                    bool tie = true;
                    for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
                        if (board[i] == CellType::None) {
                            tie = false;
                            break;
                        }
                    }

                    if (tie) {
                        std::cout << "Tie!" << "\n";
                        game_state = GameState::Tie;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        draw_board(renderer, board);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
