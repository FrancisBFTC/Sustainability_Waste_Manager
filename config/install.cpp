#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
	// Descobre o diretório atual e forma o diretório do compilador
	TCHAR szDirAtual[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szDirAtual);

	string directory = szDirAtual;
	directory.replace(directory.find("config"), 6, "");

	// Insere diretório na variável de ambiente PATH
	cout << "Inserindo '" << directory << "' na variavel de ambiente PATH" << endl;
	stringstream environment;
	environment << "setx PATH \"%PATH%;" << directory << ";\" /M";
	system(environment.str().c_str());

	// Inserir dados no Registro do Windows (Pega diretório raiz)
	string rootDir = szDirAtual;
	rootDir.replace(rootDir.find("config"), 6, "");

	// Cria uma chave principal para menu do mouse e descrição do tipo de arquivo
	cout << "Gravando Registros no Windows de arquivos WASTE" << endl;
	stringstream regadds;
	regadds << "REG ADD HKCR\\waste.file /t REG_SZ /d \"Waste Report File\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\waste.file\\DefaultIcon /t REG_SZ /d \"" << rootDir << "icons\\waste.ico\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\waste.file\\shell\\Edit /t REG_SZ /d \"&Editar\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\waste.file\\shell\\Edit\\command /t REG_SZ /d \"C:\\Program Files\\Notepad++\\notepad++.exe %1\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\waste.file\\shell\\Open /t REG_SZ /d \"&Abrir\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\waste.file\\shell\\Open\\command /t REG_SZ /d \"" << directory << "\\WManagerSystem.exe %1\" /f";
	system(regadds.str().c_str());
	regadds.str("");

	// Cria e Configura uma chave de extensão no ROOT
	regadds << "REG ADD HKCR\\.w /t REG_SZ /d \"waste.file\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\.w /v \"Content Type\" /t REG_SZ /d \"text/w\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\.w /v \"PerceivedType\" /t REG_SZ /d \"text\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\.w /v \"waste.bak\" /t REG_SZ /d \"waste.file\" /f";
	system(regadds.str().c_str());
	regadds.str("");

	regadds << "REG ADD HKCR\\.w\\OpenWithProgids /t REG_SZ /d \"waste.file\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCR\\.w\\OpenWithProgids /v waste.file /t REG_BINARY /f";
	system(regadds.str().c_str());
	regadds.str("");

	// Cria e Configura uma chave de extensão no USER
	regadds << "REG ADD HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.w\\OpenWithList /t REG_SZ /d \"WManagerSystem.exe\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.w\\OpenWithList /v \"a\" /t REG_SZ /d \"C:/Program Files/Notepad++/Notepad++.exe\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.w\\OpenWithList /v \"b\" /t REG_SZ /d \"C:/Program Files/Sublime Text/sublime_text.exe\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.w\\OpenWithList /v \"c\" /t REG_SZ /d \"code.exe\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.w\\OpenWithList /v \"MRUList\" /t REG_SZ /d \"abc\" /f";
	system(regadds.str().c_str());
	regadds.str("");

	regadds << "REG ADD HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.w\\OpenWithProgids /t REG_SZ /d \"waste.file\" /f";
	system(regadds.str().c_str());
	regadds.str("");
	regadds << "REG ADD HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.w\\OpenWithProgids /v waste.file /t REG_BINARY /f";
	system(regadds.str().c_str());
	regadds.str("");

	system("pause");

	// Confira Syntax highlight do VSCode e Sublime Text
	/*
	cout << "Configurando Highlight do VSCode e Sublime Text" << endl;
	stringstream dirsyntax;
	dirsyntax << rootDir << "Highlight\\VSCode Syntax\\plax";
	stringstream copy;
	copy << "xcopy /I /E \"" << dirsyntax.str() << "\" \"%userprofile%\\appdata\\Local\\Programs\\Microsoft VS Code\\resources\\app\\extensions\\plax\\\"";
	system(copy.str().c_str());
	dirsyntax.str("");
	copy.str("");

	dirsyntax << rootDir << "Highlight\\Sublime Syntax\\plax.sublime-syntax";
	copy << "copy \"" << dirsyntax.str() << "\" \"%appdata%\\Sublime Text\\Packages\\User\\\"";
	system(copy.str().c_str());
	dirsyntax.str("");
	copy.str("");
	*/
}
