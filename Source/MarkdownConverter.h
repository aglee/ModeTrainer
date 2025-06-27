#pragma once

#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <cctype>

class MarkdownConverter
{
public:
    MarkdownConverter() = default;
    ~MarkdownConverter() = default;

    /**
     * Convert markdown text to HTML
     * @param markdown The markdown text to convert
     * @return HTML string
     */
    std::string convertToHtml(const std::string& markdown);

private:
    /**
     * Process inline formatting (bold, italic, code) within a line
     * @param line The line to process
     * @return Processed line with HTML tags
     */
    std::string processInlineFormatting(const std::string& line);

    /**
     * Process headers (# ## ###)
     * @param line The line to check
     * @return HTML header or empty string if not a header
     */
    std::string processHeader(const std::string& line);

    /**
     * Check if line is part of a list and get list info
     * @param line The line to check
     * @param listLevel Reference to store the nesting level
     * @param isOrdered Reference to store if it's an ordered list
     * @return true if line is a list item
     */
    bool isListItem(const std::string& line, int& listLevel, bool& isOrdered);

    /**
     * Get the content of a list item (without the marker)
     * @param line The list item line
     * @return Content without list markers
     */
    std::string getListItemContent(const std::string& line);

    /**
     * Check if line starts a code block
     * @param line The line to check
     * @return true if line starts with ```
     */
    bool isCodeBlockStart(const std::string& line);

    /**
     * Escape HTML special characters
     * @param text Text to escape
     * @return Escaped text
     */
    std::string escapeHtml(const std::string& text);

    /**
     * Trim whitespace from string
     * @param str String to trim
     * @return Trimmed string
     */
    std::string trim(const std::string& str);

    /**
     * Split string into lines
     * @param text Text to split
     * @return Vector of lines
     */
    std::vector<std::string> splitLines(const std::string& text);

    // State tracking for nested lists
    struct ListState
    {
        int level = 0;
        bool isOrdered = false;
        std::vector<bool> orderedStack; // Track ordered/unordered at each level
    };
};
