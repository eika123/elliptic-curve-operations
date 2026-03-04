#include <stdint.h>
#include <stdio.h>

typedef uint32_t u32;

typedef struct Point {
    u32 x;
    u32 y;
} point_t;


/** Elliptic curve y = x^3 + bx + c over Z_q */
struct Curve {
    u32 b;
    u32 c;
    u32 q;
};

/** Find inverse of a number x modulo n 
    The number n must be a prime, or this function will return gibberish
*/
u32 modulo_multiplicative_inverse(u32 x, u32 modulo) {
    u32 res;
    for (u32 inv = 1; inv < modulo; inv++) {
        res = (x * inv) % modulo;
        if (res == 1) {
            return inv;
        }
    }
}

/** Find the negative value -x */
unsigned int additive_inverse(u32 x, u32 modulo) {
    u32 negative;
    // represent x as a number < modulo in case it was larger
    x = x % modulo;

    // this value added to $x$ always gives 0 (modulo the modulo)
    return (modulo - x);
}


point_t ec_add(point_t *P1, point_t *P2, struct Curve *crv) {
    u32 x1 = P1->x, x2 = P2->x, y1 = P1->y, y2 = P2->y, modulo = crv->q;
    u32 m, x3, y3, nominator, denom, inv;
    if ((x1 == x2) && (y1 == y2)) {
        // equal points, P1 = P2
        u32 b = crv -> b;
        nominator = (3*x1*x1 + b) % modulo;
        denom = 2 * y1 % modulo;
    } else {
        // unequal points, P1 != P2
        //printf("adding distinct points P1=(%u, %u) and P2=(%u, %u)\n", P1->x, P1->y, P2->x, P2->y);
        nominator = (y2 + additive_inverse(y1, modulo));
        denom = (x2 + additive_inverse(x1, modulo));

    }
    inv = modulo_multiplicative_inverse(denom, modulo);

    //  printf("found nominator = %u\n", nominator);
    //  printf("found denominator = %u\n", denom);
    //  printf("inverse of denominator: %u\n", inv);

    m = (nominator * inv) % modulo;
    x3 = (m*m  + additive_inverse(x1, modulo) + additive_inverse(x2, modulo)) % modulo;
    y3 = (m*( x1 + additive_inverse(x3, modulo) ) + additive_inverse(y1, modulo)) % modulo;

    point_t P3 = {x3, y3};
    return P3;
}

/** Find an integer r such that r*P = T, if such an r exists 
    This algorithm iterates a maximum number of times provided by max_iter
*/
u32 iterate_points_to_target(point_t *Start, point_t *Target, struct Curve *crv, u32 max_iter) {
    point_t P = ec_add(Start, Start, crv);
    if (P.x == Target->x && P.y == Target->y) {
        return 1;
    }

    for (u32 i=0; i < max_iter; i++) {
        P = ec_add(&P, Start, crv);

        if (P.x == Target->x && P.y == Target->y) {
            return i + 2;
        }
    }
}



int main() {
    struct Curve crv = {2, 3, 19};

    // a)
    point_t P1 = {1, 14};
    point_t P2 = {9, 16};
    point_t P3 = ec_add(&P1, &P2, &crv);
    printf("P3 = {%u, %u}\n", P3.x, P3.y);


    // c)
    point_t Start = {14, 1};
    point_t Target = {1, 14};
    u32 r = iterate_points_to_target(&Start, &Target, &crv, 200);
    printf("found r: %u\n", r);

    return 0;
}