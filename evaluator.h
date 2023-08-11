#ifndef EVALUATOR_H
#define EVALUATOR_H

#include"main.h"
#include<bits/stdc++.h>
using namespace std;

constexpr int A_MAX = 5;
constexpr int S_MAX = 100;
constexpr int D_MAX = 4000;
constexpr int S_STEP = 8;
constexpr int D_STEP = 160;

constexpr int A_DIM = A_MAX + 1;
constexpr int S_DIM = S_MAX / S_STEP + 1;
constexpr int D_DIM = D_MAX / D_STEP + 1;

constexpr int TOTAL_BUCKETS = A_DIM * A_DIM * S_DIM * S_DIM * D_DIM * D_DIM;

constexpr float INF = FLT_MAX;

typedef tuple<int, int, int, int, int, int> Bucket;

template<typename T>
using DP = T[A_DIM][A_DIM][S_DIM][S_DIM][D_DIM][D_DIM];

inline DP<float> evals = {};
inline DP<int> moves = {};
inline int step_sizes[S_MAX + 1][S_MAX + 1] = {};

struct State {
    int a0;
    int a1;
    int s0;
    int s1;
    float d0;
    float d1;
};

int quantize_val(int bucket_size, int max_val, float val);
Bucket bucket_state(const State &state);
State unbucket_state(const Bucket bucket);
State quantize_state(const State &state);

template<typename T>
T read_dp(const State &state, const DP<T> &dp);

template<typename T>
void write_dp(const State &state, T value, DP<T> &dp);

float eval_endgame(const State &state);

tuple<State, optional<State>, optional<State>> get_next_states(const State &state);
float get_loss(float time, int init_speed);
float get_cost(float time, int init_speed_0, int init_speed_1);
void compute_step_sizes();

float eval_state(const State &state);
Bucket get_ith_bucket(int n);
void populate();

template<typename T>
void save_dp(const string &filename, const DP<T> &dp);

template<typename T>
void load_dp(const string& filename, DP<T> &dp);

#endif // EVALUATOR_H
