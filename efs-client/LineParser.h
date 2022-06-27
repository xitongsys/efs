#pragma once

#include <cstring>
#include <string.h>
#include <set>
#include <vector>
#include <string>

class LineParser
{
public:
    LineParser(const std::string& line_)
    {
        _line = line_;
        _idx = 0;
        _length = _line.size();
        _delimiters = { '\t', ' ' };
    }

    bool IsDelimiter(char c)
    {
        return _delimiters.find(c) != _delimiters.end();
    }

    char ParseEscape()
    {
        char res = 0;
        _idx++;
        if (_idx >= _length)
            return res;

        switch (_line[_idx])
        {
        case 'a':
            res = '\a';
            break;
        case 'b':
            res = '\b';
            break;
        case 'f':
            res = '\f';
            break;
        case 'n':
            res = '\n';
            break;
        case 'r':
            res = '\r';
            break;
        case 't':
            res = '\t';
            break;
        case 'v':
            res = '\v';
            break;
        case '\\':
            res = '\\';
            break;
        case '\'':
            res = '\'';
            break;
        case '0':
            res = '\0';
            break;
        default:
            res = _line[_idx];
            break;
        }
        _idx++;
        return res;
    }

    std::string ParseQuote()
    {
        std::string token = "";
        _idx++;
        while (_idx < _length)
        {
            if (_line[_idx] == '\\')
            {
                token.push_back(ParseEscape());
            }
            else if (_line[_idx] == '"')
            {
                _idx++;
                break;
            }
            else
            {
                token.push_back(_line[_idx]);
                _idx++;
            }
        }
        return token;
    }

    std::string ParseOne()
    {
        std::string res;
        while (_idx < _length && IsDelimiter(_line[_idx]))
            _idx++;

        while (_idx < _length)
        {
            if (_line[_idx] == '\\')
                res += ParseEscape();
            else if (_line[_idx] == '"')
                res += ParseQuote();
            else if (IsDelimiter(_line[_idx]))
                break;
            else
                res.push_back(_line[_idx++]);
        }
        return res;
    }

    std::vector<std::string> Parse()
    {
        std::vector<std::string> res;
        while (_idx < _length)
        {
            std::string token = ParseOne();
            if (token.size())
                res.push_back(token);
        }
        return res;
    }

private:
    std::set<char> _delimiters;
    std::string _line;
    int _idx;
    int _length;
};