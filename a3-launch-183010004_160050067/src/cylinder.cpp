#include <iostream>
#include <cmath>
#include <fstream>
float PI = 355.0 / 113.0;
using namespace std;
int main(int argc, char **argv)
{
    ofstream file;
    file.open(argv[1]);
    file << "mtllib auxBooster.mtl\n";
    float radius = 0.66;
    float height = 15;
    int longNum = 100;
    for (int i = 0; i < longNum; i++)
    {
        file << "v " << radius * cos(float(i) / float(longNum) * 2 * PI) << " "
             << height / 2.0 << " " << radius * sin(float(i) / float(longNum) * 2 * PI) << endl;
        file << "v " << radius * cos(float(i) / float(longNum) * 2 * PI) << " "
             << -height / 2.0 << " " << radius * sin(float(i) / float(longNum) * 2 * PI) << endl;
    }
    for (int i = 0; i < longNum; i++)
    {
        file << "vn " << cos(float(i) / float(longNum) * 2 * PI) << " "
             << 0.0 << " " << sin(float(i) / float(longNum) * 2 * PI) << endl;
        file << "vn " << cos(float(i) / float(longNum) * 2 * PI) << " "
             << 0.0 << " " << sin(float(i) / float(longNum) * 2 * PI) << endl;
    }
    for (int i = 0; i <= longNum; i++)
    {
        file << "vt " << 1.0 - float(i) / float(longNum) << " "
             << 0.0 << endl;
        file << "vt " << 1.0 - float(i) / float(longNum) << " "
             << 1.0 << endl;
    }
    file << "usemtl boost\n";
    for (int i = 0; i < longNum; i++)
    {
        if (i != longNum - 1)
        {
            file << "f " << 2 * i + 1 << "/" << 2 * i + 1 << "/" << 2 * i + 1 << " "
                 << 2 * i + 2 << "/" << 2 * i + 2 << "/" << 2 * i + 2 << " "
                 << 2 * i + 3 << "/" << 2 * i + 3 << "/" << 2 * i + 3 << endl;
            file << "f " << 2 * i + 2 << "/" << 2 * i + 2 << "/" << 2 * i + 2 << " "
                 << 2 * i + 4 << "/" << 2 * i + 4 << "/" << 2 * i + 4 << " "
                 << 2 * i + 3 << "/" << 2 * i + 3 << "/" << 2 * i + 3 << endl;
        }
        else
        {
            file << "f " << 2 * i + 1 << "/" << 2 * i + 1 << "/" << 2 * i + 1 << " "
                 << 2 * i + 2 << "/" << 2 * i + 2 << "/" << 2 * i + 2 << " "
                 << 1 << "/" << 2 * i + 3 << "/" << 1 << endl;
            file << "f " << 2 * i + 2 << "/" << 2 * i + 2 << "/" << 2 * i + 2 << " "
                 << 2 << "/" << 2 * i + 4 << "/" << 2 << " "
                 << 1 << "/" << 2 * i + 3 << "/" << 1 << endl;
        }
    }
    return 0;
}
