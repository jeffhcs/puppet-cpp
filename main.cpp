#include"evaluator.h"
#include<bits/stdc++.h>

using namespace std;
using namespace chrono;

string sts(const State &s) {
    return "(" + to_string(s.a0) + ", " + to_string(s.a1) + ", "
           + to_string(s.s0) + ", " + to_string(s.s1) + ", "
           + to_string(s.d0) + ", " + to_string(s.d1) + ")";
}

string sts(const optional<State> &optS) {
    if (optS.has_value()) {
        return sts(optS.value());
    } else {
        return "None";
    }
}

void printBucket(const Bucket &bucket) {
    cout << "Bucket: ";
    cout << get<0>(bucket) << ", ";
    cout << get<1>(bucket) << ", ";
    cout << get<2>(bucket) << ", ";
    cout << get<3>(bucket) << ", ";
    cout << get<4>(bucket) << ", ";
    cout << get<5>(bucket) << endl;
}

void display_progress_bar(int i, int total_buckets) {
    static auto start_time = system_clock::now();
    static auto last_update_time = system_clock::now();

    if (i == 0) {
        start_time = system_clock::now();
        last_update_time = start_time;
    }

    auto current_time = system_clock::now();
    auto time_since_last_update = duration_cast<milliseconds>(current_time - last_update_time);

    if (time_since_last_update.count() < 500 && i != total_buckets) {
        return;
    }
    last_update_time = current_time;

    float percentage = round(static_cast<float>(i) / total_buckets * 10000) / 100.0;
    auto elapsed_time_ms = duration_cast<milliseconds>(current_time - start_time).count();

    int hours = elapsed_time_ms / (1000 * 60 * 60);
    int minutes = (elapsed_time_ms % (1000 * 60 * 60)) / (1000 * 60);
    int seconds = (elapsed_time_ms % (1000 * 60)) / 1000;
    int milliseconds = elapsed_time_ms % 1000;

    cout << "\r" << i << "/" << total_buckets << " | "
         << percentage << "% | Elapsed: "
         << setw(2) << setfill('0') << hours << ":"
         << setw(2) << setfill('0') << minutes << ":"
         << setw(2) << setfill('0') << seconds << "."
         << setw(2) << setfill('0') << milliseconds / 10
         << flush;
}

void peak(const State &state) {
    State q_state = quantize_state(state);
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

    cout << sts(state) << ": " << sts(q_state) << endl;
    cout << sts(state_wait) << " | " << sts(state_a0) << " | " << sts(state_a1) << endl;
    cout << eval_wait << " | " << eval_a0 << " | " << eval_a1 << endl;
    cout << eval_state(state) << endl;
    cout << endl;
}


void printAsPythonList(int arr[S_MAX + 1][S_MAX + 1]) {
    std::cout << "[" << std::endl;
    for (int i = 0; i < S_MAX + 1; ++i) {
        std::cout << "  [";
        for (int j = 0; j < S_MAX + 1; ++j) {
            std::cout << arr[i][j];
            if (j != S_MAX) std::cout << ", ";
        }
        std::cout << "]";
        if (i != S_MAX) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "]" << std::endl;
}



int main() {
//    State s = State{5, 5, 4, 4, 8, 8};
//    cout << sts(s) << " | " << sts(quantize_state(s)) << endl;
//    return 0;

    compute_step_sizes();
//    printAsPythonList(step_sizes);
//    return 0;

    cout << "Populating DP..." << endl;
    populate();
    cout << "Saving..." << endl;
    save_dp("evals.bin", evals);
//    cout << "Loading..." << endl;
//    load_dp("evals.bin", evals);
//    cout << endl;

    write_table("table.bin");

//    vector<State> k_frames = {
//            State{5, 5, 4, 4, 8, 8},
//            State{4, 5, 9, 0, 41, 41},
//            State{4, 5, 20, 11, 200, 101},
//            State{3, 5, 24, 4, 288, 110},
//            State{2, 5, 31, 0, 493, 166},
//            State{2, 5, 40, 9, 800, 203},
//            State{2, 4, 34, 11, 888, 229},
//            State{2, 3, 24, 13, 934, 253},
//            State{2, 2, 15, 17, 984, 310},
//            State{1, 2, 23, 10, 1127, 355},
//            State{1, 2, 32, 19, 1390, 493},
//            State{0, 2, 39, 6, 1617, 513},
//            State{0, 2, 51, 18, 2152, 662},
//            State{0, 2, 63, 30, 2831, 955},
//            State{0, 1, 35, 34, 2990, 1084},
//            State{0, 0, 1, 39, 3163, 1253},
//            State{0, 0, 12, 50, 3239, 1763},
//            State{0, 0, 24, 62, 3460, 2440},
//            State{0, 0, 36, 74, 3825, 3260},
//    };
//
//    for (const auto& frame : k_frames) {
//        State state(frame);
//        peak(state);
//    }

    return 0;

//    for (int i = 0; i < TOTAL_BUCKETS; i += TOTAL_BUCKETS / 17){
//        cout << i << ": " << read_dp(unbucket_state(get_ith_bucket(i)), evals) << endl;
//    }

    return 0;
}
