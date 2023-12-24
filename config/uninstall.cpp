#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
	// Deleta a chave principal para menu do mouse e descrição do tipo de arquivo
	cout << "Deletando Registros no Windows de arquivos WASTE" << endl;
	stringstream regadds;
	regadds << "REG DELETE HKCR\\waste.file /f";
	system(regadds.str().c_str());
	regadds.str("");

	// Deleta a chave de extensão no ROOT
	regadds << "REG DELETE HKCR\\.w /f";
	system(regadds.str().c_str());
	regadds.str("");

	// Deleta a chave de extensão no USER
	regadds << "REG DELETE HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.w /f";
	system(regadds.str().c_str());
	regadds.str("");

	system("pause");
}
