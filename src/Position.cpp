#include "Game.h"
#include "movegen.h"
#include "tables.h"
#include "history.h"
#include "evaluation.h"
#include "zobrist.h"
#include <iostream>
#include <cstring>

unsigned long long BADCAPTURESCORE = 16069ULL;

// The default constructor instantiates the position with the standard chess starting position.
Position::Position(){
    parseFEN((char*)std::string(startPosition).c_str());
}

// The FEN constructor instantiates the position with the FEN string.
Position::Position(const char* fen){
    parseFEN((char*)fen);
}

/**
 * @brief The Position::generateHashKey function generates the hash key of the position from scratch.
 * @note This function is called by the constructors. Otherwise the hash gets incrementally updated.
 */
HashKey Position::generateHashKey() {
    HashKey h = 0ULL;
    for (int i = Pieces::P; i <= Pieces::k; i++) {
        BitBoard pieceBB = bitboards[i];
        while (pieceBB) {
            Square square = popLsb(pieceBB);
            h ^= pieceKeys[i][square];
        }
    }
    h ^= enPassantKeys[enPassant];
    h ^= castleKeys[castle];
    if (side == Side::BLACK) h ^= sideKey;
    return h;
}

/** 
 * @brief The Position::generatePawnHashKey function generates the hash key of the pawn structure from scratch.
 * @note This function is called by the constructors. Otherwise the hash gets incrementally updated.
 */
HashKey Position::generatePawnHashKey() {
    HashKey h = 0ULL;
    for (int i = Pieces::P; i <= Pieces::k; i += 6) {
        BitBoard pieceBB = bitboards[i];
        while (pieceBB) {
            Square square = popLsb(pieceBB);
            h ^= pieceKeys[i][square];
        }
    }
    return h;
}

/**
 * @brief The Position::print function prints the position to stdout.
 */
void Position::print(){
    std::cout << "\n";
    for(int rank = 0; rank < 8; rank++){
        std::cout << 8 - rank << "  ";
        for(int file = 0; file < 8; file++){
            Square square = rank * 8 + file;
            for (int piece = Pieces::P; piece <= Pieces::k; piece++){
                if (testBit(bitboards[piece], square)){
                    std::cout << getPieceChar(piece);
                    goto NEXT;
                }
            }
            std::cout << ".";

            NEXT: std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n   a b c d e f g h\n\n";
    std::cout << "Side:\t\t\t" << ((side == WHITE)? "WHITE" : "BLACK") << "\n";
    std::cout << "En passant:\t\t" << coords[enPassant] << "\n";
    std::cout << "Castle:\t\t\t" << 
        ((castle & CastleRights::WK) ? "K" : "") <<
        ((castle & CastleRights::WQ) ? "Q" : "") <<
        ((castle & CastleRights::BK) ? "k" : "") <<
        ((castle & CastleRights::BQ) ? "q" : "") << "\n";
    std::cout << "Hash:\t\t\t" << std::hex << hashKey << std::dec << "\n";
    std::cout << "LastMove:\t\t";
    printMove(lastMove);
    std::cout << std::endl;

}

/**
 * @brief The wipe function wipes the position.
 */
void Position::wipe(){
    for(int piece = Pieces::P; piece <= Pieces::k; piece++){
        bitboards[piece] = 0;
    }
    occupancies[WHITE] = occupancies[BLACK] = occupancies[BOTH] = 0;
    side = Side::WHITE;
    enPassant = 0;
    fiftyMove = 0;
    castle = 0;
    hashKey = 0;
    psqtScore[0] = 0;
    psqtScore[1] = 0;
}

/**
 * @brief The removePiece function removes a piece from the board.
 * 
 * @param square The square where the piece is.
 * @param piece The piece to remove.
 * @param hashKey The hash key of the position, to update.
 * @param psqt The psqt score of the position, to update.
 */
static inline void removePiece(BitBoard* bbs, BitBoard* occ, Piece piece, Square square, U64& hashKey, Score (&psqt)[2]){
    // Remove the piece from the bitboard
    clearBit(bbs[piece], square);
    // Remove it also from the side and both bitboard
    clearBit(occ[piece >= 6], square);
    clearBit(occ[BOTH], square);
    // Update the hash key
    hashKey ^= pieceKeys[piece][square];
    Score sign = (piece < 6) ? 1 : -1;
    // Update the psqt score
    psqt[0] -= sign * mgTables[piece][square];
    psqt[1] -= sign * egTables[piece][square];
}

/**l
 * @brief The addPiece function adds a piece to the board.
 *
 * @param square The square where the piece is.
 * @param piece The piece to add.
 * @param hashKey The hash key of the position, to update.
 * @param psqt The psqt score of the position, to update.
 */
static inline void addPiece(BitBoard* bbs, BitBoard* occ, Piece piece, Square square, U64& hashKey, Score (&psqt)[2]){
    // Add the piece to the bitboard
    setBit(bbs[piece], square);
    // Add it also to the side and both bitboard
    setBit(occ[piece >= 6], square);
    setBit(occ[BOTH], square);
    // Update the hash key
    hashKey ^= pieceKeys[piece][square];
    Score sign = (piece < 6) ? 1 : -1;
    // Update the psqt score
    psqt[0] += sign * mgTables[piece][square];
    psqt[1] += sign * egTables[piece][square];
}


bool Position::parseFEN(char *fen) {

    wipe();

    // The FEN string is split into 6 parts:
    // 1. Piece placement
    // 2. Active color
    // 3. Castling availability
    // 4. En passant target square
    // 5. Halfmove clock
    // 6. Fullmove number

    // We will navigate the string using the pointer fen.
    // 1. Piece placement
    Square square = a8;
    int pieceCount = 0;
    
    while(*fen != ' ') {
        if (*fen == '/') {
            if (pieceCount != 8) {
                std::cout << "Error: Unexpected '/' in FEN string (pieceCount was " << pieceCount << "). Can't parse FEN string" << std::endl;
                return false;
            }
            pieceCount = 0;
            fen++;
            continue;
        }
        if (*fen >= '1' && *fen <= '8') {
            pieceCount += *fen - '0';
            square += *fen - '0';
            fen++;
            continue;
        }
        if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
            int piece = charToPiece(*fen);
            if (piece == NOPIECE) {
                std::cout << "Error: Invalid character in Piece Placement '" << fen[0] << "'. Can't parse FEN string" << std::endl;
                return false;
            }
            addPiece(bitboards, occupancies, piece, square, hashKey, psqtScore);
            fen++;
            square++;
            pieceCount++;
            continue;
        }
        std::cout << "Error: Invalid character in Piece Placement '" << fen[0] << "'. Can't parse FEN string" << std::endl;
        return false;
    }

    // 2. Active color
    fen++;
    if (*fen == 'w') {
        side = WHITE;
    } else if (*fen == 'b') {
        side = BLACK;
    } else {
        std::cout << "Error: Invalid character in Active Color '" << fen[0] << "'. Can't parse FEN string" << std::endl;
        return false;
    }

    //std::cout << "Turn is now: " << (int)side << std::endl;

    // 3. Castling availability
    fen += 2;
    castle = 0;
    if (*fen != '-') {
        while (*fen != ' ') {
            if (*fen == 'K') {
                castle |= CastleRights::WK;
            } else if (*fen == 'Q') {
                castle |= CastleRights::WQ;
            } else if (*fen == 'k') {
                castle |= CastleRights::BK;
            } else if (*fen == 'q') {
                castle |= CastleRights::BQ;
            } else {
                std::cout << "Error: Invalid character in Castling Availability '" << fen[0] << "'. Can't parse FEN string" << std::endl;
                return false;
            }
            fen++;
        }
    }
    else {
        fen++;
    }

    // 4. En passant target square
    fen++;
    if (*fen == '-') {
        enPassant = noSquare;
        fen++;
    } else {
        enPassant = squareFromName(fen);
        fen += 2;
        if (enPassant == noSquare) {
            std::cout << "Error: Invalid character in En Passant Target Square '" << fen[0] << "'. Can't parse FEN string" << std::endl;
            return false;
        }
    }

    int fullMoveNumber = 0;

    // Some fens don't have the halfmove clock and fullmove number, so we need to check for that.
    while (*fen == ' ') {
        fen++;
    }
    if (*fen == '\0') {
        // If there's no more data, we're done.
        goto FENkeyEval;
    }
    // 5. Halfmove clock
    fen++;
    fiftyMove = 0;
    while (*fen >= '0' && *fen <= '9') {
        fiftyMove = fiftyMove * 10 + *fen - '0';
        fen++;
    }

    // 6. Fullmove number (not used)
    fen++;
    while (*fen >= '0' && *fen <= '9') {
        fullMoveNumber = fullMoveNumber * 10 + *fen - '0';
        fen++;
    }

    if (*fen != '\0') {
        // If there's still data left in the string, it's invalid.
        // Since the position is already set, we won't return false, but we will print a warning.
        //std::cout << "Warning: FEN string contains more data than expected : \""<< fen <<"\". Ignoring it.\n";
    }
FENkeyEval:
    // If everything is alright, generate the hash key for the current position
    hashKey = generateHashKey();
    return true;
}

/**
 * @brief The hasNonPawns function checks if the current side to move has non-pawn material.
 * @return true if the position has non-pawn material, false otherwise.
 */
bool Position::hasNonPawns(){
    return occupancies[side] ^ bitboards[P + 6 * side] ^ bitboards[K + 6 * side];
}

/**
 * @brief The mayBeZugzwang function tries to detect positions where reductions may be unsafe.
 * @return True if the position is not to prune, false otherwise.
 */
bool Position::mayBeZugzwang(){
    // We are going to check if there is some non pawn material.
    // If there is not, then the position is not to be reduced, since it is simple enough and risky (most zugzwang positions are King and pawn positions)
    // We also need to check if there is few material on the board, in which case we make sure there is some legal move for the pawn
    // If there is no legal move for the pawn, then the position is possibly a zugzwang
    BitBoard nonPawnWhite = bitboards[N] | bitboards[B] | bitboards[R];
    BitBoard nonPawnBlack = bitboards[n] | bitboards[b] | bitboards[r];
    U8 count = (U8)std::max(popcount(nonPawnWhite),popcount(nonPawnBlack));
    if (bitboards[Q] | bitboards[q]) return false; // Queen positions have enough material to not be zugzwang, most of the time
    if (count <= 2){
        return true;
    }
    else if (count <= 5 && (bitboards[P] | bitboards[P])){
        BitBoard freeOccupancy = ~(nonPawnBlack | nonPawnWhite | bitboards[p] | bitboards[P] | bitboards[k] | bitboards[K]);
        // To see if there are free squares ahead, we just shift the pawns forward (or backward if white)
        if (side == WHITE){
            freeOccupancy &= ~(bitboards[P] >> 8);
        }
        else{
            freeOccupancy &= ~(bitboards[p] << 8);
        }
        // If there are no free squares ahead, then the position is likely to be a zugzwang
        if (freeOccupancy == 0 ){
            return true;
        }
    }
    return false;
}

/**
 * @brief The isSquareAttacked function returns true if the given square is attacked by the given side.
 * @param square The square to check.
 * @param side The side to check.
 * @return True if the square is attacked, false otherwise.
 */
inline bool Position::isSquareAttacked(U8 square, U8 side){
    if (side == WHITE) {
        return (bool)(
            (pawnAttacks[BLACK][square] & bitboards[Pieces::P]) ||
            (knightAttacks[square] & bitboards[Pieces::N]) ||
            (kingAttacks[square] & bitboards[Pieces::K]) ||
            (getBishopAttack(square, occupancies[BOTH]) & (bitboards[Pieces::B] | bitboards[Pieces::Q])) ||
            (getRookAttack(square, occupancies[BOTH]) & (bitboards[Pieces::R] | bitboards[Pieces::Q]))
        );
    }
    else {
        return (bool)(
            (pawnAttacks[WHITE][square] & bitboards[Pieces::p]) ||
            (knightAttacks[square] & bitboards[Pieces::n]) ||
            (kingAttacks[square] & bitboards[Pieces::k]) ||
            (getBishopAttack(square, occupancies[BOTH]) & (bitboards[Pieces::b] | bitboards[Pieces::q])) ||
            (getRookAttack(square, occupancies[BOTH]) & (bitboards[Pieces::r] | bitboards[Pieces::q]))
        );
    }
}

/**
 * @brief The attacksToPre function returns a bitboard with all the attackers to a square.
 * @param square The square to check.
 * @param occupancy an already calculated occupancy
 * @param diagonalAttackers an already calculated diagonal attackers
 * @param orthogonalAttackers an already calculated orthogonal attackers
 * @return A bitboard with all the attackers to the square.
 * @TODO: diagonal and orthogonal attackers can be passed as parameters as they are already calculated
 */
inline BitBoard Position::attacksToPre(Square square, BitBoard occupancy, BitBoard diagonalAttackers, BitBoard orthogonalAttackers) {
    return (
        (pawnAttacks[WHITE][square] & bitboards[p]) |
        (pawnAttacks[BLACK][square] & bitboards[P]) |
        (knightAttacks[square] & (bitboards[N] | bitboards[n])) |
        (kingAttacks[square] & (bitboards[K] | bitboards[k])) |
        (getBishopAttack(square, occupancy) & diagonalAttackers) |
        (getRookAttack(square, occupancy) & orthogonalAttackers)
    );
    
}

/**
 * @brief The makeMove function makes a move on the board.
 * @param move The move to make.
 * @return True if the move was made, false otherwise (state of the position is undefined).
 */
bool Position::makeMove(Move move) {

    // If the move is invalid, we return false
    if (!move) return false;

    // Get useful information about the move
    Square from = moveSource(move);
    Square to = moveTarget(move);
    Piece piece = movePiece(move);
    Piece captured = moveCapture(move);
    Piece promotion = movePromotion(move);
    bool captures = captured != NOPIECE;
    bool promotes = promotion != NOPIECE;
    bool doublePawnPush = isDoublePush(move) > 0;
    bool isEnPass = isEnPassant(move) > 0;
    bool isCastle = isCastling(move) > 0;

    // First, we remove the piece from the source square
    removePiece(bitboards, occupancies, piece, from, hashKey, psqtScore);

    // If the move is a capture, we remove the captured piece from the target square
    if (captures && !isEnPass) removePiece(bitboards, occupancies, captured, to, hashKey, psqtScore);
    // The reason why we don't remove the captured piece if it is an en passant is because the captured piece is not on the target square, so we will be handling this case later

    // If the move is a promotion, we replace the piece with the promoted piece
    if (promotes) addPiece(bitboards, occupancies, promotion, to, hashKey, psqtScore);
    // Otherwise, we move the piece to the target square
    else  addPiece(bitboards, occupancies, piece, to, hashKey, psqtScore);

    // Remove the current en passant square from the hash key, as it is no longer valid
    hashKey ^= enPassantKeys[enPassant];
    enPassant = noSquare;

    // If the move is a enpassant, we remove the captured pawn from the enPassant square
    if (isEnPass) removePiece(bitboards, occupancies, captured, (side == WHITE ? to + 8 : to - 8), hashKey, psqtScore);

    // If the move is a double pawn push, we set the en passant square and hash it back in. If not, we don't need to do anything, since the noSquare hash is 0
    if (doublePawnPush) enPassant = to + 8 * (1 - 2 * side);
    hashKey ^= enPassantKeys[enPassant];

    // If the move is a castle, we move the rook
    if (isCastle) { 
        switch (to) {
            case g1: {
                // Check if squares between the king and the rook are attacked
                if (isSquareAttacked(e1, BLACK) || isSquareAttacked(f1, BLACK) || isSquareAttacked(g1, BLACK))
                    return false;
                removePiece(bitboards, occupancies, R, h1, hashKey, psqtScore);
                addPiece(bitboards, occupancies, R, f1, hashKey, psqtScore);
                break;
            }
            case c1: {
                // Check if squares between the king and the rook are attacked
                if (isSquareAttacked(e1, BLACK) || isSquareAttacked(d1, BLACK) || isSquareAttacked(c1, BLACK))
                    return false;
                removePiece(bitboards, occupancies, R, a1, hashKey, psqtScore);
                addPiece(bitboards, occupancies, R, d1, hashKey, psqtScore);
                break;
            }
            case g8: {
                // Check if squares between the king and the rook are attacked
                if (isSquareAttacked(e8, WHITE) || isSquareAttacked(f8, WHITE) || isSquareAttacked(g8, WHITE))
                    return false;
                removePiece(bitboards, occupancies, r, h8, hashKey, psqtScore);
                addPiece(bitboards, occupancies, r, f8, hashKey, psqtScore);
                break;
            }
            case c8: {
                // Check if squares between the king and the rook are attacked
                if (isSquareAttacked(e8, WHITE) || isSquareAttacked(d8, WHITE) || isSquareAttacked(c8, WHITE))
                    return false;
                removePiece(bitboards, occupancies, r, a8, hashKey, psqtScore);
                addPiece(bitboards, occupancies, r, d8, hashKey, psqtScore);
                break;
            }
        }
    }

    // Now we verify if the move is legal
    Square ourKing = lsb(bitboards[K + 6 * side]);
    if (isSquareAttacked(ourKing, side ^ 1)) return false;

    // Now we update the hash for en passant, castle rights and side to move
    hashKey ^= sideKey;
    // Remove previous castle rights
    hashKey ^= castleKeys[castle];
    // Add new castle rights
    castle &= castlingRights[from];
    castle &= castlingRights[to];
    hashKey ^= castleKeys[castle];

    // Change side to move
    side ^= 1;
    lastMove = onlyMove(move);

    if (isCapture(lastMove) || isPromotion(lastMove) || ((movePiece(lastMove) % 6) == 0)) fiftyMove = 0;
    else fiftyMove++; 
	
    return true;
}

/**
 * @brief The addMove function adds a move to the move list.
 * @param ml The move list to add the move to.
 * @param move The move to add.
 */
inline void Position::addMove(MoveList* ml, ScoredMove move, Ply ply) {
    Piece movedPiece = movePiece(move);
    Piece capturedPiece = moveCapture(move);
    Square from = moveSource(move);
    Square to = moveTarget(move);

    Move oMove = onlyMove(move);
    if (isEnPassant(move)){
        ml->moves[ml->count++] = ((105ULL + GOODCAPTURESCORE) << 32) | move;
        return;
    }
    if (isCapture(oMove)) {
        ml->moves[ml->count++] = (
            (
                MvvLva[movedPiece][capturedPiece] +
                GOODCAPTURESCORE * SEE<-107>(move) + BADCAPTURESCORE
            ) 
        << 32) | move;
        return;
    }
    else if (isPromotion(oMove)){
        // TODO: check if putting knight promotions immediately after queen promotions is better
        ml->moves[ml->count++] = ((PROMOTIONBONUS + movePromotion(move)) << 32) | move;
        return;
    }
    else if (sameMovePos(oMove, killerTable[0][ply])){
        ml->moves[ml->count++] = (KILLER1SCORE << 32) | move;
        return;
    }
    else if (sameMovePos(oMove, killerTable[1][ply])){
        ml->moves[ml->count++] = (KILLER2SCORE << 32) | move;
        return;
    }
    else if (sameMovePos(oMove, counterMoveTable[movePiece(lastMove)][moveTarget(lastMove)])){
        ml->moves[ml->count++] = (COUNTERSCORE << 32) | move;
        return;
    }
    
    S64 score = (S64)(historyTable[side][from][to]); // *22 + ((S64)pieceFromHistoryTable[movedPiece][from]) + ((S64)pieceToHistoryTable[movedPiece][to]) * 2) / 25;
    score += 16384;
    ml->moves[ml->count++] = (score << 32) | move; //
}

bool Position::inCheck() {
	Square kingPos = lsb(bitboards[K + 6 * side]);
    return isSquareAttacked(kingPos, side ^ 1);
}

bool Position::insufficientMaterial() {
    if (popcount(bitboards[P] | bitboards[p]) > 0) return false;
    if (popcount(bitboards[R] | bitboards[r]) > 0) return false;
    if (popcount(bitboards[Q] | bitboards[q]) > 0) return false;
    BitBoard sideW = bitboards[N] | bitboards[B];
    BitBoard sideB = bitboards[n] | bitboards[b];
    BitBoard occupancy = sideW | sideB;
    if (popcount(occupancy) > 2) return false;
    if (popcount(occupancy) == 2) {
        if (sideW && sideB) return true; // Means K(N/B) vs K(N/B)
        if (sideW == bitboards[N] || sideB == bitboards[B]) return true; // Means KNN vs K or K v KNN
        if (sideW == bitboards[B] && (popcount(bitboards[B] & squaresOfColor[WHITE]) != 1)) return true; // Means KBB v K with same color bishops
        if (sideB == bitboards[b] && (popcount(bitboards[b] & squaresOfColor[BLACK]) != 1)) return true; // Means Kbb v K with same color bishops
		return false; // Means either KBN v K or KBB v K with different color bishops
    }
	return true; // Means K v K
}
	
	


/**
 * @brief The pieceOn function returns the piece on a square. If the square is empty, NOPIECE is returned.
 * @param square The square to check.
 * @return The piece on the square.
 */
inline Piece Position::pieceOn(Square square) {
    // We will do a branchless lookup
    // We will have a "result" variable that will contain the piece on the square
    Piece result = NOPIECE;
    // We will subtract the piece code once we find it
    result -= (NOPIECE - P) * testBit(bitboards[P], square); // This way, if there is a white pawn, the result will be 0 (P)
    result -= (NOPIECE - N) * testBit(bitboards[N], square); // This way, if there is a white knight, the result will be 1 (N)
    result -= (NOPIECE - B) * testBit(bitboards[B], square); // This way, if there is a white bishop, the result will be 2 (B)
    result -= (NOPIECE - R) * testBit(bitboards[R], square); // This way, if there is a white rook, the result will be 3 (R)
    result -= (NOPIECE - Q) * testBit(bitboards[Q], square); // This way, if there is a white queen, the result will be 4 (Q)
    result -= (NOPIECE - K) * testBit(bitboards[K], square); // This way, if there is a white king, the result will be 5 (K)

    result -= (NOPIECE - p) * testBit(bitboards[p], square); // This way, if there is a black pawn, the result will be 6 (p)
    result -= (NOPIECE - n) * testBit(bitboards[n], square); // This way, if there is a black knight, the result will be 7 (n)
    result -= (NOPIECE - b) * testBit(bitboards[b], square); // This way, if there is a black bishop, the result will be 8 (b)
    result -= (NOPIECE - r) * testBit(bitboards[r], square); // This way, if there is a black rook, the result will be 9 (r)
    result -= (NOPIECE - q) * testBit(bitboards[q], square); // This way, if there is a black queen, the result will be 10 (q)
    result -= (NOPIECE - k) * testBit(bitboards[k], square); // This way, if there is a black king, the result will be 11 (k)

    return result; // If no piece was found, the result will remain NOPIECE
}

constexpr Score pieceValues[15] = {
    100, 300, 300, 500, 900, 0, // White pieces
    100, 300, 300, 500, 900, 0, // Black pieces
    0,   0,   0 // Padding for special cases
};

// Thanks to Weiss chess engine for a clean implementation of this function
template <Score threshold>
bool Position::SEE(Move move){
    Square from = moveSource(move);
    Square to = moveTarget(move);

    Piece attacker = movePiece(move);
    Piece captured = moveCapture(move);

    // Of course the move should be a capture
    //assert(captured != NOPIECE);

    Score value = pieceValues[captured] - threshold;

    if (value < 0) return false;
    value -= pieceValues[attacker];
    if (value >= 0) return true;

    BitBoard occupancy = occupancies[BOTH] ^ squareBB(from) ^ squareBB(to);

    BitBoard diagonalAttackers = bitboards[Q] | bitboards[q];
    BitBoard orthogonalAttackers = diagonalAttackers | bitboards[R] | bitboards[r];
    diagonalAttackers |= bitboards[B] | bitboards[b];

    BitBoard attackers = attacksToPre(to, occupancy, diagonalAttackers, orthogonalAttackers);

    U8 side = attacker < p;

    while (true) {
        attackers &= occupancy;

        BitBoard myAttackers = attackers & occupancies[side];
        if (!myAttackers) break;

        Piece pt;
        for (pt = P; pt <= K; pt++) { // Capture with the lowest value piece (which makes the strongest attack)
            if (myAttackers & (bitboards[pt] | bitboards[pt + 6])) break;
        }

        // Swap side
        side = !side;
        value = -value - 1 - pieceValues[pt];

        // Check for threshold
        if (value >= 0){
            if (pt == K && (attackers & occupancies[side])) side = !side;
            break;
        }

        // Remove the used piece from occupancy
        clearBit(occupancy, lsb(myAttackers & (bitboards[pt] | bitboards[pt + 6])));

        if (pt == P || pt == B || pt == Q)
            attackers |= getBishopAttack(to, occupancy) & diagonalAttackers;

        if (pt == R || pt == Q)
            attackers |= getRookAttack(to, occupancy) & orthogonalAttackers;
    }
    
    return side != (attacker >= p);

}



void Position::reflect() {
    // First, swap white with black
    for (int i = P; i <= K; i++) {

        bitboards[i] ^= bitboards[i + 6];
        bitboards[i+6] ^= bitboards[i];
        bitboards[i] ^= bitboards[i + 6];
    }
	// First, we will reflect the bitboards
	for (int i = 0; i < 12; i++) {
		bitboards[i] = reflectBitBoard(bitboards[i]);
	}
	// Then reflect ep square
    enPassant = enPassant == noSquare ? noSquare : flipSquare(enPassant);
	// Then reflect the castling rights
    U8 newCastle = 0;
    newCastle |= WK * (!!(castle & BK));
    newCastle |= WQ * (!!(castle & BQ));
    newCastle |= BK * (!!(castle & WK));
    newCastle |= BQ * (!!(castle & WQ));

    castle = newCastle;
	// Then reflect the side to move
	side = side == WHITE ? BLACK : WHITE;
	// Recalculate hash
    hashKey = generateHashKey();
}

std::string Position::getFEN() {
	std::string fen = "";
	int empty = 0;
	for (Square square = a8; square < noSquare; ++square) {
        Piece p = pieceOn(square);
        if ( p == NOPIECE) empty++;
        else {
            if (empty) fen.append(1, ('0' + empty));
            empty = 0;
            fen.append(1, getPieceChar(p));
        }
        if ((square % 8 == 7) && (square != h1)) {
            if (empty) fen.append(1, ('0' + empty));
            empty = 0;
            fen += "/";
        }
	}
	
    fen += " ";
    fen += side ? "b " : "w ";
    if (castle) {
        if (castle & CastleRights::WK) fen += "K";
        if (castle & CastleRights::WQ) fen += "Q";
        if (castle & CastleRights::BK) fen += "k";
        if (castle & CastleRights::BQ) fen += "q";
        fen += " ";
    }
    else {
        fen += "- ";
    }
	
    if (enPassant != noSquare) {
        fen += coords[enPassant] + " ";
    }
    else {
        fen += "- ";
    }
    return fen;
}

/**
 * @brief The generateMoves function generates all pseudo legal moves for the current side.
 * @param moveList The moveList to fill with the generated moves.
 */
void Position::generateMoves(MoveList& moveList, Ply ply) {
    BitBoard ourPawns   = bitboards[P + 6 * side];
    BitBoard ourKnights = bitboards[N + 6 * side];
    BitBoard ourBishops = bitboards[B + 6 * side];
    BitBoard ourRooks   = bitboards[R + 6 * side];
    BitBoard ourQueens  = bitboards[Q + 6 * side];
    BitBoard ourKing    = bitboards[K + 6 * side];

    BitBoard theirKing      = bitboards[K + 6 * (side ^ 1)];

    BitBoard theirPieces    = occupancies[side ^ 1];
    BitBoard occupancy      = occupancies[BOTH];


    Square theirKingPos     = lsb(theirKing);

    BitBoard pawnCheckers   = pawnAttacks[side ^ 1][theirKingPos];
    BitBoard knightCheckers = knightAttacks[theirKingPos];
    BitBoard bishopCheckers = getBishopAttack(theirKingPos, occupancy);
    BitBoard rookCheckers   = getRookAttack(theirKingPos, occupancy);
    BitBoard queenCheckers  = bishopCheckers | rookCheckers;

    // We will generate the moves in a way that the sort function will need to do minimal work
    // We will generate first moves that we expect to have an higher probability of being useful (high score moves)
    // The order will be as follows:

    // 1. Pawn captures, Knight captures
    // 2. Sliders captures
    // 3. King captures, castling moves
    // 4. Pawn moves
    // 5. Sliders moves
    // 6. Knight moves
    // 7. King moves

    // The score of a move is the sum of the following values:
    // 1. The value given by the mvvlva table (for captures) and a bonus for promotions
    // 2. The value given by the killer table (for non captures)
    // 3. The value given by the history table (for non captures)
    // 4. The value given by the counter move table (for non captures)
    // 5. The value given by the follow up move table (for non captures)
    // 6. A bonus for moves that give check (for non captures)
    // 7. A bonus for castling moves and double pawn pushes

    // We will generate moves for the pawns first
    if (side == WHITE) {
        // We generate the pawn captures. Pawn captures are generated by shifting by 7 or 9 squares
        BitBoard nonAPawns = ourPawns & ~files(0);
        BitBoard nonHPawns = ourPawns & ~files(7);
        BitBoard dxPawnCaptures = nonHPawns >> 7;
        BitBoard sxPawnCaptures = nonAPawns >> 9;
        dxPawnCaptures &= theirPieces;
        sxPawnCaptures &= theirPieces;
        // Right captures
        while (dxPawnCaptures) {
            Square to = popLsb(dxPawnCaptures);
            Piece captured = pieceOn(to);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to <= h8) {
                // We have a promotion
                addMove(&moveList, encodeMove(to + 7, to, P, captured, Q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 7, to, P, captured, R, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 7, to, P, captured, B, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 7, to, P, captured, N, false, false, false, isCheck), ply);
            }
            else
                addMove(&moveList, encodeMove(to + 7, to, P, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        // Left captures
        while (sxPawnCaptures) {
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to <= h8) {
                // We have a promotion
                addMove(&moveList, encodeMove(to + 9, to, P, captured, Q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 9, to, P, captured, R, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 9, to, P, captured, B, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 9, to, P, captured, N, false, false, false, isCheck), ply);
            }
            else
                addMove(&moveList, encodeMove(to + 9, to, P, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare) {
            Square to = enPassant;
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            BitBoard epPawns = pawnAttacks[side ^ 1][to] & ourPawns;
            while (epPawns) {
                Square from = popLsb(epPawns);
                addMove(&moveList, encodeMove(from, to, P, p, NOPIECE, false, true, false, isCheck), ply);
            }
        }
    }
    else {
        // We generate the pawn captures. Pawn captures are generated by shifting by 7 or 9 squares
        BitBoard nonAPawns = ourPawns & ~files(0);
        BitBoard nonHPawns = ourPawns & ~files(7);
        BitBoard dxPawnCaptures = nonHPawns << 9;
        BitBoard sxPawnCaptures = nonAPawns << 7;
        dxPawnCaptures &= theirPieces;
        sxPawnCaptures &= theirPieces;
        // Right captures
        while (dxPawnCaptures) {
            Square to = popLsb(dxPawnCaptures);
            Piece captured = pieceOn(to);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to >= a1) {
                // We have a promotion
                addMove(&moveList, encodeMove(to - 9, to, p, captured, q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 9, to, p, captured, r, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 9, to, p, captured, b, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 9, to, p, captured, n, false, false, false, isCheck), ply);
            }
            else
                addMove(&moveList, encodeMove(to - 9, to, p, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        // Left captures
        while (sxPawnCaptures) {
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to >= a1) {
                // We have a promotion
                addMove(&moveList, encodeMove(to - 7, to, p, captured, q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 7, to, p, captured, r, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 7, to, p, captured, b, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 7, to, p, captured, n, false, false, false, isCheck), ply);
            }
            else
                addMove(&moveList, encodeMove(to - 7, to, p, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare) {
            Square to = enPassant;
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            BitBoard epPawns = pawnAttacks[side ^ 1][to] & ourPawns;
            while (epPawns) {
                Square from = popLsb(epPawns);
                addMove(&moveList, encodeMove(from, to, p, P, NOPIECE, false, true, false, isCheck), ply);
            }
        }
    }
    // We will generate the knight moves
    BitBoard knights = ourKnights;
    while (knights) {
        Square from = popLsb(knights);
        BitBoard attacks = knightAttacks[from];
        BitBoard quiets = attacks & ~occupancy;
        attacks &= theirPieces;
        while (attacks) {
            Square to = popLsb(attacks);
            Piece captured = pieceOn(to);
            bool isCheck = (knightCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, N + 6 * side, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (knightCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, N + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck), ply);
        }
    }
    // We will generate the bishop moves
    BitBoard bishops = ourBishops;
    while (bishops) {
        Square from = popLsb(bishops);
        BitBoard attacks = getBishopAttack(from, occupancy);
        BitBoard quiets = attacks & ~occupancy;
        attacks &= theirPieces;
        while (attacks) {
            Square to = popLsb(attacks);
            Piece captured = pieceOn(to);
            bool isCheck = (bishopCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, B + 6 * side, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (bishopCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, B + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck), ply);
        }
    }
    // We will generate the rook moves
    BitBoard rooks = ourRooks;
    while (rooks) {
        Square from = popLsb(rooks);
        BitBoard attacks = getRookAttack(from, occupancy);
        BitBoard quiets = attacks & ~occupancy;
        attacks &= theirPieces;
        while (attacks) {
            Square to = popLsb(attacks);
            Piece captured = pieceOn(to);
            bool isCheck = (rookCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, R + 6 * side, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (rookCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, R + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck), ply);
        }
    }
    // We will generate the queen moves
    BitBoard queens = ourQueens;
    while (queens) {
        Square from = popLsb(queens);
        BitBoard attacks = getQueenAttack(from, occupancy);
        BitBoard quiets = attacks & ~occupancy;
        attacks &= theirPieces;
        while (attacks) {
            Square to = popLsb(attacks);
            Piece captured = pieceOn(to);
            bool isCheck = (queenCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, Q + 6 * side, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (queenCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, Q + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck), ply);
        }
    }
    // We will generate the pawn pushes
    if (side == WHITE) {
        BitBoard pawnPushes = ourPawns >> 8;
        pawnPushes &= ~occupancy;

        while (pawnPushes) {
            Square to = popLsb(pawnPushes);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to <= h8) {
                // We will generate the promotion moves
                addMove(&moveList, encodeMove(to + 8, to, P, NOPIECE, Q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 8, to, P, NOPIECE, R, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 8, to, P, NOPIECE, B, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 8, to, P, NOPIECE, N, false, false, false, isCheck), ply);

            }
            else
                addMove(&moveList, encodeMove(to + 8, to, P, NOPIECE, NOPIECE, false, false, false, isCheck), ply);
        }

        BitBoard pawnDoublePushes = ourPawns & ranks(6);
        pawnDoublePushes >>= 8;
        pawnDoublePushes &= ~occupancy;
        pawnDoublePushes >>= 8;
        pawnDoublePushes &= ~occupancy;
        while (pawnDoublePushes) {
            Square to = popLsb(pawnDoublePushes);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(to + 16, to, P, NOPIECE, NOPIECE, true, false, false, isCheck), ply);
        }
    }
    else {
        BitBoard pawnPushes = ourPawns << 8;
        pawnPushes &= ~occupancy;
        while (pawnPushes) {
            Square to = popLsb(pawnPushes);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to >= a1) {
                // We will generate the promotion moves
                addMove(&moveList, encodeMove(to - 8, to, p, NOPIECE, q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 8, to, p, NOPIECE, r, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 8, to, p, NOPIECE, b, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 8, to, p, NOPIECE, n, false, false, false, isCheck), ply);
            }
            else
                addMove(&moveList, encodeMove(to - 8, to, p, NOPIECE, NOPIECE, false, false, false, isCheck), ply);
        }
        BitBoard pawnDoublePushes = ourPawns & ranks(1);
        pawnDoublePushes <<= 8;
        pawnDoublePushes &= ~occupancy;
        pawnDoublePushes <<= 8;
        pawnDoublePushes &= ~occupancy;
        while (pawnDoublePushes) {
            Square to = popLsb(pawnDoublePushes);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(to - 16, to, p, NOPIECE, NOPIECE, true, false, false, isCheck), ply);
        }
    }
    // We will generate the king moves, including castling
    Square king = lsb(ourKing);
    BitBoard kAttacks = kingAttacks[king];
    BitBoard kMoves = kAttacks & ~occupancy;
    kAttacks &= theirPieces;
    while (kAttacks) {
        Square to = popLsb(kAttacks);
        Piece captured = pieceOn(to);
        addMove(&moveList, encodeMove(king, to, K + 6 * side, captured, NOPIECE, false, false, false, false), ply);
    }
    while (kMoves) {
        Square to = popLsb(kMoves);
        addMove(&moveList, encodeMove(king, to, K + 6 * side, NOPIECE, NOPIECE, false, false, false, false), ply);
    }
    // We will generate the castling moves
    if (side == WHITE) {
        if (castle & CastleRights::WK && !(occupancy & wKCastleMask)) {
            addMove(&moveList, encodeMove(e1, g1, K, NOPIECE, NOPIECE, false, false, true, false), ply);
        }
        if (castle & CastleRights::WQ && !(occupancy & wQCastleMask)) {
            addMove(&moveList, encodeMove(e1, c1, K, NOPIECE, NOPIECE, false, false, true, false), ply);
        }
    }
    else {
        if (castle & CastleRights::BK && !(occupancy & bKCastleMask)) {
            addMove(&moveList, encodeMove(e8, g8, k, NOPIECE, NOPIECE, false, false, true, false), ply);
        }
        if (castle & CastleRights::BQ && !(occupancy & bQCastleMask)) {
            addMove(&moveList, encodeMove(e8, c8, k, NOPIECE, NOPIECE, false, false, true, false), ply);
        }
    }
}

void Position::generateCaptures(MoveList &moveList, Ply ply){
    moveList.clear();
    BitBoard ourPawns   = bitboards[P + 6 * side];
    BitBoard ourKnights = bitboards[N + 6 * side];
    BitBoard ourBishops = bitboards[B + 6 * side];
    BitBoard ourRooks   = bitboards[R + 6 * side];
    BitBoard ourQueens  = bitboards[Q + 6 * side];
    BitBoard ourKing    = bitboards[K + 6 * side];
    
    BitBoard theirKing      = bitboards[K + 6 * (side ^ 1)];

    BitBoard theirPieces = occupancies[side ^ 1];
    BitBoard occupancy = occupancies[BOTH];

    Square theirKingPos = lsb(theirKing);
    
    BitBoard pawnCheckers = pawnAttacks[side^1][theirKingPos];
    BitBoard knightCheckers = knightAttacks[theirKingPos];
    BitBoard bishopCheckers = getBishopAttack(theirKingPos, occupancy);
    BitBoard rookCheckers = getRookAttack(theirKingPos, occupancy);
    BitBoard queenCheckers = getQueenAttack(theirKingPos, occupancy);

    // We will generate the moves in a way that the sort function will need to do minimal work
    // We will generate first moves that we expect to have an higher probability of being useful (high score moves)
    // The order will be as follows:

    // 1. Pawn captures, Knight captures
    // 2. Sliders captures
    // 3. King captures, castling moves
    // 4. Pawn moves
    // 5. Sliders moves
    // 6. Knight moves
    // 7. King moves

    // The score of a move is the sum of the following values:
    // 1. The value given by the mvvlva table (for captures) and a bonus for promotions
    // 2. The value given by the killer table (for non captures)
    // 3. The value given by the history table (for non captures)
    // 4. The value given by the counter move table (for non captures)
    // 5. The value given by the follow up move table (for non captures)
    // 6. A bonus for moves that give check (for non captures)
    // 7. A bonus for castling moves and double pawn pushes

    // We will generate moves for the pawns first
    if (side == WHITE){
        // We generate the pawn captures. Pawn captures are generated by shifting by 7 or 9 squares
        BitBoard nonAPawns = ourPawns & ~files(0);
        BitBoard nonHPawns = ourPawns & ~files(7);
        BitBoard dxPawnCaptures = nonHPawns >> 7;
        BitBoard sxPawnCaptures = nonAPawns >> 9;
        dxPawnCaptures &= theirPieces;
        sxPawnCaptures &= theirPieces;
        // Right captures
        while (dxPawnCaptures){
            Square to = popLsb(dxPawnCaptures);
            Piece captured = pieceOn(to);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to <= h8) {
                // We have a promotion
                addMove(&moveList, encodeMove(to + 7, to, P, captured, Q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 7, to, P, captured, R, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 7, to, P, captured, B, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 7, to, P, captured, N, false, false, false, isCheck), ply);    
            }
            else
                addMove(&moveList, encodeMove(to+7, to, P, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        // Left captures
        while (sxPawnCaptures){
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to <= h8) {
                // We have a promotion
                addMove(&moveList, encodeMove(to + 9, to, P, captured, Q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 9, to, P, captured, R, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 9, to, P, captured, B, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to + 9, to, P, captured, N, false, false, false, isCheck), ply);
            }
            else
                addMove(&moveList, encodeMove(to+9, to, P, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare){
            Square to = enPassant;
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            BitBoard epPawns = pawnAttacks[side^1][to] & ourPawns;
            while (epPawns){
                Square from = popLsb(epPawns);
                addMove(&moveList, encodeMove(from, to, P, p, NOPIECE, false, true, false, isCheck), ply);
            }
        }
    }
    else {
        // We generate the pawn captures. Pawn captures are generated by shifting by 7 or 9 squares
        BitBoard nonAPawns = ourPawns & ~files(0);
        BitBoard nonHPawns = ourPawns & ~files(7);
        BitBoard dxPawnCaptures = nonHPawns << 9;
        BitBoard sxPawnCaptures = nonAPawns << 7;
        dxPawnCaptures &= theirPieces;
        sxPawnCaptures &= theirPieces;
        // Right captures
        while (dxPawnCaptures){
            Square to = popLsb(dxPawnCaptures);
            Piece captured = pieceOn(to);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to >= a1) {
                // We have a promotion
                addMove(&moveList, encodeMove(to - 9, to, p, captured, q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 9, to, p, captured, r, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 9, to, p, captured, b, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 9, to, p, captured, n, false, false, false, isCheck), ply);
            }
            else
                addMove(&moveList, encodeMove(to-9, to, p, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        // Left captures
        while (sxPawnCaptures){
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            if (to >= a1) {
                // We have a promotion
                addMove(&moveList, encodeMove(to - 7, to, p, captured, q, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 7, to, p, captured, r, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 7, to, p, captured, b, false, false, false, isCheck), ply);
                addMove(&moveList, encodeMove(to - 7, to, p, captured, n, false, false, false, isCheck), ply);
            }
            else
                addMove(&moveList, encodeMove(to-7, to, p, captured, NOPIECE, false, false, false, isCheck), ply);
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare){
            Square to = enPassant;
            bool isCheck = (pawnCheckers & squareBB(to)) > 0;
            BitBoard epPawns = pawnAttacks[side^1][to] & ourPawns;
            while (epPawns){
                Square from = popLsb(epPawns);
                addMove(&moveList, encodeMove(from, to, p, P, NOPIECE, false, true, false, isCheck), ply);
            }
        }
    }
    // We will generate the knight moves
    BitBoard knights = ourKnights;
    while(knights){
        Square from = popLsb(knights);
        BitBoard attacks = knightAttacks[from];
        attacks &= theirPieces;
        while(attacks){
            Square to = popLsb(attacks);
            Piece captured = pieceOn(to);
            bool isCheck = (knightCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, N + 6 * side, captured, NOPIECE, false, false, false, isCheck), ply);
        }
    }
    // We will generate the bishop moves
    BitBoard bishops = ourBishops;
    while (bishops){
        Square from = popLsb(bishops);
        BitBoard attacks = getBishopAttack(from, occupancy);
        attacks &= theirPieces;
        while(attacks){
            Square to = popLsb(attacks);
            Piece captured = pieceOn(to);
            bool isCheck = (bishopCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, B + 6 * side, captured, NOPIECE, false, false, false, isCheck), ply);
        }
    }
    // We will generate the rook moves
    BitBoard rooks = ourRooks;
    while (rooks){
        Square from = popLsb(rooks);
        BitBoard attacks = getRookAttack(from, occupancy);
        attacks &= theirPieces;
        while(attacks){
            Square to = popLsb(attacks);
            Piece captured = pieceOn(to);
            bool isCheck = (rookCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, R + 6 * side, captured, NOPIECE, false, false, false, isCheck), ply);
        }
    }
    // We will generate the queen moves
    BitBoard queens = ourQueens;
    while (queens){
        Square from = popLsb(queens);
        BitBoard attacks = getQueenAttack(from, occupancy);
        attacks &= theirPieces;
        while(attacks){
            Square to = popLsb(attacks);
            Piece captured = pieceOn(to);
            bool isCheck = (queenCheckers & squareBB(to)) > 0;
            addMove(&moveList, encodeMove(from, to, Q + 6 * side, captured, NOPIECE, false, false, false, isCheck), ply);
        }
    }
    // We will generate the king moves, including castling
    Square king = lsb(ourKing);
    BitBoard kAttacks = kingAttacks[king];
    kAttacks &= theirPieces;
    while(kAttacks){
        Square to = popLsb(kAttacks);
        Piece captured = pieceOn(to);
        addMove(&moveList, encodeMove(king, to, K + 6 * side, captured, NOPIECE, false, false, false, false), ply);
    }
    
}
/**
 * @brief The makeNullMove function makes a null move on the board.
 */
void Position::makeNullMove(){
    side ^= 1;
    hashKey ^= sideKey;
    hashKey ^= enPassantKeys[enPassant];
    enPassant = noSquare;
    lastMove = 0;
    fiftyMove = 0;
}

UndoInfo::UndoInfo(Position& position){
    hashKey = position.hashKey;
    enPassant = position.enPassant;
    castle = position.castle;
    fiftyMove = position.fiftyMove;
    side = position.side;
    lastMove = position.lastMove;
    psqtScore[0] = position.psqtScore[0];
    psqtScore[1] = position.psqtScore[1];
    memcpy(bitboards, position.bitboards, sizeof(BitBoard) * 12);
    memcpy(occupancies, position.occupancies, sizeof(BitBoard) * 3);
}

void UndoInfo::undoMove(Position& position, Move move){
    position.hashKey = hashKey;
    position.enPassant = enPassant;
    position.castle = castle;
    position.fiftyMove = fiftyMove;
    position.lastMove = lastMove;
    position.psqtScore[0] = psqtScore[0];
    position.psqtScore[1] = psqtScore[1];
    position.side = side;
    
    Piece piece = movePiece(move);
    Piece captured = moveCapture(move);
    Piece aux = isCastling(move) ? (R + 6 * position.side) : movePromotion(move);
    // We will now regenerate the bitboards. One thing to note is that captured and aux may be NOPIECE (12)
    // In that case, we will not change the bitboard. We will do this in a fast and branchless way.
    captured *= captured != NOPIECE;
    aux *= aux != NOPIECE;
    // So that we don't have to do a lot of if statements. Instead, if the piece is NOPIECE, it will become 0 (P) which means we will just make two extra copies,
    // which won't in fact change anything.
    position.bitboards[piece] = bitboards[piece];
    position.bitboards[captured] = bitboards[captured];
    position.bitboards[aux] = bitboards[aux];

    memcpy(position.occupancies, occupancies, sizeof(BitBoard) * 3);
}

void UndoInfo::undoNullMove(Position& position){
    position.hashKey = hashKey;
    position.enPassant = enPassant;
    position.castle = castle;
    position.fiftyMove = fiftyMove;
    position.lastMove = lastMove;
    position.psqtScore[0] = psqtScore[0];
    position.psqtScore[1] = psqtScore[1];
    position.side = side;
}