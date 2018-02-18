// server.cc
#include <node.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <windows.h> 
#include <lm.h>
#include <atlstr.h>
#include<iostream>
#include<clocale>

#pragma comment(lib, "netapi32.lib")

using namespace std;

namespace demo {

using v8::Exception;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

//Конвертация UTF-8 в CP-1251
string Utf8_to_cp1251(const char *str)
{
	string res;	
	int result_u, result_c;


	result_u = MultiByteToWideChar(CP_UTF8,
		0,
		str,
		-1,
		0,
		0);
	
	if (!result_u)
		return 0;

	wchar_t *ures = new wchar_t[result_u];

	if(!MultiByteToWideChar(CP_UTF8,
		0,
		str,
		-1,
		ures,
		result_u))
	{
		delete[] ures;
		return 0;
	}


	result_c = WideCharToMultiByte(
		1251,
		0,
		ures,
		-1,
		0,
		0,
		0, 0);

	if(!result_c)
	{
		delete [] ures;
		return 0;
	}

	char *cres = new char[result_c];

	if(!WideCharToMultiByte(
		1251,
		0,
		ures,
		-1,
		cres,
		result_c,
		0, 0))
	{
		delete[] cres;
		return 0;
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}

//Конвертация CP-1251 в UTF-8
string cp1251_to_utf8(const char *str)
{
	string res;	
	int result_u, result_c;


	result_u = MultiByteToWideChar(1251,
		0,
		str,
		-1,
		0,
		0);
	
	if (!result_u)
		return 0;

	wchar_t *ures = new wchar_t[result_u];

	if(!MultiByteToWideChar(1251,
		0,
		str,
		-1,
		ures,
		result_u))
	{
		delete[] ures;
		return 0;
	}


	result_c = WideCharToMultiByte(
		CP_UTF8,
		0,
		ures,
		-1,
		0,
		0,
		0, 0);

	if(!result_c)
	{
		delete [] ures;
		return 0;
	}

	char *cres = new char[result_c];

	if(!WideCharToMultiByte(
		CP_UTF8,
		0,
		ures,
		-1,
		cres,
		result_c,
		0, 0))
	{
		delete[] cres;
		return 0;
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}

//Проверка пользователя
bool check(string s1)
{

	LPUSER_INFO_3  ui = NULL;
	DWORD dwRead = 0;
	DWORD dwTotal = 0;
	//Системная функция получения всех пользователей
	NetUserEnum(NULL, 3, FILTER_NORMAL_ACCOUNT,
		(LPBYTE *)&ui, (DWORD)-1, &dwRead,
		&dwTotal, NULL);
	
	string st = Utf8_to_cp1251(s1.c_str());
	//Цикл по пользователям
	for (DWORD i = 0; i < dwRead; i++)
	{
		string s = (CW2A(ui->usri3_name));
		//Сравнение пользователя с значением списка
		if (s == st)
			return true;
		ui++;
	}
	return false;
}

//Функция получения всех пользователей
string checkall()
{

	LPUSER_INFO_3  ui = NULL;
	DWORD dwRead = 0;
	DWORD dwTotal = 0;
	//Системная функция получения всех пользователей
	NetUserEnum(NULL, 3, FILTER_NORMAL_ACCOUNT,
		(LPBYTE *)&ui, (DWORD)-1, &dwRead,
		&dwTotal, NULL);
		
	string ans = "<h2>Все пользователи:</h2>";
	
	//Цикл вывода результата
	for (DWORD i = 0; i < dwRead; i++)
	{
		string s = (CW2A(ui->usri3_name));
		string ns = cp1251_to_utf8(s.c_str());
		
		ans = ans + "<label>" + ns + "</label><br>";
		
		ui++;
	}
	return ans;
}

//Внешняя функция проверки пользователя
void Users(const FunctionCallbackInfo<Value>& args) 
{
  Isolate* isolate = args.GetIsolate();

  // Проверка параметров
  if (args.Length() < 1) {
    // Throw an Error that is passed back to JavaScript
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  // Проверка типа параметра
  if (!args[0]->IsString() ) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }
  
  // Получение параметра
  v8::String::Utf8Value param1(args[0]->ToString());

  // Конвертация параметра
  std::string value = std::string(*param1);
  
  //Проверка пользователя в системе
  bool checkValue = check(value);
  //Вывод результата
  string answer = "Пользователя не существует в системе.";
  if (checkValue)
  {
	  answer = "Пользователь существует в системе.";
  }
  
  Local<String> str = String::NewFromUtf8(isolate, answer.c_str());

  args.GetReturnValue().Set(str);
}

//Внешняя функция получения всех пользователей
void GetAll(const FunctionCallbackInfo<Value>& args) 
{
  Isolate* isolate = args.GetIsolate();
  //Получение всех пользователей
  string answer = checkall();
  //Вывод результата
  Local<String> str = String::NewFromUtf8(isolate, answer.c_str());

  args.GetReturnValue().Set(str);
}
//Список экспортируемых функций
void Init(Local<Object> exports) 
{
	NODE_SET_METHOD(exports, "users", Users);
	NODE_SET_METHOD(exports, "getall", GetAll);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Init)

}  // namespace demo