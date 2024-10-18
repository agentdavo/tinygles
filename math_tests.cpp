/* Some simple mathematical functions. Don't look for some logic in
   the function names :-) */

#include "math_tests.hpp"
#include "matrix_utils.hpp"
#include "api.hpp"

namespace fp {


namespace {

bool matricesMatches(M4* m1, M4* m2)
{
    M4* r = getM4(1, 1, 1, 1,
                  1, 1, 1, 1,
                  1, 1, 1, 1,
                  1, 1, 1, 1);

    bool result = true;
    for (int i=0; i<4; ++i) {
        for (int j=0; j<4; ++j) {
            if (m1->m[i][j] != m2->m[i][j]) {
                r->m[i][j] = 0;
                result = false;
            }
        }
    }
    if (!result) {
        std::cout<<"expected matrix"<<std::endl;
        gl_print_matrix4(m1);
        std::cout<<"actual matrix"<<std::endl;
        gl_print_matrix4(m2);
        std::cout<<"diff matrix"<<std::endl;
        gl_print_matrix4(r);
    }

    return result;
}


} // namespace

bool check_float_matches(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) {
    if ((a-b)<epsilon) {
        return true;
    }
    return false;
}

bool test_fp_addition(tGLfixed a, tGLfixed b, const tGLfixed& expected)
{
    tGLfixed r = a + b;
    if (!check_float_matches(float(r), float(expected))) {
        std::cout<<"r=a+b="<<a<<"+"<<b<<"="<<r<<std::endl;
        return false;
    }
    return true;
}
bool test_fp_multiplication(tGLfixed a, tGLfixed b, const tGLfixed& expected)
{
    tGLfixed r = a * b;
    if (!check_float_matches(float(r), float(expected))) {
        std::cout<<"c=a*b="<<a<<"*"<<b<<"="<<r<<std::endl;
        return false;
    }
    return true;
}
bool test_fp_division(tGLfixed a, tGLfixed b, const tGLfixed& expected, float epsilon = std::numeric_limits<float>::epsilon())
{
    tGLfixed r = a / b;
    if (!check_float_matches(float(r), float(expected), epsilon)) {
        std::cout<<"c=a/b="<<a<<"/"<<b<<"="<<r<<std::endl;
        return false;
    }
    return true;
}
//bool test_fp_multiplication2(tGLfixed a, tGLfixed b, const tGLfixed& expected)
//{
//    tGLfixed r = a;
//    r *= b; // check overloaded operator *=
//    if (r != expected) {
//        std::cout<<"a*=b ="<<a<<"*="<<b<<"="<<r<<std::endl;
//        return false;
//    }
//    return true;
//}

void runtest_matrix_conversion1()
{
    tGLfixed r[] = {
        1, 0, 0, 11,
        0, 2, 0, 12,
        0, 0, 3, 13,
        0, 0, 0, 1,
    };
    M4* m4 = (M4*)&r;
    int offset = 0;
    assert(r[offset+0] == m4->m[0][0]);
    assert(r[offset+1] == m4->m[0][1]);
    assert(r[offset+2] == m4->m[0][2]);
    assert(r[offset+3] == m4->m[0][3]);
    offset += 4;
    assert(r[offset+0] == m4->m[1][0]);
    assert(r[offset+1] == m4->m[1][1]);
    assert(r[offset+2] == m4->m[1][2]);
    assert(r[offset+3] == m4->m[1][3]);
    offset += 4;
    assert(r[offset+0] == m4->m[2][0]);
    assert(r[offset+1] == m4->m[2][1]);
    assert(r[offset+2] == m4->m[2][2]);
    assert(r[offset+3] == m4->m[2][3]);
    offset += 4;
    assert(r[offset+0] == m4->m[3][0]);
    assert(r[offset+1] == m4->m[3][1]);
    assert(r[offset+2] == m4->m[3][2]);
    assert(r[offset+3] == m4->m[3][3]);
}

void runtest_matrix_conversion2()
{
    M4* m = new M4;
    m->m[0][0] = 1;
    m->m[0][1] = 0;
    m->m[0][2] = 0;
    m->m[0][3] = 11;

    m->m[1][0] = 0;
    m->m[1][1] = 2;
    m->m[1][2] = 0;
    m->m[1][3] = 12;

    m->m[2][0] = 0;
    m->m[2][1] = 0;
    m->m[2][2] = 3;
    m->m[2][3] = 13;

    m->m[3][0] = 0;
    m->m[3][1] = 0;
    m->m[3][2] = 0;
    m->m[3][3] = 1;

    tGLfixed* ar = &m->m[0][0];

    tGLfixed ar_expected[] = {
        1, 0, 0, 11,
        0, 2, 0, 12,
        0, 0, 3, 13,
        0, 0, 0, 1,
    };

    int offset = 0;
    assert(ar[offset+0] == ar_expected[offset+0]);
    assert(ar[offset+1] == ar_expected[offset+1]);
    assert(ar[offset+2] == ar_expected[offset+2]);
    assert(ar[offset+3] == ar_expected[offset+3]);
    offset += 4;
    assert(ar[offset+0] == ar_expected[offset+0]);
    assert(ar[offset+1] == ar_expected[offset+1]);
    assert(ar[offset+2] == ar_expected[offset+2]);
    assert(ar[offset+3] == ar_expected[offset+3]);
    offset += 4;
    assert(ar[offset+0] == ar_expected[offset+0]);
    assert(ar[offset+1] == ar_expected[offset+1]);
    assert(ar[offset+2] == ar_expected[offset+2]);
    assert(ar[offset+3] == ar_expected[offset+3]);
    offset += 4;
    assert(ar[offset+0] == ar_expected[offset+0]);
    assert(ar[offset+1] == ar_expected[offset+1]);
    assert(ar[offset+2] == ar_expected[offset+2]);
    assert(ar[offset+3] == ar_expected[offset+3]);
}

void runtest_matrix_multiplication()
{
    M4* i1 = getIdentity();
    M4* i2 = getIdentity();

    M4* c = getM4(1,2,3,4,
                  5,6,7,8,
                  8,7,6,5,
                  4,3,2,1);

    M4* exp = getM4(1,2,3,4,
                    5,6,7,8,
                    8,7,6,5,
                    4,3,2,1);

    gl_M4_MulLeft(i1, i2);
    assert(matricesMatches(i1, i2));

    gl_M4_MulLeft(c, i2);
    assert(matricesMatches(c, exp));
}

void runtest_setup_projection_orthox()
{
//    fp::glMatrixMode(GL_PROJECTION);
//    fp::glLoadIdentity();
//    tGLfixed r = 20;
//    fp::glOrthox(0, r, 0, r, -10, 10);
}

void runtest_setup_projection_flustrumx()
{
    int height = 300;

    fp::glMatrixMode(GL_PROJECTION);
    fp::glLoadIdentity();
    fp::glFrustumx( -1.0, 1.0, -height, height, 5.0, 60.0 );

    M4* expectation = getM4(5.000000, 0.000000, 0.000000, 0.000000,
                            0.000000, 0.016667, 0.000000, 0.000000,
                            0.000000, 0.000000, -1.181818, -10.909091,
                            0.000000, 0.000000, -1.000000, 0.000000);

//  tGLfixed buf[16];
//  fp::glGetFloatv(GL_PROJECTION_MATRIX, buf); // the function swap matrix index [3][2] and [2][3]
//  M4* actual = getM4(buf);

    GLContext *c = gl_get_context();
    M4* actual = c->matrix.stack_ptr[c->matrix.mode];
    assert(matricesMatches(expectation, actual));
}

void runtests()
{
    assert(test_fp_addition(2.0f, 1.0f, 3.0f));
    assert(test_fp_addition(9999.0f, 0.01f, 9999.01f));

    assert(test_fp_multiplication(2.0f, 1.0f, 2.0f));
    assert(test_fp_multiplication(9999.0f, 0.1f, 999.9f));

    assert(test_fp_division(999.0f, 0.1f, 9990.0f, 0.92f));
    assert(test_fp_division(9999.9f, 3.0f, 3333.3f));

    //    assert(test_fp_multiplication2(3.0f, 4.0f, 12.0f));

    runtest_matrix_conversion1();
    runtest_matrix_conversion2();

    runtest_matrix_multiplication();
    runtest_setup_projection_flustrumx();

    assert(false && "end tests");
}


} // namespace fp
