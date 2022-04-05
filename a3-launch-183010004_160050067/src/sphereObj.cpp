#include <iostream>
#include <vector>
#include <algorithm>
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <fstream>
#include <cmath>
float PI = 355.0 / 113.0;
using namespace std;
int main()
{
    ofstream file;
    file.open("../images/earth.obj");
    file << "mtllib earth.mtl\n";
    float radius = 1;
    int lat_num = 100;
    int long_num = 100;
    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec2> texture;
    file << "v " << 0 << " " << radius << " " << 0 << endl;
    for (int i = 1; i < lat_num; i++)
    {
        for (int j = 0; j < long_num; j++)
        {
            file << "v " << radius * sin((float(i) / float(lat_num)) * PI) * cos((float(j) / float(long_num)) * 2 * PI) << " "
                 << radius * cos((float(i) / float(lat_num)) * PI) << " "
                 << radius * sin((float(i) / float(lat_num)) * PI) * sin((float(j) / float(long_num)) * 2 * PI) << endl;
        }
    }
    file << "v " << 0 << " " << -radius << " " << 0 << endl;

    file << "vn " << 0 << " " << radius << " " << 0 << endl;
    for (int i = 1; i < lat_num; i++)
    {
        for (int j = 0; j < long_num; j++)
        {
            file << "vn " << sin((float(i) / float(lat_num)) * PI) * cos((float(j) / float(long_num)) * 2 * PI) << " "
                 << cos((float(i) / float(lat_num)) * PI) << " "
                 << sin((float(i) / float(lat_num)) * PI) * sin((float(j) / float(long_num)) * 2 * PI) << endl;
        }
    }
    file << "vn " << 0 << " " << -radius << " " << 0 << endl;

    file << "vt " << 0.5 << " " << 1.0 << endl;
    for (int i = 1; i < lat_num; i++)
    {
        for (int j = 0; j <= long_num; j++)
        {
            file << "vt " << (1 - float(j) / float(long_num)) << " "
                 << (float(i) / float(lat_num)) << endl;
        }
    }
    file << "vt " << 0.5 << " " << 0.0 << endl;
    file << "usemtl earth\n";
    for (int j = 0; j < long_num; j++)
    {
        if (j != long_num - 1)
            file << "f 1/1/1 " << j + 2 << "/" << j + 2 << "/" << j + 2 << " " << j + 3 << "/" << j + 3 << "/" << j + 3 << endl;
        else
            file << "f 1/1/1 " << j + 2 << "/" << j + 2 << "/" << j + 2 << " "
                 << "2/" << j + 3 << "/2" << endl;
    }
    for (int i = 1; i < lat_num - 1; i++)
    {
        for (int j = 0; j < long_num; j++)
        {
            int f1_index = 1 + (i - 1) * long_num + (j + 1);
            int f1_t = 1 + (i - 1) * (long_num + 1) + (j + 1);
            int s1_index = 1 + (i - 1) * long_num + (j + 2);
            int s1_t = 1 + (i - 1) * (long_num + 1) + (j + 2);
            int f2_index = 1 + (i)*long_num + (j + 1);
            int f2_t = 1 + (i) * (long_num + 1) + (j + 1);
            int s2_index = 1 + (i)*long_num + (j + 2);
            int s2_t = 1 + (i) * (long_num + 1) + (j + 2);
            if (j == long_num - 1)
            {
                s1_index = 1 + (i - 1) * long_num + 1;
                s2_index = 1 + (i)*long_num + 1;
            }
            file << "f " << f1_index << "/" << f1_t << "/" << f1_index << " "
                 << s2_index << "/" << s2_t << "/" << s2_index << " "
                 << s1_index << "/" << s1_t << "/" << s1_index << endl;
            file << "f " << f1_index << "/" << f1_t << "/" << f1_index << " "
                 << f2_index << "/" << f2_t << "/" << f2_index << " "
                 << s2_index << "/" << s2_t << "/" << s2_index << endl;
        }
    }
    int last = 1 + long_num * (lat_num - 1) + 1;
    int last_t = 1 + (long_num + 1) * (lat_num - 1) + 1;
    int temp = 1 + long_num * (lat_num - 2);
    int temp_t = 1 + (long_num + 1) * (lat_num - 2);
    for (int j = 0; j < long_num; j++)
    {
        if (j != long_num - 1)
            file << "f " << last << "/" << last_t << "/" << last << " " << temp + j + 2 << "/" << temp_t + j + 2 << "/" << temp + j + 2 << " " << temp + j + 1 << "/" << temp_t + j + 1 << "/" << temp + j + 1 << endl;
        else
            file << "f " << last << "/" << last_t << "/" << last << " " << temp + 1 << "/" << temp_t + j + 2 << "/" << temp_t + 1 << " " << temp + j + 1 << "/" << temp_t + j + 1 << "/" << temp + j + 1 << endl;
    }
    file.close();
}
