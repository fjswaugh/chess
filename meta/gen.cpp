#include "chess/chess.h"

#include <cstring>
#include <cstdio>

#include <vector>

using namespace Chess;

constexpr const Bitboard row[8] = {0x00000000000000FF, 0x000000000000FF00, 0x0000000000FF0000,
                                   0x00000000FF000000, 0x000000FF00000000, 0x0000FF0000000000,
                                   0x00FF000000000000, 0xFF00000000000000};

constexpr const Bitboard col[8] = {0x0101010101010101, 0x0202020202020202, 0x0404040404040404,
                                   0x0808080808080808, 0x1010101010101010, 0x2020202020202020,
                                   0x4040404040404040, 0x8080808080808080};

constexpr const Bitboard fdiag[64] = {
    0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008,
    0x0000000080402010, 0x0000000000804020, 0x0000000000008040, 0x0000000000000080,
    0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804,
    0x0000008040201008, 0x0000000080402010, 0x0000000000804020, 0x0000000000008040,
    0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402,
    0x0000804020100804, 0x0000008040201008, 0x0000000080402010, 0x0000000000804020,
    0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201,
    0x0080402010080402, 0x0000804020100804, 0x0000008040201008, 0x0000000080402010,
    0x0804020100000000, 0x1008040201000000, 0x2010080402010000, 0x4020100804020100,
    0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008,
    0x0402010000000000, 0x0804020100000000, 0x1008040201000000, 0x2010080402010000,
    0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804,
    0x0201000000000000, 0x0402010000000000, 0x0804020100000000, 0x1008040201000000,
    0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402,
    0x0100000000000000, 0x0201000000000000, 0x0402010000000000, 0x0804020100000000,
    0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201,
};

constexpr const Bitboard rdiag[64] = {
    0x0000000000000001, 0x0000000000000102, 0x0000000000010204, 0x0000000001020408,
    0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080,
    0x0000000000000102, 0x0000000000010204, 0x0000000001020408, 0x0000000102040810,
    0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000,
    0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020,
    0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000,
    0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040,
    0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000,
    0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080,
    0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000,
    0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000,
    0x0408102040800000, 0x0810204080000000, 0x1020408000000000, 0x2040800000000000,
    0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000,
    0x0810204080000000, 0x1020408000000000, 0x2040800000000000, 0x4080000000000000,
    0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000,
    0x1020408000000000, 0x2040800000000000, 0x4080000000000000, 0x8000000000000000,
};

constexpr auto generate_attack_tables()
{
    struct {
        Bitboard rook[64]{};
        Bitboard bishop[64]{};
    } tables;

    constexpr Bitboard edges = col[0] | col[7] | row[0] | row[7];

    for (int i = 0; i < 64; ++i) {
        Location l(i);
        tables.rook[i] = (row[l.row()] & ~col[0] & ~col[7]) | (col[l.col()] & ~row[0] & ~row[7]);
        tables.rook[i] &= ~mask_of(l);
        tables.bishop[i] = (fdiag[i] | rdiag[i]) & ~edges;
        tables.bishop[i] &= ~mask_of(l);
    }

    return tables;
}

constexpr auto attack_tables = generate_attack_tables();

template <Piece>
constexpr int attack_lookup_table = 0;

template <>
constexpr auto attack_lookup_table<Piece::rook> = attack_tables.rook;

template <>
constexpr auto attack_lookup_table<Piece::bishop> = attack_tables.bishop;

template <Piece>
int magic_bitboard_lookup_table = 0;

template <>
Bitboard magic_bitboard_lookup_table<Piece::rook>[64]{};

template <>
Bitboard magic_bitboard_lookup_table<Piece::bishop>[64]{};

template <Piece>
int move_lookup_table = 0;

template <>
Bitboard move_lookup_table<Piece::rook>[64][4096]{};

template <>
Bitboard move_lookup_table<Piece::bishop>[64][512]{};

void print(Bitboard b) {
    std::bitset<64> bs = b;
    int i = 64 - 8;
    while (i >= 0) {
        std::printf("%i", bs.test(i));
        ++i;
        if (i % 8 == 0) {
            std::printf("\n");
            i -= 16;
        }
    }
    std::printf("\n");
}

std::vector<Bitboard> enumerate_all_possible_blocker_boards(Bitboard attack_board)
{
    const auto number_of_enumerations = 1_u32 << count(attack_board);

    std::vector<Bitboard> blocker_boards(number_of_enumerations);

    for (u32 i = 0; i < number_of_enumerations; ++i) {
        // Map i to the attack board to generate a blocker board

        std::bitset<64> b = attack_board;

        int index_of_i = 0;
        for (unsigned j = 0; j < b.size(); ++j) {
            if (b.test(j)) {
                const bool value = i & (1_u32 << index_of_i);
                b.set(j, value);
                ++index_of_i;
            }
            if (index_of_i > count(attack_board)) break;
        }

        blocker_boards[i] = b.to_ullong();
    }

    return blocker_boards;
}

u8 reverse(u8 b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

Bitboard reverse_row(Bitboard b, int row) {
    u8 byte = b >> (row * 8);
    byte = reverse(byte);
    b &= ~(0xFF_u64 << (row * 8));
    b |= (u64(byte) << (row * 8));
    return b;
}

Bitboard reverse_cols(Bitboard b) {
    return __builtin_bswap64(b);
}

template <Piece>
Bitboard move_bitboard_from_blocker(Bitboard, Location);

template <>
Bitboard move_bitboard_from_blocker<Piece::rook>(Bitboard blocker, Location l) {
    const Bitboard row_mask = row[l.row()];
    const Bitboard col_mask = col[l.col()];

    const Bitboard rook = mask_of(l);

    const Bitboard rblocker = blocker & row_mask;
    const Bitboard cblocker = blocker & col_mask;

    // Here you must reverse cols and rows separately
    const auto rc = [](u64 b) -> u64 { return reverse_cols(b); };
    const auto rr = [l](u64 b) -> u64 { return reverse_row(b, l.row()); };

    Bitboard move = 0;

    move |= ((rblocker - rook) ^ rr(rr(rblocker) - rr(rook))) & row_mask;
    move |= ((cblocker - rook) ^ rc(rc(cblocker) - rc(rook))) & col_mask;

    return move;
}

template <>
Bitboard move_bitboard_from_blocker<Piece::bishop>(Bitboard blocker, Location l) {
    const Bitboard fdiag_mask = fdiag[l];
    const Bitboard rdiag_mask = rdiag[l];

    const Bitboard bishop = mask_of(l);

    const Bitboard fblocker = blocker & fdiag_mask;
    const Bitboard bblocker = blocker & rdiag_mask;

    const auto r = [](u64 b) -> u64 { return reverse_cols(b); };

    Bitboard move = 0;

    move |= ((fblocker - bishop) ^ r(r(fblocker) - r(bishop))) & fdiag_mask;
    move |= ((bblocker - bishop) ^ r(r(bblocker) - r(bishop))) & rdiag_mask;

    return move;
}

#include <random>
#include <limits>

u64 random_u64() {
    static std::mt19937 gen{std::random_device{}()};
    return std::uniform_int_distribution<u64>{0, std::numeric_limits<u64>::max()}(gen);
}

template <Piece piece>
void generate_magic_bitboards_and_move_lookup_tables()
{
    for (int l = 0; l < 64; ++l) {
        constexpr auto key_bits = (piece == Piece::bishop ? 9 : 12);
        Bitboard database[1 << key_bits]{};

        const Bitboard attack_board = attack_lookup_table<piece>[l];
        const auto blocker_boards = enumerate_all_possible_blocker_boards(attack_board);

        u64 magic = 0;

        while (true) {
            bool good_magic = true;

            for (const auto blocker_board : blocker_boards) {
                const int index = (blocker_board * magic) >> (64 - key_bits);

                const Bitboard move = move_bitboard_from_blocker<piece>(blocker_board, l);
                if (database[index] == 0) {
                    database[index] = move;
                } else {
                    if (database[index] != move) {
                        good_magic = false;
                        break;
                    }
                }
            }

            if (good_magic) break;

            good_magic = true;
            for (auto& x : database) x = 0;
            magic = (random_u64() & random_u64() & random_u64());
        }

        magic_bitboard_lookup_table<piece>[l] = magic;

        // Want to do this but this syntax wouldn't perform a copy
        //move_lookup_table<piece>[l] = database;
        std::memcpy(move_lookup_table<piece>[l], database, (1 << key_bits) * 8);
    }
}

void print_attack_tables()
{
    std::printf("template <Piece>\nconstexpr int attack_lookup_table = 0;\n\n");

    std::printf("template <>\nconstexpr Bitboard attack_lookup_table<Piece::rook>[64] = {");
    for (int i = 0; i < 64; ++i) {
        if (i % 4 == 0) std::printf("\n    ");
        std::printf("0x%016lx, ", attack_lookup_table<Piece::rook>[i]);
    }
    std::printf("\n};\n\n");

    std::printf("template <>\nconstexpr Bitboard attack_lookup_table<Piece::bishop>[64] = {");
    for (int i = 0; i < 64; ++i) {
        if (i % 4 == 0) std::printf("\n    ");
        std::printf("0x%016lx, ", attack_lookup_table<Piece::bishop>[i]);
    }
    std::printf("\n};\n\n");
}

void print_magic_tables()
{
    std::printf("template <Piece>\nconstexpr int magic_bitboard_lookup_table = 0;\n\n");

    std::printf("template <>\nconstexpr Bitboard magic_bitboard_lookup_table<Piece::rook>[64] = {");
    for (int i = 0; i < 64; ++i) {
        if (i % 4 == 0) std::printf("\n    ");
        std::printf("0x%016lx, ", magic_bitboard_lookup_table<Piece::rook>[i]);
    }
    std::printf("\n};\n\n");

    std::printf("template <>\nconstexpr Bitboard magic_bitboard_lookup_table<Piece::bishop>[64] = {");
    for (int i = 0; i < 64; ++i) {
        if (i % 4 == 0) std::printf("\n    ");
        std::printf("0x%016lx, ", magic_bitboard_lookup_table<Piece::bishop>[i]);
    }
    std::printf("\n};\n\n");
}

void print_move_tables()
{
    std::printf("template <Piece>\nconstexpr int move_lookup_table = 0;\n\n");

    std::printf(
    "template <>\n"
    "constexpr const Bitboard move_lookup_table<Piece::knight>[64] = {\n"
    "    0x0000000000020400, 0x0000000000050800, 0x00000000000A1100, 0x0000000000142200,\n"
    "    0x0000000000284400, 0x0000000000508800, 0x0000000000A01000, 0x0000000000402000,\n"
    "    0x0000000002040004, 0x0000000005080008, 0x000000000A110011, 0x0000000014220022,\n"
    "    0x0000000028440044, 0x0000000050880088, 0x00000000A0100010, 0x0000000040200020,\n"
    "    0x0000000204000402, 0x0000000508000805, 0x0000000A1100110A, 0x0000001422002214,\n"
    "    0x0000002844004428, 0x0000005088008850, 0x000000A0100010A0, 0x0000004020002040,\n"
    "    0x0000020400040200, 0x0000050800080500, 0x00000A1100110A00, 0x0000142200221400,\n"
    "    0x0000284400442800, 0x0000508800885000, 0x0000A0100010A000, 0x0000402000204000,\n"
    "    0x0002040004020000, 0x0005080008050000, 0x000A1100110A0000, 0x0014220022140000,\n"
    "    0x0028440044280000, 0x0050880088500000, 0x00A0100010A00000, 0x0040200020400000,\n"
    "    0x0204000402000000, 0x0508000805000000, 0x0A1100110A000000, 0x1422002214000000,\n"
    "    0x2844004428000000, 0x5088008850000000, 0xA0100010A0000000, 0x4020002040000000,\n"
    "    0x0400040200000000, 0x0800080500000000, 0x1100110A00000000, 0x2200221400000000,\n"
    "    0x4400442800000000, 0x8800885000000000, 0x100010A000000000, 0x2000204000000000,\n"
    "    0x0004020000000000, 0x0008050000000000, 0x00110A0000000000, 0x0022140000000000,\n"
    "    0x0044280000000000, 0x0088500000000000, 0x0010A00000000000, 0x0020400000000000,\n"
    "};\n\n"
    "template <>\n"
    "constexpr const Bitboard move_lookup_table<Piece::king>[64] = {\n"
    "    0x0000000000000302, 0x0000000000000705, 0x0000000000000E0A, 0x0000000000001C14,\n"
    "    0x0000000000003828, 0x0000000000007050, 0x000000000000E0A0, 0x000000000000C040,\n"
    "    0x0000000000030203, 0x0000000000070507, 0x00000000000E0A0E, 0x00000000001C141C,\n"
    "    0x0000000000382838, 0x0000000000705070, 0x0000000000E0A0E0, 0x0000000000C040C0,\n"
    "    0x0000000003020300, 0x0000000007050700, 0x000000000E0A0E00, 0x000000001C141C00,\n"
    "    0x0000000038283800, 0x0000000070507000, 0x00000000E0A0E000, 0x00000000C040C000,\n"
    "    0x0000000302030000, 0x0000000705070000, 0x0000000E0A0E0000, 0x0000001C141C0000,\n"
    "    0x0000003828380000, 0x0000007050700000, 0x000000E0A0E00000, 0x000000C040C00000,\n"
    "    0x0000030203000000, 0x0000070507000000, 0x00000E0A0E000000, 0x00001C141C000000,\n"
    "    0x0000382838000000, 0x0000705070000000, 0x0000E0A0E0000000, 0x0000C040C0000000,\n"
    "    0x0003020300000000, 0x0007050700000000, 0x000E0A0E00000000, 0x001C141C00000000,\n"
    "    0x0038283800000000, 0x0070507000000000, 0x00E0A0E000000000, 0x00C040C000000000,\n"
    "    0x0302030000000000, 0x0705070000000000, 0x0E0A0E0000000000, 0x1C141C0000000000,\n"
    "    0x3828380000000000, 0x7050700000000000, 0xE0A0E00000000000, 0xC040C00000000000,\n"
    "    0x0203000000000000, 0x0507000000000000, 0x0A0E000000000000, 0x141C000000000000,\n"
    "    0x2838000000000000, 0x5070000000000000, 0xA0E0000000000000, 0x40C0000000000000,\n"
    "};\n\n");

    std::printf("template <>\nconstexpr Bitboard move_lookup_table<Piece::rook>[64][4096] = {\n");
    for (int i = 0; i < 64; ++i) {
        std::printf("    {");

        for (int j = 0; j < 4096; ++j) {
            if (j % 4 == 0) std::printf("\n        ");
            std::printf("0x%016lx, ", move_lookup_table<Piece::rook>[i][j]);
        }

        std::printf("\n    },\n");
    }
    std::printf("};\n\n");

    std::printf("template <>\nconstexpr Bitboard move_lookup_table<Piece::bishop>[64][512] = {\n");
    for (int i = 0; i < 64; ++i) {
        std::printf("    {");

        for (int j = 0; j < 512; ++j) {
            if (j % 4 == 0) std::printf("\n        ");
            std::printf("0x%016lx, ", move_lookup_table<Piece::bishop>[i][j]);
        }

        std::printf("\n    },\n");
    }
    std::printf("};\n\n");
}

int main()
{
    generate_magic_bitboards_and_move_lookup_tables<Piece::rook>();
    generate_magic_bitboards_and_move_lookup_tables<Piece::bishop>();

    std::printf("#pragma once\n\n");

    std::printf("#include \"chess/misc.h\"\n");
    std::printf("#include \"chess/bitboard.h\"\n\n");

    std::printf("namespace Chess {\n\n");

    print_attack_tables();
    print_magic_tables();
    print_move_tables();

    std::printf("}  // namespace Chess\n\n");
}

