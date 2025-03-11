#include <fcntl.h>
#include <filesystem>
#include <io.h>
#include <iostream>
#include <limits>
#include <shlwapi.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "shlwapi.lib")

// 获取可执行文件的完整路径
std::wstring GetExecutablePath(const std::wstring &exePath) {
  wchar_t fullPath[MAX_PATH];
  if (GetFullPathNameW(exePath.c_str(), MAX_PATH, fullPath, nullptr)) {
    return fullPath;
  }
  return L"";
}

bool RegisterApplication(const std::wstring &exePath) {
  // 获取完整路径
  std::wstring fullPath = GetExecutablePath(exePath);
  if (fullPath.empty()) {
    std::wcout << L"无法获取" << exePath << L"的完整路径!" << std::endl;
    return false;
  }
  // 获取可执行文件名
  std::filesystem::path path(fullPath);
  std::wstring fileName = path.filename().wstring();
  std::wstring directory = path.parent_path().wstring();
  // 打开或创建注册表键
  HKEY hKey;
  std::wstring regPath =
      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + fileName;
  LONG result = RegCreateKeyExW(
      HKEY_LOCAL_MACHINE, // 可以改为HKEY_CURRENT_USER，如果没有管理员权限
      regPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey,
      NULL);
  if (result != ERROR_SUCCESS) {
    std::wcout << L"无法创建注册表键! 错误代码: " << result << std::endl;
    return false;
  }
  // 设置默认值为EXE的完整路径
  result = RegSetValueExW(hKey,
                          NULL, // 默认值
                          0, REG_SZ, (BYTE *)fullPath.c_str(),
                          (DWORD)((fullPath.length() + 1) * sizeof(wchar_t)));
  if (result != ERROR_SUCCESS) {
    std::wcout << L"无法设置默认值! 错误代码: " << result << std::endl;
    RegCloseKey(hKey);
    return false;
  }
  // 设置Path值为EXE所在目录
  result = RegSetValueExW(hKey, L"Path", 0, REG_SZ, (BYTE *)directory.c_str(),
                          (DWORD)((directory.length() + 1) * sizeof(wchar_t)));
  if (result != ERROR_SUCCESS) {
    std::wcout << L"无法设置Path值! 错误代码: " << result << std::endl;
    RegCloseKey(hKey);
    return false;
  }
  RegCloseKey(hKey);
  return true;
}

int main(int argc, char *argv[]) {
  // 确保控制台可以正确显示Unicode字符
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);

  // 设置控制台模式以支持Unicode
  _setmode(_fileno(stdout), _O_U16TEXT);
  _setmode(_fileno(stderr), _O_U16TEXT);
  _setmode(_fileno(stdin), _O_U16TEXT);

  if (argc < 2) {
    std::wcout << L"用法: " << argv[0] << L" <可执行文件路径>" << std::endl;
    std::wcout << L"示例: " << argv[0] << L" \"C:\\Path\\To\\YourApp.exe\""
               << std::endl;
    std::wcout << L"按回车键继续..." << std::endl;
    system("pause");
    return 1;
  }

  // 转换命令行参数为宽字符
  int nChars = MultiByteToWideChar(CP_ACP, 0, argv[1], -1, NULL, 0);
  wchar_t *wExePath = new wchar_t[nChars];
  MultiByteToWideChar(CP_ACP, 0, argv[1], -1, wExePath, nChars);
  std::wstring exePath(wExePath);
  delete[] wExePath;

  // 检查文件是否存在
  if (!PathFileExistsW(exePath.c_str())) {
    std::wcout << L"文件不存在: " << exePath << std::endl;
    std::wcout << L"按回车键继续..." << std::endl;
    system("pause");
    return 1;
  } else {
    std::wcout << L"文件: " << exePath << std::endl;
  }

  // 检查文件是否为.exe
  std::filesystem::path path(exePath);
  if (path.extension() != L".exe") {
    std::wcout << L"文件不是可执行文件(.exe): " << exePath << std::endl;
    std::wcout << L"按回车键继续..." << std::endl;
    system("pause");
    return 1;
  }

  // 注册程序
  if (RegisterApplication(exePath)) {
    std::wcout << L"添加成功: " << exePath << std::endl;
  } else {
    std::wcout << L"添加失败: " << exePath << std::endl;
  }

  std::wcout << L"按回车键继续..." << std::endl;
  system("pause");

  return 0;
}