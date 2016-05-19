#include <stdio.h>
#include <string>
#include <vector>
#include <stdarg.h>

#include "UarmError.h"

void UarmError::append(UarmError *e){
  content.push_back(e);
  is_empty = false;
}
::std::string UarmError::emit(){
  ::std::string result = "";
  if (!is_empty) {
    result += error_text;
    result += "\n";
  }
  if (content.size()) {
    for (auto i = content.begin(); i != content.end(); ++i) {
      result += (*i)->emit();
    }
  }
  return result;
}
bool UarmError::isEmpty(){
  return is_empty;
}
UarmError::~UarmError() {
  if (content.size()) {
    for (auto i = content.begin(); i != content.end(); ++i) {
      delete *i;
    }
  }
}