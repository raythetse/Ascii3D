Ascii3D: a real-time ASCII ray-marcher for the Windows console
https://github.com/raythetse/Ascii3D

RUNNING

Unzip the whole folder, then double-click Ascii3D.exe, or run it from a terminal opened in this
folder. Keep the files together: the exe loads the DLLs next to it, and reads scenes from the
CustomScenes folder in the current directory.

Needs Windows 10 or 11 (64-bit). The GPU renderer needs a graphics driver with Vulkan 1.3, which
current NVIDIA, AMD and Intel drivers include. Without one, only the CPU renderer is offered.

The controls are on the "Controls and Information" menu entry. HOWTO.txt explains the controls
and the .scn scene format, for writing your own scenes.

FILES

  Ascii3D.exe                   The program
  shaderc_shared.dll            Compiles the GPU renderer's shader (shaderc, from the Vulkan SDK)
  msvcp140*.dll, vcruntime140*.dll
                                The Microsoft Visual C++ runtime
  CustomScenes\                 Example scenes
  HOWTO.txt                     Controls and scene format
  LICENSE.txt                   Ascii3D's license (MIT)
  THIRD-PARTY-NOTICES.txt, licenses\
                                Licenses of the bundled third-party code

ANTIVIRUS WARNINGS

Ascii3D.exe isn't code-signed, and few people have downloaded it, so Windows SmartScreen or an
antivirus program may warn about it or remove it. It was built by GitHub Actions from the public
source code; the release page links the build and lists the files' SHA-256 hashes. See the
"Download" section of the README on GitHub for how to check a download, report a false positive,
or build Ascii3D from source.
