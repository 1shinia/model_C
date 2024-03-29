﻿#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "Matrix.h"

#define  FALSE 0
#define  TRUE  1


/*
   Print an Iliffe matrix out to stdout.
   打印矩阵
*/
void      print_mat (dmat mat)
{
    int       i,
              j;

    fprintf (stdout, "         ");
    for (i = mat.lb2; i <= mat.ub2; i++)
        fprintf (stdout, "  %7d", i);
    fprintf (stdout, "\n");

    for (j = mat.lb1; j <= mat.ub1; j++) {
        fprintf (stdout, "  %7d", j);
        for (i = mat.lb2; i <= mat.ub2; i++)
            fprintf (stdout, "  %7.2lf", mat.el[j][i]);
        fprintf (stdout, "\n");
    }
}


/*
    Allocates and initializes memory for a double precision Iliffe matrix.
    为矩阵分配和初始化内存
    @param rs rows start 行的开始位置
    @param re rows end   行的结束位置
    @param cs cols start 列的开始位置
    @param ce cols end   列的结束位置
    @param *error        函数是否错误
*/
dmat      newdmat (int rs, int re, int cs, int ce, int *error)
{
    double   *p,
            **b;

    int       r,
              rows,
              cols;

    dmat      matrix;

    memset(&matrix, 0, sizeof(matrix));

    //判断矩阵行列是否大于零
    rows = re - rs + 1;
    cols = ce - cs + 1;
    if (rows <= 0 || cols <= 0) {
        errno = EDOM;
        *error = -1;
        return (matrix);
    }

    /*
     fill in the bounds for this matrix
     设置矩阵的边界
    */
    matrix.lb1 = rs;
    matrix.ub1 = re;
    matrix.lb2 = cs;
    matrix.ub2 = ce;

    /*
     allocate memory for the row pointers
     为行指针分配内存
    */
    b = (double **) calloc ((unsigned int) rows, (unsigned int) sizeof (double *));
    if (b == 0) {
        errno = ENOMEM;
        *error = -1;
        return (matrix);
    }

    /* adjust for non-zero lower index bounds */
    matrix.el = b -= rs;

    /*
     allocate memory for storing the actual matrix
     分配一个矩阵大小的内存
    */
    p = (double *) calloc ((unsigned int) rows * cols, (unsigned int) sizeof (double));
    if (p == 0) {
        errno = ENOMEM;
        *error = -1;
        return (matrix);
    }

    /* keep a reminder where the block is actually located */
    matrix.mat_sto = (char *) p;

    /* adjust for non-zero lower index bounds */
    p -= cs;

    /*
     fabricate row pointers into the matrix
     将行指针指向内存中对应的每一行地址
    */
    for (r = rs; r <= re; r++) {
        b[r] = p;
        p += cols;
    }

    *error = 0;
    return (matrix);
}


/*
    Perform the matrix multiplication   c = a*b   where
    a, b, and c are dynamic Iliffe matrices.

    The matrix dimensions must be commensurate.  This means that
    c and a must have the same number of rows; c and b must have
    the same number of columns; and a must have the same  number
    of columns as b has rows.  The actual index origins are
    immaterial.

    If c is the same matrix as a or b, the result will be
    correct at the expense of more time.
    矩阵相乘 c = a * b
    @param a 源矩阵
    @param b 源矩阵
    @param c 结果矩阵
*/
int       matmul (dmat a, dmat b, dmat c)
{
    int       i,
              j,
              k,
              broff,
              croff,
              ccoff,		/* coordinate origin offsets */
              mem_alloced,
              error;

    double    t;

    dmat      d;		/* temporary workspace matrix */

    //矩阵相乘的前提
    //矩阵a的列等于矩阵b的行
    //结果矩阵c的行等于矩阵a的行，结果矩阵c的列等于矩阵a的列
    if (a.ub2 - a.lb2 != b.ub1 - b.lb1  //a的列 = b的行
        || a.ub1 - a.lb1 != c.ub1 - c.lb1 //a的行 = c的行
        || b.ub2 - b.lb2 != c.ub2 - c.lb2) { //b的列 = c的列
        errno = EDOM;
        return (-1);
    }

    if (a.mat_sto != c.mat_sto && b.mat_sto != c.mat_sto) {
        d = c;
        mem_alloced = FALSE;
    } else {
        d = newdmat (c.lb1, c.ub1, c.lb2, c.ub2, &error);
        if (error) {
            fprintf (stderr, "Matmul: out of storage.\n");
            errno = ENOMEM;
            return (-1);
        }
        mem_alloced = TRUE;
    }

    broff = b.lb1 - a.lb2;	/* B's row offset from A */
    croff = c.lb1 - a.lb1;	/* C's row offset from A */
    ccoff = c.lb2 - b.lb2;	/* C's column offset from B */
    for (i = a.lb1; i <= a.ub1; i++)
        for (j = b.lb2; j <= b.ub2; j++) {
            t = 0.0;
            for (k = a.lb2; k <= a.ub2; k++)
                t += a.el[i][k] * b.el[k + broff][j];
            d.el[i + croff][j + ccoff] = t;
        }

    if (mem_alloced) {
        for (i = c.lb1; i <= c.ub1; i++)
            for (j = c.lb2; j <= c.ub2; j++)
                c.el[i][j] = d.el[i][j];
        freemat (d);
    }

    return (0);
}


/*
    Copy dynamic Iliffe matrix A into RSLT.
    Bounds need not be the same but the dimensions must be.
    矩阵复制
    @param A 源矩阵
    @param RSLT 复制矩阵
*/
int       matcopy (dmat A, dmat RSLT)
{
    int       i,
              j,
              rowsize,
              colsize;

    double  **a = A.el,
            **rslt = RSLT.el;

    rowsize = A.ub1 - A.lb1;
    colsize = A.ub2 - A.lb2;

    //判断两个矩阵的行列是否相等
    if (rowsize != RSLT.ub1 - RSLT.lb1 || colsize != RSLT.ub2 - RSLT.lb2) {
        errno = EDOM;
        return (-1);
    }

    //复制矩阵内容
    for (i = 0; i <= rowsize; i++)
        for (j = 0; j <= colsize; j++)
            rslt[RSLT.lb1 + i][RSLT.lb2 + j] = a[A.lb1 + i][A.lb2 + j];

    return (0);
}


/*
    Generate the transpose of a dynamic Iliffe matrix.
    矩阵转置
    @param A 源矩阵
    @param ATrans 转置矩阵
*/
int       transpose (dmat A, dmat ATrans)
{
    int       i,
              j,
              rowsize,
              colsize,
              error;

    double  **a = A.el,
            **atrans = ATrans.el,
              temp;

    dmat      TMP;

    rowsize = A.ub1 - A.lb1;
    colsize = A.ub2 - A.lb2;
    //判断原矩阵的行列是否大于零、以及原矩阵的行等于目的矩阵的列，原矩阵的列等于目的矩阵的行
    if (rowsize < 0 || rowsize != ATrans.ub2 - ATrans.lb2 ||
        colsize < 0 || colsize != ATrans.ub1 - ATrans.lb1) {
        errno = EDOM;
        return (-1);
    }

    if (A.mat_sto == ATrans.mat_sto
        && A.lb1 == ATrans.lb1
        && A.lb2 == ATrans.lb2) {
        for (i = 0; i <= rowsize; i++)
            for (j = i + 1; j <= colsize; j++) {
                temp = a[A.lb1 + i][A.lb2 + j];
                atrans[A.lb1 + i][A.lb2 + j] = a[A.lb1 + j][A.lb2 + i];
                atrans[A.lb1 + j][A.lb2 + i] = temp;
            }
    } else if (A.mat_sto == ATrans.mat_sto) {
        TMP = newdmat (ATrans.lb1, ATrans.ub1, ATrans.lb2, ATrans.ub2, &error);
        if (error)
            return (-2);

        for (i = 0; i <= rowsize; i++)
            for (j = 0; j <= colsize; j++) {
                TMP.el[ATrans.lb1 + j][ATrans.lb2 + i] =
                 a[A.lb1 + i][A.lb2 + j];
            }

        matcopy (TMP, ATrans);
        freemat (TMP);
    } else {
        for (i = 0; i <= rowsize; i++)
            for (j = 0; j <= colsize; j++)
                atrans[ATrans.lb1 + j][ATrans.lb2 + i]
                 = a[A.lb1 + i][A.lb2 + j];
    }

    return (0);
}


/*
   In-place Iliffe matrix inversion using full pivoting.
   The standard Gauss-Jordan method is used.
   The return value is the determinant.
   矩阵原地旋转
*/

#define PERMBUFSIZE 100		/* Mat bigger than this requires calling calloc. */

double    matinvert (dmat a)
{
    int       i,
              j,
              k,
             *l,
             *m,
              permbuf[2 * PERMBUFSIZE],
              mem_alloced;

    double    det,
              biga,
              hold;

    if (a.lb1 != a.lb2 || a.ub1 != a.ub2) {
        errno = EDOM;
        return (0.0);
    }

    /* Allocate permutation vectors for l and m, with the same origin as the matrix. */
    if (a.ub1 - a.lb1 + 1 <= PERMBUFSIZE) {
        l = permbuf;
        mem_alloced = FALSE;
    } else {
        l = (int *) calloc ((unsigned int) 2 * (a.ub1 - a.lb1 + 1), (unsigned int) sizeof (int));
        if (l == 0) {
            fprintf (stderr, "matinvert: can't get working storage.\n");
            errno = ENOMEM;
            return (0.0);
        }
        mem_alloced = TRUE;
    }

    l -= a.lb1;
    m = l + (a.ub1 - a.lb1 + 1);

    det = 1.0;
    for (k = a.lb1; k <= a.ub1; k++) {
        l[k] = k;
        m[k] = k;
        biga = a.el[k][k];

        /* Find the biggest element in the submatrix */
        for (i = k; i <= a.ub1; i++)
            for (j = k; j <= a.ub2; j++)
                if (fabs (a.el[i][j]) > fabs (biga)) {
                    biga = a.el[i][j];
                    l[k] = i;
                    m[k] = j;
                }

        /* Interchange rows */
        i = l[k];
        if (i > k)
            for (j = a.lb2; j <= a.ub2; j++) {
                hold = -a.el[k][j];
                a.el[k][j] = a.el[i][j];
                a.el[i][j] = hold;
            }

        /* Interchange columns */
        j = m[k];
        if (j > k)
            for (i = a.lb1; i <= a.ub1; i++) {
                hold = -a.el[i][k];
                a.el[i][k] = a.el[i][j];
                a.el[i][j] = hold;
            }

        /* Divide column by minus pivot (value of pivot element is contained in biga). */
        if (biga == 0.0)
            return (0.0);

        for (i = a.lb1; i <= a.ub1; i++)
            if (i != k)
                a.el[i][k] /= -biga;

        /* Reduce matrix */
        for (i = a.lb1; i <= a.ub1; i++)
            if (i != k) {
                hold = a.el[i][k];
                for (j = a.lb2; j <= a.ub2; j++)
                    if (j != k)
                        a.el[i][j] += hold * a.el[k][j];
            }

        /* Divide row by pivot */
        for (j = a.lb2; j <= a.ub2; j++)
            if (j != k)
                a.el[k][j] /= biga;

        det *= biga;		/* Product of pivots */
        a.el[k][k] = 1.0 / biga;
    }				/* K loop */

    /* Final row & column interchanges */
    for (k = a.ub1 - 1; k >= a.lb1; k--) {
        i = l[k];
        if (i > k)
            for (j = a.lb2; j <= a.ub2; j++) {
                hold = a.el[j][k];
                a.el[j][k] = -a.el[j][i];
                a.el[j][i] = hold;
            }

        j = m[k];
        if (j > k)
            for (i = a.lb1; i <= a.ub1; i++) {
                hold = a.el[k][i];
                a.el[k][i] = -a.el[j][i];
                a.el[j][i] = hold;
            }
    }

    if (mem_alloced)
        free (l + a.lb1);

    return det;
}


/*
   Solve the overconstrained linear system   Ma = b   using a least
   squares error (pseudo inverse) approach.
   根据 Ma = b，已知 M、b，求解矩阵a
*/
int       solveSystem (dmat M, dmat a,dmat b)
{
    dmat      Mt,
              MtM,
              Mdag;

    if ((M.ub1 - M.lb1) < (M.ub2 - M.lb2)) {
        fprintf (stderr, "solve_system: matrix M has more columns than rows\n");
        return (-1);
    }

    Mt = newdmat (M.lb2, M.ub2, M.lb1, M.ub1, &errno);
    if (errno) {
        fprintf (stderr, "solve_system: unable to allocate matrix M_transpose\n");
        return (-1);
    }

    transpose (M, Mt);
    if (errno) {
        fprintf (stderr, "solve_system: unable to transpose matrix M\n");
        return (-1);
    }

    MtM = newdmat (M.lb2, M.ub2, M.lb2, M.ub2, &errno);
    if (errno) {
        fprintf (stderr, "solve_system: unable to allocate matrix M_transpose_M\n");
        return (-1);
    }

    matmul (Mt, M, MtM);
    if (errno) {
        fprintf (stderr, "solve_system: unable to compute matrix product of M_transpose and M\n");
        return (-1);
    }

    if (fabs (matinvert (MtM)) < 0.001) {
        fprintf (stderr, "solve_system: determinant of matrix M_transpose_M is too small\n");
        return (-1);
    }

    if (errno) {
        fprintf (stderr, "solve_system: error during matrix inversion\n");
        return (-1);
    }

    Mdag = newdmat (M.lb2, M.ub2, M.lb1, M.ub1, &errno);
    if (errno) {
        fprintf (stderr, "solve_system: unable to allocate matrix M_diag\n");
        return (-1);
    }

    matmul (MtM, Mt, Mdag);
    if (errno) {
        fprintf (stderr, "solve_system: unable to compute matrix product of M_transpose_M and M_transpose\n");
        return (-1);
    }

    matmul (Mdag, b, a);
    if (errno) {
        fprintf (stderr, "solve_system: unable to compute matrix product of M_diag and b\n");
        return (-1);
    }

    freemat (Mt);
    freemat (MtM);
    freemat (Mdag);

    return 0;
}


/********************************************************************/
// 根据给定的点集利用最小二乘法求解刚体变换矩阵（旋转+平移)
//    xd          r11   r12    tx         xs
//    yd     = [  r21   r22    ty   ] *   ys
//     1           0     0      1          1
/********************************************************************/
bool solveTransMatrix(float *xs, float *ys, float *xd, float *yd, int count, float *para, float *meanErr)
{
        dmat M, a, b;

        M = newdmat(0, (count - 1), 0, 2, &errno);
        if(errno)
        {
                fprintf(stderr, "unable to allocate matrix M\n");
                return false;
        }

        a = newdmat(0, 2, 0, 0, &errno);
        if (errno)
        {
                fprintf(stderr, "unable to allocate vector a\n");
                return false;
        }

        b = newdmat(0, (count - 1), 0, 0, &errno);
        if(errno)
        {
                fprintf(stderr, "unable to allocate vector b\n");
                return false;
        }

        //求解转换矩阵中的r11、r12、tx
        /********************************************************************/
        // 根据给定的原坐标xs，ys 生成矩阵M
        //         xs1   ys1    1
        //  M = [  xs2   ys2    1  ]
        //         xs3   ys3    1
        // 根据目的坐标的xd 生成目标矩阵b
        //         xd1
        //  b = [  xd2  ]
        //         xd3
        /********************************************************************/
        for(int i = 0; i < count; i++)
        {
                M.el[i][0] = xs[i];
                M.el[i][1] = ys[i];
                M.el[i][2] = 1;
                b.el[i][0] = xd[i];
        }

        //根据 Ma = b，求出转变矩阵a
        if(solveSystem(M, a, b))
        {
                freemat(M);
                freemat(a);
                freemat(b);
                return false;
        }

        para[0] = (float)a.el[0][0];
        para[1] = (float)a.el[1][0];
        para[2] = (float)a.el[2][0];

        //求解转换矩阵中的r21、r22、ty
        /********************************************************************/
        // 根据目的坐标的yd 生成目标矩阵b
        //         yd1
        //  b = [  yd2  ]
        //         yd3
        /********************************************************************/
        for(int i = 0; i < count; i++)
        {
                b.el[i][0] = yd[i];
        }

        //根据 Ma = b，求出转变矩阵a
        if(solveSystem(M, a, b))
        {
                freemat(M);
                freemat(a);
                freemat(b);
                return false;
        }

        para[3] = (float)a.el[0][0];
        para[4] = (float)a.el[1][0];
        para[5] = (float)a.el[2][0];

        freemat(M);
        freemat(a);
        freemat(b);

        para[6] = 0;
        para[7] = 0;
        para[8] = 1;

        //计算拟合误差
        //利用平均误差平方和求误差
        double totalErr = 0.0;
        for(int i = 0; i < count; i++)
        {
                double x = para[0] * xs[i] + para[1] * ys[i] + para[2];
                double y = para[3] * xs[i] + para[4] * ys[i] + para[5];
                double err = sqrt((x - xd[i]) * (x - xd[i]) + (y - yd[i]) * (y - yd[i])) ;
                totalErr += err;
        }

        *meanErr = (float)totalErr / count;
        return true;
}
