#pragma once

#include "../../../../Platform/SDL2/Audio/Audio.hpp"
#include "../../../../Platform/SDL2/headers/Engine.hpp"
#include "../../../../Platform/SDL2/headers/RenderWindow.hpp"
#include "../../../../Platform/SDL2/headers/TextureManager.hpp"
#include "combo_table.hpp"
#include "damageTable.hpp"
#include "PieceDefs.hpp"
#include "ppt.h"
#include <algorithm>
#include <cassert>
#include <SDL_ttf.h>
#include <span>
#include <vector>


const PieceType getRandPiece() {
    return static_cast<PieceType>(pptRand());
};

#undef min
#undef max

constexpr inline auto BOARDWIDTH = 10;
constexpr inline auto VISUALBOARDHEIGHT = 20;
constexpr inline auto LOGICALBOARDHEIGHT = 40;

class Board;
class Piece {
public:
    Piece(PieceType kind, int_fast8_t x = 4, int_fast8_t y = VISUALBOARDHEIGHT - 2, RotationDirection spin = RotationDirection::North) {
        //force the RNG to throw away its previous bag, and make a new one

        this->kind = kind;
        setX(x); setY(y);
        this->spin = spin;

        //populate local piece definition
        for (size_t i = 0; i < MINOSINAPIECE; i++) {
            piecedef[i] = PieceDefintions[PieceTypeToColorType(kind)][i];
        }
        //match up the spin direction of the local piece definition with what is given by the params
        switch (spin) {
        case North:
            break;
        case East:
            rotateCCW(); rotateCCW(); rotateCCW();
            break;
        case South:
            rotateCCW(); rotateCCW();
            break;
        case West:
            rotateCCW();
            break;
        default:
            break;
        }
    }
    Piece() = delete;

    friend Board;

    // rotates the piece 90 degrees counter clock wise
    // Thanks MinusKelvin for the idea of using coordinates and not a definition
    void rotateCW() {
        for (auto& coords : piecedef) {
            std::swap(coords.y, coords.x);
            coords.y = -coords.y;
        }
    }

    void rotateCCW() {
        for (auto& coords : piecedef) {
            std::swap(coords.y, coords.x);
            coords.x = -coords.x;
        }
    }
    void setX(int_fast8_t setter) {
        x = setter;
    }
    void setY(int_fast8_t setter) {
        y = setter;
    }
    std::array<Coord, MINOSINAPIECE> piecedef{};
    PieceType kind{};
    RotationDirection spin{};
    int_fast8_t x{};
    int_fast8_t y{};

};

#include "rotation_constants.hpp"
#include <string>

class Board {
public:

    Board() {
        clear();
    }
    void makeBoardGarbage() {
        for (auto& lines : board)
            for (auto& mino : lines) {
                if (mino != ColorType::empty)
                    mino = ColorType::line_clear;
            }
    }
    void clear() {
        for (auto& width : board) {
            for (auto& cell : width) {
                cell = empty;
            }
        }
    }
    int clearLines() {
        int linesCleared{};
        for (size_t h = 0; h < LOGICALBOARDHEIGHT; h++) {
            for (size_t w = 0; w < BOARDWIDTH; w++) {

                if (board.at(w).at(h) == empty)
                    break;
                if (w == BOARDWIDTH - 1) {
                    clearLine(h);
                    linesCleared++;
                    h--;
                }
            }
        }
        return linesCleared;
    };
    void clearLine(uint_fast8_t whichLine) {

        if (whichLine >= LOGICALBOARDHEIGHT)
            whichLine = LOGICALBOARDHEIGHT - 1;
        // clear the line in question
        for (size_t i = 0; i < BOARDWIDTH; i++) {
            board.at(i).at(whichLine) = empty;
        }

        //pull down the rest of the lines above the cleared line down
        for (size_t w = 0; w < BOARDWIDTH; w++) {
            for (size_t h = whichLine; h < LOGICALBOARDHEIGHT; h++) {
                if (h == (LOGICALBOARDHEIGHT - 1)) {

                    board.at(w).at(h) = empty;
                    break;
                } else {
                    board.at(w).at(h) = board.at(w).at((h + 1));
                }
            }

        }
    }
    void sonicDrop(Piece& piece) {
        while (trySoftDrop(piece));
    }

    bool trySoftDrop(Piece& piece) {
        piece.setY(piece.y - 1);
        if (isCollide(piece)) {
            piece.setY(piece.y + 1); // if it collided, go back up where it should be safe
            return false;
        }
        return true;
    }

    void setPiece(const Piece& piece) {
        //Shakkar::playAudio("Asset/Sounds/Sound.wav", (128 / 5));

        for (auto& coord : piece.piecedef) {
            if ((((0 <= (coord.y + piece.y)) && ((coord.y + piece.y) < LOGICALBOARDHEIGHT))) && (((0 <= (coord.x + piece.x)) && ((coord.x + piece.x) < BOARDWIDTH)))) //if inbounds of board
            {
                board.at(coord.x + piece.x).at((coord.y + piece.y)) = PieceTypeToColorType(piece.kind);
            }
        }
    }
    bool isCollide(const Piece& piece) {

        for (auto& coord : piece.piecedef) {

            if ((((0 <= (coord.y + piece.y)) && ((coord.y + piece.y) < LOGICALBOARDHEIGHT))) && (((0 <= (coord.x + piece.x)) && ((coord.x + piece.x) < BOARDWIDTH)))) //if inbounds of board
            {
                if (board.at((coord.x + piece.x)).at((coord.y + piece.y)) != empty) // is the cell in the board matrix empty
                    return true;
            }

            if (((coord.x + piece.x) < 0) || ((coord.x + piece.x) >= BOARDWIDTH)) // cant be out of bounds on either direction
            {
                return true;
            }

            if ((coord.y + piece.y) < 0) // can be above, but not below the board
            {
                return true;
            }

        }
        return false;
    }

    constexpr const char colorTypeToString(const ColorType color) {
        switch (color) {
        case S:
            return'S';
            break;
        case Z:
            return 'Z';
            break;
        case J:
            return 'J';
            break;
        case L:
            return'L';
            break;
        case T:
            return'T';
            break;
        case O:
            return'O';
            break;
        case I:
            return 'I';
            break;
        case empty:
        case line_clear:
        case number_of_ColorTypes:
        default:
            return '#';
        }
    }

    std::array< std::array<ColorType, (LOGICALBOARDHEIGHT)>, BOARDWIDTH> board{};
};

constexpr int softdropCountdownMAX = (UPDATES_A_SECOND);
constexpr uint_fast8_t pieceSpawnDelayMAX = 0;
constexpr uint_fast16_t lockDelayMAX = (UPDATES_A_SECOND);
constexpr auto perfectClearDamage = 10;
constexpr auto backToBackBonus = 2;
class Game {
public:

    Game();
    ~Game();

    void gameLogic(const Shakkar::inputs& input);
    void render(Window& window);

    void Init(Window& window) {
        hold = Piece(PieceType::empty);
        queue.clear();
        queue.reserve(7);
        forceReRollBag();

        currentPiece = Piece(getRandPiece());

        for (size_t i = 0; i < 7; i++)
            queue.emplace_back(Piece(getRandPiece()));

        this->ghostPieces.load(window, "Asset/Sprites/exampleAssets/ghostPieces.png", 16, 16);
        this->pieces.load(window, "Asset/Sprites/exampleAssets/TetrisPieces.png", 16, 16);
        this->matrix.load(window, "Asset/Sprites/Tetris_images/Matrix.png"); // original size is 224 by 299
        // surface matrix should start on 50,34, and end at 169,273 relative to the matrix.png
        // 16 for the size of the minos
        // 10x20 for the size of the board
        this->surface_matrix = SurfaceTexture(window, 16 * 10, 16 * 20);
        //SDL_SetSurfaceBlendMode(surface_matrix.surface, SDL_BLENDMODE_NONE);

        for (auto& surface : this->surface_queue) {
            surface = SurfaceTexture(window, 16 * 5, 16 * 5);
        }

        this->surface_hold = SurfaceTexture(window, 16 * 5, 16 * 5);

        this->background.load(window, "Asset/Sprites/exampleAssets/TetrisBackground.png"); //1080p background
        this->background.destRect = { 0,0,1920,1080 };

        if (!font)
            this->font = TTF_OpenFont("Asset/Sprites/exampleAssets/font.ttf", 36);
        //this->matrixBackground.Init("Asset/Sprites/exampleAssets/matrixBackground", window); //need this later



        timeWhenInit = SDL_GetPerformanceCounter();
        piecesPlaced = 0;
        softdropCountdown = softdropCountdownMAX;
        pieceSpawnDelay = pieceSpawnDelayMAX;
        lockDelayIncrementer = 0;
        isDie = false;
        checkForLineClear = false;
        alreadyHeld = false;
        currentCombo = 0;
        b2b = false;
        numLinesCleared = 0;
        numDamageSent = 0;
        Tspinned = false;



        /*
        this->background.sprite = { 0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT };
        this->matrix.sprite = {
            matrixX ,
            matrixY,
            (224 * 4),
            (299 * 4)
        }; // first pixel on the matrix should be 50, 34
        pieces.textureRegion = { 0, 0, 16, 16 };
        pieces.sprite = { 0,0,24,24 };
        ghostPieces.textureRegion = { 0, 0, 16, 16 };
        ghostPieces.sprite = { 0,0,24,24 };*/
        pieces.updateSection(0, 0);
        ghostPieces.updateSection(0, 0);
    }

    void reload() {
        board.clear();
    }

    void updateSettings(uint32_t das, uint32_t arr) {
        dasSetting = das;
        arrSetting = arr;
    }

private:
    void tryMovePiece(MoveDirection direction, bool JustPressed) {

        int offset{};
        if (direction == MoveDirection::Left)
            offset = -1;
        else
            offset = 1;

        if (JustPressed) {
            dasIterator = 0;
            arrIterator = 0;
            currentPiece.setX(currentPiece.x + offset);
            if (board.isCollide(currentPiece)) {
                currentPiece.setX(currentPiece.x - offset); // failed, go back
                return;
            } else
                return;

        }

        if (dasIterator >= dasSetting) {
            // success! now try to das
            if (arrIterator >= arrSetting) {
                for (; arrIterator >= arrSetting; arrIterator -= arrSetting) {
                    currentPiece.setX(currentPiece.x + offset);
                    if (board.isCollide(currentPiece)) {
                        currentPiece.setX(currentPiece.x - offset); // failed, go back
                        return;
                    }
                }
            } else
                arrIterator += (1000 / UPDATES_A_SECOND);
        } else
            dasIterator += (1000 / UPDATES_A_SECOND);

        return;
    }

    bool tryRotate(Piece& piece, TurnDirection direction) {
        auto incrRotClockWise = [&](RotationDirection& spin) {
            switch (spin) {
            case North:
                spin = East;
                break;
            case East:
                spin = South;
                break;
            case South:
                spin = West;
                break;
            case West:
                spin = North;
                break;
            default:
                break;
            }
            };
        auto TspinDetection = [](Piece& piece, Board& board, bool& Tspinned) {
            auto isEmpty = [](int x, int y, Board& board) {
                if ((((0 <= y) && (y < LOGICALBOARDHEIGHT))) && (((0 <= x) && (x < BOARDWIDTH)))) //if inbounds of board
                {
                    //if (piece.piecedef[y][x] != empty) // is the cell empty in the piece matrix is empty
                    if (board.board.at(x).at(y) != empty) // is the cell in the board matrix empty
                        return true;
                    else
                        return false;
                }

                if ((x < 0) || (x >= BOARDWIDTH)) // cant be out of bounds on either direction
                {
                    return true;
                }

                if (y < 0) // can be above, but not below the board
                {
                    return true;
                }
                return false;
                };
            if (piece.kind == PieceType::T) {
                bool topright(isEmpty(piece.x + 1, piece.y + 1, board));
                bool topleft(isEmpty(piece.x - 1, piece.y + 1, board));
                bool bottomright(isEmpty(piece.x + 1, piece.y - 1, board));
                bool bottomleft(isEmpty(piece.x - 1, piece.y - 1, board));
                uint8_t corners = topright + topleft + bottomright + bottomleft;
                if (corners >= 3)
                    Tspinned = true;
                else
                    Tspinned = false;
            }
            };
        // temporary x and y to know their initial location
        const int_fast8_t x = piece.x;
        const int_fast8_t y = piece.y;
        if (direction == Right) {
            piece.rotateCW();
        } else if (direction == Left) {
            piece.rotateCCW();
        } else if (direction == oneEighty) {
            piece.rotateCCW();
            piece.rotateCCW();
        }

        // spinclockwise should be a bool, but it can also be 2 as in rotating twice
        // in one frame aka 180 spin
        if (direction != TurnDirection::oneEighty) {
            RotationDirection nextDir = piece.spin;
            if (direction == Right)
                incrRotClockWise(nextDir);
            else if (direction == Left) {
                incrRotClockWise(nextDir); incrRotClockWise(nextDir); incrRotClockWise(nextDir);
            }

            auto* offsetData = &JLSTZPieceOffsetData[piece.spin];
            auto* nextOffset = &JLSTZPieceOffsetData[nextDir];
            if (piece.kind == PieceType::I) {
                offsetData = &IPieceOffsetData[piece.spin];
                nextOffset = &IPieceOffsetData[nextDir];
            } else if (piece.kind == PieceType::O) {
                offsetData = &OPieceOffsetData[piece.spin];
                nextOffset = &OPieceOffsetData[nextDir];
            }

            for (int i = 0; i < kicks; ++i) {

                piece.setX(x + (*offsetData)[i].x - (*nextOffset)[i].x);
                piece.setY(y + (*offsetData)[i].y - (*nextOffset)[i].y);

                if (!board.isCollide(piece)) {
                    piece.spin = nextDir;
                    TspinDetection(piece, board, Tspinned);
                    return true;
                }
            }
            piece.setX(x); piece.setY(y);
        } else {
            RotationDirection nextDir = piece.spin;
            incrRotClockWise(nextDir); incrRotClockWise(nextDir);

            auto* kickdata = &wallkick180data[piece.spin];
            bool isI = false;
            if (piece.kind == PieceType::I) {
                isI = true;
                kickdata = &Iwallkick180data[piece.spin];
            }

            for (int i = 0; i < 12; ++i) {

                piece.setX(x + ((*kickdata)[i].x + (isI * IPrecalculatedwallkick180offsets[piece.spin].x)));
                piece.setY(y - ((*kickdata)[i].y + (isI * IPrecalculatedwallkick180offsets[piece.spin].y)));

                if (!board.isCollide(piece)) {
                    piece.spin = nextDir;
                    TspinDetection(piece, board, Tspinned);
                    return true;
                }
            }
            piece.setX(x); piece.setY(y);
        }

        //rotate the matrix back if nothing worked
        if (direction == Right) {
            piece.rotateCCW();
        } else if (direction == Left) {
            piece.rotateCW();
        } else if (direction == oneEighty) {
            piece.rotateCCW();
            piece.rotateCCW();
        }
        return false;
    }

    Board board;
    SurfaceSpriteSheet pieces;
    SurfaceSpriteSheet ghostPieces;
    Sprite background;
    Sprite matrix;
    SurfaceTexture surface_matrix;
    std::array<SurfaceTexture, 5> surface_queue;
    SurfaceTexture surface_hold;
    std::vector<Piece> queue{};
    Piece hold{ PieceType::empty };
    Piece currentPiece{ PieceType::empty };
    TTF_Font* font{};
    uint64_t timeWhenInit{};
    int32_t piecesPlaced{};
    int32_t softdropCountdown = softdropCountdownMAX;
    int32_t pieceSpawnDelay = pieceSpawnDelayMAX;

    // these are in miliseconds
    uint_fast16_t dasSetting = 80;
    uint_fast16_t arrSetting = 0;

    int16_t matrixX = 0;
    int16_t matrixY = 0;

    uint16_t dasIterator = 0;
    uint16_t arrIterator = 0;
    uint16_t lockDelayIncrementer{};
    uint16_t numLinesCleared{};
    uint16_t numDamageSent{};
    int8_t currentCombo{};
    bool placedPiece{};
    bool isDie{};
    bool checkForLineClear{};
    bool alreadyHeld{};
    bool b2b{};
    bool leftPressedMostRecent{};
    bool rightPressedMostRecent{};
    bool Tspinned{};

};

Game::Game() {

}

void Game::gameLogic(const Shakkar::inputs& input) {
    Shakkar::Key k_left = input.getKey(SDL_KeyCode::SDLK_a);
    Shakkar::Key k_right = input.getKey(SDLK_d);
    Shakkar::Key k_rotLeft = input.getKey(SDLK_LEFT);
    Shakkar::Key k_rotRight = input.getKey(SDLK_RIGHT);
    Shakkar::Key k_rot180 = input.getKey(SDLK_DOWN);
    Shakkar::Key k_hardDrop = input.getKey(SDLK_w);
    Shakkar::Key k_softDrop = input.getKey(SDLK_s);
    Shakkar::Key k_sonicDrop = input.getKey(SDLK_x);
    Shakkar::Key k_hold = input.getKey(SDLK_UP);

    if (k_left.pressed) {
        leftPressedMostRecent = true;
        rightPressedMostRecent = false;
    } else if (k_right.pressed) {
        rightPressedMostRecent = true;
        leftPressedMostRecent = false;
    }

    bool leftState{};
    bool rightState{};

    // fucking magic
    leftState = (rightPressedMostRecent && (k_right.pressed || k_right.held)) ? false : (k_left.pressed || k_left.held);
    rightState = (leftPressedMostRecent && (k_left.pressed || k_left.held)) ? false : (k_right.pressed || k_right.held);

    if (checkForLineClear) {
        int linesCleared = board.clearLines();
        bool perfectCleared = true;
        for (auto& row : board.board) {
            for (auto& mino : row) {
                if (mino == empty)
                    continue;
                else {
                    perfectCleared = false;
                    break;
                }
            }
        }
        if (linesCleared == 0)
            currentCombo = 0;
        auto tmp1 = std::clamp(int(linesCleared), 0, DAMAGETABLESIZE - 1);
        auto tmp2 = std::clamp(int(currentCombo), 0, COMBOTABLESIZE - 1);
        if (perfectCleared) {
            numDamageSent += perfectClearDamage + comboTable[tmp2];
        } else if (Tspinned) {
            numDamageSent += (linesCleared * 2) + comboTable[tmp2];
        } else
            numDamageSent += damageTable[tmp1] + comboTable[tmp2];

        if (linesCleared > 0) {
            currentCombo++;
            this->numLinesCleared += linesCleared;
        }

        if ((linesCleared == 4) || Tspinned) {
            if (b2b) {
                numDamageSent += backToBackBonus;
            }
            b2b = true;
            Tspinned = false;
        } else if (linesCleared > 0)
            b2b = false;
        checkForLineClear = false;
    }

    if ((currentPiece.kind != PieceType::empty) && !isDie) // we have a piece!
    {

        if (leftState) {
            tryMovePiece(MoveDirection::Left, !(k_left.held || k_left.released));
        } else if (rightState) {
            tryMovePiece(MoveDirection::Right, !(k_right.held || k_right.released));
        }

        {
            if (softdropCountdown <= 0) {
                //softdrop pls
                if (!board.trySoftDrop(currentPiece)) {
                    //failed, we are on the ground
                    if (lockDelayIncrementer >= lockDelayMAX) {
                        board.setPiece(currentPiece);
                        currentPiece.kind = PieceType::empty;
                        checkForLineClear = true;
                        lockDelayIncrementer = 0;
                        alreadyHeld = false;
                        placedPiece = true;
                    } else
                        lockDelayIncrementer++;

                } else
                    softdropCountdown = softdropCountdownMAX;
            } else {

                if (k_softDrop.pressed || k_softDrop.held)
                    softdropCountdown -= 40;
                else
                    softdropCountdown -= 1;
            }
            // rotate right logic
            if (k_rotLeft.pressed)
                tryRotate(currentPiece, TurnDirection::Left);

            //rotate left logic
            else if (k_rotRight.pressed)
                tryRotate(currentPiece, TurnDirection::Right);

            // 180 logic
            else if (k_rot180.pressed)
                tryRotate(currentPiece, TurnDirection::oneEighty);

            //sonic drop logic
            if (k_sonicDrop.pressed || k_sonicDrop.held)
                board.sonicDrop(currentPiece);

            // hold logic
            if (k_hold.pressed) {
                if (!alreadyHeld) {
                    std::swap(currentPiece, hold);
                    currentPiece.setX(4);
                    currentPiece.setY(VISUALBOARDHEIGHT - 1);

                    switch (currentPiece.spin) {
                    case North:
                        break;
                    case East:
                        currentPiece.rotateCCW();
                        break;
                    case South:
                        currentPiece.rotateCCW(); currentPiece.rotateCCW();
                        break;
                    case West:
                        currentPiece.rotateCW();
                        break;
                    default:
                        break;
                    }
                    currentPiece.spin = North;

                    switch (hold.spin) {
                    case North:
                        break;
                    case East:
                        hold.rotateCCW();
                        break;
                    case South:
                        hold.rotateCCW(); hold.rotateCCW();
                        break;
                    case West:
                        hold.rotateCW();
                        break;
                    default:
                        break;
                    }
                    hold.spin = North;
                    Tspinned = false;
                    alreadyHeld = true;

                    if (board.isCollide(currentPiece)) {
                        currentPiece.setY(currentPiece.y + 1);
                        if (board.isCollide(currentPiece)) {
                            currentPiece.setY(currentPiece.y - 1);
                            board.makeBoardGarbage();
                            isDie = true;
                        }
                    }
                    Shakkar::inputs temp = input;
                    temp.update();
                    this->gameLogic(temp);
                }
            }

            //harddrop logic
            if (k_hardDrop.pressed) {
                board.sonicDrop(currentPiece);
                board.setPiece(currentPiece);
                currentPiece.kind = PieceType::empty;
                checkForLineClear = true;
                lockDelayIncrementer = 0;
                alreadyHeld = false;
                placedPiece = true;
            }


        }
    } else if ((pieceSpawnDelay <= 0) && !isDie) { //guarenteed no piece

        pieceSpawnDelay = pieceSpawnDelayMAX;
        softdropCountdown = softdropCountdownMAX;

        currentPiece = queue.at(0);
        queue.erase(queue.begin());
        queue.emplace_back(Piece(getRandPiece()));

        if (board.isCollide(currentPiece)) {
            currentPiece.setY(currentPiece.y + 1);
            if (board.isCollide(currentPiece)) {
                currentPiece.setY(currentPiece.y - 1);
                board.makeBoardGarbage();
                isDie = true;
            }
        }
        leftState = false;
        rightState = false;
        checkForLineClear = false;

    } else
        pieceSpawnDelay--;
}


void Game::render(Window& window) {
    window.clear();
    auto [w, h] = window.get_window_size();

    background.destRect = window.calculate_outer_rect({ 0, 0, w, h }, (float)background.srcRect.w / background.srcRect.h);
    background.render(window);

    //game_surface.createSurface(window, matrix.srcRect.w, matrix.srcRect.h);
    //game_surface.blitSurface(window, matrix.texture, 0, 0);

    matrix.destRect = window.calculate_inner_rect({ 0, 0, w, h }, (float)matrix.srcRect.w / matrix.srcRect.h);
    matrix.destRect = window.horizontal_align({ 0, 0, w, h }, matrix.destRect);
    matrix.render(window);

    auto doesPieceHaveMinoHere = [](Piece& piece, int x, int y) {
        for (auto& coord : piece.piecedef)
            if (((coord.x + piece.x) == x) && ((coord.y + piece.y) == y)) {
                return true;
            }
        return false;
        };

    auto doesVisualHaveMinoHere = [](Piece& piece, int x, int y) {
        for (auto& coord : piece.piecedef)
            if (((coord.x + 2) == x) && ((coord.y + 2) == y)) {
                return true;
            }
        return false;
        };
    auto get_render_index = [](ColorType block) {
        switch (block) {
        case S:
            return 5;
        case Z:
            return 2;
        case J:
            return 7;
        case L:
            return 3;
        case T:
            return 8;
        case O:
            return 4;
        case I:
            return 6;
        case empty:
            return 1;
        case garbage:
            return 0;
        case line_clear:
            return 9;
        default:
            break;
        }
        return 0;
        };

    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < board.board.size(); x++) {
            auto& column = board.board[x];
            auto& mino = column[y];

            pieces.updateSection(get_render_index(mino), 0);
            SDL_Rect dest = { 16 * x, 16 * (VISUALBOARDHEIGHT - 1 - y), 16, 16 };
            surface_matrix.blitSpriteSurface(window, pieces, dest);
        }
    }

    // render the current piece
    for (auto& mino_cord : this->currentPiece.piecedef) {
        pieces.updateSection(get_render_index(PieceTypeToColorType(this->currentPiece.kind)), 0);
        int x = mino_cord.x + this->currentPiece.x;
        int y = mino_cord.y + this->currentPiece.y;
        if (y > VISUALBOARDHEIGHT - 1)
            continue;

        SDL_Rect dest = { 16 * x, 16 * (VISUALBOARDHEIGHT - 1 - y), 16, 16 };
        surface_matrix.blitSpriteSurface(window, pieces, dest);
    }


    // calculate the ghost piece
    Piece ghost(currentPiece.kind, currentPiece.x, currentPiece.y, currentPiece.spin);
    if (ghost.kind != PieceType::empty)
        board.sonicDrop(ghost);

    // render the ghost piece on the matrix
    for (auto& mino_cord : ghost.piecedef) {
        ghostPieces.updateSection(get_render_index(PieceTypeToColorType(ghost.kind)), 0);
        int x = mino_cord.x + ghost.x;
        int y = mino_cord.y + ghost.y;
        if (y > VISUALBOARDHEIGHT - 1)
            continue;

        SDL_Rect dest = { 16 * x, 16 * (VISUALBOARDHEIGHT - 1 - y), 16, 16 };

        surface_matrix.blitSpriteSurface(window, ghostPieces, dest);
    }

    // render the queue pieces
    for (size_t i = 0; i < 5; i++) {
        auto& piece = queue[i];
        auto& queue_view = this->surface_queue[i];

        // make the queue empty before rendering the queue
        pieces.updateSection(get_render_index(ColorType::empty), 0);
        for (int x = 0; x < 5; x++)
            for (int y = 0; y < 5; y++) {
                SDL_Rect dest = { 16 * (x), 16 * (4 - y), 16, 16 };
                queue_view.blitSpriteSurface(window, pieces, dest);
            }
        for (auto& mino_cord : piece.piecedef) {
            pieces.updateSection(get_render_index(PieceTypeToColorType(piece.kind)), 0);
            int x = mino_cord.x + 2;
            int y = mino_cord.y + 2;
            SDL_Rect dest = { 16 * x, 16 * (5 - y - 1), 16, 16 };
            queue_view.blitSpriteSurface(window, pieces, dest);
        }
    }

    // render the hold onto the surface
	for (int x = 0; x < 5; x++) {
		for (int y = 0; y < 5; y++) {
			// make the hold empty before rendering the hold
			pieces.updateSection(get_render_index(ColorType::empty), 0);
			SDL_Rect dest = { 16 * (x), 16 * (4 - y), 16, 16 };
			surface_hold.blitSpriteSurface(window, pieces, dest);
		}
	}

    if (hold.kind != PieceType::empty) {
        for (auto& mino : hold.piecedef) {
			// render the hold piece
			pieces.updateSection(get_render_index(PieceTypeToColorType(hold.kind)), 0);
			int x = mino.x + 2; // offset by 2 to center it in the 5x5 grid
			int y = mino.y + 2; // offset by 2 to center it in the 5x5 grid
			SDL_Rect dest = { 16 * x, 16 * (4 - y), 16, 16 };
			surface_hold.blitSpriteSurface(window, pieces, dest);
        }
    }


    // render the surface matrix
    surface_matrix.destRect = window.calculate_logical_rect(matrix.destRect, { 50, 34, 170 - 50,274 - 34 }, matrix.srcRect.w, matrix.srcRect.h);
    surface_matrix.createTexture(window);
    surface_matrix.render(window);

    // render the queue to the screen
    for (int i = 0; i < 5; i++) {
        // the first should be at 172,34, and be 40x40 
        auto& queue_view = this->surface_queue[i];
        queue_view.destRect = window.calculate_logical_rect(matrix.destRect, { 172, 34 + (i * 40) + (i * 2), 40, 40 }, matrix.srcRect.w, matrix.srcRect.h);
        queue_view.createTexture(window);
        queue_view.render(window);
    }

    // render the hold onto the location 8,34,40,40
	surface_hold.destRect = window.calculate_logical_rect(matrix.destRect, { 8,34, 40, 40 }, matrix.srcRect.w, matrix.srcRect.h);
	surface_hold.createTexture(window);
	surface_hold.render(window);


    SDL_Color color{};
    color.r = 0xff; color.g = 0xff; color.b = 0xff;
    if (placedPiece) {
        piecesPlaced++;
        placedPiece = false;
    }

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 timePassed = ((double)(now - timeWhenInit) / SDL_GetPerformanceFrequency()) * 1000;
    auto pps = std::to_string((static_cast<const double>(piecesPlaced) / timePassed) * 1000);
    std::string textbuf = "PPS: ";
    pps.erase(pps.length() - 4, 4);
    textbuf.append(pps);
    SDL_Surface* TextSurface = TTF_RenderText_Solid(font, textbuf.c_str(), color);
    SDL_Texture* text = window.create_texture_from_surface(TextSurface);

    SDL_Rect test = { 0, surface_matrix.destRect.y + int(surface_matrix.destRect.h / 1.9), 0, 0 };
    int text_x = surface_matrix.destRect.x;

    test.w = TextSurface->w;
    test.h = TextSurface->h;
    test.x = text_x - test.w - 10;

    SDL_SetTextureColorMod(text, 255, 0, 0);
    window.render_copy(text, NULL, &test);


    textbuf = "APM: ";
    pps = std::to_string((static_cast<const double>(numDamageSent) / (timePassed / 60)) * 1000);
    pps.erase(pps.length() - 4, 4);
    textbuf.append(pps);
    SDL_FreeSurface(TextSurface);
    TextSurface = TTF_RenderText_Solid(font, textbuf.c_str(), color);
    SDL_DestroyTexture(text);
    text = window.create_texture_from_surface(TextSurface);
    test.y += test.h + 20;
    test.w = TextSurface->w;
    test.h = TextSurface->h;
    test.x = text_x - test.w - 10;
    SDL_SetTextureColorMod(text, 255, 0, 0);
    window.render_copy(text, NULL, &test);

    textbuf = "damage sent: ";
    pps = std::to_string(numDamageSent);
    textbuf.append(pps);
    SDL_FreeSurface(TextSurface);
    TextSurface = TTF_RenderText_Solid(font, textbuf.c_str(), color);
    SDL_DestroyTexture(text);
    text = window.create_texture_from_surface(TextSurface);
    test.y += test.h + 20;
    test.w = TextSurface->w;
    test.h = TextSurface->h;
    test.x = text_x - test.w - 10;
    SDL_SetTextureColorMod(text, 255, 0, 0);
    window.render_copy(text, NULL, &test);

    textbuf = "normalized Cords";
    SDL_FreeSurface(TextSurface);
    TextSurface = TTF_RenderText_Solid(font, textbuf.c_str(), color);
    SDL_DestroyTexture(text);
    text = window.create_texture_from_surface(TextSurface);
    test.y += test.h + 20;
    test.w = TextSurface->w;
    test.h = TextSurface->h;
    test.x = text_x - test.w - 10;
    SDL_SetTextureColorMod(text, 255, 0, 0);
    window.render_copy(text, NULL, &test);

    SDL_FreeSurface(TextSurface);
    SDL_DestroyTexture(text);
}

Game::~Game() {

}