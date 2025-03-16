#include <filesystem>
#include <iostream>

#include "utils.h"



int main(int argc, char *argv[]) {
    if (argc < 2)
        return 1;

    for (int i = 1; i < argc; i++) {
        time_t ctm = time(nullptr);
        tm *tm2 = localtime(&ctm);
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "_%Y%m%d%H%M%S", tm2);

        std::string out = std::filesystem::path(argv[i]).replace_extension().string() + timeStr;

        std::cout << std::filesystem::path(argv[i]).extension().string() << std::endl;
        if (std::filesystem::path(argv[i]).extension().string() == ".map") {
            std::cout << "Processing " << argv[i] << std::endl;
            map_export_xy2(argv[i], out);
            std::cout << "Done" << std::endl;
        } else {
            std::cout << "Processing " << argv[i] << std::endl;
            wdf_export_xy2(argv[i], out, 0);
            std::cout << "Done" << std::endl;
        }
    }

    // std::cout << "Processing " << std::endl;
    // wdf_export_nes("C:/games/dhxy/shape.wdf", "C:/Users/chend/Desktop/shape2", 0);
    // wdf_export_xy2("C:/games/dhxy/gires2.wdf", "C:/Users/chend/Desktop/gires2", 0);
    //
    // std::cout << "Done" << std::endl;

    // // 地图
    // std::cout << "Processing " << std::endl;
    // map_export_xy2("C:/games/dhxy/scene/1208.map", "C:/games/dhxy/scene/1208");
    // std::cout << "Done" << std::endl;
}
