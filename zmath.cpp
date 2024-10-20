/* Some simple mathematical functions. Don't look for some logic in
   the function names :-) */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "zmath.hpp"
#include "zgl.hpp"

namespace fp
{

    void gl_M4_Id(M4 *a)
    {
        // Unrolling the loops for better performance
        a->m[0][0] = 1.0;
        a->m[0][1] = 0.0;
        a->m[0][2] = 0.0;
        a->m[0][3] = 0.0;
        a->m[1][0] = 0.0;
        a->m[1][1] = 1.0;
        a->m[1][2] = 0.0;
        a->m[1][3] = 0.0;
        a->m[2][0] = 0.0;
        a->m[2][1] = 0.0;
        a->m[2][2] = 1.0;
        a->m[2][3] = 0.0;
        a->m[3][0] = 0.0;
        a->m[3][1] = 0.0;
        a->m[3][2] = 0.0;
        a->m[3][3] = 1.0;
    }

    int gl_M4_IsId(M4 *a)
    {
        // Unrolling loops for checking identity matrix
        return (a->m[0][0] == 1.0 && a->m[0][1] == 0.0 && a->m[0][2] == 0.0 && a->m[0][3] == 0.0 &&
                a->m[1][0] == 0.0 && a->m[1][1] == 1.0 && a->m[1][2] == 0.0 && a->m[1][3] == 0.0 &&
                a->m[2][0] == 0.0 && a->m[2][1] == 0.0 && a->m[2][2] == 1.0 && a->m[2][3] == 0.0 &&
                a->m[3][0] == 0.0 && a->m[3][1] == 0.0 && a->m[3][2] == 0.0 && a->m[3][3] == 1.0);
    }

    void gl_M4_Mul(M4 *c, M4 *a, M4 *b)
    {
        // Unrolling the inner loops to improve performance
        for (int i = 0; i < 4; i++)
        {
            c->m[i][0] = a->m[i][0] * b->m[0][0] + a->m[i][1] * b->m[1][0] + a->m[i][2] * b->m[2][0] + a->m[i][3] * b->m[3][0];
            c->m[i][1] = a->m[i][0] * b->m[0][1] + a->m[i][1] * b->m[1][1] + a->m[i][2] * b->m[2][1] + a->m[i][3] * b->m[3][1];
            c->m[i][2] = a->m[i][0] * b->m[0][2] + a->m[i][1] * b->m[1][2] + a->m[i][2] * b->m[2][2] + a->m[i][3] * b->m[3][2];
            c->m[i][3] = a->m[i][0] * b->m[0][3] + a->m[i][1] * b->m[1][3] + a->m[i][2] * b->m[2][3] + a->m[i][3] * b->m[3][3];
        }
    }

    void gl_M4_MulLeft(M4 *c, M4 *b)
    {
        M4 a = *c;
        // Unroll loop for matrix multiplication
        for (int i = 0; i < 4; i++)
        {
            c->m[i][0] = a.m[i][0] * b->m[0][0] + a.m[i][1] * b->m[1][0] + a.m[i][2] * b->m[2][0] + a.m[i][3] * b->m[3][0];
            c->m[i][1] = a.m[i][0] * b->m[0][1] + a.m[i][1] * b->m[1][1] + a.m[i][2] * b->m[2][1] + a.m[i][3] * b->m[3][1];
            c->m[i][2] = a.m[i][0] * b->m[0][2] + a.m[i][1] * b->m[1][2] + a.m[i][2] * b->m[2][2] + a.m[i][3] * b->m[3][2];
            c->m[i][3] = a.m[i][0] * b->m[0][3] + a.m[i][1] * b->m[1][3] + a.m[i][2] * b->m[2][3] + a.m[i][3] * b->m[3][3];
        }
    }

    void gl_M4_Move(M4 *a, M4 *b)
    {
        // Use direct assignment for aligned structures
        *a = *b;
    }

    void gl_MoveV3(V3 *a, V3 *b)
    {
        *a = *b; // Direct assignment for aligned vectors
    }

    void gl_MulM4V3(V3 *a, M4 *b, V3 *c)
    {
        // Matrix-vector multiplication with unrolled loops
        a->X = b->m[0][0] * c->X + b->m[0][1] * c->Y + b->m[0][2] * c->Z + b->m[0][3];
        a->Y = b->m[1][0] * c->X + b->m[1][1] * c->Y + b->m[1][2] * c->Z + b->m[1][3];
        a->Z = b->m[2][0] * c->X + b->m[2][1] * c->Y + b->m[2][2] * c->Z + b->m[2][3];
    }

    void gl_MulM3V3(V3 *a, M4 *b, V3 *c)
    {
        // Unrolling the loop for 3x3 matrix-vector multiplication
        a->X = b->m[0][0] * c->X + b->m[0][1] * c->Y + b->m[0][2] * c->Z;
        a->Y = b->m[1][0] * c->X + b->m[1][1] * c->Y + b->m[1][2] * c->Z;
        a->Z = b->m[2][0] * c->X + b->m[2][1] * c->Y + b->m[2][2] * c->Z;
    }

    void gl_M4_MulV4(V4 *a, M4 *b, V4 *c)
    {
        // Matrix-vector multiplication (4x4)
        a->X = b->m[0][0] * c->X + b->m[0][1] * c->Y + b->m[0][2] * c->Z + b->m[0][3] * c->W;
        a->Y = b->m[1][0] * c->X + b->m[1][1] * c->Y + b->m[1][2] * c->Z + b->m[1][3] * c->W;
        a->Z = b->m[2][0] * c->X + b->m[2][1] * c->Y + b->m[2][2] * c->Z + b->m[2][3] * c->W;
        a->W = b->m[3][0] * c->X + b->m[3][1] * c->Y + b->m[3][2] * c->Z + b->m[3][3] * c->W;
    }

    /* Transposition of a 4x4 matrix */
    void gl_M4_Transpose(M4 *a, M4 *b)
    {
        a->m[0][0] = b->m[0][0];
        a->m[0][1] = b->m[1][0];
        a->m[0][2] = b->m[2][0];
        a->m[0][3] = b->m[3][0];

        a->m[1][0] = b->m[0][1];
        a->m[1][1] = b->m[1][1];
        a->m[1][2] = b->m[2][1];
        a->m[1][3] = b->m[3][1];

        a->m[2][0] = b->m[0][2];
        a->m[2][1] = b->m[1][2];
        a->m[2][2] = b->m[2][2];
        a->m[2][3] = b->m[3][2];

        a->m[3][0] = b->m[0][3];
        a->m[3][1] = b->m[1][3];
        a->m[3][2] = b->m[2][3];
        a->m[3][3] = b->m[3][3];
    }

    /* Inversion of an orthogonal matrix of type Y=M.X+P */
    void gl_M4_InvOrtho(M4 *a, M4 b)
    {
        tGLfixed s;

        /* Copy the transpose of the upper 3x3 part */
        a->m[0][0] = b.m[0][0];
        a->m[0][1] = b.m[1][0];
        a->m[0][2] = b.m[2][0];

        a->m[1][0] = b.m[0][1];
        a->m[1][1] = b.m[1][1];
        a->m[1][2] = b.m[2][1];

        a->m[2][0] = b.m[0][2];
        a->m[2][1] = b.m[1][2];
        a->m[2][2] = b.m[2][2];

        /* Set the last row and column */
        a->m[3][0] = 0.0;
        a->m[3][1] = 0.0;
        a->m[3][2] = 0.0;
        a->m[3][3] = 1.0;

        /* Compute the translation part */
        for (int i = 0; i < 3; i++)
        {
            s = 0;
            for (int j = 0; j < 3; j++)
            {
                s -= b.m[j][i] * b.m[j][3];
            }
            a->m[i][3] = s;
        }
    }

    /* Inversion of a general nxn matrix (matrix is destroyed) */
    int Matrix_Inv(tGLfixed *r, tGLfixed *m, int n)
    {
        int i, k;
        tGLfixed max, tmp, t;

        /* Identity matrix in r */
        for (int i = 0; i < n * n; i++)
            r[i] = 0;
        for (int i = 0; i < n; i++)
            r[i * n + i] = 1;

        for (int j = 0; j < n; j++)
        {
            /* Find the pivot element */
            max = m[j * n + j];
            k = j;
            for (i = j + 1; i < n; i++)
            {
                if (fabs(m[i * n + j]) > fabs(max))
                {
                    k = i;
                    max = m[i * n + j];
                }
            }

            /* Non-invertible matrix */
            if (max == 0)
                return 1;

            /* Swap rows j and k */
            if (k != j)
            {
                for (int i = 0; i < n; i++)
                {
                    tmp = m[j * n + i];
                    m[j * n + i] = m[k * n + i];
                    m[k * n + i] = tmp;

                    tmp = r[j * n + i];
                    r[j * n + i] = r[k * n + i];
                    r[k * n + i] = tmp;
                }
            }

            /* Scale row j */
            max = 1 / max;
            for (int i = 0; i < n; i++)
            {
                m[j * n + i] *= max;
                r[j * n + i] *= max;
            }

            /* Subtract multiples of row j from other rows */
            for (int l = 0; l < n; l++)
            {
                if (l != j)
                {
                    t = m[l * n + j];
                    for (int i = 0; i < n; i++)
                    {
                        m[l * n + i] -= m[j * n + i] * t;
                        r[l * n + i] -= r[j * n + i] * t;
                    }
                }
            }
        }

        return 0;
    }

    /* Inversion of a 4x4 matrix */
    void gl_M4_Inv(M4 *a, M4 *b)
    {
        M4 tmp = *b; // No need for memcpy, direct assignment works here
        Matrix_Inv(&a->m[0][0], &tmp.m[0][0], 4);
    }

    /* Rotation of a 4x4 matrix around a given axis */
    void gl_M4_Rotate(M4 *a, tGLfixed t, int u)
    {
        tGLfixed s = sin(t), c = cos(t);
        int v = (u + 1) % 3;
        int w = (v + 1) % 3;

        gl_M4_Id(a); // Set identity matrix

        a->m[v][v] = c;
        a->m[v][w] = -s;
        a->m[w][v] = s;
        a->m[w][w] = c;
    }

    /* Inversion of a 3x3 matrix */
    void gl_M3_Inv(M3 *a, M3 *m)
    {
        tGLfixed det;

        /* Compute the determinant */
        det = m->m[0][0] * m->m[1][1] * m->m[2][2] - m->m[0][0] * m->m[1][2] * m->m[2][1] - m->m[1][0] * m->m[0][1] * m->m[2][2] + m->m[1][0] * m->m[0][2] * m->m[2][1] + m->m[2][0] * m->m[0][1] * m->m[1][2] - m->m[2][0] * m->m[0][2] * m->m[1][1];

        if (det == 0)
            return; // Handle non-invertible matrix

        /* Compute the inverse */
        tGLfixed inv_det = 1 / det;
        a->m[0][0] = (m->m[1][1] * m->m[2][2] - m->m[1][2] * m->m[2][1]) * inv_det;
        a->m[0][1] = -(m->m[0][1] * m->m[2][2] - m->m[0][2] * m->m[2][1]) * inv_det;
        a->m[0][2] = (m->m[0][1] * m->m[1][2] - m->m[0][2] * m->m[1][1]) * inv_det;

        a->m[1][0] = -(m->m[1][0] * m->m[2][2] - m->m[1][2] * m->m[2][0]) * inv_det;
        a->m[1][1] = (m->m[0][0] * m->m[2][2] - m->m[0][2] * m->m[2][0]) * inv_det;
        a->m[1][2] = -(m->m[0][0] * m->m[1][2] - m->m[0][2] * m->m[1][0]) * inv_det;

        a->m[2][0] = (m->m[1][0] * m->m[2][1] - m->m[1][1] * m->m[2][0]) * inv_det;
        a->m[2][1] = -(m->m[0][0] * m->m[2][1] - m->m[0][1] * m->m[2][0]) * inv_det;
        a->m[2][2] = (m->m[0][0] * m->m[1][1] - m->m[0][1] * m->m[1][0]) * inv_det;
    }

    /* Vector normalization */
    int gl_V3_Norm(V3 *a)
    {
        tGLfixed n = sqrt(a->X * a->X + a->Y * a->Y + a->Z * a->Z);
        if (n == 0)
            return 1;
        a->X = a->X / n; // Use direct division if operator/= is not supported
        a->Y = a->Y / n;
        a->Z = a->Z / n;
        return 0;
    }

    /* Create a new V3 vector */
    V3 gl_V3_New(tGLfixed x, tGLfixed y, tGLfixed z)
    {
        return {x, y, z}; // Direct initialization
    }

    /* Create a new V4 vector */
    V4 gl_V4_New(tGLfixed x, tGLfixed y, tGLfixed z, tGLfixed w)
    {
        return {x, y, z, w}; // Direct initialization
    }

    void V4_to_V3(V3 &v3, const V4 &v4) {
        v3.X = v4.X;
        v3.Y = v4.Y;
        v3.Z = v4.Z;
    }


} // namespace fp
