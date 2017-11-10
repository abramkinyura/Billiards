#include <math.h>
#include <iostream>
#include "R2Graph.h"

using namespace std;

int main() {
    cout << "Test of R2Graph package\n";
    R2Vector u, v;
    while (true) {
        cout << "Input 2 vectors:\n";
        cin >> u.x >> u.y >> v.x >> v.y;
        if (u == R2Vector(0., 0.) || v == R2Vector(0., 0.))
            break;
        double a = u.angle(v);
        cout << "angle from first to secong = " << a
            << " (" << (int) (floor(a*180./3.14158) + 0.5) 
            << " degrees)" << endl;
    }
    return 0;
}
