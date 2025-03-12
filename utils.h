#ifndef UTILS_H
#define UTILS_H

unsigned int string_id(const char* str);

/**
 * 导出精灵表或单张精灵图
 *
 * @param wdfPath Wdf 路径
 * @param outDir 输出目录
 * @param mode 输出文件模式 \n 0： 合并为一张精灵表保存 \n 1：统一尺寸保存每一张精灵图 \n 2：原始尺寸保存每一张精灵图
 */
void wdf_export_nes(const std::string &wdfPath, const std::string &outDir, int mode);

void wdf_export_xy2(const std::string &wdfPath, const std::string &outDir, int mode);
#endif //UTILS_H
