#include "dataset.h"

vector<float> getAttribute(string dataStr)
{
    vector<float> retData;
    const char* str = dataStr.c_str();

    float value = 0.0;
    for (int index = 0; index < dataStr.size(); index++) {
        if (isspace(*str)) {
            str++;
            continue;
        }

        string s;
        while (!isspace(*str)) {
            s.insert(s.end(), *str);
            str++;
            index++;
            if (index >= dataStr.size())
                break;
        }
        value = atof(s.c_str());
        retData.push_back(value);
        str++;
    }
    return retData;
}


