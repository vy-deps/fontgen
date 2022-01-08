#include "bmfont/entry.h"
#include "inipp.h"

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <iostream>
#include <cstdio>

namespace fs = std::filesystem;

struct TmpFile
{
  fs::path path;

  TmpFile()
  {
    path = std::tmpnam(nullptr);
  }

  ~TmpFile()
  {
    fs::remove(path);
  }
};

struct GeneralSettings
{
  std::string characters;
};

struct FontDefinition
{
  std::string name;
  std::string file;
  int         size;
  int         texture_size;
};

static void write_config_file(const fs::path& path, const GeneralSettings& general_settings, const FontDefinition& font)
{
  const char fmt[] = R"V0G0N(# AngelCode Bitmap Font Generator configuration file
fileVersion=1

# font settings
fontName=%s
fontFile=%s
charSet=0
fontSize=%d
aa=1
scaleH=100
useSmoothing=1
isBold=0
isItalic=0
useUnicode=1
disableBoxChars=1
outputInvalidCharGlyph=1
dontIncludeKerningPairs=0
useHinting=1
renderFromOutline=0
useClearType=1
autoFitNumPages=0
autoFitFontSizeMin=0
autoFitFontSizeMax=0

# character alignment
paddingDown=0
paddingUp=0
paddingRight=0
paddingLeft=0
spacingHoriz=1
spacingVert=1
useFixedHeight=1
forceZero=1
widthPaddingFactor=0.00

# output file
outWidth=%d
outHeight=%d
outBitDepth=32
fontDescFormat=1
fourChnlPacked=1
textureFormat=tga
textureCompression=1
alphaChnl=1
redChnl=0
greenChnl=0
blueChnl=0
invA=0
invR=0
invG=0
invB=0

# outline
outlineThickness=0

# selected chars
chars=%s

# imported icon images
)V0G0N";

  char formatted[2048] = {0};

  int e = sprintf_s(formatted, fmt,
                    font.name.c_str(),
                    font.file.c_str(), font.size,
                    font.texture_size, font.texture_size,
                    general_settings.characters.c_str());

  if(e == -1)
  {
    std::cerr << "error formatting bmfont config file\n";
    abort();
  }

  std::cout << formatted;

  std::ofstream stream(path);
  stream << formatted;
  stream.flush();
  stream.close();

  std::cout << "tmp config file written: " << path.generic_string() << "\n";
}

int main(int argc, char** argv)
{
  bmfont::Init();

  std::cout << "current path: " << fs::current_path() << '\n';

  inipp::Ini<char> ini;
  std::ifstream    input_stream("fontgen.ini");
  ini.parse(input_stream);

  GeneralSettings general_settings;

  if(auto it = ini.sections.find("@generator"); it != ini.sections.end())
  {
    inipp::get_value(it->second, "characters", general_settings.characters);
  }
  else
  {
    std::cerr << "[@general] section not found\n";
    return EXIT_FAILURE;
  }

  for(auto& [section_name, section]: ini.sections)
  {
    std::cout << "section: " << section_name << '\n';

    if(section_name.starts_with("@"))
    {
      continue;
    }

    FontDefinition def;
    inipp::get_value(section, "name", def.name);
    inipp::get_value(section, "file", def.file);
    inipp::get_value(section, "size", def.size);
    inipp::get_value(section, "texture_size", def.texture_size);

    def.file = (fs::current_path() / def.file).generic_string();

    TmpFile config_tmp_file;
    write_config_file(config_tmp_file.path, general_settings, def);

    fs::path    out_dir  = fs::current_path() / section_name;
    std::string out_file = (out_dir / (section_name + ".fnt")).generic_string();

    fs::remove_all(out_dir);
    fs::create_directory(out_dir);
    std::cout << "output dir: " << out_dir.generic_string() << '\n';
    std::cout << "output file: " << out_file << '\n';

    int res = bmfont::Run(config_tmp_file.path.generic_string(), out_file);

    if(res != 0)
    {
      std::cerr << "bmfont generation failed!\n";
      return res;
    }
  }

  bmfont::Destroy();
  std::cout << "exit ok\n";

  return 0;
}
