#include "Game.h"
#include "movegen.h"
#include "tables.h"
#include "history.h"
#include "evaluation.h"
#include "zobrist.h"
#include <iostream>
#include <cstring>

unsigned long long BADCAPTURESCORE = MAXHISTORYABS - 424ULL; // TODO: make this tunable

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
            h ^= pieceKeysTable[i][square];
        }
    }
    h ^= enPassantKeysTable[enPassant];
    h ^= castleKeysTable[castle];
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
            h ^= pawnKeysTable[i][square];
        }
    }
    h ^= enPassantKeysTable[enPassant];
    return h;
}

/** 
 * @brief The Position::generateNonPawnHashKey function generates the hash key of the non pawn structure from scratch, for a given side.
 * @note This function is called by the constructors. Otherwise the hash gets incrementally updated.
 */
HashKey Position::generateNonPawnHashKey(const bool side) {
    const S32 offset = side ? 6 : 0;
    HashKey h = 0ULL;
    for (int i = Pieces::N + offset ; i <= Pieces::K + offset; i++) {
        BitBoard pieceBB = bitboards[i];
        while (pieceBB) {
            Square square = popLsb(pieceBB);
            h ^= nonPawnKeysTable[i][square];
        }
    }
    return h;
}

/**
 * @brief The Position::generatePtHashKey function generates the hash key for a single pt from scratch.
 * @tparam The pt to generate
 * @note This function is called by the constructors. Otherwise the hash gets incrementally updated.
 */
template <Piece pt>
HashKey Position::generatePtHashKey(){
    HashKey h = 0ULL;
    for (int i = pt; i <= Pieces::k; i += 6) {
        BitBoard pieceBB = bitboards[i];
        while (pieceBB) {
            Square square = popLsb(pieceBB);
            h ^= pieceKeysTable[i][square];
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
    totalPly = 0;
    plyFromNull = 0;
    castle = 0;
    hashKey = 0;
    pawnHashKey = 0;
    nonPawnKeys[0] = nonPawnKeys[1] = 0;
    memset(ptKeys,0,sizeof(ptKeys));
    memset(psqtScores,0,sizeof(psqtScores));
}

/**
 * @brief The addPiece function removes a piece from the board. It also incrementally updates psqt and various hash keys
 * @tparam unsafe Set it to true if we are removing a piece that we are sure is there
 * @param pos the Position object to update
 * @param square The square where the piece is.
 * @param piece The piece to remove.
 */
template<bool unsafe = true>
static inline void removePiece(Position& pos, const Piece piece, const Square square){
    const auto pieceSide = piece >= p;
    if constexpr (!unsafe){
        // Remove the piece from the bitboard
        clearBit(pos.bitboards[piece], square);
        // Add it also to the side and both bitboard
        clearBit(pos.occupancies[pieceSide], square);
        clearBit(pos.occupancies[BOTH], square);
    }
    else {
        const BitBoard sqb = squareBB(square);
        pos.bitboards[piece] ^= sqb;
        pos.occupancies[pieceSide] ^= sqb;
        pos.occupancies[BOTH] ^= sqb;
    }
    // Update the hash key
    pos.hashKey ^= pieceKeysTable[piece][square];
    pos.pawnHashKey ^= pawnKeysTable[piece][square];
    pos.nonPawnKeys[pieceSide] ^= nonPawnKeysTable[piece][square];
    pos.ptKeys[piece - 6 * (pieceSide)] ^= pieceKeysTable[piece][square];
    // Update the psqt score
    // Same side kings, own king on right (flip square)
    pos.psqtScores[indexColorKingsKingside(pieceSide, 0, 0)] -= PSQTs[0][piece][flipSquareHorizontal(square)];
    // Same side kings, own king on left 
    pos.psqtScores[indexColorKingsKingside(pieceSide, 0, 1)] -= PSQTs[0][piece][square];
    // Opposite side kings, own king on right (flip square)
    pos.psqtScores[indexColorKingsKingside(pieceSide, 1, 0)] -= PSQTs[1][piece][flipSquareHorizontal(square)];
    // Opposite side kings, own king on left 
    pos.psqtScores[indexColorKingsKingside(pieceSide, 1, 1)] -= PSQTs[1][piece][square];
}

/**
 * @brief The addPiece function adds a piece to the board. It also incrementally updates psqt and various hash keys
 * @tparam unsafe Set it to true if we are removing a piece that we are sure is not already there
 * @param pos the Position object to update
 * @param square The square to add the piece to.
 * @param piece The piece to add.
 */
template<bool unsafe = true>
static inline void addPiece(Position& pos, const Piece piece, const Square square){
    const auto pieceSide = piece >= p;
    if constexpr (!unsafe){
        // Remove the piece from the bitboard
        setBit(pos.bitboards[piece], square);
        // Add it also to the side and both bitboard
        setBit(pos.occupancies[piece >= p], square);
        setBit(pos.occupancies[BOTH], square);
    }
    else {
        const BitBoard sqb = squareBB(square);
        pos.bitboards[piece] ^= sqb;
        pos.occupancies[pieceSide] ^= sqb;
        pos.occupancies[BOTH] ^= sqb;
    }
    // Update the hash key
    pos.hashKey ^= pieceKeysTable[piece][square];
    pos.pawnHashKey ^= pawnKeysTable[piece][square];
    pos.nonPawnKeys[pieceSide] ^= nonPawnKeysTable[piece][square];
    pos.ptKeys[piece - 6 * (pieceSide)] ^= pieceKeysTable[piece][square];
    // Update the psqt score
    // Same side kings, own king on right (flip square)
    pos.psqtScores[indexColorKingsKingside(pieceSide, 0, 0)] += PSQTs[0][piece][flipSquareHorizontal(square)];
    // Same side kings, own king on left 
    pos.psqtScores[indexColorKingsKingside(pieceSide, 0, 1)] += PSQTs[0][piece][square];
    // Opposite side kings, own king on right (flip square)
    pos.psqtScores[indexColorKingsKingside(pieceSide, 1, 0)] += PSQTs[1][piece][flipSquareHorizontal(square)];
    // Opposite side kings, own king on left
    pos.psqtScores[indexColorKingsKingside(pieceSide, 1, 1)] += PSQTs[1][piece][square];
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
    fiftyMove = 0;
    
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
            addPiece(*this, piece, square);
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
    totalPly = 0;
    while (*fen >= '0' && *fen <= '9') {
        totalPly = totalPly * 10 + *fen - '0';
        fen++;
    }

    assert(totalPly >= 0);

    // If not given, the fullmove is at least equal to the fifty move counter
    if (totalPly == 0) totalPly = fiftyMove;

    if (*fen != '\0') {
        // If there's still data left in the string, it's invalid.
        // Since the position is already set, we won't return false, but we will print a warning.
        //std::cout << "Warning: FEN string contains more data than expected : \""<< fen <<"\". Ignoring it.\n";
    }
FENkeyEval:
    // If everything is alright, generate the hash key for the current position
    hashKey = generateHashKey();
    pawnHashKey = generatePawnHashKey();
    nonPawnKeys[WHITE] = generateNonPawnHashKey(WHITE);
    nonPawnKeys[BLACK] = generateNonPawnHashKey(BLACK);
    ptKeys[P] = generatePtHashKey<P>();
    ptKeys[N] = generatePtHashKey<N>();
    ptKeys[B] = generatePtHashKey<B>();
    ptKeys[R] = generatePtHashKey<R>();
    ptKeys[Q] = generatePtHashKey<Q>();
    ptKeys[K] = generatePtHashKey<K>();
    checkers = calculateCheckers();
    generateThreats();
    blockers[WHITE] = getPinnedPieces(occupancies[BOTH], occupancies[WHITE], lsb(bitboards[K]), bitboards[r] | bitboards[q], bitboards[b] | bitboards[q]);
    blockers[BLACK] = getPinnedPieces(occupancies[BOTH], occupancies[BLACK], lsb(bitboards[k]), bitboards[R] | bitboards[Q], bitboards[B] | bitboards[Q]);
    return true;
}

void Position::generateThreats()
{
    threats = 0ULL;
    const BitBoard occ = occupancies[BOTH];

    const BitBoard queens  = bitboards[q - 6*side];
    BitBoard rooks   = bitboards[r - 6*side];
    BitBoard bishops = bitboards[b - 6*side];
    BitBoard knights = bitboards[n - 6*side];
    BitBoard pawns   = bitboards[p - 6*side];

    rooks |= queens;
    while (rooks)
    {
        Square rook = popLsb(rooks);
        threats |= getRookAttack(rook, occ);
    }

    bishops |= queens;
    while (bishops)
    {
        Square bishop = popLsb(bishops);
        threats |= getBishopAttack(bishop, occ);
    }

    while (knights)
    {
        Square knight = popLsb(knights);
        threats |= knightAttacks[knight];
    }

    if (side == WHITE)
        threats |= makePawnAttacks<WHITE>(pawns);
    else
        threats |= makePawnAttacks<BLACK>(pawns);

    threats |= kingAttacks[lsb(bitboards[k - 6*side])];

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
bool Position::makeMove(Move move)
{

    // If the move is invalid, we return false
    if (!move)
        return false;

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
    removePiece(*this, piece, from);

    // If the move is a capture, we remove the captured piece from the target square
    // If the move is a enpassant, we remove the captured pawn from the enPassant square
    if (isEnPass){
        removePiece(*this, captured, (side == WHITE ? to + 8 : to - 8));
        addPiece(*this, piece, to);
    }
    else {
        if (captures)
            removePiece(*this, captured, to);

        // If the move is a promotion, we replace the piece with the promoted piece
        if (promotes)
            addPiece(*this, promotion, to);
        // Otherwise, we move the piece to the target square
        else
            addPiece(*this, piece, to);
    }

    // Remove the current en passant square from the hash key, as it is no longer valid
    hashKey ^= enPassantKeysTable[enPassant];
    pawnHashKey ^= enPassantKeysTable[enPassant];
    enPassant = noSquare;

    
    // If the move is a double pawn push, we set the en passant square and hash it back in. If not, we don't need to do anything, since the noSquare hash is 0
    if (doublePawnPush)
        enPassant = to + 8 * (1 - 2 * side);
    hashKey ^= enPassantKeysTable[enPassant];
    pawnHashKey ^= enPassantKeysTable[enPassant];

    // If the move is a castle, we move the rook
    if (isCastle)
    {
        switch (to)
        {
        case g1:
        {
            // Check if squares between the king and the rook are attacked
            if (isSquareAttacked(e1, BLACK) || isSquareAttacked(f1, BLACK) || isSquareAttacked(g1, BLACK))
                return false;
            removePiece(*this, R, h1);
            addPiece(*this, R, f1);
            break;
        }
        case c1:
        {
            // Check if squares between the king and the rook are attacked
            if (isSquareAttacked(e1, BLACK) || isSquareAttacked(d1, BLACK) || isSquareAttacked(c1, BLACK))
                return false;
            removePiece(*this, R, a1);
            addPiece(*this, R, d1);
            break;
        }
        case g8:
        {
            // Check if squares between the king and the rook are attacked
            if (isSquareAttacked(e8, WHITE) || isSquareAttacked(f8, WHITE) || isSquareAttacked(g8, WHITE))
                return false;
            removePiece(*this, r, h8);
            addPiece(*this, r, f8);
            break;
        }
        case c8:
        {
            // Check if squares between the king and the rook are attacked
            if (isSquareAttacked(e8, WHITE) || isSquareAttacked(d8, WHITE) || isSquareAttacked(c8, WHITE))
                return false;
            removePiece(*this, r, a8);
            addPiece(*this, r, d8);
            break;
        }
        }
    }
    else{
        // Now we verify if the move is legal. We don't check for legality in the case of a castle, since we already checked for that when we generated the move (castling rights and squares between king and rook are not attacked)
        Square ourKing = lsb(bitboards[K + 6 * side]);
        if (isSquareAttacked(ourKing, side ^ 1))
            return false;
    }

    // Now we update the hash for en passant, castle rights and side to move
    hashKey ^= sideKey;

    // Remove previous castle rights
    hashKey ^= castleKeysTable[castle];
    
    // Add new castle rights
    castle &= castlingRights[from];
    castle &= castlingRights[to];
    hashKey ^= castleKeysTable[castle];
    // Change side to move
    side ^= 1;

    totalPly++;
    plyFromNull++;

    if (isCapture(move) || isPromotion(move) || ((movePiece(move) % 6) == 0))
        fiftyMove = 0;
    else
        fiftyMove++;
    checkers = calculateCheckers();
    generateThreats();
    blockers[WHITE] = getPinnedPieces(occupancies[BOTH], occupancies[WHITE], lsb(bitboards[K]), bitboards[r] | bitboards[q], bitboards[b] | bitboards[q]);
    blockers[BLACK] = getPinnedPieces(occupancies[BOTH], occupancies[BLACK], lsb(bitboards[k]), bitboards[R] | bitboards[Q], bitboards[B] | bitboards[Q]);
    return true;
}

inline void Position::addEp(MoveList* ml, ScoredMove move){

    const S32 score = pieceValues[P] * captScoreMvvMultiplier()
                    + captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][P][getThreatsIndexing(threats, move)];

    ml->moves[ml->count++] = ((
        score
        + GOODNOISYMOVE * SEE(move, -score / (captScoreMvvMultiplier() * 4)) 
        + BADNOISYMOVE
    ) << 32) | move;
}

template <Piece promotion>
inline void Position::addPromoCapture(MoveList* ml, ScoredMove move, Piece movedPiece, Piece capturedPiece){
    if constexpr (promotion == R || promotion == B || promotion == r || promotion == b){
        const S32 score = + pieceValues[capturedPiece] * captScoreMvvMultiplier()
            + pieceValues[promotion]
            + pieceValues[capturedPiece]
            - pieceValues[P]
            + captureHistoryTable[indexPieceTo(movedPiece, moveTarget(move))][capturedPiece - 6 * (capturedPiece >= 6)][getThreatsIndexing(threats, move)]; // Piece captured can't be NOPIECE (12), so this works
        
        ml->moves[ml->count++] = ((
            score
            + BADNOISYMOVE
        ) << 32) | move;
    }
    else {
            const S32 score = + pieceValues[capturedPiece] * captScoreMvvMultiplier()
            + pieceValues[promotion]
            + pieceValues[capturedPiece]
            - pieceValues[P]
            + captureHistoryTable[indexPieceTo(movedPiece, moveTarget(move))][capturedPiece - 6 * (capturedPiece >= 6)][getThreatsIndexing(threats, move)]; // Piece captured can't be NOPIECE (12), so this works
        
        ml->moves[ml->count++] = ((
            score
            + GOODNOISYMOVE * SEE(move, -score / (captScoreMvvMultiplier() * 4)) 
            + BADNOISYMOVE
        ) << 32) | move;
    }
}

inline void Position::addCapture(MoveList* ml, ScoredMove move, Piece movedPiece, Piece capturedPiece){

    const S32 score = pieceValues[capturedPiece] * captScoreMvvMultiplier()
    + captureHistoryTable[indexPieceTo(movedPiece, moveTarget(move))][capturedPiece - 6 * (capturedPiece >= 6)][getThreatsIndexing(threats, move)]; // Piece captured can't be NOPIECE (12), so this works

    ml->moves[ml->count++] = ((
        score 
        + GOODNOISYMOVE * SEE(move, -score / (captScoreMvvMultiplier() * 4)) 
        + BADNOISYMOVE
    ) << 32) | move;
    return;
}

template <Piece promotion>
inline void Position::addPromotion(MoveList* ml, ScoredMove move){
    if constexpr (promotion == R || promotion == B || promotion == r || promotion == b){
        const S32 score = 
            + pieceValues[promotion]
            - pieceValues[P]
            + captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][P][getThreatsIndexing(threats, move)];
        ml->moves[ml->count++] = ((
            score 
            + BADNOISYMOVE
        ) << 32) | move;
    }
    else {
        const S32 score = 
            + pieceValues[promotion]
            - pieceValues[P]
            + captureHistoryTable[indexPieceTo(movePiece(move), moveTarget(move))][P][getThreatsIndexing(threats, move)];

        ml->moves[ml->count++] = ((
            score 
            + GOODNOISYMOVE * SEE(move, -score / (captScoreMvvMultiplier() * 4))  
            + BADNOISYMOVE
        ) << 32) | move;
    }
    return;
}

inline void Position::addQuiet(MoveList *ml, ScoredMove move, Square source, Square target, Move killer1, Move killer2, Move counterMove, const S16 *ply1contHist, const S16 *ply2contHist, const S16 *ply4contHist) {
    if (sameMovePos(move, killer1)){
        ml->moves[ml->count++] = (KILLER1SCORE << 32) | move;
        return;
    }
    else if (sameMovePos(move, killer2)){
        ml->moves[ml->count++] = (KILLER2SCORE << 32) | move;
        return;
    }
    else if (sameMovePos(move, counterMove)){
        ml->moves[ml->count++] = (COUNTERSCORE << 32) | move;
        return;
    }
    ml->moves[ml->count++] = ((
        (S64)(historyTable[side][indexFromTo(source, target)][getThreatsIndexing(threats,move)])
        + (S64)(ply1contHist ? ply1contHist[indexPieceTo(movePiece(move), target)] : 0)
        + (S64)(ply2contHist ? ply2contHist[indexPieceTo(movePiece(move), target)] : 0)
        + (S64)(ply4contHist ? ply4contHist[indexPieceTo(movePiece(move), target)] : 0)
        + QUIETSCORE
    ) << 32) | move;
}

inline void Position::addUnsorted(MoveList* ml, ScoredMove move){
    ml->moves[ml->count++] = move;
}

bool Position::inCheck() {
	Square kingPos = lsb(bitboards[K + 6 * side]);
    return isSquareAttacked(kingPos, side ^ 1);
}

bool Position::insufficientMaterial() {
    if (bitboards[P] | bitboards[p] | bitboards[R] | bitboards[r] | bitboards[Q] | bitboards[q]) return false;
    BitBoard whiteMinors = bitboards[B] | bitboards[N];
    BitBoard blackMinors = bitboards[b] | bitboards[n];
    switch (popcount(whiteMinors | blackMinors)) {
        case 0: case 1: return true; // K vs K + N or K vs K + B
        case 2: {
            if (whiteMinors && blackMinors) return true; // K + minor vs K + minor is a dead draw
            
            if (whiteMinors) 
                return !bitboards[B] || (!bitboards[N] && ((bitboards[B] & squaresOfColor[WHITE]) == bitboards[B])); // KBN or KBB with different color bishops
            
            return !bitboards[b] || (!bitboards[n] && ((bitboards[b] & squaresOfColor[BLACK]) == bitboards[b])); // Kbn or Kbb with different color bishops
        }
        default: return false; // 5 pieces are too complex to analyze, we fall back to standard evaluation
    }
}

// Thanks to Weiss chess engine for a clean implementation of this function
bool Position::SEE(const Move move, const Score threshold) {
    if (isCastling(move)) return threshold <= 0; // Castling moves don't capture anything, and they don't put the king nor the rook in danger (because of the check rule for castling)

    Square from = moveSource(move);
    Square to = moveTarget(move);

    Piece target = moveCapture(move);
    Piece promo = movePromotion(move);
    Score value = pieceValues[target] - threshold;

    // If the move is a promotion, we need to consider the value of the promoted piece (and remove the value of the pawn)
    if (promo != NOPIECE) value += pieceValues[promo] - pieceValues[P];

    // Check for early SEE exit (if the threshold is still negative)
    if (value < 0) return false;

    Piece attacker = movePiece(move);

    // Check for another early SEE exit (if immediate recapture still beats the threshold)
    value -= promo != NOPIECE ? pieceValues[promo] : pieceValues[attacker];
    if (value >= 0) return true;

    // Initialize the attackers, occupancies, vertical and horizontal sliders
    BitBoard occupied = occupancies[BOTH] ^ squareBB(from);
    if (isEnPassant(move)) occupied ^= squareBB(to + (side == WHITE ? 8 : -8));

    BitBoard diagonalSliders = bitboards[Q] | bitboards[q] | bitboards[B] | bitboards[b];
    BitBoard orthogonalSliders = bitboards[Q] | bitboards[q] | bitboards[R] | bitboards[r];

    BitBoard attackers = attacksToPre(to, occupied, diagonalSliders, orthogonalSliders);

    const U8 us = attacker < 6 ? WHITE : BLACK; // Side of the attacker

    U8 side = us ^ 1; // Side of the attacker, flipped (one move already executed)

    BitBoard pinned[2] = {
        blockers[WHITE] & occupancies[WHITE],
        blockers[BLACK] & occupancies[BLACK]
    };

    BitBoard kingRays[2] = {
        alignedSquares[to][lsb(bitboards[K])],
        alignedSquares[to][lsb(bitboards[k])],
    };

    BitBoard pinned = pinned[WHITE] | pinned[BLACK];
    BitBoard pinnedAligned = (pinned[WHITE] & kingRays[WHITE]) | (pinned[BLACK] & kingRays[BLACK]);

    while (true){
        attackers &= occupied;
        BitBoard ourAttackers = attackers & occupancies[side];

        // MISSING HERE CHECK FOR PIN

        if (!ourAttackers) break; // We run out of attackers

        // Find the least valuable attacker
        Piece pt;
        for (pt = P; pt < K; ++pt)
            if (ourAttackers & bitboards[pt + 6 * side]) break;
        
        side ^= 1;

        // Update the value
        value = -value -1 - pieceValues[pt];

        // Check if value beats threshold
        if (value >= 0) {
            // Special case for king captures, as we need to check if the king is in check after the capture
            if (pt == K && (attackers & occupancies[side])) side ^= 1; // So that if our kings ends up in check we fail, if their king ends up in check we succeed
            break;
        }

        // Remove the used piece from the board
        clearBit(occupied, lsb(ourAttackers & (bitboards[pt] | bitboards[pt + 6])));

        // Update the attackers
        if (pt == P || pt == B || pt == Q) attackers |= getBishopAttack(to, occupied) & diagonalSliders;
        if (pt == R || pt == Q) attackers |= getRookAttack(to, occupied) & orthogonalSliders;
    }

    // Check who runs out of attackers
    return side != us;
}


void Position::reflect() {
    // reflect psqt
    std::swap(psqtScores[indexColorKingsKingside(WHITE,0,0)], psqtScores[indexColorKingsKingside(BLACK,0,0)]);
    std::swap(psqtScores[indexColorKingsKingside(WHITE,0,1)], psqtScores[indexColorKingsKingside(BLACK,0,1)]);
    std::swap(psqtScores[indexColorKingsKingside(WHITE,1,0)], psqtScores[indexColorKingsKingside(BLACK,1,0)]);
    std::swap(psqtScores[indexColorKingsKingside(WHITE,1,1)], psqtScores[indexColorKingsKingside(BLACK,1,1)]);
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
    occupancies[WHITE] = bitboards[P] | bitboards[N] | bitboards[B] | bitboards[R] | bitboards[Q] | bitboards[K];
    occupancies[BLACK] = bitboards[p] | bitboards[n] | bitboards[b] | bitboards[r] | bitboards[q] | bitboards[k];
    occupancies[BOTH]  = occupancies[WHITE] | occupancies[BLACK];
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
    pawnHashKey = generatePawnHashKey();
    nonPawnKeys[WHITE] = generateNonPawnHashKey(WHITE);
    nonPawnKeys[BLACK] = generateNonPawnHashKey(BLACK);
    ptKeys[P] = generatePtHashKey<P>();
    ptKeys[N] = generatePtHashKey<N>();
    ptKeys[B] = generatePtHashKey<B>();
    ptKeys[R] = generatePtHashKey<R>();
    ptKeys[Q] = generatePtHashKey<Q>();
    ptKeys[K] = generatePtHashKey<K>();
    generateThreats();
    blockers[WHITE] = getPinnedPieces(occupancies[BOTH], occupancies[WHITE], lsb(bitboards[K]), bitboards[r] | bitboards[q], bitboards[b] | bitboards[q]);
    blockers[BLACK] = getPinnedPieces(occupancies[BOTH], occupancies[BLACK], lsb(bitboards[k]), bitboards[R] | bitboards[Q], bitboards[B] | bitboards[Q]);
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
 * @param ss The search stack, to get the killers and counter moves.
 */
void Position::generateMoves(MoveList& moveList, SStack* ss) {

    const Move killer1 = ss->killers[0];
    const Move killer2 = ss->killers[1];
    const Move lastMove = (ss-1)->move;
    const Move counterMove = lastMove ? counterMoveTable[indexFromTo(moveSource(lastMove), moveTarget(lastMove))] : noMove;
    const S16 *ply1contHist = lastMove ? (ss - 1)->contHistEntry : nullptr;
    const S16 *ply2contHist = (ss - 2)->move ? (ss - 2)->contHistEntry : nullptr;
    const S16 *ply4contHist = (ss - 4)->move ? (ss - 4)->contHistEntry : nullptr;

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
            if (to <= h8) {
                // We have a promotion
                addPromoCapture<Q>(&moveList, encodeMove(to + 7, to, P, captured, Q, false, false, false, (queenCheckers & squareBB(to)) > 0) , P, captured);
                addPromoCapture<R>(&moveList, encodeMove(to + 7, to, P, captured, R, false, false, false, (rookCheckers & squareBB(to)) > 0) , P, captured);
                addPromoCapture<B>(&moveList, encodeMove(to + 7, to, P, captured, B, false, false, false, (bishopCheckers & squareBB(to)) > 0) , P, captured);
                addPromoCapture<N>(&moveList, encodeMove(to + 7, to, P, captured, N, false, false, false, (knightCheckers & squareBB(to)) > 0) , P, captured);
            }
            else
                addCapture(&moveList, encodeMove(to + 7, to, P, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), P, captured);
        }
        // Left captures
        while (sxPawnCaptures) {
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            if (to <= h8) {
                // We have a promotion
                addPromoCapture<Q>(&moveList, encodeMove(to + 9, to, P, captured, Q, false, false, false, (queenCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<R>(&moveList, encodeMove(to + 9, to, P, captured, R, false, false, false, (rookCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<B>(&moveList, encodeMove(to + 9, to, P, captured, B, false, false, false, (bishopCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<N>(&moveList, encodeMove(to + 9, to, P, captured, N, false, false, false, (knightCheckers & squareBB(to)) > 0), P, captured);
            }
            else
                addCapture(&moveList, encodeMove(to + 9, to, P, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), P, captured);
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare) {
            Square to = enPassant;
            BitBoard epPawns = pawnAttacks[side ^ 1][to] & ourPawns;
            while (epPawns) {
                Square from = popLsb(epPawns);
                addEp(&moveList, encodeMove(from, to, P, p, NOPIECE, false, true, false, (pawnCheckers & squareBB(to)) > 0));
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
            if (to >= a1) {
                // We have a promotion
                addPromoCapture<q>(&moveList, encodeMove(to - 9, to, p, captured, q, false, false, false, (queenCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<r>(&moveList, encodeMove(to - 9, to, p, captured, r, false, false, false, (rookCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<b>(&moveList, encodeMove(to - 9, to, p, captured, b, false, false, false, (bishopCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<n>(&moveList, encodeMove(to - 9, to, p, captured, n, false, false, false, (knightCheckers & squareBB(to)) > 0), p, captured);
            }
            else
                addCapture(&moveList, encodeMove(to - 9, to, p, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), p, captured);
        }
        // Left captures
        while (sxPawnCaptures) {
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            if (to >= a1) {
                // We have a promotion
                addPromoCapture<q>(&moveList, encodeMove(to - 7, to, p, captured, q, false, false, false, (queenCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<r>(&moveList, encodeMove(to - 7, to, p, captured, r, false, false, false, (rookCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<b>(&moveList, encodeMove(to - 7, to, p, captured, b, false, false, false, (bishopCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<n>(&moveList, encodeMove(to - 7, to, p, captured, n, false, false, false, (knightCheckers & squareBB(to)) > 0), p, captured);
            }
            else
                addCapture(&moveList, encodeMove(to - 7, to, p, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), p, captured);
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare) {
            Square to = enPassant;
            BitBoard epPawns = pawnAttacks[side ^ 1][to] & ourPawns;
            while (epPawns) {
                Square from = popLsb(epPawns);
                addEp(&moveList, encodeMove(from, to, p, P, NOPIECE, false, true, false, (pawnCheckers & squareBB(to)) > 0));
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
            addCapture(&moveList, encodeMove(from, to, N + 6 * side, captured, NOPIECE, false, false, false, isCheck), N + 6 * side, captured);
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (knightCheckers & squareBB(to)) > 0;
            addQuiet(&moveList, encodeMove(from, to, N + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck), from, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
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
            addCapture(&moveList, encodeMove(from, to, B + 6 * side, captured, NOPIECE, false, false, false, isCheck), B + 6 * side, captured);
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (bishopCheckers & squareBB(to)) > 0;
            addQuiet(&moveList, encodeMove(from, to, B + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck), from, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
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
            addCapture(&moveList, encodeMove(from, to, R + 6 * side, captured, NOPIECE, false, false, false, isCheck), R + 6 * side, captured);
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (rookCheckers & squareBB(to)) > 0;
            addQuiet(&moveList, encodeMove(from, to, R + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck), from, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
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
            addCapture(&moveList, encodeMove(from, to, Q + 6 * side, captured, NOPIECE, false, false, false, isCheck), Q + 6 * side, captured);
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (queenCheckers & squareBB(to)) > 0;
            addQuiet(&moveList, encodeMove(from, to, Q + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck), from, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
        }
    }
    // We will generate the pawn pushes
    if (side == WHITE) {
        BitBoard pawnPushes = ourPawns >> 8;
        pawnPushes &= ~occupancy;

        while (pawnPushes) {
            Square to = popLsb(pawnPushes);
            if (to <= h8) {
                // We will generate the promotion moves
                addPromotion<Q>(&moveList, encodeMove(to + 8, to, P, NOPIECE, Q, false, false, false, (queenCheckers & squareBB(to)) > 0));
                addPromotion<R>(&moveList, encodeMove(to + 8, to, P, NOPIECE, R, false, false, false, (rookCheckers & squareBB(to)) > 0));
                addPromotion<B>(&moveList, encodeMove(to + 8, to, P, NOPIECE, B, false, false, false, (bishopCheckers & squareBB(to)) > 0));
                addPromotion<N>(&moveList, encodeMove(to + 8, to, P, NOPIECE, N, false, false, false, (knightCheckers & squareBB(to)) > 0));
            }
            else
                addQuiet(&moveList, encodeMove(to + 8, to, P, NOPIECE, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), to + 8, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
        }

        BitBoard pawnDoublePushes = ourPawns & ranks(6);
        pawnDoublePushes >>= 8;
        pawnDoublePushes &= ~occupancy;
        pawnDoublePushes >>= 8;
        pawnDoublePushes &= ~occupancy;
        while (pawnDoublePushes) {
            Square to = popLsb(pawnDoublePushes);
            addQuiet(&moveList, encodeMove(to + 16, to, P, NOPIECE, NOPIECE, true, false, false, (pawnCheckers & squareBB(to)) > 0), to + 16, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
        }
    }
    else {
        BitBoard pawnPushes = ourPawns << 8;
        pawnPushes &= ~occupancy;
        while (pawnPushes) {
            Square to = popLsb(pawnPushes);
            if (to >= a1) {
                // We will generate the promotion moves
                addPromotion<q>(&moveList, encodeMove(to - 8, to, p, NOPIECE, q, false, false, false, (queenCheckers & squareBB(to)) > 0)); 
                addPromotion<r>(&moveList, encodeMove(to - 8, to, p, NOPIECE, r, false, false, false, (rookCheckers & squareBB(to)) > 0));
                addPromotion<b>(&moveList, encodeMove(to - 8, to, p, NOPIECE, b, false, false, false, (bishopCheckers & squareBB(to)) > 0));
                addPromotion<n>(&moveList, encodeMove(to - 8, to, p, NOPIECE, n, false, false, false, (knightCheckers & squareBB(to)) > 0));
            }
            else
                addQuiet(&moveList, encodeMove(to - 8, to, p, NOPIECE, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), to - 8, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
        }
        BitBoard pawnDoublePushes = ourPawns & ranks(1);
        pawnDoublePushes <<= 8;
        pawnDoublePushes &= ~occupancy;
        pawnDoublePushes <<= 8;
        pawnDoublePushes &= ~occupancy;
        while (pawnDoublePushes) {
            Square to = popLsb(pawnDoublePushes);
            addQuiet(&moveList, encodeMove(to - 16, to, p, NOPIECE, NOPIECE, true, false, false, (pawnCheckers & squareBB(to)) > 0), to - 16, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
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
        addCapture(&moveList, encodeMove(king, to, K + 6 * side, captured, NOPIECE, false, false, false, false), K + 6 * side, captured);
    }
    while (kMoves) {
        Square to = popLsb(kMoves);
        addQuiet(&moveList, encodeMove(king, to, K + 6 * side, NOPIECE, NOPIECE, false, false, false, false), king, to, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
    }
    // We will generate the castling moves
    if (side == WHITE) {
        if (castle & CastleRights::WK && !(occupancy & wKCastleMask)) {
            addQuiet(&moveList, encodeMove(e1, g1, K, NOPIECE, NOPIECE, false, false, true, false), e1, g1, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
        }
        if (castle & CastleRights::WQ && !(occupancy & wQCastleMask)) {
            addQuiet(&moveList, encodeMove(e1, c1, K, NOPIECE, NOPIECE, false, false, true, false), e1, c1, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
        }
    }
    else {
        if (castle & CastleRights::BK && !(occupancy & bKCastleMask)) {
            addQuiet(&moveList, encodeMove(e8, g8, k, NOPIECE, NOPIECE, false, false, true, false), e8, g8, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
        }
        if (castle & CastleRights::BQ && !(occupancy & bQCastleMask)) {
            addQuiet(&moveList, encodeMove(e8, c8, k, NOPIECE, NOPIECE, false, false, true, false), e8, c8, killer1, killer2, counterMove, ply1contHist, ply2contHist, ply4contHist);
        }
    }
}

void Position::generateCaptures(MoveList &moveList){
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
            if (to <= h8) {
                // We have a promotion
                addPromoCapture<Q>(&moveList, encodeMove(to + 7, to, P, captured, Q, false, false, false, (queenCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<R>(&moveList, encodeMove(to + 7, to, P, captured, R, false, false, false, (rookCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<B>(&moveList, encodeMove(to + 7, to, P, captured, B, false, false, false, (bishopCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<N>(&moveList, encodeMove(to + 7, to, P, captured, N, false, false, false, (knightCheckers & squareBB(to)) > 0), P, captured);
            }
            else
                addCapture(&moveList, encodeMove(to + 7, to, P, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), P, captured);
        }
        // Left captures
        while (sxPawnCaptures){
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            if (to <= h8) {
                // We have a promotion
                addPromoCapture<Q>(&moveList, encodeMove(to + 9, to, P, captured, Q, false, false, false, (queenCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<R>(&moveList, encodeMove(to + 9, to, P, captured, R, false, false, false, (rookCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<B>(&moveList, encodeMove(to + 9, to, P, captured, B, false, false, false, (bishopCheckers & squareBB(to)) > 0), P, captured);
                addPromoCapture<N>(&moveList, encodeMove(to + 9, to, P, captured, N, false, false, false, (knightCheckers & squareBB(to)) > 0), P, captured);
            }
            else
                addCapture(&moveList, encodeMove(to + 9, to, P, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), P, captured);
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare){
            Square to = enPassant;
            BitBoard epPawns = pawnAttacks[side^1][to] & ourPawns;
            while (epPawns){
                Square from = popLsb(epPawns);
                addEp(&moveList, encodeMove(from, to, P, p, NOPIECE, false, true, false, (pawnCheckers & squareBB(to)) > 0));
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
            if (to >= a1) {
                // We have a promotion
                addPromoCapture<q>(&moveList, encodeMove(to - 9, to, p, captured, q, false, false, false, (queenCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<r>(&moveList, encodeMove(to - 9, to, p, captured, r, false, false, false, (rookCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<b>(&moveList, encodeMove(to - 9, to, p, captured, b, false, false, false, (bishopCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<n>(&moveList, encodeMove(to - 9, to, p, captured, n, false, false, false, (knightCheckers & squareBB(to)) > 0), p, captured);
            }
            else
                addCapture(&moveList, encodeMove(to - 9, to, p, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), p, captured);
        }
        // Left captures
        while (sxPawnCaptures){
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            if (to >= a1) {
                // We have a promotion
                addPromoCapture<q>(&moveList, encodeMove(to - 7, to, p, captured, q, false, false, false, (queenCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<r>(&moveList, encodeMove(to - 7, to, p, captured, r, false, false, false, (rookCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<b>(&moveList, encodeMove(to - 7, to, p, captured, b, false, false, false, (bishopCheckers & squareBB(to)) > 0), p, captured);
                addPromoCapture<n>(&moveList, encodeMove(to - 7, to, p, captured, n, false, false, false, (knightCheckers & squareBB(to)) > 0), p, captured);
            }
            else
                addCapture(&moveList, encodeMove(to - 7, to, p, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0), p, captured);
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare){
            Square to = enPassant;
            BitBoard epPawns = pawnAttacks[side^1][to] & ourPawns;
            while (epPawns){
                Square from = popLsb(epPawns);
                addEp(&moveList, encodeMove(from, to, p, P, NOPIECE, false, true, false, (pawnCheckers & squareBB(to)) > 0));
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
            addCapture(&moveList, encodeMove(from, to, N + 6 * side, captured, NOPIECE, false, false, false, isCheck), N + 6 * side, captured);
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
            addCapture(&moveList, encodeMove(from, to, B + 6 * side, captured, NOPIECE, false, false, false, isCheck), B + 6 * side, captured);
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
            addCapture(&moveList, encodeMove(from, to, R + 6 * side, captured, NOPIECE, false, false, false, isCheck), R + 6 * side, captured);
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
            addCapture(&moveList, encodeMove(from, to, Q + 6 * side, captured, NOPIECE, false, false, false, isCheck), Q + 6 * side, captured);
        }
    }
    // We will generate the king moves, including castling
    Square king = lsb(ourKing);
    BitBoard kAttacks = kingAttacks[king];
    kAttacks &= theirPieces;
    while(kAttacks){
        Square to = popLsb(kAttacks);
        Piece captured = pieceOn(to);
        addCapture(&moveList, encodeMove(king, to, K + 6 * side, captured, NOPIECE, false, false, false, false), K + 6 * side, captured);
    }
}

/**
 * @brief The generateUnsortedMoves function generates all pseudo legal moves for the current side. Used for perft testing (when we don't care about the move ordering).
 * @param moveList The moveList to fill with the generated moves.
 */
void Position::generateUnsortedMoves(MoveList& moveList) {

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
            if (to <= h8) {
                // We have a promotion
                addUnsorted(&moveList, encodeMove(to + 7, to, P, captured, Q, false, false, false, (queenCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 7, to, P, captured, R, false, false, false, (rookCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 7, to, P, captured, B, false, false, false, (bishopCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 7, to, P, captured, N, false, false, false, (knightCheckers & squareBB(to)) > 0));
            }
            else
                addUnsorted(&moveList, encodeMove(to + 7, to, P, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0));
        }
        // Left captures
        while (sxPawnCaptures) {
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            if (to <= h8) {
                // We have a promotion
                addUnsorted(&moveList, encodeMove(to + 9, to, P, captured, Q, false, false, false, (queenCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 9, to, P, captured, R, false, false, false, (rookCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 9, to, P, captured, B, false, false, false, (bishopCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 9, to, P, captured, N, false, false, false, (knightCheckers & squareBB(to)) > 0));
            }
            else
                addUnsorted(&moveList, encodeMove(to + 9, to, P, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0));
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare) {
            Square to = enPassant;
            BitBoard epPawns = pawnAttacks[side ^ 1][to] & ourPawns;
            while (epPawns) {
                Square from = popLsb(epPawns);
                addUnsorted(&moveList, encodeMove(from, to, P, p, NOPIECE, false, true, false, (pawnCheckers & squareBB(to)) > 0));
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
            if (to >= a1) {
                // We have a promotion
                addUnsorted(&moveList, encodeMove(to - 9, to, p, captured, q, false, false, false, (queenCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 9, to, p, captured, r, false, false, false, (rookCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 9, to, p, captured, b, false, false, false, (bishopCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 9, to, p, captured, n, false, false, false, (knightCheckers & squareBB(to)) > 0));
            }
            else
                addUnsorted(&moveList, encodeMove(to - 9, to, p, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0));
        }
        // Left captures
        while (sxPawnCaptures) {
            Square to = popLsb(sxPawnCaptures);
            Piece captured = pieceOn(to);
            if (to >= a1) {
                // We have a promotion
                addUnsorted(&moveList, encodeMove(to - 7, to, p, captured, q, false, false, false, (queenCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 7, to, p, captured, r, false, false, false, (rookCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 7, to, p, captured, b, false, false, false, (bishopCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 7, to, p, captured, n, false, false, false, (knightCheckers & squareBB(to)) > 0));
            }
            else
                addUnsorted(&moveList, encodeMove(to - 7, to, p, captured, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0));
        }
        // En passant capture (if there is one)
        if (enPassant != noSquare) {
            Square to = enPassant;
            BitBoard epPawns = pawnAttacks[side ^ 1][to] & ourPawns;
            while (epPawns) {
                Square from = popLsb(epPawns);
                addUnsorted(&moveList, encodeMove(from, to, p, P, NOPIECE, false, true, false, (pawnCheckers & squareBB(to)) > 0));
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
            addUnsorted(&moveList, encodeMove(from, to, N + 6 * side, captured, NOPIECE, false, false, false, isCheck));
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (knightCheckers & squareBB(to)) > 0;
            addUnsorted(&moveList, encodeMove(from, to, N + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck));
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
            addUnsorted(&moveList, encodeMove(from, to, B + 6 * side, captured, NOPIECE, false, false, false, isCheck));
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (bishopCheckers & squareBB(to)) > 0;
            addUnsorted(&moveList, encodeMove(from, to, B + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck));
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
            addUnsorted(&moveList, encodeMove(from, to, R + 6 * side, captured, NOPIECE, false, false, false, isCheck));
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (rookCheckers & squareBB(to)) > 0;
            addUnsorted(&moveList, encodeMove(from, to, R + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck));
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
            addUnsorted(&moveList, encodeMove(from, to, Q + 6 * side, captured, NOPIECE, false, false, false, isCheck));
        }
        while (quiets) {
            Square to = popLsb(quiets);
            bool isCheck = (queenCheckers & squareBB(to)) > 0;
            addUnsorted(&moveList, encodeMove(from, to, Q + 6 * side, NOPIECE, NOPIECE, false, false, false, isCheck));
        }
    }
    // We will generate the pawn pushes
    if (side == WHITE) {
        BitBoard pawnPushes = ourPawns >> 8;
        pawnPushes &= ~occupancy;

        while (pawnPushes) {
            Square to = popLsb(pawnPushes);
            if (to <= h8) {
                // We will generate the promotion moves
                addUnsorted(&moveList, encodeMove(to + 8, to, P, NOPIECE, Q, false, false, false, (queenCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 8, to, P, NOPIECE, R, false, false, false, (rookCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 8, to, P, NOPIECE, B, false, false, false, (bishopCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to + 8, to, P, NOPIECE, N, false, false, false, (knightCheckers & squareBB(to)) > 0));
            }
            else
                addUnsorted(&moveList, encodeMove(to + 8, to, P, NOPIECE, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0));
        }

        BitBoard pawnDoublePushes = ourPawns & ranks(6);
        pawnDoublePushes >>= 8;
        pawnDoublePushes &= ~occupancy;
        pawnDoublePushes >>= 8;
        pawnDoublePushes &= ~occupancy;
        while (pawnDoublePushes) {
            Square to = popLsb(pawnDoublePushes);
            addUnsorted(&moveList, encodeMove(to + 16, to, P, NOPIECE, NOPIECE, true, false, false, (pawnCheckers & squareBB(to)) > 0));
        }
    }
    else {
        BitBoard pawnPushes = ourPawns << 8;
        pawnPushes &= ~occupancy;
        while (pawnPushes) {
            Square to = popLsb(pawnPushes);
            if (to >= a1) {
                // We will generate the promotion moves
                addUnsorted(&moveList, encodeMove(to - 8, to, p, NOPIECE, q, false, false, false, (queenCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 8, to, p, NOPIECE, r, false, false, false, (rookCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 8, to, p, NOPIECE, b, false, false, false, (bishopCheckers & squareBB(to)) > 0));
                addUnsorted(&moveList, encodeMove(to - 8, to, p, NOPIECE, n, false, false, false, (knightCheckers & squareBB(to)) > 0));
            }
            else
                addUnsorted(&moveList, encodeMove(to - 8, to, p, NOPIECE, NOPIECE, false, false, false, (pawnCheckers & squareBB(to)) > 0));
        }
        BitBoard pawnDoublePushes = ourPawns & ranks(1);
        pawnDoublePushes <<= 8;
        pawnDoublePushes &= ~occupancy;
        pawnDoublePushes <<= 8;
        pawnDoublePushes &= ~occupancy;
        while (pawnDoublePushes) {
            Square to = popLsb(pawnDoublePushes);
            addUnsorted(&moveList, encodeMove(to - 16, to, p, NOPIECE, NOPIECE, true, false, false, (pawnCheckers & squareBB(to)) > 0));
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
        addUnsorted(&moveList, encodeMove(king, to, K + 6 * side, captured, NOPIECE, false, false, false, false));
    }
    while (kMoves) {
        Square to = popLsb(kMoves);
        addUnsorted(&moveList, encodeMove(king, to, K + 6 * side, NOPIECE, NOPIECE, false, false, false, false));
    }
    // We will generate the castling moves
    if (side == WHITE) {
        if (castle & CastleRights::WK && !(occupancy & wKCastleMask)) {
            addUnsorted(&moveList, encodeMove(e1, g1, K, NOPIECE, NOPIECE, false, false, true, false));
        }
        if (castle & CastleRights::WQ && !(occupancy & wQCastleMask)) {
            addUnsorted(&moveList, encodeMove(e1, c1, K, NOPIECE, NOPIECE, false, false, true, false));
        }
    }
    else {
        if (castle & CastleRights::BK && !(occupancy & bKCastleMask)) {
            addUnsorted(&moveList, encodeMove(e8, g8, k, NOPIECE, NOPIECE, false, false, true, false));
        }
        if (castle & CastleRights::BQ && !(occupancy & bQCastleMask)) {
            addUnsorted(&moveList, encodeMove(e8, c8, k, NOPIECE, NOPIECE, false, false, true, false));
        }
    }
}

/**
 * @brief The makeNullMove function makes a null move on the board.
 */
void Position::makeNullMove(){
    side ^= 1;
    hashKey ^= sideKey;
    hashKey ^= enPassantKeysTable[enPassant];
    pawnHashKey ^= enPassantKeysTable[enPassant];
    enPassant = noSquare;
    fiftyMove++;
    totalPly++;
    plyFromNull = 0;
    checkers = 0ULL; // null moves will never be done while in check.
    generateThreats();
    // Pins dont change after null move
}

UndoInfo::UndoInfo(Position& position){
    hashKey = position.hashKey;
    pawnsHashKey = position.pawnHashKey;
    nonPawnsHashKey[WHITE] = position.nonPawnKeys[WHITE];
    nonPawnsHashKey[BLACK] = position.nonPawnKeys[BLACK];
    enPassant = position.enPassant;
    castle = position.castle;
    fiftyMove = position.fiftyMove;
    totalPly = position.totalPly;
    plyFromNull = position.plyFromNull;
    side = position.side;
    checkers = position.checkers;
    threats = position.threats;
    memcpy(psqtScores, position.psqtScores, sizeof(psqtScores));
    memcpy(bitboards, position.bitboards, sizeof(BitBoard) * 12);
    memcpy(occupancies, position.occupancies, sizeof(BitBoard) * 3);
    memcpy(ptHashKey, position.ptKeys, sizeof(HashKey) * 6);
    memcpy(blockers, position.blockers, sizeof(BitBoard) * 2);
}

void UndoInfo::undoMove(Position& position, Move move){
    position.hashKey = hashKey;
    position.pawnHashKey = pawnsHashKey;
    position.nonPawnKeys[WHITE] = nonPawnsHashKey[WHITE];
    position.nonPawnKeys[BLACK] = nonPawnsHashKey[BLACK];
    memcpy(position.ptKeys, ptHashKey, sizeof(HashKey) * 6);
    position.enPassant = enPassant;
    position.castle = castle;
    position.fiftyMove = fiftyMove;
    position.totalPly = totalPly;
    position.plyFromNull = plyFromNull;
    position.side = side;
    position.threats = threats;
    
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
    position.checkers = checkers;
    memcpy(position.psqtScores, psqtScores, sizeof(PScore) * 8);
    memcpy(position.occupancies, occupancies, sizeof(BitBoard) * 3);
    memcpy(position.blockers, blockers, sizeof(BitBoard) * 2);
}

void UndoInfo::undoNullMove(Position& position){
    position.hashKey = hashKey;
    position.pawnHashKey = pawnsHashKey;
    position.nonPawnKeys[WHITE] = nonPawnsHashKey[WHITE];
    position.nonPawnKeys[BLACK] = nonPawnsHashKey[BLACK];
    memcpy(position.ptKeys, ptHashKey, sizeof(HashKey) * 6);
    position.enPassant = enPassant;
    position.castle = castle;
    position.fiftyMove = fiftyMove;
    position.totalPly = totalPly;
    position.plyFromNull = plyFromNull;
    position.side = side;
    position.checkers = checkers;
    position.threats = threats;
    // Pins dont change with null moves
}
