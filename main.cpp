#include <iostream>

#include "utils.h"



int main(int argc, char *argv[]) {
    // if (argc < 2)
    //     return 1;
    //
    // for (int i = 1; i < argc; i++) {
    //     time_t ctm = time(nullptr);
    //     tm *tm2 = localtime(&ctm);
    //     char timeStr[20];
    //     strftime(timeStr, sizeof(timeStr), "_%Y%m%d%H%M%S", tm2);
    //
    //     std::string out = std::filesystem::path(argv[i]).replace_extension().string() + timeStr;
    //
    //     std::cout << "Processing " << argv[i] << std::endl;
    //     wdf_export_nes(argv[i], out, 0);
    //     std::cout << "Done" << std::endl;
    // }

    std::cout << "Processing " << std::endl;
    // wdf_export_nes("C:/Users/chend/Desktop/shape.wdf", "C:/Users/chend/Desktop/shape", 0);
    wdf_export_xy2("C:/Users/chend/Desktop/shape.wdf", "C:/Users/chend/Desktop/shape", 0);

    std::cout << "Done" << std::endl;
}
