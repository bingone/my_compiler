::�趨��������
set CL_HOME=C:\Users\shaoyan.xsy\Desktop\study\use_tool\mycl

echo off
cls
color 0A

echo �趨��������...
set INCLUDE=%CL_HOME%/include
set LIB=%CL_HOME%/lib
::ִ��˳��Ҫ�ұ䣬�����˳����Ҫ��
%CL_HOME% cl /c DynString.cpp DynArray.cpp GlobalVariable.cpp exception_handler.cpp TkWord.cpp get_token.cpp color_token.cpp init_lex.cpp main.cpp
%CL_HOME% link /out:main.exe DynString.obj DynArray.obj GlobalVariable.obj exception_handler.obj TkWord.obj get_token.obj color_token.obj init_lex.obj main.obj