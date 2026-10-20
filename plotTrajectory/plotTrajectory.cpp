#include <pangolin/pangolin.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace Eigen;

// 轨迹文件路径
string trajectory_file = "./trajectory.txt";

// 函数声明
void DrawTrajectory(vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> poses);

int main(int argc, char **argv) {
    // ⚠️ 非常重要：必须用 Eigen::aligned_allocator
    // 原因：Eigen固定大小结构体需要16字节对齐，默认vector分配器会导致段错误
    vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> poses;

    ifstream fin(trajectory_file);
    if (!fin) {
        cout << "cannot find trajectory file at " << trajectory_file << endl;
        return 1;
    }

    // 逐行读取轨迹
    while (!fin.eof()) {
        double time, tx, ty, tz, qx, qy, qz, qw;
        fin >> time >> tx >> ty >> tz >> qx >> qy >> qz >> qw;

        // ⚠️ 四元数构造顺序：Eigen是 实部w在前，虚部x,y,z在后
        // 文件里顺序是 qx qy qz qw，所以构造时把qw放第一个
        Isometry3d Twr(Quaterniond(qw, qx, qy, qz));
        Twr.pretranslate(Vector3d(tx, ty, tz));

        poses.push_back(Twr);
    }

    cout << "read total " << poses.size() << " pose entries" << endl;

    // 绘制轨迹
    DrawTrajectory(poses);
    return 0;
}

void DrawTrajectory(vector<Isometry3d, Eigen::aligned_allocator<Isometry3d>> poses) {
    // 创建窗口：标题，宽1024，高768
    pangolin::CreateWindowAndBind("Trajectory Viewer", 1024, 768);

    // OpenGL 基础设置：深度测试、透明混合
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 设置相机：投影矩阵 + 视角位置
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1024, 768, 500, 500, 512, 389, 0.1, 1000),
        pangolin::ModelViewLookAt(0, -0.1, -1.8, 0, 0, 0, 0.0, -1.0, 0.0)
    );

    // 创建显示视图
    pangolin::View &d_cam = pangolin::CreateDisplay()
        .SetBounds(0.0, 1.0, 0.0, 1.0, -1024.0f / 768.0f)
        .SetHandler(new pangolin::Handler3D(s_cam));

    // 主循环：直到关闭窗口
    while (!pangolin::ShouldQuit()) {
        // 清空颜色和深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 背景白色
        glLineWidth(2);

        // ========== 1. 画每个位姿的三个坐标轴 ==========
        for (size_t i = 0; i < poses.size(); i++) {
            // 位姿原点在世界坐标系下的坐标
            Vector3d Ow = poses[i].translation();

            // 计算三个坐标轴端点的世界坐标（长度0.1）
            Vector3d Xw = poses[i] * (0.1 * Vector3d(1, 0, 0));
            Vector3d Yw = poses[i] * (0.1 * Vector3d(0, 1, 0));
            Vector3d Zw = poses[i] * (0.1 * Vector3d(0, 0, 1));

            glBegin(GL_LINES);
            // X轴：红色
            glColor3f(1.0, 0.0, 0.0);
            glVertex3d(Ow[0], Ow[1], Ow[2]);
            glVertex3d(Xw[0], Xw[1], Xw[2]);

            // Y轴：绿色
            glColor3f(0.0, 1.0, 0.0);
            glVertex3d(Ow[0], Ow[1], Ow[2]);
            glVertex3d(Yw[0], Yw[1], Yw[2]);

            // Z轴：蓝色
            glColor3f(0.0, 0.0, 1.0);
            glVertex3d(Ow[0], Ow[1], Ow[2]);
            glVertex3d(Zw[0], Zw[1], Zw[2]);
            glEnd();
        }

        // ========== 2. 画轨迹连线（黑色） ==========
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        for (size_t i = 0; i < poses.size() - 1; i++) {
            auto p1 = poses[i].translation();
            auto p2 = poses[i+1].translation();
            glVertex3d(p1[0], p1[1], p1[2]);
            glVertex3d(p2[0], p2[1], p2[2]);
        }
        glEnd();

        pangolin::FinishFrame(); // 结束一帧
        usleep(5000); // 暂停5ms，控制帧率
    }
}
