#pragma once

#include <string>
#include "SimpleIni.h"
using namespace System;

class CUtils {
 private:
  SI_Error m_IniError;
  ::std::string m_CongigPath;

 protected:
  CSimpleIniA m_Ini;

 public:
	 explicit CUtils(std::string ConfigName);

  SI_Error getIniError() { return m_IniError; }

  ::std::string getConfigPath() { return m_CongigPath; }

  int getIniValueInt(::std::string section_name, ::std::string key_name);
  ::std::string getIniValueStr(::std::string section_name, ::std::string key_name);

  static ::std::string GetDLLPath();

  /// String tools
  static ::std::string Str_To_StdStr(String ^ Str);
  static void printError(Exception ^ E);

  /// Check tools
  static bool CheckAnlgeVal(int angle, int min = 0, int max = 180);
  static bool CheckAnlgeVal(double angle, int min = 0, int max = 180) {
    return CheckAnlgeVal((int)angle, min, max);
  }
  static bool CheckServoIdx(double idx){
	  return idx > 0 && idx <= 4;
  }
};