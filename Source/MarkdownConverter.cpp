#include "MarkdownConverter.h"

std::string MarkdownConverter::convertToHtml(const std::string& markdown) {
    std::stringstream html;
    std::vector<std::string> lines = splitLines(markdown);
    ListState listState;
    bool inCodeBlock = false;
    
    for (const auto& line : lines) {
        std::string trimmedLine = trim(line);

        if (isCodeBlockStart(trimmedLine)) {
            html << (inCodeBlock ? "</code></pre>" : "<pre><code>");
            inCodeBlock = !inCodeBlock;
            continue;
        }

        if (inCodeBlock) {
            html << escapeHtml(line) << "\n";
            continue;
        }

        std::string header = processHeader(trimmedLine);
        if (!header.empty()) {
            html << header;
            continue;
        }

        int listLevel;
        bool isOrdered;
        if (isListItem(trimmedLine, listLevel, isOrdered)) {
            if (listLevel > listState.level) {
                html << (isOrdered ? "<ol>" : "<ul>");
                listState.orderedStack.push_back(isOrdered);
            } else if (listLevel < listState.level) {
                html << (listState.orderedStack.back() ? "</ol>" : "</ul>");
                listState.orderedStack.pop_back();
            }

            listState.level = listLevel;
            html << "<li>" << processInlineFormatting(getListItemContent(trimmedLine)) << "</li>";
            continue;
        } else if (listState.level > 0) {
            while (listState.level > 0) {
                listState.level--;
                html << (listState.orderedStack.back() ? "</ol>" : "</ul>");
                listState.orderedStack.pop_back();
            }
        }

        html << "<p>" << processInlineFormatting(trimmedLine) << "</p>";
    }

    while (listState.level > 0) {
        listState.level--;
        html << (listState.orderedStack.back() ? "</ol>" : "</ul>");
        listState.orderedStack.pop_back();
    }

    return html.str();
}

std::string MarkdownConverter::processInlineFormatting(const std::string& line) {
    std::string processed = line;
    
    // Process links first (before bold/italic to avoid conflicts)
    processed = std::regex_replace(processed, std::regex(R"(\[([^\]]+)\]\(([^\)]+)\))"), "<a href=\"$2\">$1</a>");
    
    // Process bold, italic, and code
    processed = std::regex_replace(processed, std::regex(R"(\*\*(.*?)\*\*)"), "<strong>$1</strong>");
    processed = std::regex_replace(processed, std::regex(R"(\*(.*?)\*)"), "<em>$1</em>");
    processed = std::regex_replace(processed, std::regex(R"(`(.*?)`)"), "<code>$1</code>");
    
    return processed;
}

std::string MarkdownConverter::processHeader(const std::string& line) {
    std::smatch match;
    if (std::regex_match(line, match, std::regex(R"(^(#{1,6})\s*(.+))"))) {
        auto level = static_cast<int>(match[1].str().length());
        return "<h" + std::to_string(level) + ">" + escapeHtml(match[2].str()) + "</h" + std::to_string(level) + ">";
    }
    return "";
}

bool MarkdownConverter::isListItem(const std::string& line, int& listLevel, bool& isOrdered) {
    std::smatch match;
    if (std::regex_match(line, match, std::regex(R"(^\s*((?:\*|\-|\+)|\d+\.)\s+.+)"))) {
        listLevel = static_cast<int>(line.find_first_not_of(" \t") / 2); // Assumes 2 spaces per indent level
        isOrdered = std::isdigit(match[1].str().front());
        return true;
    }
    return false;
}

std::string MarkdownConverter::getListItemContent(const std::string& line) {
    return std::regex_replace(line, std::regex(R"(^\s*((?:\*|\-|\+)|\d+\.)\s+)"), "");
}

bool MarkdownConverter::isCodeBlockStart(const std::string& line) {
    return line == "```";
}

std::string MarkdownConverter::escapeHtml(const std::string& text) {
    std::string escaped = text;
    escaped = std::regex_replace(escaped, std::regex("&"), "&amp;");
    escaped = std::regex_replace(escaped, std::regex("<"), "&lt;");
    escaped = std::regex_replace(escaped, std::regex(">"), "&gt;");
    return escaped;
}

std::string MarkdownConverter::trim(const std::string& str) {
    const auto strBegin = str.find_first_not_of(" \t");
    if (strBegin == std::string::npos) return "";

    const auto strEnd = str.find_last_not_of(" \t");
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::vector<std::string> MarkdownConverter::splitLines(const std::string& text) {
    std::stringstream ss(text);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    return lines;
}

