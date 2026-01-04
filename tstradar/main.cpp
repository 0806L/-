#include "mainwindow.h"
#include <QtWidgets/QApplication>

// 生成极坐标噪声数据（模拟雷达回波）
const int RADAR_RESOLUTION = 512; // 分辨率
std::vector<float> radarData(RADAR_RESOLUTION* RADAR_RESOLUTION);

void generateRadarData() {
    for (int theta = 0; theta < RADAR_RESOLUTION; theta++) {
        for (int r = 0; r < RADAR_RESOLUTION; r++) {
            // 基础噪声（Perlin 噪声或随机噪声）
            float noise = static_cast<float>(rand()) / RAND_MAX * 0.3f;

            // 模拟固定目标（如岛屿）
            if (r < 200 && theta % 50 == 0) noise += 0.5f;

            // 模拟动态目标（如船只）
            if (r > 300 && r < 320 && theta == 0) {
                noise += 0.8f;
            }

            radarData[theta * RADAR_RESOLUTION + r] = noise;
        }
    }

    int stop = 1;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
