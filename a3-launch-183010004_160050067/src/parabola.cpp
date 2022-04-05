#include <iostream>
#include <cmath>
#include <fstream>
float PI = 355.0 / 113.0;
using namespace std;
int main(int argc, char **argv)
{
    float radius = 0.66; // the base radius
    float height = 3; // the height of the top
    int latNum = 50;    // the number of points from top to bottom
    int longNum = 100;
    ofstream file;
    file.open(argv[1]);

    file << "mtllib sideExh.mtl\n";

    file << "v 0.0 " << height << " 0\n";
    for (int i = 1; i <= latNum; i++)
        for (int j = 0; j < longNum; j++)
        {
            float r = sqrt(height * (float(i) / float(latNum)) * (radius * radius / height));
            file << "v " << r * cos(2.0 * PI * float(j) / float(longNum)) << " "
                 << height * float(latNum - i) / float(latNum) << " "
                 << r * sin(2.0 * PI * float(j) / float(longNum)) << endl;
        }
    file << "vn 0.0 1.0 0.0\n";
    for (int i = 1; i <= latNum; i++)
        for (int j = 0; j < longNum; j++)
        {
            float tan = radius / (2.0 * sqrt(height) * sqrt(height * float(latNum - i) / float(latNum)));
            file << "vn " << cos(2 * PI * float(j) / float(longNum)) << " "
                 << tan << " "
                 << sin(2.0 * PI * float(j) / float(longNum)) << endl;
        }
    file << "vt 0.5 1.0\n";
    for (int i = 1; i <= latNum; i++)
        for (int j = 0; j <= longNum; j++)
        {
            float r = sqrt(height * (float(i) / float(latNum)) * (radius * radius / height));
            file << "vt " << float(j) / float(longNum) << " "
                 << float(latNum - i) / float(latNum) << endl;
        }
    file << "usemtl exh\n";
    for (int j = 0; j < longNum; j++)
    {
        if (j != longNum - 1)
        {
            file << "f 1/1/1 " << j + 3 << "/" << j + 3 << "/" << j + 3 << " "
                 << j + 2 << "/" << j + 2 << "/" << j + 2 << endl;
        }
        else
        {
            file << "f 1/1/1 " << 2 << "/" << j + 3 << "/" << 2 << " "
                 << j + 2 << "/" << j + 2 << "/" << j + 2 << endl;
        }
    }

    for (int i = 1; i < latNum; i++)
        for (int j = 0; j < longNum; j++)
        {
            int f1_index = 1 + (i - 1) * longNum + (j + 1);
            int f1_t = 1 + (i - 1) * (longNum + 1) + (j + 1);
            int s1_index = 1 + (i - 1) * longNum + (j + 2);
            int s1_t = 1 + (i - 1) * (longNum + 1) + (j + 2);
            int f2_index = 1 + (i)*longNum + (j + 1);
            int f2_t = 1 + (i) * (longNum + 1) + (j + 1);
            int s2_index = 1 + (i)*longNum + (j + 2);
            int s2_t = 1 + (i) * (longNum + 1) + (j + 2);
            if (j == longNum - 1)
            {
                s1_index = 1 + (i - 1) * longNum + 1;
                s2_index = 1 + (i)*longNum + 1;
            }
            file << "f " << f1_index << "/" << f1_t << "/" << f1_index << " "
                 << s2_index << "/" << s2_t << "/" << s2_index << " "
                 << s1_index << "/" << s1_t << "/" << s1_index << endl;
            file << "f " << f1_index << "/" << f1_t << "/" << f1_index << " "
                 << f2_index << "/" << f2_t << "/" << f2_index << " "
                 << s2_index << "/" << s2_t << "/" << s2_index << endl;
        }

    file.close();
    return 0;
}
