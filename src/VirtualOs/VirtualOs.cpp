#define _CRT_SECURE_NO_WARNINGS
#include "../Kernel/Kernel.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

VirtualOs::VirtualOs(DeskTop* d_top) : desktop(d_top), kernel(new Kernel(processes, &desktop, this)) {
	desktop->interrupt_vec = &kernel->interrupts;
	desktop->kernel_mut = &kernel->mutex;

	if (fs::exists("DeskTop")) {

		for (const auto& entry : fs::directory_iterator("DeskTop")) {

			if (entry.path().filename().string().starts_with("Wallpaper")) {

				wxImage img(entry.path().filename().string());
				desktop->bmp = wxBitmap(img);
				continue;

			}
			
			if (entry.is_directory()) {


				if (fs::exists(entry.path() / "main.dat")) {
					std::string icon_name;

					for (const auto& img : fs::directory_iterator(entry.path())) {
						if (img.path().filename().string().starts_with("icon")) {
							icon_name = img.path().string();
						}
					}
					
					desktop->addIcon(new OsAppIcon(desktop, icon_name, (entry.path() / "main.dat").string(),
						entry.path().filename().string(), desktop->interrupt_vec, desktop->kernel_mut));

				}

				continue;
			}
		}

	}
	else {
		fs::create_directory("DeskTop");
	}


	desktop->init();
	kernel->desktop = &desktop;
	kernel->doLifeCycle();
};