REM 批量将本目录中的所有C++文件用Astyle进行代码美化操作
REM 设置Astyle命令位置和参数
@echo off
REM 循环遍历目录
for /r . %%f in (*.cpp;*.c) do astyle --style=kr "%%f"
for /r . %%f in (*.hpp;*.h) do astyle --style=kr "%%f"
REM 删除所有的astyle生成文件
for /r . %%f in (*.orig) do del "%%f"
pause