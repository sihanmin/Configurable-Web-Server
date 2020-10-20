#include <string>
#include <iostream>

#include "utilities.h"

/// from http://www.cplusplus.com/forum/general/94849/, decodes URL
/// Added parsing for '+'
/// '%'
std::string urlDecode(std::string &eString) {
  std::string ret;
  char ch;
  int i, j;
  for (i=0; i<eString.length(); i++) {
    if (int(eString[i])==37) {
      sscanf(eString.substr(i+1,2).c_str(), "%x", &j);
      ch=static_cast<char>(j);
      ret+=ch;
      i=i+2;
    } else if (int(eString[i])==43) {
        ret+=' ';
    }
    else {
      ret+=eString[i];
    }
  }
  return (ret);
}