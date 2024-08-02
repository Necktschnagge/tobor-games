#pragma once

#include <string>

namespace tobor {

	class License {
	private:
		inline static const std::string MD_LIC{
			R"xxx(## Tobor License Information

* All rights reserved, unless stated otherwise for certain parts.
* Tobor is a software written as a private project for the authors' own fulfillment.
* Authors:
   * Maximilian Starke: [GitHub: https://github.com/Necktschnagge](https://github.com/Necktschnagge), [e-mail: maximilian.starke@medionmail.com](mailto:maximilian.starke@medionmail.com)
   * Daniel Wehmeyer: [GitHub: https://github.com/r0ost3r](https://github.com/r0ost3r), [e-mail: danweh@spamfence.net](mailto:danweh@spamfence.net)
* You are alllowed to download and enjoy this software for your private purpose of entertainment or together with friends.
* You are not allowed to re-upload or distribute Tobor in any way.
* Commercial use is forbidden unless you purchase an aditional commercial license.
   * In this case, please contact main author Maximilian Starke [GitHub: https://github.com/Necktschnagge](https://github.com/Necktschnagge), [e-mail: maximilian.starke@medionmail.com](mailto:maximilian.starke@medionmail.com).

* License on certain parts:
   * This project uses several external libraries via e.g. git submodules. Please stick to their licenses when using them.
   * Some tools may be committed as files to Tobor's repository. If a file has its own license information, only the license information given in this particular file will be valid for this particular file.
   * License on SVG content:
      * All SVG content in this software is licensed under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/), including entire SVGs that may be exported runnign Tobor as well as snippets of SVGs to be found in the code.
      * For some of our SVG content we remixed or simply reused SVG emojis by [OpenMoji](https://openmoji.org). Therefore we decided to share *every* part of our SVG content under the same license CC BY-SA 4.0.


By using this software you agree to the license restrictions above.

Have fun and enjoy our software!
)xxx"
		};

	public:

		inline static std::string get_markdown_license() {
			return MD_LIC;
		}
	};

}
