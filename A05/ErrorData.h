#ifndef ERRORDATA_HPP
#define ERRORDATA_HPP

struct ErrorData {
    std::string lineText;
    int line;
    int column;
    ErrorData(char *text, int line, int column)
        :lineText(text ? std::string(text) : ""), line(line), column(column){}
};

#endif