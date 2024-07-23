/** 
        Do this #define GLT_CHESS_IMPLEMENTATION
        before you include this file when you need function implementation

        There is a example code at the end of the file 

        TODO: 
        1. Implement fen 
        2. Implement move auditing
        3. Move forward and backward (fen -> board -> fen)
        4. Implement search algorithm
        5. Read chess database
*/


#define GLT_CHESS_API static

// typedefs for convience might need to exclude it later

#include <assert.h>
#include <stdint.h>
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint8_t u8;
typedef int8_t i8;
typedef float f32;
typedef double f64;

/**
 * This represents all the pieces possible to in chess board
 * So the chess board is just a array of length 64 of this enum
 * */
typedef enum {
        GLT_none = 0,
        GLT_white_pawn,
        GLT_white_king,
        GLT_white_queen,
        GLT_white_rook,
        GLT_white_bishop,
        GLT_white_knight,
        GLT_black_pawn,
        GLT_black_king,
        GLT_black_queen,
        GLT_black_rook,
        GLT_black_bishop,
        GLT_black_knight,
} glt_piece_i32;

/* Just to reduce size as glt_pieces is i32*/
typedef u8 glt_piece;

/*
 *
 *  000000000000000
 * 8                0
 * 7                0 //black
 * 6                0
 * 5                0
 * 4                0
 * 3                0
 * 2                0 //white
 * 1                0
 *  1 2 3 4 5 6 7 8
 * It's better to use the left buttom up because 
 * It can easily converted to and from the coord struct
 * x represents the file and y represents the rank
 */
typedef struct {
        i8 x, y;
}glt_pos;

/**
        * Moves and move audit are a linked list node by default 
        * Because most of the time they are store in multiple
*/
typedef struct glt_move glt_move;
struct glt_move{
        glt_pos start, end;
        glt_move* next;
};

/**
         * This is used to store the moves that has been played 
         * The move in notation is stored to help revert the move
 */
typedef struct glt_move_audit glt_move_audit;
struct glt_move_audit{
        glt_pos start, end;
        char str[10]; //move in notation
        glt_move_audit* next;
};


/*
        * This represents coordinate in the board
        * File is represented by char 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'
        * rank is represented by u8 1, 2, 3, 4, 5, 6, 7, 8
        * TODO: just keep one of the glt_coord or glt_pos
        * 
        *  a b c d e f g h 
        * 8                8
        * 7                7 //black
        * 6                6
        * 5                5
        * 4                4
        * 3                3
        * 2                2 //white
        * 1                1
        *  a b c d e f g h 
*/
typedef struct {
        char file;
        u8 rank;
} glt_coord;


typedef enum{
        glt_flag_active_color  = 1 << 1, /* white if 1 black of 0 */ 
        //TODO: implementt storing castling rights and en_pasant
        //glt_active_color  = 1 << 2,
        //glt_active_color  = 1 << 3,
        glt_white_queen_castle = 1 << 2,
        glt_white_king_castle  = 1 << 3,
        glt_black_king_castle  = 1 << 4,
        glt_black_queen_castle = 1 << 5,
}glt_flags;
typedef struct{
        glt_piece pieces[64];
        u32 flags;  
        u8 half_move_clock, full_move_clock;
} glt_chess_board;

GLT_CHESS_API int glt_pos_is_equal(glt_pos a, glt_pos b);
GLT_CHESS_API int glt_piece_is_black(glt_piece piece);
GLT_CHESS_API int glt_piece_is_white(glt_piece piece);

/**
         * Initilizes board to the start
         * Sets the active color to white
 */
GLT_CHESS_API void glt_initilize_board(glt_chess_board* board);

/** 
         * Layout of pieces in board->pieces
         *  0 is A, 1 
         *  it moves along the files so  index 1 is B, 1
*/

GLT_CHESS_API inline int glt_pos_to_index(glt_pos pos);

/**
         * Given the pos and board returns the piece enum
 */
GLT_CHESS_API inline int glt_piece_at_pos(glt_chess_board* board, glt_pos pos);

/** 
         * Takes in a board state and returns if the piece is of same color as active color 
*/
GLT_CHESS_API inline int glt_piece_is_active_color(glt_chess_board* board, glt_piece piece);

/** 
         * This function checks if both pieces are of same color not if they are equal
*/
GLT_CHESS_API inline int glt_pieces_is_same_color(glt_piece a, glt_piece b);

/** 
         * Checks if the position is within the bound of the board
*/
GLT_CHESS_API inline int glt_pos_in_bounds(glt_pos pos) ;



/**
 * Returns the forsyth-edwards notation from the given board
 * https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation

 * Make sure that the string fen has enough memory allocated
*/
GLT_CHESS_API void glt_get_fen_from_board(glt_chess_board *board, char* fen, int len);


/**
 * Given a pawn's position in a board assuming it's white pawn,
 * Generate a linked list for the possible moves of the pawn
 * 
 * https://en.wikipedia.org/wiki/Pawn_(chess)
 * 
 */
GLT_CHESS_API glt_move* glt_generate_white_pawn_moves(glt_chess_board* board, glt_pos start);
GLT_CHESS_API glt_move* glt_generate_black_pawn_moves(glt_chess_board* board, glt_pos start);

/*
  Given a knight position generate the move
   position where the knight can move
   This should also check if the postion is empty or not
   Empty means position without the current color of night



    First get all 8 of the knight moves

    1. Calculate the position the 8 moves
    2. Don't add the ones that are outside the board
    2. Don't add the ones that are at the same collered board

 */
GLT_CHESS_API glt_move* glt_generate_knight_moves(glt_chess_board* board, glt_pos start);

/**
        Start from the start position
        Go thru all 4 direciton untill 
                - you get to our of board
                - you stumble upon a piece of the same color
                - one step forward if you stumble upon a piece of different color
*/
GLT_CHESS_API glt_move* glt_generate_rook_moves(glt_chess_board* board, glt_pos start);

/** 
         * Same rule as the rook moves but diagonal instead of straight 
 */
GLT_CHESS_API glt_move* glt_generate_bishop_moves(glt_chess_board* board, glt_pos start);


/**  
         * Same as combining glt_generate_rook_moves and glt_generate_bishop_moves but just go one step forward
*/
GLT_CHESS_API glt_move* glt_generate_king_moves(glt_chess_board* board, glt_pos start);

/** 
         * Same as Same as combining glt_generate_rook_moves and glt_generate_bishop_moves 
*/
GLT_CHESS_API glt_move* glt_generate_queen_moves(glt_chess_board* board, glt_pos start);

/** 
        * Generates possible moves for the piece at the position 
        * by Switch over the piece and return the possible moves of the piece
*/
GLT_CHESS_API glt_move* glt_generate_moves(glt_chess_board * board, glt_pos pos);

/**
        * Take in a glt_move and make that move
        * Let the user code handle iterating over the possible moves
        * It'll make our api simpler
        * TODO: don't make illigal moves
*/
GLT_CHESS_API int glt_make_move(glt_chess_board* board, glt_move move);
        
#ifdef GLT_CHESS_IMPLEMENTATION

#ifndef GLT_malloc
#include <stdlib.h>
#define GLT_malloc(x) (malloc(x))
#define GLT_free(x) (free(x))
#endif
/* Bit operations for accessing and manipulating flags */
static inline int glt__is_flag_set(u32 flags, glt_flags flag){
        return (flag & flags) > 0;
}

static inline void glt__flag_set(u32 *flags, glt_flags flag){
        *flags |= flag;
}

static inline void glt__clear_flag(u32 *flags, glt_flags flag){
        *flags &= ~flag;
}
static inline void glt__flip_flag(u32 *flags, glt_flags flag){
        *flags ^= flag;
}

static int glt_pos_is_equal(glt_pos a, glt_pos b)
{
        return (a.x == b.x) && ( a.y == b.y);
}

static int glt_piece_is_black(glt_piece piece)
{
        return (int)piece > 6;
}

static int glt_piece_is_white(glt_piece piece)
{
        return (int)piece <= 6 &&  (int)piece >=1;
}

static inline int glt_pos_to_index(glt_pos pos){
        return (pos.y-1) * 8 + (pos.x -1);
}

//index starts from 0 and ends at 63
static inline glt_pos glt_index_to_pos(int index){
        glt_pos ret;
        ret.x = (index % 8) + 1;
        ret.y = (index / 8) + 1;
        return ret;
}

/* Get in the piece at the pos */
static inline int glt_piece_at_pos(glt_chess_board* board, glt_pos pos)
{
        return board->pieces[glt_pos_to_index(pos)];
}


static inline int glt_piece_is_active_color(glt_chess_board* board, glt_piece piece)
{
        if (piece == GLT_none) return 1;
        else if (glt_piece_is_black(piece) && 
                !glt__is_flag_set(board->flags, glt_flag_active_color)) return 1;
        else if (glt_piece_is_white(piece) &&
                glt__is_flag_set(board->flags, glt_flag_active_color)) return 1;

        return 0;

}

static inline int glt_pieces_is_same_color(glt_piece a, glt_piece b){
        if (a == GLT_none || b == GLT_none) return 0;
        else if(glt_piece_is_black(a) && glt_piece_is_black(b)) return 1;
        else if(glt_piece_is_white(a) && glt_piece_is_white(b)) return 1;
        return 0;
}

static inline int glt_pos_in_bounds(glt_pos pos) {
  return !((pos.x > 8 || pos.x <= 0) || (pos.y > 8 || pos.y <= 0));
}

static inline glt_pos glt_coord_to_pos(glt_coord coord){
        glt_pos ret = { (int)coord.file - 64,  coord.rank};
        return ret;
}

static inline glt_coord glt_pos_to_coord(glt_pos pos){
        glt_coord ret = { 64 + (int)pos.x ,  pos.y};
        return ret;
}



static void glt_initilize_board(glt_chess_board* board){
        board->pieces[0] = GLT_white_rook;
        board->pieces[1] = GLT_white_knight;
        board->pieces[2] = GLT_white_bishop;
        board->pieces[3] = GLT_white_queen;
        board->pieces[4] = GLT_white_king;
        board->pieces[5] = GLT_white_bishop;
        board->pieces[6] = GLT_white_knight;
        board->pieces[7] = GLT_white_rook;

        for (int i = 0; i < 8; ++i) {
                board->pieces[8 + i] = GLT_white_pawn;
        }

        for (int i = 8; i < 48; ++i) {
                board->pieces[8 + i] = GLT_none;
        }

        board->pieces[56 + 0] = GLT_black_rook;
        board->pieces[56 + 1] = GLT_black_knight;
        board->pieces[56 + 2] = GLT_black_bishop;
        board->pieces[56 + 3] = GLT_black_queen;
        board->pieces[56 + 4] = GLT_black_king;
        board->pieces[56 + 5] = GLT_black_bishop;
        board->pieces[56 + 6] = GLT_black_knight;
        board->pieces[56 + 7] = GLT_black_rook;

        for (int i = 0; i < 8; ++i) {
                board->pieces[48 + i] = GLT_black_pawn;
        }

        board->flags = 0;
        glt__flag_set(&board->flags, glt_flag_active_color);
        //board->fen = (char*)malloc(1000);
}

static void glt_moves_delte(glt_move** head_ptr){
        
        glt_move* prev = *head_ptr;
        glt_move* curr = prev;

        while(curr){
                prev = curr->next;
                GLT_free(curr);
                curr = prev;
        }

        *head_ptr = NULL;
}

/**
 * This is just a simple append function for the linked list
 * this is heavely used inside the chess engies for generating moves and might not
 * be required outside the library
 */
static void glt__move_append(glt_move** head_ptr, glt_pos start, glt_pos end){
        if (!glt_pos_in_bounds(end)) return;

        /* you can't insert if the head pointer is null */
        assert(head_ptr != NULL);


        /* Cast to glt_move because c++ gives you error if you don't*/
        glt_move* new_move = (glt_move*)GLT_malloc(sizeof(glt_move));

        if (new_move != NULL)
        {
                new_move->start = start;
                new_move->end = end;
                new_move->next = NULL;

                if(*head_ptr == NULL)
                {
                        *head_ptr = new_move;
                }
                else {
                        glt_move* curr = *head_ptr;

                        /* go to the end*/
                        while (curr->next != NULL) { curr = curr->next; }

                        curr->next = new_move;
                }
        }else
        {
                //Failed to malloc
                assert(0);
        }

}


static glt_move* glt_generate_white_pawn_moves(glt_chess_board* board, glt_pos start)
{
        /* Should we malloc glt_move* just to store the pointer? */
        glt_move* head = NULL;

        glt_pos move_frd = start;
        move_frd.y += 1;

        /* Check if it can move one step forward */
        if (glt_piece_at_pos(board, move_frd) == GLT_none)
        {
                glt__move_append(&head, start, move_frd);

                /* Check if it can move two step forward (when it's in the second rank)*/

                if (start.y == 2){
                        move_frd.y += 1;

                        if(glt_piece_at_pos(board, move_frd) == GLT_none)
                        {
                                glt__move_append(&head, start, move_frd);
                        }
                }

        }

        /* check if it can some pieces */

        /* Right diagonal */
        glt_pos right_diag = start;
        right_diag.x += 1;
        right_diag.y += 1;

        if((glt_piece_at_pos(board, right_diag) != GLT_none) &&
        glt_piece_is_black(glt_piece_at_pos(board, right_diag))
        ){
                glt__move_append(&head, start, right_diag);
        }

        /* Left diagonal */
        glt_pos left_diag = start;
        left_diag.x -= 1;
        left_diag.y += 1;

        if((glt_piece_at_pos(board, left_diag) != GLT_none) &&
        glt_piece_is_black(glt_piece_at_pos(board, left_diag))
        ){
                glt__move_append(&head, start, left_diag);
        }

        return head;
}

static glt_move* glt_generate_black_pawn_moves(glt_chess_board* board, glt_pos start)
{
        /* Should we malloc glt_move* just to store the pointer? */
        glt_move* head = NULL;

        glt_pos move_frd = start;
        move_frd.y -= 1;

        /* Check if it can move one step forward */
        if (glt_piece_at_pos(board, move_frd) == GLT_none)
        {
                glt__move_append(&head, start, move_frd);

                /* Check if it can move two step forward (when it's in the second rank)*/

                if (start.y == 7){
                        move_frd.y -= 1;

                        if(glt_piece_at_pos(board, move_frd) == GLT_none)
                        {
                                glt__move_append(&head, start, move_frd);
                        }
                }

        }

        /* check if it can some pieces */

        /* Right diagonal */
        glt_pos right_diag = start;
        right_diag.x += 1;
        right_diag.y -= 1;

        if((glt_piece_at_pos(board, right_diag) != GLT_none) &&
        glt_piece_is_white(glt_piece_at_pos(board, right_diag))
        ){
                glt__move_append(&head, start, right_diag);
        }

        /* Left diagonal */
        glt_pos left_diag = start;
        left_diag.x -= 1;
        left_diag.y -= 1;

        if((glt_piece_at_pos(board, left_diag) != GLT_none) &&
        glt_piece_is_white(glt_piece_at_pos(board, left_diag))
        ){
                glt__move_append(&head, start, left_diag);
        }
        return head;
}


static glt_move* glt_generate_knight_moves(glt_chess_board* board, glt_pos start){
        glt_move* head = NULL;
        glt_pos moves[8] = { {-2, -1}, {-2, +1}, {-1, +2}, {-1, -2}, {+2, -1}, {+2, +1}, {+1, -2}, {+1, +2} };

        glt_piece start_piece = glt_piece_at_pos(board, start);
        for (size_t i = 0; i < 8; i++)
        {
                glt_pos new_move = {start.x + moves[i].x, start.y + moves[i].y};

                /* if position is empty just add it*/ 
                if (board->pieces[glt_pos_to_index(new_move)] == GLT_none) {
                        glt__move_append(&head, start, new_move);
                } else {

                        glt_piece curr_piece = glt_piece_at_pos(board, new_move);

                        /* if the pieces are different color than add it */
                        if(!glt_pieces_is_same_color(start_piece, curr_piece))
                        {
                                glt__move_append(&head, start, new_move);
                        }

                }
        }

        return head;
}

static glt_move* glt_generate_rook_moves(glt_chess_board* board, glt_pos start){
        glt_move* head = NULL;
        glt_piece start_piece = glt_piece_at_pos(board, start);

        glt_pos addr_arr[4] = {{ 0, 1}, {0, -1}, { 1, 0}, {-1, 0}};

        for (size_t i = 0; i < 4; i++)
        {
                glt_pos adder = addr_arr[i];
                glt_pos new_move = start;
                new_move.y += adder.y ;
                new_move.x += adder.x ;

                while(glt_pos_in_bounds(new_move))
                {
                        glt_piece curr_piece = glt_piece_at_pos(board, new_move);

                        if (curr_piece == GLT_none)  glt__move_append(&head, start, new_move);

                         else if(glt_pieces_is_same_color(start_piece, curr_piece)) break;

                        else{
                                glt__move_append(&head, start, new_move);
                                break;
                        }
                        new_move.y += adder.y;
                        new_move.x += adder.x;
                }
        }
        return head;
        

}

static glt_move* glt_generate_bishop_moves(glt_chess_board* board, glt_pos start)
{
        glt_move * head = NULL;

        glt_piece start_piece = glt_piece_at_pos(board, start);

        glt_pos addr_arr[4] = {{ 1, 1}, {1, -1}, { -1, 1}, {-1, -1}};

        for (size_t i = 0; i < 4; i++)
        {
                glt_pos adder = addr_arr[i];
                glt_pos new_move = start;
                new_move.y += adder.y ;
                new_move.x += adder.x ;

                while(glt_pos_in_bounds(new_move))
                {
                        glt_piece curr_piece = glt_piece_at_pos(board, new_move);

                        if (curr_piece == GLT_none)  glt__move_append(&head, start, new_move);

                         else if(glt_pieces_is_same_color(start_piece, curr_piece)) break;

                        else{
                                glt__move_append(&head, start, new_move);
                                break;
                        }
                        new_move.y += adder.y;
                        new_move.x += adder.x;
                }
        }
        return head;
}

static glt_move* glt_generate_king_moves(glt_chess_board* board, glt_pos start){

        glt_move * head = NULL;
        glt_piece start_piece = glt_piece_at_pos(board, start);

        glt_pos addr_arr[] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},  // diag
                              {0, 1}, {0, -1}, {1, 0},  {-1, 0}};  // staright


        for (size_t i = 0; i < 8; i++)
        {
                glt_pos adder = addr_arr[i];
                glt_pos new_move = start;
                new_move.y += adder.y ;
                new_move.x += adder.x ;

                if(glt_pos_in_bounds(new_move))
                {
                        glt_piece curr_piece = glt_piece_at_pos(board, new_move);

                        if (curr_piece == GLT_none)  {
                                glt__move_append(&head, start, new_move);
                        } else if(!glt_pieces_is_same_color(start_piece, curr_piece)) { 
                                glt__move_append(&head, start, new_move); 
                        }
                }
        }
        return head;
}

static glt_move* glt_generate_queen_moves(glt_chess_board* board, glt_pos start) 
{
        glt_move* head = NULL;
        glt_piece start_piece = glt_piece_at_pos(board, start);

        glt_pos addr_arr[] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},  // diag
                              {0, 1}, {0, -1}, {1, 0},  {-1, 0}};  // staright

        for (size_t i = 0; i < 8; i++)
        {
                glt_pos adder = addr_arr[i];
                glt_pos new_move = start;
                new_move.y += adder.y ;
                new_move.x += adder.x ;

                while(glt_pos_in_bounds(new_move))
                {
                        glt_piece curr_piece = glt_piece_at_pos(board, new_move);

                        if (curr_piece == GLT_none)  glt__move_append(&head, start, new_move);

                         else if(glt_pieces_is_same_color(start_piece, curr_piece)) break;

                        else{
                                glt__move_append(&head, start, new_move);
                                break;
                        }
                        new_move.y += adder.y;
                        new_move.x += adder.x;
                }
        }
        return head;
}


static glt_move* glt_generate_moves(glt_chess_board * board, glt_pos pos) {
        glt_piece piece = glt_piece_at_pos(board, pos);
        glt_move * ret = NULL;
        switch (piece) {
                case GLT_black_knight:
                case GLT_white_knight:
                        ret = glt_generate_knight_moves(board, pos);
                        return ret;
                case GLT_black_pawn:
                        ret = glt_generate_black_pawn_moves(board, pos);
                        return ret;
                case GLT_white_pawn:
                        ret = glt_generate_white_pawn_moves(board, pos);
                        return ret;
                case GLT_white_bishop:
                case GLT_black_bishop:
                        ret = glt_generate_bishop_moves(board, pos);
                        return ret;
                case GLT_white_rook:
                case GLT_black_rook:
                        ret = glt_generate_rook_moves(board, pos);
                        return ret;
                case GLT_black_queen:
                case GLT_white_queen:
                        ret = glt_generate_queen_moves(board, pos);
                        return ret;
                case GLT_black_king:
                case GLT_white_king:
                        ret = glt_generate_king_moves(board, pos);
                        return ret;
                default:
                        ret = NULL;
        }
        return ret;
}


static int glt_make_move(glt_chess_board* board, glt_move move){

        glt_piece piece = glt_piece_at_pos(board, move.start);

        assert(glt_pos_in_bounds(move.start));
        assert(glt_pos_in_bounds(move.end));

        if(!glt_piece_is_active_color(board, piece)) return 0;

        /**  TODO: add to audit */

        board->pieces[glt_pos_to_index(move.start)] = GLT_none;
        board->pieces[glt_pos_to_index(move.end)]   = piece;

        glt__flip_flag(&board->flags, glt_flag_active_color);

        return 1;

}

static char glt_get_fen_char(glt_piece piece ) {
  switch (piece) {
    case GLT_white_pawn:
      return 'P';
    case GLT_white_king:
      return 'K';
    case GLT_white_queen:
      return 'Q';
    case GLT_white_rook:
      return 'R';
    case GLT_white_bishop:
      return 'B';
    case GLT_white_knight:
      return 'N';
    case GLT_black_pawn:
      return 'p';
    case GLT_black_king:
      return 'k';
    case GLT_black_queen:
      return 'q';
    case GLT_black_rook:
      return 'r';
    case GLT_black_bishop:
      return 'b';
    case GLT_black_knight:
      return 'n';
    case GLT_none:
      return '-';  // Placeholder for empty square
    default:
      return '-';
  }
}

static void glt_get_fen_from_board(glt_chess_board *board, char* fen, int len)
{
        /* Assert that len is greater than 100  */
        assert(len >= 100);
        memset(fen, 0x00, 100);

        u8 empty_count = 0;
        u8 index = 0;

        /* go thru all the squares */
        for (int rank = 1; rank <= 8; rank++)
        {
                for (int file = 1; file <= 8; file++)
                {
                        glt_pos pos;
                        pos.x = file;
                        pos.y = rank;
                        glt_piece piece = board->pieces[glt_pos_to_index(pos)];

                        if(piece == GLT_none){
                                empty_count += 1; 
                        }else{
                                if (empty_count > 0){
                                        fen[index++] = '0' + empty_count; //Convert int to char
                                        empty_count  = 0;
                                }
                        }
                        fen[index++] = glt_get_fen_char(piece);
                }

                if(empty_count > 0)
                {
                        fen[index++] = '0' + empty_count;
                        empty_count  = 0;
                }

                //add a '/' after 
                if(rank < 7)  fen[index++] = '/'; 
        }
        fen[index++] = ' ';

        //active color

        if(glt__is_flag_set(board->flags, glt_flag_active_color)) fen[index++] = 'w';
        else fen[index++] = 'b';


        if (glt__is_flag_set(board->flags, glt_white_king_castle))  fen[index++] = 'K';
        if (glt__is_flag_set(board->flags, glt_white_queen_castle)) fen[index++] = 'Q';
        if (glt__is_flag_set(board->flags, glt_black_king_castle))  fen[index++] = 'k';
        if (glt__is_flag_set(board->flags, glt_black_queen_castle)) fen[index++] = 'q';


        /** if none of the flag is set */
        if (!glt__is_flag_set(board->flags, 
                glt_white_queen_castle |
                glt_white_king_castle  | 
                glt_black_king_castle  | 
                glt_black_queen_castle
                ))
        {
                fen[index++] = '-';
        }


        fen[index++] = ' ';

        /* en pasant */
        fen[index++] = '-';

        /* change to something with clib independent */
        index += sprintf(&fen[index], " %d", board->half_move_clock);
        index += sprintf(&fen[index], " %d", board->full_move_clock);

        fen[index] = '\0';


}

//DEMO application
#if 0
#include <stdio.h>
#define GLT_CHESS_IMPLEMENTATION 1 
#include "glt_chess.h"

static void print_board(glt_chess_board board)
{
        printf("\n");
        for (int i = 63; i >= 0; i)
        {
                for (size_t j = 0; j < 8; j++)
                {
                        printf("%c ", glt_get_fen_char(board.pieces[(i-7) + j]));
                }
                i -= 8;
                printf("\n");
        }
        printf("\n");
}

static void print_moves(glt_move* moves){
        glt_move* curr = moves;
        while(curr){
                glt_coord cord = glt_pos_to_coord(curr->end);
                printf("{%c, %d}", cord.file,  cord.rank);
                curr = curr->next;
        }
}

int main(int argc, char const *argv[])
{
        glt_chess_board board;
        glt_initilize_board(&board);
        print_board(board);

        /*
        glt_coord cord = {'A', 2};
        glt_move* moves =   glt_generate_moves(&board, glt_coord_to_pos(cord));
        print_moves(moves);
        glt_make_move(&board, *moves);
        print_board(board);
        */

        glt_coord cord = {'B', 2};
        glt_move* moves = glt_generate_moves(&board, glt_coord_to_pos(cord));
        print_moves(moves);
        glt_make_move(&board, *moves);
        print_board(board);

        char fen[100] = {0};

        glt_get_fen_from_board(&board, fen, 100);

        printf("%s\n", fen);

        cord.file = 'C';
        cord.rank = 1;
        moves = glt_generate_moves(&board, glt_coord_to_pos(cord));
        print_moves(moves);
        glt_make_move(&board, *moves);
        print_board(board);
        return 0;
}

#endif
#endif //GLT_CHESS_IMPLEMENTATION

















