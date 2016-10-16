//
// Created by Samoilov Sergei on 16.10.2016.
//

#include "Transform.h"

vector<int> & Transform::getRandomIndexes(int size, int count) {
    qsrand((uint)QTime::currentTime().msec());
    vector<int> result(count);
    label:
    for (int i = 0; i < count; ++i)
        result[i] = qrand() % size;
    for (int i = 0; i < count-1; ++i)
        for (int j = i+1; j < count; ++j)
            if (result[i] == result[j])
                goto label;
    return result;
}

array<double, 9> &Transform::getTransform(vector<array<int, 4>> matches) {
    array<double, 9> result;
    gsl_matrix * A = gsl_matrix_alloc(8, 9);
    gsl_matrix * ATA = (gsl_matrix_alloc(9, 9));
    gsl_matrix *U = gsl_matrix_alloc(9, 9);
    gsl_vector *D = gsl_vector_alloc(9);
    gsl_vector *work = gsl_vector_alloc(9);


    for (int i = 0; i < 4; i++) {
        auto match = matches[i];
        double row1[] {match[0], match[1], 1., 0, 0, 0, -match[2] * match[0], -match[2] * match[1], -match[2]};
        double row2[] {0, 0, 0, match[0], match[1], 1., -match[3] * match[0], -match[3] * match[1], -match[3]};
        for (int j = 0; j < 9; j++) {
            gsl_matrix_set(A, i * 2, j, row1[j]);
            gsl_matrix_set(A, i * 2 + 1, j, row2[j]);
        }
    }

    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A, A, 0., ATA);
    gsl_linalg_SV_decomp(ATA, U, D, work);
    for (int i = 0; i < 9; i++)
        result[i] = gsl_matrix_get(U, i, 8);

    gsl_matrix_free(A);
    gsl_matrix_free(ATA);
    gsl_matrix_free(U);
    gsl_vector_free(D);
    gsl_vector_free(work);

    return result;
}

array<double, 9> &Transform::ransac(vector<array<int, 4>> matches) {
    int bestInlinersCount = 0;
    array<double, 9> best;
    for (int iter = 0; iter < ITERATIONS_COUNT; ++iter) {
        vector<array<int, 4>> selected(4);
        auto indexes = getRandomIndexes(matches.size(), 4);
        for (int i = 0; i < 4; ++i)
            selected[i] = matches[indexes[i]];
        auto trans = getTransform(selected);
        int inlinersCount = 0;
        for (auto match : matches) {
            double k = match[0] * trans[6] + match[1] * trans[7] + trans[8];
            double dx = (match[0] * trans[0] + match[1] * trans[1] + trans[2]) / k - match[2];
            double dy = (match[0] * trans[3] + match[1] * trans[4] + trans[5]) / k - match[3];
            if (dx * dx + dy * dy <= INLINERS_THRESHOLD)
                inlinersCount++;
        }
        if (inlinersCount > bestInlinersCount) {
            bestInlinersCount = inlinersCount;
            best = trans;
        }
    }
    qDebug() << "matches.size(): " << matches.size();
    qDebug() << "bestInlinersCount: " << bestInlinersCount;
    return best;
}
