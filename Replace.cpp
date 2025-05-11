#include <iostream>
#include <string>
#include <unordered_map>
#include <windows.h>

// 创建中英文符号映射表
std::unordered_map<wchar_t, wchar_t> createSymbolMap() {
    std::unordered_map<wchar_t, wchar_t> symbolMap;
    // 常见的中英文符号对应关系
    symbolMap[L'；'] = L';';
    symbolMap[L'：'] = L':';
    symbolMap[L'，'] = L',';
    symbolMap[L'。'] = L'.';
    symbolMap[L'！'] = L'!';
    symbolMap[L'？'] = L'?';
    symbolMap[L'（'] = L'(';
    symbolMap[L'）'] = L')';
    symbolMap[L'【'] = L'[';
    symbolMap[L'】'] = L']';
    symbolMap[L'《'] = L'<';
    symbolMap[L'》'] = L'>';
    
    // 修正中文引号的Unicode值
    symbolMap[8220] = L'"'; // 中文左引号 "
    symbolMap[8221] = L'"'; // 中文右引号 "
    
    symbolMap[0x2018] = L'\''; // 左单引号 '
    symbolMap[0x2019] = L'\''; // 右单引号 '
    symbolMap[L'、'] = L'\\';
    symbolMap[L'｛'] = L'{';
    symbolMap[L'｝'] = L'}';
    
    std::cout << "中文引号的Unicode值: " << 8220 << ", " << 8221 << std::endl;
    
    return symbolMap;
}

// 替换文本中的中文符号为英文符号
std::wstring replaceSymbols(const std::wstring& text, const std::unordered_map<wchar_t, wchar_t>& symbolMap) {
    std::wstring result = text;
    bool inString = false;        // 标记是否在字符串内
    bool inCharLiteral = false;   // 标记是否在字符字面量内
    bool inLineComment = false;   // 标记是否在行注释内
    bool inBlockComment = false;  // 标记是否在块注释内
    bool escaped = false;         // 标记前一个字符是否为转义字符
    
    // 使用std::cout替代std::wcout进行调试输出
    std::cout << "输入文本的Unicode值: ";
    for (wchar_t c : text) {
        std::cout << (int)c << " ";
    }
    std::cout << std::endl;
    
    for (size_t i = 0; i < result.size(); ++i) {
        // 处理转义字符
        if (result[i] == L'\\' && !escaped) {
            escaped = true;
            continue;
        }
        
        // 如果前一个字符是转义字符，当前字符不会改变状态
        if (!escaped) {
            // 处理字符串
            if (result[i] == L'"' && !inCharLiteral && !inLineComment && !inBlockComment) {
                inString = !inString;
            }
            // 处理字符字面量
            else if (result[i] == L'\'' && !inString && !inLineComment && !inBlockComment) {
                inCharLiteral = !inCharLiteral;
            }
            // 处理行注释
            else if (result[i] == L'/' && i + 1 < result.size() && result[i+1] == L'/' && !inString && !inCharLiteral && !inBlockComment) {
                inLineComment = true;
            }
            // 处理块注释开始
            else if (result[i] == L'/' && i + 1 < result.size() && result[i+1] == L'*' && !inString && !inCharLiteral && !inLineComment) {
                inBlockComment = true;
            }
            // 处理块注释结束
            else if (result[i] == L'*' && i + 1 < result.size() && result[i+1] == L'/' && inBlockComment) {
                inBlockComment = false;
                i++; // 跳过 '/'
                escaped = false; // 重置转义状态
                continue;
            }
        }
        
        // 处理行注释结束（换行符不受转义影响）
        if (result[i] == L'\n' && inLineComment) {
            inLineComment = false;
        }
        
        // 重置转义状态
        if (escaped) {
            escaped = false;
        }
        
        // 如果不在字符串、字符字面量或注释中，则替换中文符号
        if (!inString && !inCharLiteral && !inLineComment && !inBlockComment) {
            auto it = symbolMap.find(result[i]);
            if (it != symbolMap.end()) {
                result[i] = it->second;
                // 使用std::cout替代std::wcout进行调试输出
                std::cout << "替换: " << (int)result[i] << " 在位置: " << i << std::endl;
            }
        }
    }
    
    return result;
}

// 将UTF-8编码的字符串转换为宽字符串
std::wstring utf8ToWstring(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// 将宽字符串转换为UTF-8编码的字符串
std::string wstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

int main() {
    // 设置控制台输出编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 使用std::cout替代std::wcout
    std::cout << "请输入要替换中英文符号的文本（输入完成后按Ctrl+Z然后回车结束输入）：" << std::endl;
    
    // 读取用户输入的所有文本
    std::string inputText;
    std::string line;
    while (std::getline(std::cin, line)) {
        inputText += line + "\n";
    }
    
    // 如果输入为空，则退出
    if (inputText.empty()) {
        std::wcout << L"没有输入文本，程序退出。" << std::endl;
        return 0;
    }
    
    // 转换为宽字符串
    std::wstring wideContent = utf8ToWstring(inputText);
    
    // 创建符号映射表
    auto symbolMap = createSymbolMap();
    
    // 替换符号
    std::wstring replacedWideContent = replaceSymbols(wideContent, symbolMap);
    
    // 转换回UTF-8
    std::string replacedUtf8Content = wstringToUtf8(replacedWideContent);
    
    // 输出替换后的文本
    std::cout << "\n替换后的文本：\n" << std::endl;
    std::cout << replacedUtf8Content;
    
    return 0;
}