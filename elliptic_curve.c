#include <stdint.h>
#include <stdio.h>

typedef int32_t i32;

typedef struct Point {
    i32 x;
    i32 y;
} point_t;


/** Elliptic curve y = x^3 + bx + c over Z_q */
struct Curve {
    i32 b;
    i32 c;
    i32 q;
};

/** Find inverse of a number x modulo n 
    The number n must be a prime, or this function will return gibberish
*/
i32 modulo_multiplicative_inverse(i32 x, i32 modulo) {
    i32 res;
    for (i32 inv = 1; inv < modulo; inv++) {
        res = (x * inv) % modulo;
        if (res == 1) {
            return inv;
        }
    }
}

/** Find the negative value -x */
unsigned int additive_inverse(i32 x, i32 modulo) {
    i32 negative;
    // represent x as a number < modulo in case it was larger
    x = x % modulo;

    // this value added to $x$ always gives 0 (modulo the modulo)
    return (modulo - x);
}


point_t ec_add(point_t *P1, point_t *P2, struct Curve *crv) {
    i32 x1 = P1->x, x2 = P2->x, y1 = P1->y, y2 = P2->y, modulo = crv->q;
    i32 m, x3, y3, nominator, denom, inv;
    if ((x1 == x2) && (y1 == y2)) {
        // equal points, P1 = P2
        i32 b = crv -> b;
        nominator = (3*x1*x1 + b) % modulo;
        denom = 2 * y1 % modulo;
    } else {
        // unequal points, P1 != P2
        nominator = (y2 + additive_inverse(y1, modulo));
        denom = (x2 + additive_inverse(x1, modulo));
    }
    // check for zero denominator, in that case the result is the point at infinity.
    if (denom == 0) {
        point_t inf = {-1, -1}; // let this (-1, -1) indicate point at infinity.
        return inf;
    };
    inv = modulo_multiplicative_inverse(denom, modulo);
    m = (nominator * inv) % modulo;
    x3 = (m*m  + additive_inverse(x1, modulo) + additive_inverse(x2, modulo)) % modulo;
    y3 = (m*( x1 + additive_inverse(x3, modulo) ) + additive_inverse(y1, modulo)) % modulo;

    point_t P3 = {x3, y3};
    return P3;
}

/** Find an integer r such that r*P = T, if such an r exists 
    This algorithm iterates a maximum number of times provided by max_iter
*/
i32 iterate_points_to_target(point_t *Start, point_t *Target, struct Curve *crv, i32 max_iter) {
    point_t P = ec_add(Start, Start, crv);
    if (P.x == Target->x && P.y == Target->y) {
        return 1;
    }

    for (i32 i=0; i < max_iter; i++) {
        P = ec_add(&P, Start, crv);

        if (P.x == Target->x && P.y == Target->y) {
            return i + 2;
        }
    }
}


i32 iterate_until_point_at_infinity(point_t *Start, struct Curve *crv, i32 max_iter) {
    point_t result; 
    result = ec_add(Start, Start, crv);
    for (int s = 0; s < max_iter; s++) {
        result = ec_add(Start, &result, crv);
        if (result.y == 0) {
            return s;
        }
    }
}


int main() {
    struct Curve crv = {2, 3, 19};

    // a)
    point_t P1 = {1, 14};
    point_t P2 = {9, 16};
    point_t P3 = ec_add(&P1, &P2, &crv);
    printf("P3 = {%d, %d}\n", P3.x, P3.y);


    // c)
    point_t Start = {14, 1};
    point_t Target = {1, 14};
    i32 r = iterate_points_to_target(&Start, &Target, &crv, 200);
    printf("found r: %d\n", r);

    Start.x = 5; Start.y = 9;
    i32 s = iterate_until_point_at_infinity(&Start, &crv, 200);
    printf("found (s - 1) = %d \n", s);


    return 0;
}