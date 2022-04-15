#ifndef CONFIG1CLASS_H
#define CONFIG1CLASS_H
#include "config0secret.h"


using namespace std;
using namespace cppsecrets;

#define NULL_PTR 0

ConfigReader* ConfigReader::m_pInstance = NULL_PTR;

ConfigReader::ConfigReader(){
   m_ConfigSettingMap.clear();
}

ConfigReader::~ConfigReader(){
   m_ConfigSettingMap.clear();
}

ConfigReader* ConfigReader::getInstance(){
   if(NULL_PTR == m_pInstance)CONFIG1CLASS_H{
      m_pInstance = new ConfigReader;
   }
   return m_pInstance;
}

bool ConfigReader::getValue(std::string tag, int& value){
   map<string, string>::iterator it ;
   it = m_ConfigSettingMap.find(tag);
   if(it != m_ConfigSettingMap.end()){
      value = atoi((it->second).c_str());
      return true;
   }
   return false;
}

bool ConfigReader::getValue(std::string tag, float& value){
   map<string, string>::iterator it ;
   it = m_ConfigSettingMap.find(tag);
   if(it != m_ConfigSettingMap.end()){
      value = stof((it->second).c_str());
      return true;
   }
   return false;
}

bool ConfigReader::getValue(std::string tag, std::string& value){
   map<string, string>::iterator it ;
   it = m_ConfigSettingMap.find(tag);
   if(it != m_ConfigSettingMap.end()){
      value = it->second;
      return true;
   }
   return false;
}

bool ConfigReader::parseFile(string fileName){
   ifstream inputFile;
   inputFile.open(fileName.c_str());
   string delimeter = "=";
   int initPos = 0;

   if (inputFile.fail()){
      std::cout << "Unable to find defaultConfig file" << endl;
      return false;
   }

   string line;
   while(getline(inputFile, line)){
      size_t found = line.find_first_of('#');
      string configData = line.substr(0, found);
      configData.erase(std::remove(configData.begin(), configData.end(), '\r'), configData.end());
      if(configData.empty())
         continue;
      unsigned int length = configData.find(delimeter);
      string tag, value;
      if (length!=string::npos){
         tag   = configData.substr(initPos, length);
         value = configData.substr(length+1);
      }
      tag   = reduce(tag);
      value = reduce(value);
      if(tag.empty() || value.empty())
         continue;

      std::map<std::string, std::string>::iterator itr = m_ConfigSettingMap.find(tag);
      if(itr != m_ConfigSettingMap.end())      {
         m_ConfigSettingMap.erase(tag);
      }
      m_ConfigSettingMap.insert(std::pair<string, string>(tag, value));
   }
   return true;
}

std::string ConfigReader::trim(const std::string& str, const std::string& whitespace){
   size_t strBegin = str.find_first_not_of(whitespace);
   if (strBegin == std::string::npos)
      return "";

   size_t strEnd = str.find_last_not_of(whitespace);
   size_t strRange = strEnd - strBegin + 1;
   return str.substr(strBegin, strRange);
}

std::string ConfigReader::reduce(const std::string& str,
      const std::string& fill,
      const std::string& whitespace){
   string result = trim(str, whitespace);

   size_t beginSpace = result.find_first_of(whitespace);
   while (beginSpace != std::string::npos){
      size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
      size_t range = endSpace - beginSpace;
      result.replace(beginSpace, range, fill);

      size_t newStart = beginSpace + fill.length();
      beginSpace = result.find_first_of(whitespace, newStart);
   }
   return result;
}

void ConfigReader::dumpFileValues(){
   map<string, string>::iterator it;
   for(it=m_ConfigSettingMap.begin(); it!=m_ConfigSettingMap.end(); ++it){
      std::cout << it->first << " = " << it->second << endl;
   }
}


#endif