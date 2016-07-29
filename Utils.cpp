
#include <windows.h>
#include <msclr\marshal_cppstd.h>

#include "Utils.h"
using namespace System::Reflection;

#include "UarmError.h"

// CUtils
//////////////////////
CUtils::CUtils(std::string ConfigName) {
  ::std::string ConfigFullName(GetDLLPath() + "\\" + ConfigName);
  m_IniError = m_Ini.LoadFile(ConfigFullName.c_str());
}

std::string CUtils::GetDLLPath() {
  auto assembly = Assembly::GetExecutingAssembly();
  String ^ path = IO::Path::GetDirectoryName(assembly->Location);

  ::std::string Res = msclr::interop::marshal_as<::std::string>(path);

  return Res;
}

::std::string CUtils::getIniValueStr(::std::string section_name,
                                   ::std::string key_name) {
  const char* res(m_Ini.GetValue(section_name.c_str(), key_name.c_str(), NULL));
  if (!res) {
    char buff[1024];
    sprintf_s(buff, "Not specified value for \"%s\" in section \"%s\"!\n",
                    key_name.c_str(), section_name.c_str());
    throw new UarmError();
  }

  return std::string(res);
}

int CUtils::getIniValueInt(std::string section_name, std::string key_name) {
  std::string StrVal(getIniValueStr(section_name, key_name));

  return std::stoi(StrVal);
}

std::string CUtils::Str_To_StdStr(String ^ Str) {
  return msclr::interop::marshal_as<std::string>(Str);
}

void CUtils::printError(Exception ^ E) {
  std::string Msg(Str_To_StdStr(E->ToString()));
  Msg += "\n";
  printf(Msg.c_str());
}

bool CUtils::CheckAnlgeVal(int angle, int min /* = 0*/, int max /* = 180*/) {
  return angle >= min && angle <= max;
}