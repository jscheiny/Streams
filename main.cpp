#include "Stream.h"

#include "Reducers.h"
#include "Utility.h"

#include <vector>
#include <iterator>
#include <iostream>
#include <tuple>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;

int main(int argc, char const *argv[])
{

    int count = MakeStream::generate([]() {
        using namespace chrono;
        using clock = high_resolution_clock;
        static auto first_call = clock::now();
        auto current_call = clock::now();
        double seconds = duration_cast<duration<double>>(current_call - first_call).count();
        if(seconds > 1) {
            throw StopStream();
        }
        return 1;
    }).count();
    cout << count << endl;
}
