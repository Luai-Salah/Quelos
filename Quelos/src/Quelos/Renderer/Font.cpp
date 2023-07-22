#include "QSPCH.h"
#include "Font.h"

#undef INFINITE
#include "msdfgen.h"
#include "msdfgen-ext.h"

namespace Quelos
{
	Font::Font(const std::filesystem::path& filePath)
	{
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();

		if (ft)
		{
			std::string fileString = filePath.string();
			msdfgen::FontHandle* fontHandle = msdfgen::loadFont(ft, fileString.c_str());
			if (fontHandle)
			{
				msdfgen::Shape shape;
				if (msdfgen::loadGlyph(shape, fontHandle, 'A'))
				{
					shape.normalize();
					msdfgen::edgeColoringSimple(shape, 3.0f);
					msdfgen::Bitmap<float, 3> msdf(32, 32);
					msdfgen::generateMSDF(msdf, shape, 4, 1, msdfgen::Vector2(4, 4));
					msdfgen::savePng(msdf, "output.png");
				}

				msdfgen::destroyFont(fontHandle);
			}

			msdfgen::deinitializeFreetype(ft);
		}
	}
}
