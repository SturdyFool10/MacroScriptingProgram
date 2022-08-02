# MacroScriptingProgram
Autohotkey is known for being the thing to use if you need more advanced macros, this program is trying to replace it with one that does the same thing with scripting languages that are widely used
in order to compile you will need to use vcpkg to install duktape:x64-windows and then add the include directory to project, include the .lib file as well then place
the .dll file in /out/x64/release in the directory with your application to make it run, at that point you can compile and run

to set the script file use your command line of choice and use "--js" to set lang to javascript, to set a script file use "--File <filename and path here>" documentation
can be found in source under the docs pragma region, enjoy
