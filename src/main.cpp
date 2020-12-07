#include "clHCA.h"

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace VGMToolbox::format;

int main(array<System::String ^> ^args) {
	unsigned int ciphKey1 = 0, ciphKey2 = 0;
	switch (args->Length) {
		case 3: {
			ciphKey1 = System::Convert::ToInt32(args[1], 16);
			ciphKey2 = System::Convert::ToInt32(args[2], 16);
		}
		case 1: {
			clHCA hca(ciphKey1, ciphKey2);
			float volume = 1;
			int mode = 16;
			int loop = 0;
			auto dir = gcnew DirectoryInfo(args[0]);
			auto dir2 = Directory::CreateDirectory(args[0] + "/waveout");
			
			for each (auto cpkfile in dir->GetFiles("*.cpk", SearchOption::AllDirectories)) {
				Console::WriteLine(L"Extract CPK Archive: '{0}'", cpkfile->Name);
				auto fs = gcnew FileStream(cpkfile->FullName, FileMode::Open, FileAccess::Read, FileShare::Read);
				auto af = gcnew CriCpkArchive();
				af->Initialize(fs, 0, false);
				af->ExtractAll();
				fs->Close();
			}

			for each (auto acbfile in dir->GetFiles("*.acb", SearchOption::AllDirectories)) {
				Console::WriteLine(L"Extract ACB/AWB Archive: '{0}'", acbfile->Name);
				auto fs = gcnew FileStream(acbfile->FullName, FileMode::Open, FileAccess::Read, FileShare::Read);
				auto af = gcnew CriAcbFile(fs, 0, false);
				af->ExtractAll();
				fs->Close();
				auto destinationFolder = gcnew DirectoryInfo(Path::Combine(acbfile->DirectoryName, L"_vgmt_acb_ext_" + Path::GetFileNameWithoutExtension(acbfile->FullName)));
				auto hcafiles = destinationFolder->GetFiles(L"*.hca", SearchOption::AllDirectories);
				auto OutFolder = dir2->FullName;
				if (hcafiles->Length > 1) {
					OutFolder = Path::Combine(dir2->FullName, Path::GetFileNameWithoutExtension(acbfile->FullName));
					Directory::CreateDirectory(OutFolder);
				}
				for each (auto hcafile in hcafiles) {
					Console::WriteLine(L"Convert file: '{0}'", hcafile->Name);
					auto filenameIn = (char*)(void*)Marshal::StringToHGlobalAnsi(hcafile->FullName);
					auto filenameOut = (char*)(void*)Marshal::StringToHGlobalAnsi(Path::Combine(OutFolder, Path::GetFileNameWithoutExtension(hcafile->FullName) + L".wav"));
					hca.DecodeToWavefile(filenameIn, filenameOut, volume, mode, loop);
				}
			}
			return 0;
		}
		default: {
			Console::WriteLine(L"Usage: cri2wav input_directory [key1] [key2]");
			return 1;
		}
	}
}