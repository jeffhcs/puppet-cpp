#include "evaluator.h"
#include "main.h"
#include<bits/stdc++.h>

using namespace std;

int quantize_val(int bucket_size, int max_val, float val) {
    int bucket = floor((val + bucket_size / 2.0) / bucket_size);
    return min(max_val, bucket * bucket_size);
}

Bucket bucket_state(const State &state) {
    int s0_bucket = quantize_val(S_STEP, S_MAX, state.s0) / S_STEP;
    int s1_bucket = quantize_val(S_STEP, S_MAX, state.s1) / S_STEP;
    int d0_bucket = quantize_val(D_STEP, D_MAX, state.d0) / D_STEP;
    int d1_bucket = quantize_val(D_STEP, D_MAX, state.d1) / D_STEP;

    return make_tuple(state.a0, state.a1, s0_bucket, s1_bucket, d0_bucket, d1_bucket);
}

State unbucket_state(const Bucket bucket) {
    return State{
            get<0>(bucket),
            get<1>(bucket),
            get<2>(bucket) * S_STEP,
            get<3>(bucket) * S_STEP,
            static_cast<float>(get<4>(bucket) * D_STEP),
            static_cast<float>(get<5>(bucket) * D_STEP)
    };
}

State quantize_state(const State &state) {
    return unbucket_state(bucket_state(state));
}

template<typename T>
T read_dp(const State &state, const DP<T> &dp) {
    auto [a0, a1, s0, s1, d0, d1] = bucket_state(state);
    return dp[a0][a1][s0][s1][d0][d1];
}

template<typename T>
void write_dp(const State &state, T value, DP<T> &dp) {
    auto [a0, a1, s0, s1, d0, d1] = bucket_state(state);
    dp[a0][a1][s0][s1][d0][d1] = value;
}

float eval_endgame(const State &state) {

    auto f = [](float s, float b) -> float {
        return sqrt(s * s + 2 * b) - s;
    };

    auto sf = [](float s, float t) -> float {
        return (2 * s + t) * t / 2;
    };

    float f0 = f(state.s0, D_MAX - state.d0);
    float f1 = f(state.s1, D_MAX - state.d1);

    if (f1 > f0) {
        return D_MAX - sf(state.s1, f0) - state.d1;
    } else {
        return -(D_MAX - sf(state.s0, f1) - state.d0);
    }
}

tuple<State, optional<State>, optional<State>> get_next_states(const State &state) {

    int step_size = max(step_sizes[state.s0][state.s1], S_STEP);

    State state_wait = {
            state.a0,
            state.a1,
            state.s0 + step_size,
            state.s1 + step_size,
            static_cast<float>(state.d0 + (state.s0 + state.s0 + step_size) * step_size / 2.0),
            static_cast<float>(state.d1 + (state.s1 + state.s1 + step_size) * step_size / 2.0)
    };
    optional<State> state_a0;
    optional<State> state_a1;

    if (state.a0 > 0) {
        state_a0.emplace(State{state.a0 - 1, state.a1, state.s0, max(0, state.s1 - state.s0), state.d0, state.d1});
    }

    if (state.a1 > 0) {
        state_a1.emplace(State{state.a0, state.a1 - 1, max(0, state.s0 - state.s1), state.s1, state.d0, state.d1});
    }

    return make_tuple(state_wait, state_a0, state_a1);
}

float get_loss(float time, int init_speed) {
    float distance = (2 * init_speed + time) * time / 2.0;
    float loss = D_STEP / 2.0 - fmod(distance + D_STEP / 2.0, D_STEP);
    return abs(loss / distance);
}

float get_cost(float time, int init_speed_0, int init_speed_1) {
    float l0 = get_loss(time, init_speed_0);
    float l1 = get_loss(time, init_speed_1);
    float loss = max(l0, l1);
    const float COST_RATIO = 0.2;
    const float MAX_LOSS = 0.4;
    if (loss >= MAX_LOSS) {
        return INF;
    }
    return time + (loss / COST_RATIO);
}

void compute_step_sizes() {
    const int MAX_T = 25;
    for (int s0 = 0; s0 < S_MAX + 1; s0++) {
        for (int s1 = 0; s1 < S_MAX + 1; s1++) {
            float min_cost = INF;
            int min_cost_t = 0;
            for (int t = S_STEP; t < min(MAX_T, S_MAX + 1); t += S_STEP) {
                float current_cost = get_cost(t, s0, s1);
                if (current_cost < min_cost) {
                    min_cost = current_cost;
                    min_cost_t = t;
                }
            }
            step_sizes[s0][s1] = min_cost_t;
        }
    }
}

float eval_state(const State &state) {
    float eval;
    int move = read_dp(state, moves);
    State q_state = quantize_state(state);

    if ((state.a0 == 0 && state.a1 == 0) || state.d0 >= D_MAX || state.d1 >= D_MAX) {
        eval = eval_endgame(state);
    } else if (move != 0) {
        return read_dp(state, evals);
    } else {
        State state_wait{};
        optional<State> state_a0, state_a1;
        tie(state_wait, state_a0, state_a1) = get_next_states(q_state);

        float eval_wait, eval_a0, eval_a1;

        if (state_a0.has_value()) {
            eval_a0 = eval_state(state_a0.value());
        } else {
            eval_a0 = -INF;
        }
        if (state_a1.has_value()) {
            eval_a1 = eval_state(state_a1.value());
        } else {
            eval_a1 = INF;
        }
        float rounding_diff = (state_wait.d0 - quantize_val(D_STEP, D_MAX, state_wait.d0)) -
                              (state_wait.d1 - quantize_val(D_STEP, D_MAX, state_wait.d1));
        eval_wait = eval_state(state_wait) + rounding_diff;

        if (eval_wait <= eval_a0 && eval_a0 <= eval_a1) {
            eval = eval_a0;
            move = 1;
        } else if (eval_a0 <= eval_a1 && eval_a1 <= eval_wait) {
            eval = eval_a1;
            move = 2;
        } else if (eval_a0 <= eval_wait && eval_wait <= eval_a1) {
            eval = eval_wait;
            move = 3;
        } else {
            eval = (eval_a0 + eval_a1) / 2;
            move = 4;
        }
        write_dp(state, move, moves);
    }
    write_dp(state, eval, evals);
    return eval;
}

Bucket get_ith_bucket(int n) {
    int resultArr[6];
    int dimensions[6] = {A_DIM, A_DIM, S_DIM, S_DIM, D_DIM, D_DIM};

    for (int i = 5; i >= 0; --i) {
        resultArr[i] = n % dimensions[i];
        n /= dimensions[i];
    }

    assert(n == 0 && "index too large");

    return make_tuple(resultArr[0], resultArr[1], resultArr[2], resultArr[3], resultArr[4], resultArr[5]);
}

void populate() {
    cout << "Total buckets: " << TOTAL_BUCKETS << endl;

    for (int i = 0; i < TOTAL_BUCKETS; i++) {
        if (i % 10000 == 0) {
            display_progress_bar(i, TOTAL_BUCKETS);
        }
        eval_state(unbucket_state(get_ith_bucket(i)));
    }

    display_progress_bar(TOTAL_BUCKETS, TOTAL_BUCKETS);
    cout << endl;
}

template<typename T>
void save_dp(const string &filename, const DP<T> &dp) {
    ofstream out(filename, ios::binary);

    if (!out) {
        cerr << "Cannot open " << filename << " for writing." << endl;
        return;
    }

    for (int i = 0; i < TOTAL_BUCKETS; ++i) {
        display_progress_bar(i, TOTAL_BUCKETS);
        float value = read_dp(unbucket_state(get_ith_bucket(i)), dp);
        out.write(reinterpret_cast<const char *>(&value), sizeof(float));
    }

    out.close();
}
template void save_dp<float>(const string &, const DP<float> &);
template void save_dp<int>(const string &, const DP<int> &);

template<typename T>
void load_dp(const string& filename, DP<T> &dp) {
    ifstream in(filename, ios::binary);

    if (!in) {
        cerr << "Cannot open " << filename << " for reading." << endl;
        return;
    }

    int index = 0;
    T value;
    while (in.read(reinterpret_cast<char*>(&value), sizeof(float))) {
        display_progress_bar(index, TOTAL_BUCKETS);
        write_dp(unbucket_state(get_ith_bucket(index)), value, dp);

        index++;
    }

    in.close();
}
template void load_dp<float>(const string&, DP<float>&);
template void load_dp<int>(const string&, DP<int>&);


