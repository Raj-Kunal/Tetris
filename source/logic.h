#pragma once

/// Required libraries
#include <iostream>
#include <algorithm>
#include <cassert> // Error handling library, abort program if false
#include <vector>
#include <random>

using namespace std;
const int N_Pieces = 7;

/* Enum are used for index aliasing */

enum TileColor
{
    kEmpty = -1,
    kCyan,
    kBlue,
    kOrange,
    kYellow,
    kGreen,
    kPurple,
    kRed
};
enum PieceKind
{
    kNone = -1,
    kPieceI,
    kPieceJ,
    kPieceL,
    kPieceO,
    kPieceS,
    kPieceT,
    kPieceZ
};
enum class Rotation
{
    kRight,
    kLeft
};
enum class Motion
{
    kNone,
    kRight,
    kLeft
};

/* Class Piece represents a game piece ("tetrimino"), 
it defines how a piece rotates and kicks off obstacles. */
class Piece
{
public:
    // Disables implicit converdion
    explicit Piece(PieceKind kind); // Explicit Constructor

    PieceKind kind() const { return kind_; }
    TileColor color() const { return color_; }
    int bBoxSide() const { return bBoxSide_; }
    int nRows() const { return nRows_; }
    int nCols() const { return nCols_; }

    // Store Current Shape of figure
    const vector<TileColor> &shape() const { return shape_; }
    // Initial shape of spawned figure
    const vector<TileColor> &initialShape() const { return shape_; }

    void rotate(Rotation rotate);
    const vector<pair<int, int>> kicks(Rotation rotation) const;

private:
    // Value doesn't change even after multiple function calls
    static const int NumStates_;
    // Left right rotations
    static const vector<vector<pair<int, int>>> KicksIRight_, KicksILeft_;
    static const vector<vector<pair<int, int>>> KicksOtherRight_, KicksOtherLeft_;

    PieceKind kind_;
    TileColor color_;
    int nRows_, nCols_;
    int bBoxSide_;
    int state_;

    // stores initial state
    vector<TileColor> initialShape_;
    vector<TileColor> shape_;

    // Store rotational movement
    vector<vector<pair<int, int>>> KicksLeft_, KicksRight_;
};

/* Class Board represents the geometric state of the board. 
It stores which tiles are occupied, the position of the current piece and processes required motions obeying geometric constraints.  */
class Board
{
public:
    const int nRows, nCols;
    Board(int nRows, int nCols);
    void clear(); // Board clearing

    TileColor tileAt(int row, int col) const { return tiles_[((row + RowsAbove_) * nCols) + col]; };

    bool frozePiece(); // specify when to stop moving for pieces
    bool spawnPiece(PieceKind kind);
    bool moveHorizontal(int Col);
    bool moveVertical(int Row);
    bool rotate(Rotation rotate);
    bool isOnGround() const;
    int hardDrop();

    // Depedency function for point system
    int numLinesToClear() const { return linesToClear_.size(); };
    void clearLines();

    // Stores which line to clear after filling
    const vector<int> &linesToClear() const { return linesToClear_; }
    Piece piece() const { return piece_; }
    int pieceRow() const { return row_; }
    int pieceCol() const { return col_; }

    // guidance feature to users
    int ghostRow() const { return ghostRow_; }

private:
    static const int RowsAbove_;
    vector<TileColor> tiles_;
    // Stores updated state
    vector<TileColor> tilesAfterClear_;
    vector<int> linesToClear_;
    Piece piece_;
    int row_, col_;
    int ghostRow_;

    // Master function for tile creation
    void setTile(int row, int col, TileColor color);
    // Check for color filled tile
    bool isTileFilled(int row, int col) const;
    // Check for valid function
    bool isPositionPossible(int row, int col, const Piece &piece) const;
    void updateGhostRow();
    // Dependency function for point system
    void findLinesToClear();
};

/* Class Tetris operates on Board and defines game timings, user input processing and scoring. */
class Tetris
{
public:
    // Board Constructor for board generation
    Tetris(Board &board, double timeStep, u_int randomSeed);
    // Restart the game with level as parameter
    void restart(int lvl);
    bool isGameOver() const { return gameOver_; }

    void update(bool sDrop, bool mRight, bool mLeft);
    void rotate(Rotation rotate);
    // Use of manually set tile position
    void hardDrop();
    void hold();

    double lockPercent() const { return lockingTimer_ / kLockDownTimeLimit_; }
    // time lag after line deletion
    bool isPausedForLinesClear() const { return pausedForLinesClear_; }
    double linesClearPausePercent() const { return linesClearTimer_ / kPauseAfterLineClear_; }

    // level for game
    int level() const { return level_; }
    // Keep track of lines cleared
    int linesCleared() const { return linesCleared_; }
    int score() const { return score_; }
    // Return next piece state
    Piece nextPiece() const { return Piece(bag_[nextPiece_]); };
    Piece heldPiece() const { return Piece(heldPiece_); }

private:
    static const int kLinesToClearPerLevel_;
    static const int kMaxLevel_;
    static const double kMoveDelay_;
    static const double kMoveRepeatDelay_;
    static const double kSoftDropSpeedFactor_;
    static const double kLockDownTimeLimit_;
    static const int kLockDownMovesLimit_;
    static const double kPauseAfterLineClear_;

    Board &board_;

    bool gameOver_;

    double timeStep_;
    // It generates sudo random numbers
    default_random_engine rng_;
    vector<PieceKind> bag_;

    int level_;
    int linesCleared_;
    int score_;
    int nextPiece_;
    int nMovesWhileLocking_;

    PieceKind heldPiece_;
    bool moveLeftPrev_, moveRightPrev_;
    bool pausedForLinesClear_;
    bool isOnGround_;
    bool canHold_;

    double secondsPerLine_;
    double moveDownTimer_;
    double moveRepeatDelayTimer_;
    double moveRepeatTimer_;
    double linesClearTimer_;
    double lockingTimer_;

    Motion motion_;
    /* Required functions for board generation */
    void moveHorizontal(int dCol);
    void checkLock();
    void lock();
    void spawnPiece();
    void updateScore(int linesCleared);

    // Difficulty Setting
    double secondsPerLineForLevel(int level) const { return pow(0.8 - (level - 1) * 0.007, level - 1); }
};
