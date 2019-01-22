#pragma once

// 内部实现，不导出，外部模块请勿使用，如有需要，调用tools\string_tools.h中包装的接口
void x_private_strptime(const char *buf, const char *fmt, struct tm *tm);
