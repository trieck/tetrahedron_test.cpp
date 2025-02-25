// tetrahedron_test.cpp (by M. Q. Rieck)

// Note: This is test code for the results in my "tetrahedron and toroids" paper.

// Note: This C++ program uses passing-by-reference. It can be easily converted to a C
// program by altering this aspect of function call, and by changing the includes.

#define _USE_MATH_DEFINES

#include <cstdio>
#include <cmath>

static constexpr auto M = 1000; // how many (alpha, beta, gamma) points (M^3)?
static constexpr auto N = 80;   // how fine to subdivide the interval [0, pi]
static constexpr auto O = 1;    // set higher to avoid low "tilt planes"
static constexpr auto pi = M_PI;

// #define USE_NEAR_RULES

// The taus are "tilt angles" for three planes, each containing one of the sidelines of
// the triangle ABC. Dihedral angle formulas are used to find the "view angles", alpha, 
// beta and gamma, at the point of intersection of the three planes.
bool tilt_to_view_angles(double tau1, double tau2, double tau3, double cosA, double cosB,
                         double cosC, double& alpha, double& beta, double& gamma, int& rejected)
{
    double cos_tau1, cos_tau2, cos_tau3, sin_tau1, sin_tau2, sin_tau3;
    double cos_delta1, cos_delta2, cos_delta3, sin_delta1, sin_delta2, sin_delta3;

    cos_tau1 = cos(tau1), cos_tau2 = cos(tau2), cos_tau3 = cos(tau3);
    sin_tau1 = sin(tau1), sin_tau2 = sin(tau2), sin_tau3 = sin(tau3);
    cos_delta1 = sin_tau2 * sin_tau3 * cosA - cos_tau2 * cos_tau3;
    cos_delta2 = sin_tau3 * sin_tau1 * cosB - cos_tau3 * cos_tau1;
    cos_delta3 = sin_tau1 * sin_tau2 * cosC - cos_tau1 * cos_tau2;
    sin_delta1 = sqrt(1 - cos_delta1 * cos_delta1);
    sin_delta2 = sqrt(1 - cos_delta2 * cos_delta2);
    sin_delta3 = sqrt(1 - cos_delta3 * cos_delta3);

    alpha = acos((cos_delta1 + cos_delta2 * cos_delta3) / (sin_delta2 * sin_delta3));
    beta = acos((cos_delta2 + cos_delta3 * cos_delta1) / (sin_delta3 * sin_delta1));
    gamma = acos((cos_delta3 + cos_delta1 * cos_delta2) / (sin_delta1 * sin_delta2));

    if (alpha < 0 || alpha > pi || beta < 0 || beta > pi || gamma < 0 || gamma > pi ||
        alpha > beta + gamma || beta > gamma + alpha || gamma > alpha + beta || alpha + beta +
                                                                                gamma > 2 * pi) {
        rejected++;
        return false;
    }

    return true;
}

int ind(double angle)
{
    int i = (int) (N * angle / pi);
    if (i < 0) i = 0;
    if (i >= N) i = N - 1;
    return i;
}

void show_array(int a[N][N][N])
{
    printf("\n\n\n");

    for (auto i = 0; i < N; i++) {
        for (auto j = 0; j < N; j++) {
            for (auto k = 0; k < N; k++) {
                switch (a[i][j][k]) {
                case 0:
                    printf(".");
                    break;  // a "prohibited" cell that is empty
                case 1:
                    printf("x");
                    break;  // a "prohibited" cell containing a data pt.
                case 2:
                    printf(" ");
                    break;  // an "allowable" cell that is empty
                case 3:
                    printf("o");         // an "allowable" cell containing a data pt.
                }
            }
            printf("\n");
        }

        printf("\n");
        for (auto k = 0; k < N; k++) printf("_");
        printf("\n\n");
    }

    printf("\n");
}

int main()
{
    int rejected = 0;

    // Set angles for an ACUTE base triangles ABC
    double constexpr A = 8 * pi / 19;
    double constexpr B = 6 * pi / 19;
    double constexpr C = 5 * pi / 19;

    double const cosA = cos(A);
    double const cosB = cos(B);
    double const cosC = cos(C);

    double alpha, beta, gamma;
    
    int states[N][N][N] = {};

    // Use 3D array to record possible (alpha, beta, gamma) triples for given triangle
    for (auto i = O; i < M - O; i++)
        for (auto j = O; j < M - O; j++)
            for (auto k = O; k < M - O; k++)
                if (tilt_to_view_angles(i * pi / M, j * pi / M, k * pi / M, cosA, cosB, cosC, alpha,
                                        beta, gamma, rejected))
                    states[ind(alpha)][ind(beta)][ind(gamma)] = 1;

    // Also use array to record which cells in the array are within system of bounds
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            for (auto k = 0; k < N; k++) {
                alpha = (i + .5) * pi / N;
                beta = (j + .5) * pi / N;
                gamma = (k + .5) * pi / N;
                if (A + beta + gamma < 2 * pi &&
                    alpha + B + gamma < 2 * pi &&
                    alpha + beta + C < 2 * pi &&
                    (alpha > A || beta < B || beta < C + alpha) &&
                    (alpha > A || gamma < C || gamma < B + alpha) &&
                    (beta > B || gamma < C || gamma < A + beta) &&
                    (beta > B || alpha < A || alpha < C + beta) &&
                    (gamma > C || alpha < A || alpha < B + gamma) &&
                    (gamma > C || beta < B || beta < A + gamma) &&
                    (alpha > A || cosC * cos(beta) + cosB * cos(gamma) > 0) &&
                    (beta > B || cosA * cos(gamma) + cosC * cos(alpha) > 0) &&
                    (gamma > C || cosB * cos(alpha) + cosA * cos(beta) > 0)
#ifdef USE_NEAR_RULES
                    && (alpha > A || B < C || B > A + C ||
                      A * (beta + gamma - alpha) + (B - C) * (alpha + beta - gamma) < 2 * A * B)
                    && (alpha > A || C < B || C > A + B ||
                      A * (gamma + beta - alpha) + (C - B) * (alpha + gamma - beta) < 2 * A * C)
                    && (beta  > B || C < A || C > B + A ||
                      B * (gamma + alpha - beta) + (C - A) * (beta + gamma - alpha) < 2 * B * C)
                    && (beta  > B || A < C || A > B + C ||
                      B * (alpha + gamma - beta) + (A - C) * (beta + alpha - gamma) < 2 * B * A)
                    && (gamma > C || A < B || A > C + B ||
                      C * (alpha + beta - gamma) + (A - B) * (gamma + alpha - beta) < 2 * C * A)
                    && (gamma > C || B < A || B > C + A ||
                      C * (beta + alpha - gamma) + (B - A) * (gamma + beta - alpha) < 2 * C * B)
                    && (alpha > A || B < C || beta  < B ||
                      A * (beta + gamma - alpha) + (B - C) * (beta - alpha - gamma) < 2 * A * C)
                    && (alpha > A || C < B || gamma < C ||
                      A * (gamma + beta - alpha) + (C - B) * (gamma - alpha - beta) < 2 * A * B)
                    && (beta  > B || C < A || gamma < C ||
                      B * (gamma + alpha - beta) + (C - A) * (gamma - beta - alpha) < 2 * B * A)
                    && (beta  > B || A < C || alpha < A ||
                      B * (alpha + gamma - beta) + (A - C) * (alpha - beta - gamma) < 2 * B * C)
                    && (gamma > C || A < B || alpha < A ||
                      C * (alpha + beta - gamma) + (A - B) * (alpha - gamma - beta) < 2 * C * B)
                    && (gamma > C || B < A || beta  < B ||
                      C * (beta + alpha - gamma) + (B - A) * (beta - gamma - alpha) < 2 * C * A)
#endif
                        )
                    states[i][j][k] += 2;
            }

    // Show slices of the array, indicating the nature of each cell.
    show_array(states);

    // Compute and display statistices for the given triangle ABC.
    int total = 0, count0 = 0, count1 = 0, count2 = 0, count3 = 0;

    for (const auto& state: states)
        for (const auto& j: state)
            for (int k: j) {
                switch (k) {
                case 0:
                    count0++;
                    break;
                case 1:
                    count1++;
                    break;
                case 2:
                    count2++;
                    break;
                case 3:
                    count3++;
                    break;
                default:
                    break;
                }
                total++;
            }

    printf("Number of   occupied   allowable cells:    %d\n", count3);
    printf("Number of unoccupied   allowable cells:    %d\n", count2);
    printf("Number of   occupied unallowable cells:    %d\n", count1);
    printf("Number of unoccupied unallowable cells:    %d\n", count0);
    printf("Total number of cells in the array:        %d\n", total);
    printf("Number of rejected calls for a data point: %d\n", rejected);
    printf("(Note: near the boundary, an \"unallowable\" cell might actually ");
    printf("have an allowable portion.)\n\n");
}

