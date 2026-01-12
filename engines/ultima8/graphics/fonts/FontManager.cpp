/*
Copyright (C) 2004-2006 The Pentagram team
Copyright (C) 2025 SDL3 port - TTF support disabled

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "pent_include.h"

#include "FontManager.h"
#include "Font.h"
#include "GameData.h"
#include "FontShapeArchive.h"
#include "SettingManager.h"
#include "FileSystem.h"
#include "IDataSource.h"
#include "ShapeFont.h"

// TTF support disabled - SDL3_ttf not available
// #include "TTFont.h"

FontManager* FontManager::fontmanager = 0;

FontManager::FontManager(bool ttf_antialiasing_) : ttf_antialiasing(ttf_antialiasing_)
{
	con.Print(MM_INFO, "Creating Font Manager...\n");
	con.Print(MM_INFO, "Note: TTF font support disabled (SDL3_ttf not available)\n");

	assert(fontmanager == 0);
	fontmanager = this;

	SettingManager* settingman = SettingManager::get_instance();
	settingman->setDefault("ttf_highres", true);

	// TTF_Init() - disabled
}

FontManager::~FontManager()
{
	con.Print(MM_INFO, "Destroying Font Manager...\n");

	resetGameFonts();

	for (unsigned int i = 0; i < ttfonts.size(); ++i)
		delete ttfonts[i];
	ttfonts.clear();

	// TTF font cleanup disabled
	ttf_fonts.clear();

	// TTF_Quit() - disabled

	assert(fontmanager == this);
	fontmanager = 0;
}

// Reset the font manager
void FontManager::resetGameFonts()
{
	for (unsigned int i = 0; i < overrides.size(); ++i)
		delete overrides[i];
	overrides.clear();
}

Pentagram::Font* FontManager::getGameFont(unsigned int fontnum,
										  bool allowOverride)
{
	if (allowOverride && fontnum < overrides.size() && overrides[fontnum])
		return overrides[fontnum];

	// ShapeFont inherits from Pentagram::Font (multiple inheritance)
	ShapeFont* sf = GameData::get_instance()->getFonts()->getFont(fontnum);
	return sf;  // implicit upcast to Pentagram::Font*
}

Pentagram::Font* FontManager::getTTFont(unsigned int fontnum)
{
	if (fontnum >= ttfonts.size())
		return 0;
	return ttfonts[fontnum];
}


TTF_Font* FontManager::getTTF_Font(std::string filename, int pointsize)
{
	// TTF support disabled
	perr << "TTF font support disabled: " << filename << std::endl;
	return 0;
}

void FontManager::setOverride(unsigned int fontnum, Pentagram::Font* override)
{
	if (fontnum >= overrides.size())
		overrides.resize(fontnum+1);
	if (overrides[fontnum])
		delete overrides[fontnum];
	overrides[fontnum] = override;
}


bool FontManager::addTTFOverride(unsigned int fontnum, std::string filename,
								 int pointsize, uint32 rgb, int bordersize,
								 bool SJIS)
{
	// TTF support disabled
	perr << "TTF font support disabled, cannot override font " << fontnum << std::endl;
	return false;
}

bool FontManager::addJPOverride(unsigned int fontnum,
								unsigned int jpfont, uint32 rgb)
{
	// TTF support disabled
	perr << "TTF font support disabled, cannot add JP override" << std::endl;
	return false;
}

bool FontManager::loadTTFont(unsigned int ttfnum, std::string filename,
							 int pointsize, uint32 rgb, int bordersize)
{
	// TTF support disabled
	perr << "TTF font support disabled, cannot load TTF font " << filename << std::endl;
	return false;
}
