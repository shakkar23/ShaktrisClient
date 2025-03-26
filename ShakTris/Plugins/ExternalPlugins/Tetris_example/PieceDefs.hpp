#pragma once
#include <stdint.h>

// the definition needs to be a square, otherwise change how you rotate pieces pls
constexpr auto MINOSINAPIECE = 4;
struct Coord {
    int8_t x; int8_t y;
};

enum ColorType : uint_fast8_t {

    //Color for pieces
    S, Z, J, L, T, O, I,
    //special types
    empty,
    line_clear,
    garbage,
    number_of_ColorTypes
}; 

enum class PieceType : uint_fast8_t {
    //actual pieces
    S = ColorType::S,
    Z = ColorType::Z,
    J = ColorType::J,
    L = ColorType::L,
    T = ColorType::T,
    O = ColorType::O,
    I = ColorType::I,
    empty = ColorType::empty,
    number_of_PieceTypes
};

struct Colors{
    ColorType a : 4;
    ColorType b : 4;
};

const ColorType PieceTypeToColorType(PieceType color) {
    switch (color)
    {
    case PieceType::S:
        return S;
    case PieceType::Z:
        return Z;
    case PieceType::J:
        return J;
    case PieceType::L:
        return L;
    case PieceType::T:
        return T;
    case PieceType::O:
        return O;
    case PieceType::I:
        return I;
    case PieceType::empty:
    case PieceType::number_of_PieceTypes:
    default:
        return empty;
    }
    return empty;
}
enum RotationDirection : uint_fast8_t {
    North,
    East,
    South,
    West,
    number_of_RotationDirections
};
enum TurnDirection : uint_fast8_t {
    Left,
    Right,
    number_of_TurnDirections,
    oneEighty,
};
enum class MoveDirection : uint_fast8_t {
    Left,
    Right,
    number_of_MoveDirections,
};

// note: for some reason the piece definition is [piecetype][PIECEHEIGHT - 1 - y][x]
// everything else in the repo accounts for this, so there is no point in changing it as of writing this note, ¯\_(ツ)_/¯
//constexpr ColorType PieceDefinition[(int)PieceType::number_of_PieceTypes][4] = {
constexpr std::array<std::array<Coord, MINOSINAPIECE>, (int)PieceType::number_of_PieceTypes> PieceDefintions{

    {
            {{{-1, 0}, {0, 0}, {0, 1}, { 1, 1}}},  // S
            {{{-1, 1}, {0, 1}, {0, 0}, { 1, 0}}},  // Z
            {{{-1, 0}, {0, 0}, {1, 0}, {-1, 1}}},  // J
            {{{-1, 0}, {0, 0}, {1, 0}, { 1, 1}}},  // L
            {{{-1, 0}, {0, 0}, {1, 0}, { 0, 1}}},  // T
            {{{ 0, 0}, {1, 0}, {0, 1}, { 1, 1}}},  // O
            {{{-1, 0}, {0, 0}, {1, 0}, { 2, 0}}},  // I
            {{{ 0, 0}, {0, 0}, {0, 0}, { 0, 0}}}   // NULL
    }
};
